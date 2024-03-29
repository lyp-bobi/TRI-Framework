//
// Created by chuang on 4/23/19.
//


#pragma once
#ifndef SPATIALINDEX_XTRAJ_H
#define SPATIALINDEX_XTRAJ_H
#include <cmath>
#include "storagemanager/xStore.h"
#define subTrajFile "./subTrajFile.stj"

#define random(x, y) (rand()%(y-x+1)+x)

#define bip auto start = std::chrono::system_clock::now();
#define bbip auto end = std::chrono::system_clock::now();auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);calcuTime[testPhase]+=double(duration.count()) * std::chrono::microseconds::period::num/ std::chrono::microseconds::period::den;
using std::vector;
using std::pair;
using std::string;
using namespace SpatialIndex;

extern double splitSoftThres;
extern thread_local double dist_sense_thres;
extern thread_local double global_rdpn_ed;
extern int rmdtwprecision;
//#define TJDEBUG

enum supported_distance{
    IED = 0,
    RMDTW = 1,
    SDDTW = 2
};
extern supported_distance current_distance;


namespace SpatialIndex
{
    struct LOCALE{
        uint32_t prev;
        double place;
        LOCALE(uint32_t a, double b)
                :prev(a), place(b){}
    };
    static inline LOCALE locateTime(std::vector<xPoint> &mp,double t) {
        int low = 0;
        int high = mp.size()-1;
        int mid;
        if(t<=mp.front().m_t) return LOCALE(0,0);
        if(t>= mp.back().m_t) return LOCALE(mp.size()-1,0);
        while(low<=high){
            mid = (low+high)/2;
            if(mp[mid].m_t>t)
                high=mid-1;
            else if(mp[mid].m_t<t)
                low=mid+1;
            else{
                return LOCALE(mid,0);
            }
        }
        low -=1;high+=1;
        return LOCALE(low,(t-mp[low].m_t)/(mp[high].m_t - mp[low].m_t));
    }
    static inline xPoint* pos(std::vector<xPoint> &mp, const LOCALE &p, double t){
        double x = (1-p.place) * mp[p.prev].m_x + (p.place) * mp[p.prev+1].m_x;
        double y = (1-p.place) * mp[p.prev].m_y + (p.place) * mp[p.prev+1].m_y;
        return new xPoint(x,y,t);
    }

    class SIDX_DLL xTrajectory: public Tools::IObject, public virtual IxShape{

        public:
        class fakeTpVector{
        public:
            int m_is=0;
            int m_size=0;
            xPoint *m_front= nullptr;
            xPoint *m_back = nullptr;
            std::vector<xPoint> *m_vectorPointer;
            ~fakeTpVector(){
                if(m_front) delete(m_front);
                if(m_back) delete(m_back);
            }
            fakeTpVector(const std::vector<xPoint> *vector,double ts,double te)
            {
                m_vectorPointer= const_cast<std::vector<xPoint>*>(vector);
                if(ts==te) return;
                if(ts>m_vectorPointer->back().m_t || te < m_vectorPointer->front().m_t) return;
                m_is=0;
                int m_ie;
                LOCALE s = locateTime(*m_vectorPointer, ts);
                LOCALE e = locateTime(*m_vectorPointer, te);
                if(s.place == 0){
                    m_is = s.prev;
                }else{
                    m_is = s.prev;
                    m_front = pos(*m_vectorPointer,s,ts);
                }
                if(e.place ==0){
                    m_ie = e.prev;
                }else{
                    m_ie = e.prev+1;
                    m_back = pos(*m_vectorPointer,e,te);
                }
                m_size=m_ie-m_is+1;
            }
            inline xPoint& operator[](int n){
                if(n==0&&m_front!= nullptr) return *m_front;
                if(n==m_size-1&&m_back!= nullptr) return *m_back;
                return (*m_vectorPointer)[m_is+n];
            }
            inline xPoint& front(){
                if(m_front!= nullptr) return *m_front;
                else return (*m_vectorPointer)[m_is];
            }
            inline xPoint& back(){
                if(m_back!= nullptr) return *m_back;
                return (*m_vectorPointer)[m_is+m_size-1];
            }
        };
        xTrajectory();
        explicit xTrajectory(std::vector<xPoint> &in);
        explicit xTrajectory(bool fakehead,bool fakeback,std::vector<xPoint> &in);
        xTrajectory(const xTrajectory& in);
        xTrajectory &operator=(const xTrajectory &r);

        virtual bool operator==(const xTrajectory &r) const;


