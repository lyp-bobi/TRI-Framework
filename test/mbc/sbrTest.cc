//
// Created by Chuang on 2019/6/28.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <tuple>
#include <map>
#include<stdlib.h>
#include<time.h>
#include <cmath>
#define random(x,y) (((double)rand()/RAND_MAX)*(y-x)+x)
#include <spatialindex/SpatialIndex.h>
#include "storagemanager/TrajStore.h"
#define sourceFile "D://t50n10ks.txt"
//#define sourceFile "D://t1000.txt"
#define maxLinesToRead 1e10
#define testtime 100
#define dimension 2
#define indexcap 10
#define leafcap 10000
#define QueryType 1
//1 for time-slice range, 2 for 5-NN

using namespace std;
using namespace SpatialIndex;

class MyVisitor : public IVisitor
{
public:
    size_t m_indexIO;
    size_t m_leafIO;
    size_t m_indexvisited;
    size_t m_leafvisited;
    size_t m_resultGet;
    id_type m_lastResult;
    IShape *m_query;
    TrajStore *ts=nullptr;

public:
    MyVisitor() : m_indexIO(0), m_leafIO(0),m_resultGet(0),m_indexvisited(0),m_leafvisited(0) {}

    void visitNode(const INode& n)
    {
//        if (n.isLeaf()) m_leafIO++;
//        else m_indexIO++;
        uint32_t size=n.getIndexByteArraySize();

        if (n.isLeaf()) {m_leafvisited++;m_leafIO+=size;}
        else {m_indexvisited++;m_indexIO+=size;}
    }

    void visitData(const IData& d)
    {
        m_resultGet++;
        IShape* pS;
        d.getShape(&pS);
        // do something.
        delete pS;
//        cout<<"data"<<endl;

        // data should be an array of characters representing a Region as a string.
        uint8_t* pData = 0;
        uint32_t cLen = 0;
        d.getData(cLen, &pData);
        m_leafIO+=cLen;
        // do something.
//        double *s = reinterpret_cast<double*>(pData);
//        cout << *s << endl;

        m_lastResult=d.getIdentifier();


//        //id of the data
//        cerr << d.getIdentifier() << endl;
//        //the traj
//        Trajectory traj;
//        if(ts== nullptr) {
//            traj.loadFromByteArray(pData);
//        }
//        else{
//            id_type tid=d.getIdentifier();
//            traj=ts->getTrajByTime(tid,0,1000);
//        }
//        double mindist=m_query->getMinimumDistance(traj);
//        cerr<<"traj dist is"<<mindist<<"\n";


        delete[] pData;
    }

    void visitData(std::vector<const IData*>& v)
    {
        cout << v[0]->getIdentifier() << " " << v[1]->getIdentifier() << endl;
    }
};


class TrajMbrStream: public IDataStream{
public:
    vector<pair<id_type,Region> > mbrs;
    vector<pair<id_type,Trajectory> > *trajs;

    int i=0;
    void feedTraj(vector<pair<id_type ,Trajectory> > *period){
        cerr<<"feeding traj to TrajMbrStream\n";
        mbrs.clear();
        for(auto &idt:*period){
            Region mbr;
            idt.second.getMBRfull(mbr);
            mbrs.emplace_back(make_pair(idt.first,mbr));
        }
        trajs=period;
        rewind();
    }
    virtual bool hasNext() override
    {
        return i<mbrs.size();
    }
    virtual IData* getNext() override{
        uint8_t *data;
        uint32_t len;
        trajs->at(i).second.storeToByteArray(&data,len);
        RTree::Data* d=new RTree::Data(len, data, mbrs[i].second, mbrs[i].first);
        i++;
        return d;
    }
    virtual uint32_t size()
    {
        return mbrs.size();
    }

    virtual void rewind(){i=0;}
};

struct xyt{
    double x;
    double y;
    double t;
};
xyt makemid(xyt p1, xyt p2, double t){
    if(t>p2.t)
        cout<<p1.x<<" "<<p2.x<<endl<<
            p1.y<<" "<<p2.y<<endl<<
            p1.t<<" "<<p2.t<<" "<<t<<endl;
    assert(p1.t<=t);
    assert(t<=p2.t);
    double h1= (t-p1.t)/(p2.t-p1.t);
    double h2= (p2.t-t)/(p2.t-p1.t);
    double x=h2*p1.x+h1*p2.x;
    double y=h2*p1.y+h1*p2.y;
    xyt ret ={x,y,t};
    return ret;
}

template <class Type>
Type stringToNum(const std::string& str)
{
    std::istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}
