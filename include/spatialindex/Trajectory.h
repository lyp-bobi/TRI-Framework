//
// Created by chuang on 4/23/19.
//


#pragma once

#include "ShapeList.h"

extern double calcuTime[2];
extern int testPhase;
extern int disttype;

#define bip auto start = std::chrono::system_clock::now();
#define bbip auto end = std::chrono::system_clock::now();auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);calcuTime[testPhase]+=double(duration.count()) * std::chrono::microseconds::period::num/ std::chrono::microseconds::period::den;

namespace SpatialIndex
{
    class SIDX_DLL Trajectory: public Tools::IObject, public virtual IShape{

        public:
        class fakeTpVector{
        public:
            int m_is=0;
            int m_size=0;
            STPoint *m_front= nullptr;
            STPoint *m_back = nullptr;
            std::vector<STPoint> *m_vectorPointer;
            ~fakeTpVector(){
                delete(m_front);
                delete(m_back);
            }
            fakeTpVector(const std::vector<STPoint> *vector,double ts,double te)
            {
                m_vectorPointer= const_cast<std::vector<STPoint>*>(vector);
                if(ts==te) return;
                if(ts>m_vectorPointer->back().m_time||te<m_vectorPointer->front().m_time) return;
                m_is=0;
                int m_ie=m_vectorPointer->size()-1;
                while(m_vectorPointer->at(m_is).m_time<ts) m_is++;
                while(m_vectorPointer->at(m_ie).m_time>te) m_ie--;
                if(m_is!=0&&m_vectorPointer->at(m_is).m_time!=ts){
                    double x=makemidmacro(m_vectorPointer->at(m_is-1).m_pCoords[0],m_vectorPointer->at(m_is-1).m_time,
                                          m_vectorPointer->at(m_is).m_pCoords[0],m_vectorPointer->at(m_is).m_time,ts);
                    double y=makemidmacro(m_vectorPointer->at(m_is-1).m_pCoords[1],m_vectorPointer->at(m_is-1).m_time,
                                          m_vectorPointer->at(m_is).m_pCoords[1],m_vectorPointer->at(m_is).m_time,ts);
                    double xy[2]={x,y};
                    m_front=new STPoint(xy,ts,2);
                    m_is--;
                }
                if(m_ie!=m_vectorPointer->size()-1&&m_vectorPointer->at(m_ie).m_time!=te){
                    double x=makemidmacro(m_vectorPointer->at(m_ie).m_pCoords[0],m_vectorPointer->at(m_ie).m_time,
                                          m_vectorPointer->at(m_ie+1).m_pCoords[0],m_vectorPointer->at(m_ie+1).m_time,ts);
                    double y=makemidmacro(m_vectorPointer->at(m_ie).m_pCoords[1],m_vectorPointer->at(m_ie).m_time,
                                          m_vectorPointer->at(m_ie+1).m_pCoords[1],m_vectorPointer->at(m_ie+1).m_time,ts);
                    double xy[2]={x,y};
                    m_back=new STPoint(xy,te,2);
                    m_ie++;
                }
                m_size=m_ie-m_is+1;
            }
            STPoint& operator[](int n){
                if(n==0&&m_front!= nullptr) return *m_front;
                if(n==m_is+m_size-1&&m_back!= nullptr) return *m_back;
                return m_vectorPointer->at(m_is+n);
            }
        };
        Trajectory();
        explicit Trajectory(std::vector<STPoint> &in);
        Trajectory(const Trajectory& in);
        Trajectory &operator=(const Trajectory &r);

        virtual bool operator==(const Trajectory &r) const;


        //
        // IObject interface
        //
        virtual Trajectory *clone();

        //
        // ISerializable interface
        //
        virtual uint32_t getByteArraySize() const;

        virtual void loadFromByteArray(const uint8_t *data);

        virtual void storeToByteArray(uint8_t **data, uint32_t &len);



        //
        // IShape interface
        //
        virtual bool intersectsShape(const IShape& in) const;
        virtual bool containsShape(const IShape& in) const;
        virtual bool touchesShape(const IShape& in) const;
        virtual void getCenter(Point& out) const;
        virtual uint32_t getDimension() const;
        virtual void getMBR(Region& out) const;
        virtual double getArea() const;
        virtual double getMinimumDistance(const IShape& in) const;
        virtual double getMinimumDistance(const STPoint& in) const;
        virtual double getMinimumDistance(const Region& in) const;
        virtual double getMinimumDistance(const MBC& in) const;
        virtual double getMinimumDistance(const SBR& in) const;
        virtual double getMinimumDistance(const Trajectory& in) const;
        virtual double getMinimumDistance(const ShapeList& in) const;
        virtual double getMinimumDistance(double x,double y, double t1,double t2) const;


        virtual double getPeriodMinimumDistance(const Region& in,double MaxVelocity) const;
        virtual double getPeriodMinimumDistance(const MBC& in,double MaxVelocity) const;

        virtual double minSED(const Region& in) const;
        virtual double minSED(const MBC& in) const;


        virtual bool intersectsTimeRegion(const TimeRegion& in) const;
        virtual bool intersectsRegion(const Region& in) const;
        virtual bool intersectsTrajectory(const Trajectory& in) const;

        virtual void combineTrajectory(const Trajectory& in);
        virtual bool containsTrajectory(const Trajectory& in);
        virtual void getCombinedTrajectory(Trajectory& out, const Trajectory& in) const;

        virtual void getMBC(MBC& out) const;
        virtual void getMBRfull(Region& out) const;
        virtual void getTimeMBR(TimeRegion& out) const;
        STPoint getPointAtTime(double time) const;
        static std::vector<SpatialIndex::STPoint> simplifyWithRDP(std::vector<SpatialIndex::STPoint>& Points, double threshold);
        std::vector<Trajectory> cuttraj(std::vector<SpatialIndex::STPoint>);
        std::vector<Trajectory> getSegments(double threshold);
        void linkTrajectory(Trajectory other);

        double m_startTime() const{return m_points.front().m_time;}
        double m_endTime() const { return m_points.back().m_time;}

        void loadFromString(std::string s);

        static double line2lineIED(const STPoint &p1s, const STPoint &p1e, const STPoint &p2s, const STPoint &p2e);
        static double line2MBRDistance(const STPoint &ps,const STPoint &pe,const Region &r);
        static double line2MBRIED_impl(const STPoint &ps, const STPoint &pe, const Region &r, int sr);
        static double line2MBCDistance(const STPoint &ps,const STPoint &pe,const MBC &r);

        std::vector<STPoint> m_points;

        friend SIDX_DLL std::ostream& operator<<(std::ostream& os,const Trajectory &r);

        static const uint32_t m_dimension=2;
        virtual void makeInfinite(uint32_t dimension);
    private:
};
typedef Tools::PoolPointer<Trajectory> TrajectoryPtr;
SIDX_DLL std::ostream& operator<<(std::ostream& os, const Trajectory& r);
}