        //
        // IObject interface
        //
        virtual xTrajectory *clone();

        //
        // ISerializable interface
        //
        virtual uint32_t getByteArraySize() const;

        virtual void loadFromByteArray(const uint8_t *data);

        virtual void storeToByteArray(uint8_t **data, uint32_t &len);

        virtual void storeToByteArrayE(uint8_t** data, uint32_t& len);



        //
        // IShape interface
        //
        virtual bool intersectsShape(const IShape& in) const;
        virtual bool containsShape(const IShape& in) const;
        virtual bool touchesShape(const IShape& in) const;
        virtual void getCenter(Point& out) const;
        virtual uint32_t getDimension() const;
        virtual void getxMBR(xMBR& out) const;

        virtual void getPartialxTrajectory(double tstart, double tend, SpatialIndex::xTrajectory &out) const;

        virtual double getArea() const;
        virtual double getMinimumDistance(const IShape& in) const;

        static double line2lineIED(const xPoint &p1s, const xPoint &p1e, const xPoint &p2s, const xPoint &p2e);
        static double line2lineMinSED(const xPoint &p1s, const xPoint &p1e, const xPoint &p2s, const xPoint &p2e);
        static DISTE line2MBRIED(const xPoint &ps, const xPoint &pe, const xMBR &r);
        static double line2MBRIED_impl(const xPoint &ps, const xPoint &pe, const xMBR &r, int sr);
        static double line2MBRMinSED(const xPoint &ps, const xPoint &pe, const xMBR &r);
        static double line2MBRMinSED_impl(const xPoint &ps, const xPoint &pe, const xMBR &r, int sr);
        static DISTE line2MBCIED(const xPoint &ps, const xPoint &pe, const xMBC &r);
        static DISTE line2MBLIED(const xPoint &ps, const xPoint &pe, const xLine &r);

        double getStaticDistance(double x, double y, double t1, double t2) const;
        double getStaticDistance(SpatialIndex::xMBR in, double ints, double inte) const;
        double getMinimumDistance(const SpatialIndex::xTrajectory &in) const;
        DISTE getPartialRMDTW(const SpatialIndex::xTrajectory &parttraj) const;
        DISTE getRMDTW(std::vector<std::pair<xPoint, double>> &cross) const;

        int nPointDuring(double tmin, double tmax) const;
        double nodeDist(const xSBB &b) const;
        DISTE sbbDist(const xSBB &b) const;
        DISTE frontDist(const xSBB &b, double vmax) const;
        DISTE backDist(const xSBB &b, double vmax) const;
        DISTE gapDist(const xSBB &prev,const xSBB &next, double vmax) const;
        DISTE frontDistStatic(const xSBB &b) const;
        DISTE backDistStatic(const xSBB &b) const;

        DISTE gapDist(const xPoint &prev,const xPoint &next, double vmax) const;
        DISTE frontDistStatic(const xPoint &b) const;
        DISTE backDistStatic(const xPoint &b) const;


        virtual bool intersectsxMBR(const xMBR& in) const;
        virtual bool intersectsxCylinder(const xCylinder& in) const;

        virtual void getxMBC(xMBC& out) const;
        xPoint getPointAtTime(double time) const;
        xTrajectory resampleQuery(double ts, double te, int nsegment) const;
        static std::vector<std::vector<SpatialIndex::xPoint>> simplifyWithRDPN(const std::vector<SpatialIndex::xPoint>& Points, int numPart);
        static std::vector<SpatialIndex::xPoint> simplifyWithRDP(const std::vector<SpatialIndex::xPoint>& Points, double threshold);
        std::vector<xTrajectory> cuttraj(std::vector<SpatialIndex::xPoint>);


        std::vector<xTrajectory> getSegments(double len) const;
        std::vector<xTrajectory> getHybridSegments(double len) const;
        std::vector<xTrajectory> getRDPSegments(double len) const;
        std::vector<xTrajectory> getStaticSegments(double len) const;
        std::vector<xTrajectory> getStaticSegmentsCut(double len) const;
        std::vector<xTrajectory> getFixedSegments(int len=169) const;
        std::vector<xTrajectory> getGlobalSegmentsCut(double len) const;
        std::vector<xTrajectory> getItself() const;

