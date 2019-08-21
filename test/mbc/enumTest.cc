//
// Created by Chuang on 2019/8/19.
//

//
// Created by Chuang on 2019/5/21.
//

//
// Created by chuang on 4/9/19.
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
#include "../../src/storagemanager/DiskStorageManager.h"
#include "../../src/mbcrtree/MBCRTree.h"
//#define sourceFile "D://t1000.txt"
#define genFile "D://t500n200se.txt"
#define GLFile "/root/GLS.csv"
#define maxLinesToRead 1e10
#define testtime 100
#define dimension 2
#define indexcap 10
#define leafcap 10000
#define QueryType 2
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
//        auto mou=dynamic_cast<const MBCRTree::MBCRTree::simpleData*>(&d);
//        cerr << d.getIdentifier()<<"\t"<<mou->m_dist << endl;
//        //id of the data
//        if(ts== nullptr)
//            cerr << d.getIdentifier() << endl;
//        else
//            cerr << d.getIdentifier() /100<< endl;
//        //the traj
//        Trajectory traj;
//        if(ts== nullptr) {
//            traj.loadFromByteArray(pData);
//            double mindist=m_query->getMinimumDistance(traj);
//            cerr<<"traj dist is"<<mindist<<"\n\n";
//        }
//        else{
//            id_type tid=d.getIdentifier();
//            traj=ts->getTrajByTime(tid,0,1000);
//            auto brs=ts->getMBRsByTime(tid,0,1000);
//            auto bcs=ts->getMBCsByTime(tid,0,1000);
//            cerr<<"traj dist is"<<m_query->getMinimumDistance(traj)<<"\n"
//                    <<m_query->getMinimumDistance(brs)<<"\n"
//                    <<m_query->getMinimumDistance(bcs)<<"\n\n";
//        }

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
#ifndef NDEBUG
    cerr<<"loading generated trajectories from txt to trajectories"<<endl;
#endif
    ifstream inFile(genFile, ios::in);
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
    cerr<<curLine<<" "<<minx<<" "<<maxx<<" "<<miny<<" "<<maxy<<endl;
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
#ifndef NDEBUG
    std::cerr<<"load data finished\n";
#endif
    return res;
}