vector<pair<id_type ,Trajectory> >  loadGTToTrajs(){
    //first level: vector of time period
    //second level: vector of segments in the time period
    cerr<<"loading generated trajectories from txt to trajectories"<<endl;

    ifstream inFile(sourceFile, ios::in);
    string lineStr;
    set<id_type> ids;
    multimap<id_type,xyt> trajs;
    vector<pair<id_type ,Trajectory> > res;
    int curLine=0;
    double minx=1e300,maxx=-1e300,miny=1e300,maxy=-1e300;
    while (getline(inFile, lineStr)&&curLine<maxLinesToRead){
        try {
            string str;
            stringstream ss(lineStr);
            getline(ss, str, '\t');
            getline(ss, str, '\t');
            int id = stringToNum<int>(str);
            getline(ss, str, '\t');
            getline(ss, str, '\t');
            getline(ss, str, '\t');
            double t = stringToNum<double>(str);
            getline(ss, str, '\t');
            double x = stringToNum<double>(str);
            getline(ss, str, '\t');
            double y = stringToNum<double>(str);
            getline(ss, str, '\t');
            double speed = stringToNum<double>(str);
            xyt p = {x, y, t};
            if (x > maxx) maxx = x;
            if (x < minx) minx = x;
            if (y > maxy) maxy = y;
            if (y < miny) miny = y;
            ids.insert(id);
            trajs.insert(make_pair(id, p));
            curLine++;
        }
        catch(...) {
            break;
        }
    }
    cout<<curLine<<" "<<minx<<" "<<maxx<<" "<<miny<<" "<<maxy<<endl;
    for(auto id:ids){
        multimap<id_type ,xyt>::iterator beg,end,iter;
        vector<xyt> traj;
        beg = trajs.lower_bound(id);
        end = trajs.upper_bound(id);
        for(iter=beg;iter!=end;iter++){
            traj.emplace_back(iter->second);
        }
        trajs.erase(id);
        if(traj.size()>=10){
            vector<STPoint> tps;
            for(auto p:traj){
                double xy[]={p.x,p.y};
                tps.emplace_back(STPoint(xy, p.t, dimension));
            }
            if(!tps.empty()){
                res.emplace_back(make_pair(id,Trajectory(tps)));
            }
        }
    }
    return res;
}

void TreeQueryBatch(ISpatialIndex* tree,const vector<IShape*> &queries,TrajStore *ts= nullptr){
    clock_t start,end;
    MyVisitor vis;
    vis.ts=ts;
    start=clock();
    for(int i=0;i<queries.size();i++){
//        cerr<<"Query is "<<queries.at(i)->toString();
        if(QueryType==1){
            tree->intersectsWithQuery(*queries[i],vis);
        }else if(QueryType==2){
            vis.m_query=queries[i];
            tree->nearestNeighborQuery(5,*queries[i],vis);
//            cerr<<"finished "<<i<<"already\n";
        }
    }
    end=clock();
    cerr<< "=================\n\n";
    cerr<<"Querying time: "<< end-start<<endl;
    cerr<<"VISIT NODE "<<vis.m_indexvisited<<"\t"<<vis.m_leafvisited<<endl;
    cerr<<"Byte loaded "<<vis.m_indexIO<<"\t"<<vis.m_leafIO<<endl;
    cerr << *tree;
    cerr << "================\n\n";
}

int TreeQuery(ISpatialIndex* tree,IShape* query,TrajStore *ts= nullptr){
    clock_t start,end;
    MyVisitor vis;
    if(ts!= nullptr)
        vis.ts=ts;
    vis.m_query=query;
    start=clock();
    if(QueryType==1){
        tree->intersectsWithQuery(*query,vis);
    }else if(QueryType==2){
        vis.m_query=query;
        tree->nearestNeighborQuery(5,*query,vis);
    }
    end=clock();
    if(QueryType==1){
        return vis.m_resultGet;
    }else if(ts!= nullptr){
        return vis.m_lastResult/100;
    }
    else return vis.m_lastResult;
}