        int locate_time_cut(double t, int dir);
        static queue<CUTENTRY> ISS(xTrajectory &traj, double len);
        static queue<CUTENTRY> GSS(xTrajectory &traj, double len);
        static queue<CUTENTRY> OPTS(xTrajectory &traj, double len1);
        static queue<CUTENTRY> FP(xTrajectory &traj, double np);
        static queue<CUTENTRY> RDP(xTrajectory &traj, double len);
        static queue<CUTENTRY> EveryLine(xTrajectory &traj);
        static queue<CUTENTRY> OneBox(xTrajectory &traj, double len);

        void linkxTrajectory(xTrajectory &other);

        double maxSpeed() const;

        static int cutTrajsIntoFile(std::vector<std::pair<id_type, xTrajectory> > &trajs,double segLen, int strat=0,std::string filename=subTrajFile);
        class subTrajStream{
        public:
            std::ifstream inFile;
            string nextline="";
            subTrajStream(std::string str=subTrajFile){
                inFile.open(str,std::ios::in);
            }
            bool hasNext(){
                return nextline!="END";
            }
            pair<id_type,vector<xTrajectory>> getNext(){
                id_type id;
                vector<xTrajectory> v;
                if(!hasNext()){
                    return std::make_pair(id,v);
                }
                std::string s;
                s=nextline;
                xTrajectory tmp;
                bool isId=false;
                while(true){
                    if(s=="SubTraj"){
                        isId=true;
                    }
                    else if(s=="ESubTraj"){
                        std::getline(inFile,s);
                        break;
                    }
                    else if(isId){
                        id=std::stoll(s);
                        isId=false;
                    }
                    else if(!s.empty()){
                        tmp.loadFromString(s);
                        v.emplace_back(tmp);
                    } else{
                        if(!s.empty()) std::cout<<"some err";
                    }
                    std::getline(inFile,s);
                }
                nextline=s;
                return std::make_pair(id,v);
            }
            ~subTrajStream(){
                inFile.close();
            }
        };
        
        inline double m_startTime() const{return m_points.front().m_t;}
        inline double m_endTime() const { return m_points.back().m_t;}
        inline double m_dist() const{
            double res=0;
            for(int i=1;i<m_points.size();i++){
                res += std::sqrt(sq(m_points[i].m_x-m_points[i-1].m_x)
                        +sq(m_points[i].m_y-m_points[i-1].m_y));
            }
            return res;
        }

        xPoint randomPoint(){
            int i = int(random(0,m_points.size()-1));
            return m_points[i];
        }

        std::string toString() const ;
        void loadFromString(std::string s);

//        static inline int getPhase(const SpatialIndex::xMBR &r,const xPoint &p1,const xPoint &p2);
        static std::vector<std::pair<xPoint,xPoint>> cutByPhase(const SpatialIndex::xPoint &ps, const SpatialIndex::xPoint &pe,
                                                                  const SpatialIndex::xMBR &r);

        std::vector<xPoint> m_points;
        bool m_fakehead=false,m_fakeback=false;

        friend SIDX_DLL std::ostream& operator<<(std::ostream& os,const xTrajectory &r);

        static const uint32_t m_dimension=2;
        virtual void makeInfinite(uint32_t dimension);
    private:
};
typedef Tools::PoolPointer<xTrajectory> xTrajectoryPtr;
SIDX_DLL std::ostream& operator<<(std::ostream& os, const xTrajectory& r);
}


static inline int getPhase(const SpatialIndex::xMBR &r, const xPoint &p1, const xPoint &p2) {
    // 7 8 9
    // 4 5 6
    // 1 2 3
    double xd1=r.m_xmin,xd2=r.m_xmax,yd1=r.m_ymin,yd2=r.m_ymax;
    int res=0;
    if(p1.m_x<=xd2+1e-7&&p2.m_x<=xd2+1e-7&&p1.m_x>=xd1-1e-7&&p2.m_x>=xd1-1e-7)
        res+=2;
    else if(p1.m_x<=xd1+1e-7&&p2.m_x<=xd1+1e-7)
        res+=1;
    else if(p1.m_x>=xd2-1e-7&&p2.m_x>=xd2-1e-7)
        res+=3;
    else return -1;
    if(p1.m_y<=yd2+1e-7&&p2.m_y<=yd2+1e-7&&p1.m_y>=yd1-1e-7&&p2.m_y>=yd1-1e-7)
        res+=3;
    else if(p1.m_y<=yd1+1e-7&&p2.m_y<=yd1+1e-7)
        res+=0;
    else if(p1.m_y>=yd2-1e-7&&p2.m_y>=yd2-1e-7)
        res+=6;
    else return -1;
    return res;
}
#endif //SPATIALINDEX_XTRAJ_H