vector<pair<id_type ,Trajectory> >  loadGLToTrajs(){
    //first level: vector of time period
    //second level: vector of segments in the time period
    cerr<<"loading geolife trajectories from txt to trajectories"<<endl;

    ifstream inFile(GLFile, ios::in);
    string lineStr;
    set<id_type> ids;
    multimap<id_type,xyt> trajs;
    vector<pair<id_type ,Trajectory> > res;
    int curLine=0;
    double minx=1e300,maxx=-1e300,miny=1e300,maxy=-1e300;
    getline(inFile, lineStr);
    while (getline(inFile, lineStr)&&curLine<maxLinesToRead){
        try {
            string str;
            stringstream ss(lineStr);
            getline(ss, str, ',');
            int id= stringToNum<int>(str);
            getline(ss, str, ',');
            double x= stringToNum<double>(str);
            getline(ss, str, ',');
            double y= stringToNum<double>(str);
            getline(ss, str, ',');
            double t= stringToNum<double>(str);
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
    cerr<<curLine<<" "<<minx<<" "<<maxx<<" "<<miny<<" "<<maxy<<endl;
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
#ifndef NDEBUG
    std::cerr<<"load data finished\n";
#endif
    return res;
}


void TreeQueryBatch(ISpatialIndex* tree,const vector<IShape*> &queries,TrajStore *ts= nullptr,int thennk=5){
    MyVisitor vis;
    vis.ts=ts;
    auto start = std::chrono::system_clock::now();
    for(int i=0;i<queries.size();i++){
//        cerr<<"Query is "<<queries.at(i)->toString();
        if(QueryType==1){
            tree->intersectsWithQuery(*queries[i],vis);
        }else if(QueryType==2){
            vis.m_query=queries[i];
            tree->nearestNeighborQuery(thennk,*queries[i],vis);
//            cerr<<"finished "<<i<<"already\n";
        }
    }
    double time;
    auto end = std::chrono::system_clock::now();auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    time=double(duration.count()) * std::chrono::microseconds::period::num/ std::chrono::microseconds::period::den;
    cerr <<"Querying time: "<< time<<endl;
    cerr <<"VISIT NODE "<<vis.m_indexvisited<<"\t"<<vis.m_leafvisited<<endl;
    cerr <<"TrajStore Statistic"<< ts->m_indexIO<<"\t"<<ts->m_trajIO<<endl;
}

void kNNQueryBatch(ISpatialIndex* tree,const vector<IShape*> &queries,TrajStore *ts= nullptr,int thennk=5){
    ts->cleanStatistic();
    int num=queries.size();
    MyVisitor vis;
    vis.ts=ts;
    auto start = std::chrono::system_clock::now();
    for(int i=0;i<queries.size();i++){
        vis.m_query=queries[i];
        tree->nearestNeighborQuery(thennk,*queries[i],vis);
    }
    double time;
    auto end = std::chrono::system_clock::now();auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    time=double(duration.count()) * std::chrono::microseconds::period::num/ std::chrono::microseconds::period::den;
//    cerr <<"Average Querying time: "<< time/num<<endl;
//    cerr <<"Averaged VISIT NODE "<<1.0*vis.m_indexvisited/num<<"\t"<<1.0*vis.m_leafvisited/num<<endl;
//    cerr <<"TrajStore Statistic"<< 1.0*ts->m_indexIO/num<<"\t"<<1.0*ts->m_trajIO/num<<endl;
//    cerr <<time/num<<"\t"<<1.0*vis.m_indexvisited/num<<"\t"<<1.0*vis.m_leafvisited/num<< 1.0*ts->m_indexIO/num<<"\t"<<1.0*ts->m_trajIO/num<<endl;
    cerr <<time/num<<"\n";
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
        return vis.m_lastResult;
    }
    else return vis.m_lastResult;
}


int main(){
    try {
        calcuTime[0] = 0;
        srand((int) time(NULL));
        vector<pair<id_type, Trajectory> > trajs = loadGLToTrajs();
        vector<pair<id_type, vector<Trajectory>>> segs;
        vector<pair<id_type, vector<Trajectory>>> emptyseg;
        int totallen = 0, totalseg = 0;
        int maxseg = 0;
        double avgSegLen=100;
        for (double segpara = 0.1; avgSegLen>10 ; segpara/=2) {
            maxseg=0;
            segs.clear();
            emptyseg.clear();
            for (auto &traj:trajs) {
                totallen += traj.second.m_points.size();
                auto seg = traj.second.getSegments(segpara);
                totalseg += seg.size();
                maxseg = std::max(int(seg.size()), maxseg);
                segs.emplace_back(make_pair(traj.first, seg));
            }
            avgSegLen=double(totallen)/totalseg;
            std::cerr<<"segments' average length is "<<totallen*1.0/totalseg<<"\n";

            string name0 =std::to_string(segpara)+ "name0", name1 = std::to_string(segpara)+"name1", name2 = std::to_string(segpara)+"name2";
            id_type indexIdentifier0, indexIdentifier1, indexIdentifier2;
            IStorageManager *diskfile0 = StorageManager::createNewDiskStorageManager(name0, 4096),
                    *diskfile1 = StorageManager::createNewDiskStorageManager(name1, 4096),
                    *diskfile2 = StorageManager::createNewDiskStorageManager(name2, 4096);
            // Create a new storage manager with the provided base name and a 4K page size.
            StorageManager::IBuffer *file0 = StorageManager::createNewRandomEvictionsBuffer(*diskfile0, 100, false),
                    *file1 = StorageManager::createNewRandomEvictionsBuffer(*diskfile1, 10, false),
                    *file2 = StorageManager::createNewRandomEvictionsBuffer(*diskfile2, 10, false);

            TrajStore *ts1 = new TrajStore(file1, 4096, maxseg+1);
            ts1->loadSegments(segs);
            ISpatialIndex *r = MBCRTree::createAndBulkLoadNewRTreeWithTrajStore(ts1, 4096, 3, indexIdentifier1);

            TrajStore *ts2 = new TrajStore(file2, 4096, maxseg+1);
            ts2->loadSegments(segs);
            ISpatialIndex *rc = MBCRTree::createAndBulkLoadNewMBCRTreeWithTrajStore(ts2, 4096, 3, indexIdentifier2);

            //kNN
            segs.swap(emptyseg);
            vector<IShape *> queries;
            double segattri[]={900,3600,1000000};
            for (auto queryLen:segattri) {
//                for(int thek=1;thek<=21;thek+=5){
                for (int thek = 5; thek == 5; thek++) {
                    for (int i = 0; i < 200; i++) {
                        auto ori = &trajs[(int(random(0, trajs.size()))) % trajs.size()].second;
                        Trajectory *concate = new Trajectory();
                        double ts = random(ori->m_startTime(), ori->m_endTime() - queryLen);
                        ori->getPartialTrajectory(ts, ts + queryLen, *concate);
                        if (!concate->m_points.empty())
                            queries.emplace_back(concate);
                    }
//                    cerr << "=================\n\n";
//                    std::cerr << "Querying with segmenting len " << seglen <<
//                              ", querying len " << queryLen << ", NN's k" << thek << "\n";
                    cerr<< segpara<<"\t"<<queryLen<<"\n";
                    kNNQueryBatch(r, queries, ts1);
                    kNNQueryBatch(rc, queries, ts2);
                    cerr << "\n";
                    for(auto &tras:queries){
                        delete tras;
                    }
                    queries.clear();
                }
            }
            delete r;
            delete ts1;
            delete rc;
            delete ts2;
            delete file0;
            delete file1;
            delete file2;
            delete diskfile0;
            delete diskfile1;
            delete diskfile2;
        }
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