int main(){
    try {
        srand((int) time(NULL));
        vector<pair<id_type, Trajectory> > trajs = loadGTToTrajs();
        vector<pair<id_type, vector<Trajectory>>> segs;
        vector<pair<id_type, Trajectory> > empty1;
        for(auto traj:trajs){
            segs.push_back(make_pair(traj.first,traj.second.getSegments(300000)));
        }
        int i;
        vector<IShape *> queries;
//        double plow[3]={16083.3,16481.8,485};
//        double pHigh[3]={17853,17749.1,485};
//        Region* rg=new Region(plow,pHigh,3);
//        queries.push_back(rg);
        int realtesttime=(QueryType==2)?testtime:(100*testtime);
        for (int i = 0; i < realtesttime; i++) {
            if (QueryType == 1) {
                double t = int(random(0, 50));
                double pLow[3] = {random(0, 25000), random(0, 30000), t};
                double pHigh[3] = {pLow[0] + random(500, 2000), pLow[1] + random(500, 2000), t};
                Region *rg = new Region(pLow, pHigh, 3);
                queries.emplace_back(rg);
            }
            else if(QueryType==2){
                queries.emplace_back(&trajs[(457+i)%trajs.size()].second);
            }
        }
        cin>>i;
//        trajs.swap(empty1);
        string name0 = "name0", name1 = "name1", name2 = "name2";
        id_type indexIdentifier0, indexIdentifier1, indexIdentifier2;
        IStorageManager *diskfile0 = StorageManager::createNewDiskStorageManager(name0, 4096),
                *diskfile1 = StorageManager::createNewDiskStorageManager(name1, 4096),
                *diskfile2 = StorageManager::createNewDiskStorageManager(name2, 4096);
        // Create a new storage manager with the provided base name and a 4K page size.
        StorageManager::IBuffer *file0 = StorageManager::createNewRandomEvictionsBuffer(*diskfile0, 10, false),
                *file1 = StorageManager::createNewRandomEvictionsBuffer(*diskfile1, 10, false),
                *file2 = StorageManager::createNewRandomEvictionsBuffer(*diskfile2, 10, false);


        TrajStore ts1(file1,4096);
        ts1.loadSegments(segs);

//        auto query=trajs[457].second;
//        id_type that = 61000;
//        auto traj=ts1.getTrajByTime(that,0,1000);
//        auto br = ts1.getMBR(that);
//        auto bc = ts1.getMBC(that);
//        auto brs = ts1.getMBRsByTime(that,0,1000);
//        auto bcs = ts1.getMBCsByTime(that,0,1000);
//        double a,b,c;
//        a=query.getMinimumDistance(traj);
//        b=query.getMinimumDistance(br);
////        b=query.getPeriodMinimumDistance(br,ts1.m_maxVelocity);
//        c=query.getMinimumDistance(bcs);
//        cout<<query<<traj<<br<<"\n";
//        cout<<"abc is"<<a<<" "<<b<<" "<<c<<endl;


//        if((b>a||c>a||std::isnan(b)||std::isnan(c))&&a!=std::numeric_limits<double>::max()) {
//            cout << queries[0]->getMinimumDistance(trajs[i].second) << endl;
//            cout << queries[0]->getMinimumDistance(brs) << endl;
//            cout << queries[0]->getMinimumDistance(bcs) << endl;
//        }
        TrajStore ts2(file2,4096);
        ts2.loadSegments(segs);


//        TrajMbrStream ds1;
//        ds1.feedTraj(&trajs);
//        ds1.rewind();
//        ISpatialIndex* real = RTree::createAndBulkLoadNewRTree(
//                RTree::BulkLoadMethod::BLM_STR, ds1, *file0, 0.9, 10000,100000, 3,RTree::RV_RSTAR, indexIdentifier0);
//        real->m_DataType=TrajectoryType;


        ISpatialIndex *r=MBCRTree::createAndBulkLoadNewMBCRTreeWithTrajStore(&ts1,32,3,indexIdentifier1);
        ISpatialIndex *rc=SBRTree::createAndBulkLoadNewSBRTreeWithTrajStore(&ts2,32,2,indexIdentifier2);
        cerr << "start query!" << endl << endl << endl;

//        TreeQueryBatch(real,queries);
        TreeQueryBatch(r, queries,&ts1);
        TreeQueryBatch(rc, queries,&ts2);
//        double aa,bb,oo;
//        for(int j=0;j<queries.size();j++){
//            auto q=queries[j];
//            oo=TreeQuery(real,q);
//            aa=TreeQuery(r,q,&ts1);
//            bb=TreeQuery(rc,q,&ts1);
//            Trajectory *qtraj= dynamic_cast<Trajectory*>(q);
//            if(aa!=oo||bb!=oo){
//                cout<<j<<endl;
//                cout<<aa<<endl<<bb<<endl<<oo<<endl;
//                cout<<*qtraj<<endl;
//            }
//        }



        std::cerr<<"index IO:"<<ts1.m_indexIO<<" "<<ts2.m_indexIO<<endl;
        std::cerr<<"traj IO:"<<ts1.m_trajIO<<" "<<ts2.m_trajIO<<endl;
        std::cerr<<"total IO:"<<ts1.m_indexIO+ts1.m_trajIO<<" "<<ts2.m_indexIO+ts2.m_trajIO<<endl;
        std::cerr<<"bounding IO:"<<ts1.m_boundingVisited<<" "<<ts2.m_boundingVisited<<endl;
    }
    catch (Tools::Exception& e)
    {
        cerr << "******ERROR******" << endl;
        std::string s = e.what();
        cerr << s << endl;
        return -1;
    }
    return 0;
}