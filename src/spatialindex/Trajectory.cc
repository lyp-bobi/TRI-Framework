//
// Created by chuang on 4/23/19.
//

#include <cstring>
#include <cmath>
#include <limits>
#include <algorithm>
#include <string>

#include <spatialindex/SpatialIndex.h>

using namespace SpatialIndex;
using std::vector;
using std::cout;
using std::endl;
using std::sqrt;

Trajectory::Trajectory() {
}
Trajectory::Trajectory(std::vector<SpatialIndex::TimePoint>& in) {
    m_points=in;
}

Trajectory::Trajectory(const SpatialIndex::Trajectory &in) {
    m_points=in.m_points;
}

Trajectory& Trajectory::operator=(const Trajectory& r)
{
    if(this != &r)
    {
        m_points=r.m_points;
    }

    return *this;
}

bool Trajectory::operator==(const SpatialIndex::Trajectory &r) const {
    if (m_points==r.m_points)
        return true;
    return false;
}
//
// IObject interface
//
Trajectory* Trajectory::clone() {
    return new Trajectory(*this);
}
//
// ISerializable interface
//
uint32_t Trajectory::getByteArraySize() const {
    if(m_points.size()<=0) throw Tools::IllegalStateException("traj with length 0!");
    return sizeof(unsigned long)+m_points[0].getByteArraySize()*m_points.size();
}

void Trajectory::loadFromByteArray(const uint8_t* ptr) {
    unsigned long size;
    memcpy(&size, ptr, sizeof(unsigned long));
    ptr += sizeof(unsigned long);
    std::vector<TimePoint> p(size);
    for(int i=0;i<size;i++){
        p[i].loadFromByteArray(ptr);
        if(i!=size-1){
            ptr+=p[i].getByteArraySize();
        }
    }
    m_points.clear();
    m_points=p;
}

void Trajectory::storeToByteArray(uint8_t **data, uint32_t &len) {
    len = getByteArraySize();
    *data = new uint8_t[len];
    uint8_t* ptr = *data;
    uint8_t* tmpb;
    uint32_t tmplen;
    unsigned long size=m_points.size();
    memcpy(ptr, &size, sizeof(unsigned long));
    ptr += sizeof(unsigned long);
    for(int i=0;i<size;i++){
        m_points[i].storeToByteArray(&tmpb,tmplen);
        memcpy(ptr, tmpb, tmplen);
        if(i!=size-1){
            ptr += tmplen;
        }
    }

    assert(len==(ptr - *data)+tmplen);
}


//
// IShape interface
//
bool Trajectory::intersectsShape(const SpatialIndex::IShape& s) const {
    const Trajectory* pTrajectory = dynamic_cast<const Trajectory*>(&s);
    if (pTrajectory != 0) return intersectsTrajectory(*pTrajectory);

    const TimeRegion* ptr = dynamic_cast<const TimeRegion*>(&s);
    if (ptr != 0) return intersectsTimeRegion(*ptr);


    const Region* pr = dynamic_cast<const Region*>(&s);
    if (pr != 0) return intersectsRegion(*pr);

    throw Tools::IllegalStateException(
            "Trajectory::intersectsShape: Not implemented yet!"
    );
}

TimePoint Trajectory::getPointAtTime(const double time) const {
    if(time<m_points.front().m_startTime||time>m_points.back().m_endTime){
        throw Tools::IllegalArgumentException(
                "Trajectory::getPointAtTime: time"+std::to_string(time)+"is illegal."
                );}
    if(m_points.size()==1){
        return m_points[0];
    }
    auto pre =m_points.begin(),next=m_points.begin();
    while(next->m_startTime-pre->m_startTime<0.01) next++;
    while(next->m_startTime<time&&next!=m_points.end()){
        pre=next;
        while(next->m_startTime-pre->m_startTime<0.01) next++;
    }
    double h1= (time-pre->m_startTime)/(next->m_startTime-pre->m_startTime);
    double h2= (next->m_startTime-time)/(next->m_startTime-pre->m_startTime);
    double *coords= new double[m_dimension];
    for (int i = 0; i < m_dimension; ++i) {
        coords[i]=h2*pre->m_pCoords[i]+h1*next->m_pCoords[i];
    }
    return TimePoint(coords,time,time,m_dimension);
}

bool Trajectory::intersectsTimeRegion(const SpatialIndex::TimeRegion &in) const {
    if(in.m_startTime==in.m_endTime){//time slice
        if(in.m_startTime<m_points.front().m_startTime||in.m_startTime>m_points.back().m_endTime){
            return false;
        }
        TimePoint tp=getPointAtTime(in.m_startTime);
        return tp.intersectsShape(in);
    }else{
        throw Tools::NotSupportedException("time interval range not supported");
    }
}
bool Trajectory::intersectsRegion(const Region& in) const{
    if(m_dimension==in.m_dimension-1){
        Region spatial(in.m_pLow,in.m_pHigh,m_dimension);
        if(in.m_pHigh[m_dimension]<m_points.front().m_startTime||in.m_pLow[m_dimension]>m_points.back().m_endTime){
            return false;
        }
        TimePoint tp=getPointAtTime(in.m_pLow[m_dimension]);
        return tp.intersectsShape(spatial);
    }
    else if(m_dimension==in.m_dimension) {
        for (int i = 0; i < m_points.size(); i++) {
            if (m_points[i].intersectsShape(in)) {
                return true;
            }
        }
    }
    return false;
}

bool Trajectory::intersectsTrajectory(const Trajectory& in) const{
    throw Tools::NotSupportedException(
            "Trajectory:::getMinimumDistance Not implemented yet!"
    );
}

bool Trajectory::containsShape(const SpatialIndex::IShape& in) const{
    throw Tools::NotSupportedException(
            "Trajectory::...: Not implemented yet!"
    );
}
bool Trajectory::touchesShape(const SpatialIndex::IShape& in) const{
    throw Tools::NotSupportedException(
            "Trajectory::...: Not implemented yet!"
    );
}
void Trajectory::getCenter(Point& out) const{
    throw Tools::NotSupportedException("not supported now");
}
uint32_t Trajectory::getDimension() const{return 2;}
void Trajectory::getMBR(Region& out) const{
    out.makeInfinite(m_dimension);
    for(int i=0;i<m_points.size();i++){
        out.combinePoint(m_points[i]);
    }
}


void Trajectory::getMBC(SpatialIndex::MBC &out) const {
    if(m_points.size()<=1){
        std::cerr<<"WARNING: getting MBC at a Trajectory with 0 or 1 points\n";
        out.makeInfinite(m_dimension+1);
        return;
    }
    double startx=m_points[0].m_pCoords[0],starty=m_points[0].m_pCoords[1],startt=m_points[0].m_startTime;
    double endx=m_points.back().m_pCoords[0],endy=m_points.back().m_pCoords[1],endt=m_points.back().m_startTime;
    double avx=(endx-startx)/(endt-startt),avy=(endy-starty)/(endt-startt);
    TimePoint p1=*m_points.begin(),p2=m_points.back();
    double rd=0,rv=0;
    double vx,vy;
    for(int i=0;i<m_points.size();i++){
        TimePoint p;
        double ptime=m_points[i].m_startTime;
        double prd,prv;
        if(ptime-startt>0){
            vx=(m_points[i].m_pCoords[0]-startx)/(ptime-startt);
            vy=(m_points[i].m_pCoords[1]-starty)/(ptime-startt);
            prv=std::sqrt((vx-avx)*(vx-avx)+(vy-avy)*(vy-avy));
            if(prv>rv) rv=prv;
        }
        if(endt-m_points[i].m_startTime>0) {
            vx = (endx - m_points[i].m_pCoords[0]) / (endt - ptime);
            vy = (endy - m_points[i].m_pCoords[1]) / (endt - ptime);
            prv=std::sqrt((vx-avx)*(vx-avx)+(vy-avy)*(vy-avy));
            if(prv>rv) rv=prv;
        }
        p=TimePoint::makemid(p1,p2,ptime);
        prd=m_points[i].getMinimumDistance(p);
//        cout<<p<<endl<<prd<<endl;
//        std::cout<<"calculated point\n"<<p.toString()<<endl<<"real point\n"<<m_points[i].toString()<<endl<<prd<<endl<<"\n\n\n\n";
        if(prd>rd) rd=prd;
    }
    out=MBC(p1.m_pCoords,p2.m_pCoords,startt,endt,m_dimension,rd,rv);
}

void Trajectory::getMBRfull(SpatialIndex::Region &out) const {
    out.makeInfinite(m_dimension+1);
    double *pc=new double[m_dimension+1];
    for(int i=0;i<m_points.size();i++){
        for(int d=0;d<m_dimension;d++) pc[d]=m_points[i].m_pCoords[d];
        pc[m_dimension]=m_points[i].m_startTime;
        out.combinePoint(Point(pc,m_dimension+1));
//        std::cout<<out<<std::endl;
    }
    delete[](pc);
}

void Trajectory::getTimeMBR(SpatialIndex::TimeRegion &out) const {
    out.makeInfinite(m_dimension);
    for(int i=0;i<m_points.size();i++){
        out.combinePoint(m_points[i]);
        if(m_points[i].m_startTime<out.m_startTime) out.m_startTime=m_points[i].m_startTime;
        if(m_points[i].m_startTime>out.m_endTime) out.m_endTime=m_points[i].m_startTime;
    }
}
//[[deprecated]]
////todo: should have implement a time divivsion class!
//void  Trajectory::getMbbc(Mbbc& out,bool tight) const{
//    out.makeInfinite(m_dimension);
//    if(m_points.size()<=1){
//        return;
//    }
//    double startx=m_points.begin()->m_pCoords[0],starty=m_points.begin()->m_pCoords[1],startt=m_points.begin()->m_startTime;
//    double endx=m_points.back().m_pCoords[0],endy=m_points.back().m_pCoords[1],endt=m_points.back().m_startTime;
//    double maxvxP=(endx-startx)/(endt-startt),
//        maxvxN=(endx-startx)/(endt-startt),
//        maxvyP=(endy-starty)/(endt-startt),
//        maxvyN=(endy-starty)/(endt-startt);
//    double minx=startx,maxx=startx,miny=starty,maxy=starty;
//    double vx,vy;
//    for(int i=0;i<m_points.size();i++){
//        if(tight){
//            if(m_points[i].m_startTime-startt>0){
//                vx=(m_points[i].m_pCoords[0]-startx)/(m_points[i].m_startTime-startt);
//                if(vx>maxvxP) maxvxP=vx;
//                if(vx<maxvxN) maxvxN=vx;
//
//                vy=(m_points[i].m_pCoords[1]-starty)/(m_points[i].m_startTime-startt);
//                if(vy>maxvyP) maxvyP=vy;
//                if(vy<maxvyN) maxvyN=vy;
//            }
//            if(endt-m_points[i].m_startTime>0){
//                vx=(endx-m_points[i].m_pCoords[0])/(endt-m_points[i].m_startTime);
//                if(vx>maxvxP) maxvxP=vx;
//                if(vx<maxvxN) maxvxN=vx;
//                vy=(endy-m_points[i].m_pCoords[1])/(endt-m_points[i].m_startTime);
//                if(vy>maxvyP) maxvyP=vy;
//                if(vy<maxvyN) maxvyN=vy;
//            }
//        }else{
//            if(i>0){
//                vx=(m_points[i].m_pCoords[0]-m_points[i-1].m_pCoords[0])/(m_points[i].m_startTime-m_points[i-1].m_startTime);
//                vy=(m_points[i].m_pCoords[1]-m_points[i-1].m_pCoords[1])/(m_points[i].m_startTime-m_points[i-1].m_startTime);
//                if(vx>maxvxP) maxvxP=vx;
//                if(vx<maxvxN) maxvxN=vx;
//                if(vy>maxvyP) maxvyP=vy;
//                if(vy<maxvyN) maxvyN=vy;
//            }
//        }
//        if(m_points[i].m_pCoords[0]<minx) minx=m_points[i].m_pCoords[0];
//        if(m_points[i].m_pCoords[0]>maxx) maxx=m_points[i].m_pCoords[0];
//        if(m_points[i].m_pCoords[1]<miny) miny=m_points[i].m_pCoords[1];
//        if(m_points[i].m_pCoords[1]>maxy) maxy=m_points[i].m_pCoords[1];
//    }
//    double sLow[2]={startx,starty};
//    double sHigh[2]={startx,starty};
//    double eLow[2]={endx,endy};
//    double eHigh[2]={endx,endy};
//    double vLow[2]={maxvxN,maxvyN};
//    double vHigh[2]={maxvxP,maxvyP};
//    double wLow[2]={minx,miny};
//    double wHigh[2]={maxx,maxy};
////    double nstartx,nendx,nstarty,nendy;
////    nstartx=startx-(endx-startx)/(endt-startt)*startt;
////    nstarty=starty-(endy-starty)/(endt-startt)*startt;
////    nendx=endx+(endx-startx)/(endt-startt)*(PeriodLen-endt);
////    nendy=endy+(endy-starty)/(endt-startt)*(PeriodLen-endt);
////    double sLow[2]={nstartx,nstarty};
////    double sHigh[2]={nstartx,nstarty};
////    double eLow[2]={nendx,nendy};
////    double eHigh[2]={nendx,nendy};
////    double vLow[2]={maxvxN,maxvyN};
////    double vHigh[2]={maxvxP,maxvyP};
////    double wLow[2]={minx,miny};
////    double wHigh[2]={maxx,maxy};
////    double stime=int(startt/PeriodLen)*PeriodLen;
////    double etime=int(startt/PeriodLen)*PeriodLen+PeriodLen;
//    out= Mbbc(Region(sLow,sHigh,2),Region(eLow,eHigh,2),
//                Region(vLow,vHigh,2),Region(wLow,wHigh,2),startt,endt);
//
//}
//[[deprecated]]
//void Trajectory::getMbbc(SpatialIndex::Mbbc &out, bool tight, double tstart, double tend) const {
//    out.makeInfinite(m_dimension);
//    if(m_points.size()<=1){
//        out.m_smbr=Region(m_points[0],m_points[0]);
//        out.m_embr=Region(m_points[0],m_points[0]);
//        double a[2]={0,0};
//        out.m_vmbr=Region(a,a,2);
//        out.m_wmbr=Region(m_points[0],m_points[0]);
//        return;
//    }
//    double startx=m_points.begin()->m_pCoords[0],starty=m_points.begin()->m_pCoords[1],startt=m_points.begin()->m_startTime;
//    double endx=m_points.back().m_pCoords[0],endy=m_points.back().m_pCoords[1],endt=m_points.back().m_startTime;
//    double maxvxP=(endx-startx)/(endt-startt),
//            maxvxN=(endx-startx)/(endt-startt),
//            maxvyP=(endy-starty)/(endt-startt),
//            maxvyN=(endy-starty)/(endt-startt);
//    double minx=startx,maxx=startx,miny=starty,maxy=starty;
//    double vx,vy;
//    for(int i=0;i<m_points.size();i++){
//        if(tight){
//            if(m_points[i].m_startTime-startt>0){
//                vx=(m_points[i].m_pCoords[0]-startx)/(m_points[i].m_startTime-startt);
//                if(vx>maxvxP) maxvxP=vx;
//                if(vx<maxvxN) maxvxN=vx;
//
//                vy=(m_points[i].m_pCoords[1]-starty)/(m_points[i].m_startTime-startt);
//                if(vy>maxvyP) maxvyP=vy;
//                if(vy<maxvyN) maxvyN=vy;
//            }
//            if(endt-m_points[i].m_startTime>0){
//                vx=(endx-m_points[i].m_pCoords[0])/(endt-m_points[i].m_startTime);
//                if(vx>maxvxP) maxvxP=vx;
//                if(vx<maxvxN) maxvxN=vx;
//                vy=(endy-m_points[i].m_pCoords[1])/(endt-m_points[i].m_startTime);
//                if(vy>maxvyP) maxvyP=vy;
//                if(vy<maxvyN) maxvyN=vy;
//            }
//        }else{
//            if(i>0){
//                vx=(m_points[i].m_pCoords[0]-m_points[i-1].m_pCoords[0])/(m_points[i].m_startTime-m_points[i-1].m_startTime);
//                vy=(m_points[i].m_pCoords[1]-m_points[i-1].m_pCoords[1])/(m_points[i].m_startTime-m_points[i-1].m_startTime);
//                if(vx>maxvxP) maxvxP=vx;
//                if(vx<maxvxN) maxvxN=vx;
//                if(vy>maxvyP) maxvyP=vy;
//                if(vy<maxvyN) maxvyN=vy;
//            }
//        }
//        if(m_points[i].m_pCoords[0]<minx) minx=m_points[i].m_pCoords[0];
//        if(m_points[i].m_pCoords[0]>maxx) maxx=m_points[i].m_pCoords[0];
//        if(m_points[i].m_pCoords[1]<miny) miny=m_points[i].m_pCoords[1];
//        if(m_points[i].m_pCoords[1]>maxy) maxy=m_points[i].m_pCoords[1];
//    }
//    double nstartx,nendx,nstarty,nendy;
//    nstartx=startx-(endx-startx)/(endt-startt)*(startt-tstart);
//    nstarty=starty-(endy-starty)/(endt-startt)*(startt-tstart);
//    nendx=endx+(endx-startx)/(endt-startt)*(tend-endt);
//    nendy=endy+(endy-starty)/(endt-startt)*(tend-endt);
//    double sLow[2]={nstartx,nstarty};
//    double sHigh[2]={nstartx,nstarty};
//    double eLow[2]={nendx,nendy};
//    double eHigh[2]={nendx,nendy};
//    double vLow[2]={maxvxN,maxvyN};
//    double vHigh[2]={maxvxP,maxvyP};
//    double wLow[2]={minx,miny};
//    double wHigh[2]={maxx,maxy};
//    out= Mbbc(Region(sLow,sHigh,2),Region(eLow,eHigh,2),
//              Region(vLow,vHigh,2),Region(wLow,wHigh,2),tstart,tend);
//}
//
//[[deprecated]]
//void Trajectory::getMBRk(int k, SpatialIndex::MBRk &out) const {
//    out.m_k=k;
//    out.makeInfinite(m_dimension,k);
//    int oldPhase=0;
//    int newPhase=out.getPhase(m_points[0].m_startTime);
//    for(int j=oldPhase;j<=newPhase;j++){
//        out.m_mbrs[j].combinePoint(m_points[0]);
//    }
//    oldPhase=newPhase;
//    for(int i=1;i<m_points.size()-1;i++){
//        newPhase=out.getPhase(m_points[i].m_startTime);
//        out.m_mbrs[newPhase].combinePoint(m_points[i]);
//        if(oldPhase!=newPhase){
//            for(int j=oldPhase;j<newPhase;j++){
//                TimePoint mid=TimePoint::makemid(m_points[i-1],m_points[i],(j+1)*PeriodLen/double(k));
//                out.m_mbrs[j].combinePoint(mid);
//                out.m_mbrs[j+1].combinePoint(mid);
//            }
//            oldPhase=newPhase;
//        }
//    }
//    oldPhase=out.getPhase(m_points[m_points.size()-1].m_startTime);
//    newPhase=k-1;
//    for(int j=oldPhase;j<=newPhase;j++){
//        out.m_mbrs[j].combinePoint(m_points[m_points.size()-1]);
//    }
//}
//[[deprecated]]
//void Trajectory::getMBBCk(int k, SpatialIndex::MBBCk &out, double eps) const {
//    out.m_k=k;
//    out.makeInfinite(m_dimension,k);
//    vector<vector<TimePoint> > seg(k);
//    int oldPhase=0;
//    int newPhase=out.getPhase(m_points[0].m_startTime);
//    for(int j=oldPhase;j<=newPhase;j++){
//        seg[j].emplace_back(m_points[0]);
//    }
//    oldPhase=newPhase;
//    for(int i=1;i<m_points.size()-1;i++){
//        newPhase=out.getPhase(m_points[i].m_startTime);
//        if(oldPhase!=newPhase){
//            for(int j=oldPhase;j<newPhase;j++){
//                TimePoint mid=TimePoint::makemid(m_points[i-1],m_points[i],(j+1)*PeriodLen/double(k));
//                seg[j].emplace_back(mid);
//                seg[j+1].emplace_back(mid);
//            }
//            oldPhase=newPhase;
//        }
//        seg[newPhase].emplace_back(m_points[i]);
//    }
//    oldPhase=out.getPhase(m_points[m_points.size()-1].m_startTime);
//    newPhase=k-1;
//    for(int j=oldPhase;j<=newPhase;j++){
//        seg[j].emplace_back(m_points[m_points.size()-1]);
//    }
//    Mbbc tmpbc;
//    std::vector<Region> mbrs;
//    std::vector<Region> vmbrs;
//    std::vector<Region> wmbrs;
//    for(int i=0;i<k;i++){
//        Trajectory t(seg[i]);
//        t.getMbbc(tmpbc,true,i*PeriodLen/(k),(i+1)*PeriodLen/(k));
//        mbrs.emplace_back(tmpbc.m_smbr);
//        vmbrs.emplace_back(tmpbc.m_vmbr);
//        wmbrs.emplace_back(tmpbc.m_wmbr);
//    }
//    mbrs.emplace_back(tmpbc.m_embr);
//    out.m_mbrs=mbrs;
//    out.m_vmbrs=vmbrs;
//    std::cout<<vmbrs[0]<<std::endl;
//    out.m_wmbrs=wmbrs;
//    out.m_startTime=m_points[0].m_startTime;
//    out.m_endTime=m_points[m_points.size()-1].m_startTime;
////    out=MBBCk(mbrs,vmbrs,wmbrs,m_points[0].m_startTime,m_points[m_points.size()-1].m_startTime);
//}

void Trajectory::getPartialTrajectory(double tstart, double tend, SpatialIndex::Trajectory &out) const {
    //may produce non-exist points through makemid
    //get the inner or equal points
    if(tstart==tend) return;
    int is=0,ie=m_points.size()-1;
    while(m_points[is].m_startTime<tstart) is++;
    while(m_points[ie].m_startTime>tend) ie--;
    out.makeInfinite(2);
    if(is!=0&&m_points[is].m_startTime!=tstart){
        out.m_points.emplace_back(TimePoint::makemid(m_points[is-1],m_points[is],tstart));
    }
    for(int i=is;i<=ie;i++){
        out.m_points.emplace_back(m_points[i]);
    }
    if(ie!=m_points.size()-1&&m_points[ie].m_startTime!=tend){
        out.m_points.emplace_back(TimePoint::makemid(m_points[ie],m_points[ie+1],tend));
    }
}

const std::pair<int, double> findMaximumDistance(const vector<SpatialIndex::TimePoint>& points) {
    SpatialIndex::TimePoint firstpoint=points[0];
    SpatialIndex::TimePoint lastpoint=points[points.size()-1];
    int index=0;  //index to be returned
    double Mdist=-1; //the Maximum distance to be returned

    //distance calculation
    for(int i=1;i<points.size()-1;i++){ //traverse through second point to second last point
        double Dist=SpatialIndex::TimePoint::makemid(firstpoint,lastpoint,points[i].m_startTime).getMinimumDistance(points[i]);
        if (Dist>Mdist){
            Mdist=Dist;
            index=i;
        }
    }
    return std::make_pair(index, Mdist);
}
std::vector<SpatialIndex::TimePoint> Trajectory::simplifyWithRDP(std::vector<SpatialIndex::TimePoint> &Points,
                                                                 double epsilon) {
    if(Points.size()<3){  //base case 1
        return Points;
    }
    std::pair<int, double> maxDistance=findMaximumDistance(Points);
    if(maxDistance.second>=epsilon){
        int index=maxDistance.first;
        vector<SpatialIndex::TimePoint>::iterator it=Points.begin();
        vector<SpatialIndex::TimePoint> path1(Points.begin(),it+index+1); //new path l1 from 0 to index
        vector<SpatialIndex::TimePoint> path2(it+index,Points.end()); // new path l2 from index to last

        vector<SpatialIndex::TimePoint> r1 =simplifyWithRDP(path1,epsilon);
        vector<SpatialIndex::TimePoint> r2=simplifyWithRDP(path2,epsilon);

        //Concat simplified path1 and path2 together
        vector<SpatialIndex::TimePoint> rs(r1);
        rs.pop_back();
        rs.insert(rs.end(),r2.begin(),r2.end());
        return rs;
    }
    else { //base case 2, all points between are to be removed.
        vector<SpatialIndex::TimePoint> r(1,Points[0]);
        r.emplace_back(Points[Points.size()-1]);
        return r;
    }
}

std::vector<Trajectory> Trajectory::cuttraj(std::vector<SpatialIndex::TimePoint> mask){
    vector<TimePoint> seg;
    vector<Trajectory> res;
    auto iter1=m_points.begin();
    auto iter2=mask.begin();
    assert(m_points[0]==mask[0]);
    iter2++;
    for(;iter2!=mask.end();iter2++){
        seg.clear();
        while(iter1!=m_points.end()&&iter1->m_startTime<=iter2->m_startTime){
//            std::cerr<<"placed"<<*iter1<<"\n";
            seg.emplace_back(*iter1);
            iter1++;
        }
        if(seg.size()>0) {
            res.emplace_back(Trajectory(seg));
            iter1--;
//            std::cerr<<iter1->m_startTime<<" "<<iter2->m_startTime;
        }
    }


//    for(int i=0;i<res.size();i++){
//        std::cerr<<"part "<<i<<"is\n";
//        std::cerr<<res[i];
//    }
    return res;
}

std::vector<Trajectory> Trajectory::getSegments(double threshold) {
    auto mask=simplifyWithRDP(m_points,threshold);
    return cuttraj(mask);
}

double Trajectory::getArea() const{ return 0;}

double theF(double c1,double c2,double c3,double c4,double t){
    //the c4 should be the length of that time period
    double delta=4*c1*c3-c2*c2;
    if(delta<=1e-3){
        return (2*c1*t+c2)*sqrt(std::max(0.0,c1*t*t+c2*t+c3))/4/c1/c4;
    }
    else {
        return asinh((2 * c1 * t + c2) / sqrt(delta)) * delta / 8 / c1 / sqrt(c1) / c4
               + (2 * c1 * t + c2) * sqrt(std::max(0.0,c1 * t * t + c2 * t + c3)) / 4 / c1 / c4;
    }
}
double theD(double c1,double c2,double c3,double c4,double t){
    //the c4 should be the length of that time period
    return sqrt(c1*t*t+c2*t+c3)/c4;
}

double line2lineDistance_impl(const SpatialIndex::TimePoint &p1s, const SpatialIndex::TimePoint &p1e,
                              const SpatialIndex::TimePoint &p2s, const SpatialIndex::TimePoint &p2e){
    double ts = p1s.m_startTime, te = p1e.m_endTime;
    double dxs=p1s.m_pCoords[0]-p2s.m_pCoords[0];
    double dys=p1s.m_pCoords[1]-p2s.m_pCoords[1];
    double dxe=p1e.m_pCoords[0]-p2e.m_pCoords[0];
    double dye=p1e.m_pCoords[1]-p2e.m_pCoords[1];
    double c1=sq(dxs-dxe)+sq(dys-dye),
            c2=2*((dxe*ts-dxs*te)*(dxs-dxe)+(dye*ts-dys*te)*(dys-dye)),
            c3=sq(dxe*ts-dxs*te)+sq(dye*ts-dys*te),
            c4=te-ts;
    if(c1==0){
        return std::sqrt(sq(dxs)+sq(dys));
    }else{
        if(c2*c2-4*c1*c3==0){
            return 1/sq(c4)*( (2*c1*te+c2)/4/c1*std::sqrt(c1*te*te+c2*te+c3)
                              -(2*c1*ts+c2)/4/c1*std::sqrt(c1*ts*ts+c2*ts+c3) );
        }
        else{
            return (theF(c1,c2,c3,c4,te)-theF(c1,c2,c3,c4,ts));
        }
    }
}

double Trajectory::line2lineDistance(const SpatialIndex::TimePoint &p1s, const SpatialIndex::TimePoint &p1e,
                                const SpatialIndex::TimePoint &p2s, const SpatialIndex::TimePoint &p2e) {
    if(p1s.m_startTime!=p2s.m_startTime|p1e.m_startTime!=p2e.m_startTime)
        throw Tools::IllegalStateException("line2lineDistance: time period not the same");
    TimePoint np1s=p1s,np1e=p1e,np2s=p2s,np2e=p2e;
    double te=p1e.m_startTime-p1s.m_startTime;
    np1s.m_startTime=np1s.m_endTime=np2s.m_startTime=np2s.m_endTime=0;
    np1e.m_startTime=np1e.m_endTime=np2e.m_startTime=np2e.m_endTime=te;
    np1s.m_pCoords[0]=np1s.m_pCoords[1]=0;
    np2s.m_pCoords[0]-=p1s.m_pCoords[0];np2s.m_pCoords[1]-=p1s.m_pCoords[1];
    np1e.m_pCoords[0]-=p1s.m_pCoords[0];np1e.m_pCoords[1]-=p1s.m_pCoords[1];
    np2e.m_pCoords[0]-=p1s.m_pCoords[0];np2e.m_pCoords[1]-=p1s.m_pCoords[1];
    return line2lineDistance_impl(np1s,np1e,np2s,np2e);
}

int getRealm(const SpatialIndex::Region &r,const Point &p1,const Point &p2){
    // 7 8 9
    // 4 5 6
    // 1 2 3
    if(p1.m_dimension!=2) throw Tools::NotSupportedException("Trajectory::getRealm:only for dimension 2");
    double xd1=r.m_pLow[0],xd2=r.m_pHigh[1],yd1=r.m_pLow[0],yd2=r.m_pHigh[1];
    int res=0;
    if(p1.m_pCoords[0]<=xd2&&p2.m_pCoords[0]<=xd2&&p1.m_pCoords[0]>=xd1&&p2.m_pCoords[0]>=xd1)
        res+=2;
    else if(p1.m_pCoords[0]<=xd1&&p2.m_pCoords[0]<=xd1)
        res+=1;
    else if(p1.m_pCoords[0]>=xd2&&p2.m_pCoords[0]>=xd2)
        res+=3;
    else return -1;
    if(p1.m_pCoords[1]<=yd2&&p2.m_pCoords[1]<=yd2&&p1.m_pCoords[1]>=yd1&&p2.m_pCoords[1]>=yd1)
        res+=3;
    else if(p1.m_pCoords[1]<=yd1&&p2.m_pCoords[1]<=yd1)
        res+=0;
    else if(p1.m_pCoords[1]>=yd2&&p2.m_pCoords[1]>=yd2)
        res+=6;
    else return -1;
    return res;
}

TimePoint* cutByLine(const SpatialIndex::TimePoint &ps, const SpatialIndex::TimePoint &pe,double value,int axis){
    int otheraxis=1-axis;
    double axisv1=ps.m_pCoords[axis],axisv2=pe.m_pCoords[axis];
    if((axisv1<value)==(axisv2<value)||axisv1==value||axisv2==value) //need no cut
        return nullptr;
    else {
        double d = std::abs(ps.m_pCoords[axis] - pe.m_pCoords[axis]);
        double d1 = std::abs(ps.m_pCoords[axis] - value) / d;
        double d2 = std::abs(pe.m_pCoords[axis] - value) / d;
        //get p=d2*ps+d1*pe
        double xyt[3];
        if(axis==0) {
            xyt[0]=value;
            xyt[1]=d2 * ps.m_pCoords[otheraxis] + d1 * pe.m_pCoords[otheraxis];
            xyt[2]=d2 * ps.m_startTime + d1 * pe.m_endTime;
        }else{
            xyt[0]=d2 * ps.m_pCoords[otheraxis] + d1 * pe.m_pCoords[otheraxis];
            xyt[1]=value;
            xyt[2]=d2 * ps.m_startTime + d1 * pe.m_endTime;
        }
        return new TimePoint(xyt, xyt[2], xyt[2], 2);
    }
}
std::vector<std::pair<TimePoint,TimePoint>> cutByRealm(const SpatialIndex::TimePoint &ps, const SpatialIndex::TimePoint &pe,
                                                     const SpatialIndex::Region &r){
    double xd1=r.m_pLow[0],xd2=r.m_pHigh[1],yd1=r.m_pLow[0],yd2=r.m_pHigh[1];
    std::vector<std::pair<TimePoint,TimePoint>> res;
    std::vector<std::pair<TimePoint,TimePoint>> tmp;
    res.emplace_back(std::make_pair(ps,pe));
    for(auto line:res){
        TimePoint *tp=cutByLine(line.first,line.second,xd1,0);
        if(tp!= nullptr){
            tmp.emplace_back(std::make_pair(line.first,*tp));
            tmp.emplace_back(std::make_pair(*tp,line.second));
        }
        else{
            tmp.push_back(line);
        }
    }
    res=tmp;tmp.clear();
    for(auto line:res){
        TimePoint *tp=cutByLine(line.first,line.second,xd2,0);
        if(tp!= nullptr){
            tmp.emplace_back(std::make_pair(line.first,*tp));
            tmp.emplace_back(std::make_pair(*tp,line.second));
        }
        else{
            tmp.push_back(line);
        }
    }
    res=tmp;tmp.clear();
    for(auto line:res){
        TimePoint *tp=cutByLine(line.first,line.second,yd1,1);
        if(tp!= nullptr){
            tmp.emplace_back(std::make_pair(line.first,*tp));
            tmp.emplace_back(std::make_pair(*tp,line.second));
        }
        else{
            tmp.push_back(line);
        }
    }
    res=tmp;tmp.clear();
    for(auto line:res){
        TimePoint *tp=cutByLine(line.first,line.second,yd2,1);
        if(tp!= nullptr){
            tmp.emplace_back(std::make_pair(line.first,*tp));
            tmp.emplace_back(std::make_pair(*tp,line.second));
        }
        else{
            tmp.push_back(line);
        }
    }
    res=tmp;tmp.clear();
//    for(auto seg:res){
//        cout<<seg.first<<"\n"<<seg.second<<"\n";
//    }
    return res;
}
double line2MBRDistance_impl(const SpatialIndex::TimePoint &ps, const SpatialIndex::TimePoint &pe,
                                    const SpatialIndex::Region &r,int sr) {
    double ts = ps.m_startTime, te = pe.m_startTime;
    Region r_2d=Region(r.m_pLow,r.m_pHigh,2);
    double res;
    if (sr == 5) return 0;
    else if (sr % 2 == 0)
        return 0.5 * (ps.getMinimumDistance(r_2d) + pe.getMinimumDistance(r_2d)) * (te - ts);
    else {
        double px, py;
        if (sr == 1 || sr == 7) px = r.m_pLow[0];
        else px = r.m_pHigh[0];
        if (sr == 1 || sr == 3) py = r.m_pLow[1];
        else py = r.m_pHigh[1];
        double coord[2]={px,py};
        return Trajectory::line2lineDistance(ps,pe,TimePoint(coord,ts,ts,2),TimePoint(coord,te,te,2));
    }
}

double Trajectory::line2MBRDistance(const SpatialIndex::TimePoint &ps, const SpatialIndex::TimePoint &pe,
                                    const SpatialIndex::Region &r) {
    //the line's time period should be in the MBR's time period
    assert(r.m_pLow[m_dimension]<=ps.m_startTime);
    assert(r.m_pHigh[m_dimension]>=pe.m_startTime);
    //check if need cutting
    int sr=getRealm(r,ps,pe);
    if(sr>0){
        return line2MBRDistance_impl(ps,pe,r,sr);
    }else{
        double sum =0;
        auto part=cutByRealm(ps,pe,r);
        for(const auto &p:part) {
            int tmpsr=getRealm(r,p.first,p.second);
            double tmpres=line2MBRDistance_impl(p.first,p.second,r,tmpsr);
//            cout<<tmpsr<<" "<<tmpres<<"\n";
            sum+=tmpres;
        }
        return sum;
    }
}

double mbcArea(const SpatialIndex::MBC &r,double ts,double te){
    double t0=r.m_startTime,t1=t0+r.m_rd/r.m_rv,t3=r.m_endTime,t2=t3-r.m_rd/r.m_rv;
    double sum=0;
    double tlow,thigh,rlow,rhigh;
    if(ts<t1){
        tlow=ts;
        thigh=std::min(te,t1);
        rlow=r.getCenterRdAtTime(tlow).second;
        rhigh=r.getCenterRdAtTime(thigh).second;
        sum+=(thigh-tlow)*0.5*(rlow+rhigh);
    }
    if(ts<t2&&te>t1) sum+=r.m_rd*(std::min(t2,te)-std::max(t1,ts));
    if(te>t2){
        tlow=std::max(ts,t2);
        thigh=te;
        rlow=r.getCenterRdAtTime(tlow).second;
        rhigh=r.getCenterRdAtTime(thigh).second;
        sum+=(thigh-tlow)*0.5*(rlow+rhigh);
    }
    return sum;
}

double Trajectory::line2MBCDistance(const SpatialIndex::TimePoint &ps, const SpatialIndex::TimePoint &pe,
                                    const SpatialIndex::MBC &r) {
    assert(std::isfinite(r.m_rv));
    double t0=r.m_startTime,t1=t0+r.m_rd/r.m_rv,t3=r.m_endTime,t2=t3-r.m_rd/r.m_rv;
    double ts=ps.m_startTime,te=pe.m_startTime;
    TimePoint p1s=ps,p1e=pe,p2s=r.getCenterRdAtTime(ts).first,
            p2e=r.getCenterRdAtTime(te).first;
    double dxs=p1s.m_pCoords[0]-p2s.m_pCoords[0];
    double dys=p1s.m_pCoords[1]-p2s.m_pCoords[1];
    double dxe=p1e.m_pCoords[0]-p2e.m_pCoords[0];
    double dye=p1e.m_pCoords[1]-p2e.m_pCoords[1];
    double c1=sq(dxs-dxe)+sq(dys-dye),
            c2=2*((dxe*ts-dxs*te)*(dxs-dxe)+(dye*ts-dys*te)*(dys-dye)),
            c3=sq(dxe*ts-dxs*te)+sq(dye*ts-dys*te),
            c4=te-ts;
    if(c2*c2-4*c1*(c3-sq(r.m_rd*c4))<0){
        //root not exist
        return theF(c1,c2,c3,c4,te)-theF(c1,c2,c3,c4,ts)-mbcArea(r,ts,te);
    }else{
        double root1=(-c2-sqrt(c2*c2-4*c1*(c3-sq(r.m_rd*c4))))/2/c1;
        double root2=(-c2+sqrt(c2*c2-4*c1*(c3-sq(r.m_rd*c4))))/2/c1;
//        if(theD(c1,c2,c3,c4,root1)-r.m_rd>1){
//            double d=theD(c1,c2,c3,c4,root1);
//            std::cerr<<"ha?";
//        }
        double inter1,inter2;
        if(root1>t2||root2<t1) {
            if (root1 > t2) {
                //check if inter1 and inter2 in range
                double a = c1 - sq(c4 * r.m_rv),
                        b = c2 + sq(c4 * r.m_rv) * 2 * t3,
                        c = c3 - sq(c4 * r.m_rv) * sq(t3);
                inter1 = (-b -(std::abs(a)/a)*sqrt(b * b - 4 * a * c)) / 2 / a;
                inter2 = (-b + (std::abs(a)/a)*sqrt(b * b - 4 * a * c)) / 2 / a;
                if(inter1<t3&&inter1>t2){}
                else {
                    //inter1 and inter2 don't exist
                    double aa,bb,cc,d,e;
                    aa=theF(c1,c2,c3,c4,te);
                    bb=theF(c1,c2,c3,c4,ts);
                    cc=theD(c1,c2,c3,c4,te);
                    d=theD(c1,c2,c3,c4,ts);
                    e=mbcArea(r,ts,te);

                    auto f=r.getCenterRdAtTime(ts);
                    auto g=r.getCenterRdAtTime(te);
                    return theF(c1, c2, c3, c4, te) - theF(c1, c2, c3, c4, ts) - mbcArea(r, ts, te);
                }
            }
            if (root2 < t1) {
                double a = c1 - sq(c4 * r.m_rv),
                        b = c2 + sq(c4 * r.m_rv) * 2 * t0,
                        c = c3 - sq(c4 * r.m_rv) * sq(t0);
                inter1 = (-b -(std::abs(a)/a)*sqrt(b * b - 4 * a * c)) / 2 / a;
                inter2 = (-b + (std::abs(a)/a)*sqrt(b * b - 4 * a * c)) / 2 / a;
                if(inter2<t1&&inter2>t0){}
                else{
                    //inter1 and inter2 don't exist
                    double aa,bb,cc,d,e;
                    aa=theF(c1,c2,c3,c4,te);
                    bb=theF(c1,c2,c3,c4,ts);
                    cc=theD(c1,c2,c3,c4,te);
                    d=theD(c1,c2,c3,c4,ts);
                    e=mbcArea(r,ts,te);

                    auto f=r.getCenterRdAtTime(ts);
                    auto g=r.getCenterRdAtTime(te);
                    return theF(c1, c2, c3, c4, te) - theF(c1, c2, c3, c4, ts) - mbcArea(r, ts, te);
                }
            }
        }
        else {
            if (root1 < t1) {
                //calculte inter1
                double a = c1 - sq(c4 * r.m_rv),
                        b = c2 + sq(c4 * r.m_rv) * 2 * t0,
                        c = c3 - sq(c4 * r.m_rv) * sq(t0);
                if (a > 0) inter1 = (-b - sqrt(b * b - 4 * a * c)) / 2 / a;
                else inter1 = (-b + sqrt(b * b - 4 * a * c)) / 2 / a;
            } else {//if(root1>=t1&&root1<t2)
                inter1 = root1;
            }
            if (root2 > t2) {
                //calculate inter2
                double a = c1 - sq(c4 * r.m_rv),
                        b = c2 + sq(c4 * r.m_rv) * 2 * t3,
                        c = c3 - sq(c4 * r.m_rv) * sq(t3);
                if (a > 0) inter2 = (-b + sqrt(b * b - 4 * a * c)) / 2 / a;
                else inter2 = (-b - sqrt(b * b - 4 * a * c)) / 2 / a;
            } else {//if(root2>t1&&root2<=t2)
                inter2 = root2;
            }
        }
        double sum=0;
        if(ts<inter1){
            double tlow=ts;
            double thigh=std::min(inter1,te);
            double aa,bb,cc,d,e;
            aa=theF(c1,c2,c3,c4,thigh);
            bb=theF(c1,c2,c3,c4,tlow);
            cc=theD(c1,c2,c3,c4,thigh);
            d=theD(c1,c2,c3,c4,tlow);
            e=mbcArea(r,thigh,tlow);

            auto f=r.getCenterRdAtTime(thigh);
            auto g=r.getCenterRdAtTime(tlow);
            sum+= theF(c1,c2,c3,c4,thigh)-theF(c1,c2,c3,c4,tlow)-mbcArea(r,thigh,tlow);
        }
        if(te>inter2){
            double tlow=std::max(inter2,ts);
            double thigh=te;
            double aa,bb,cc,d,e;
            aa=theF(c1,c2,c3,c4,te);
            bb=theF(c1,c2,c3,c4,ts);
            cc=theD(c1,c2,c3,c4,te);
            d=theD(c1,c2,c3,c4,ts);
            e=mbcArea(r,ts,te);

            auto f=r.getCenterRdAtTime(ts);
            auto g=r.getCenterRdAtTime(te);
            sum+= theF(c1,c2,c3,c4,thigh)-theF(c1,c2,c3,c4,tlow)-mbcArea(r,thigh,tlow);
        }
        return sum;
    }
}

double Trajectory::getMinimumDistance(const IShape& s) const{
    //using Integrative Squared Euclidean Distance
    const Trajectory* pTrajectory = dynamic_cast<const Trajectory*>(&s);
    if (pTrajectory != 0) return getMinimumDistance(*pTrajectory);

//    const TimePoint* ptp = dynamic_cast<const TimePoint*>(&s);
//    if (ptp != 0) return getMinimumDistance(*ptp);

    const MBC* pmbc= dynamic_cast<const MBC*>(&s);
    if(pmbc!=0) return getMinimumDistance(*pmbc);

    const Region* pr = dynamic_cast<const Region*>(&s);
    if (pr != 0) return getMinimumDistance(*pr);

    const ShapeList *psl = dynamic_cast<const ShapeList*>(&s);
    if (psl != 0) return getMinimumDistance(*psl);

    throw Tools::NotSupportedException(
            "Trajectory::...: Not implemented yet!"
    );
}


double Trajectory::getMinimumDistance(const SpatialIndex::Region &in) const {
        //note here we only calculate the partial distance
        //so just calculate intersection of time period
        double tstart, tend;
        tstart = std::max(m_startTime(), in.m_pLow[in.m_dimension - 1]);
        tend = std::min(m_endTime(), in.m_pHigh[in.m_dimension - 1]);
        if(tstart>=tend) return std::numeric_limits<double>::max();
        Trajectory timedtraj;
        getPartialTrajectory(tstart, tend, timedtraj);
        double sum = 0;
        for (int i = 0; i < timedtraj.m_points.size()-1; i++) {
            sum += line2MBRDistance(timedtraj.m_points[i],timedtraj.m_points[i+1],in);
        }
        return sum;
}

double Trajectory::getMinimumDistance(const SpatialIndex::MBC &in) const {
    assert(m_dimension==in.m_dimension);
    double tstart, tend;
    tstart = std::max(m_startTime(), in.m_startTime);
    tend = std::min(m_endTime(), in.m_endTime);
    if(tstart>=tend) return std::numeric_limits<double>::max();
    Trajectory timedtraj;
    getPartialTrajectory(tstart,tend,timedtraj);
    double sum = 0;
    for (int i = 0; i < timedtraj.m_points.size()-1; i++) {
        double pd = line2MBCDistance(timedtraj.m_points[i],timedtraj.m_points[i+1],in);
        line2MBCDistance(timedtraj.m_points[i],timedtraj.m_points[i+1],in);
//        if(!(pd>-1)){
//            std::cerr<<timedtraj.m_points[i]<<timedtraj.m_points[i+1]<<in<<endl;
//            line2MBCDistance(timedtraj.m_points[i],timedtraj.m_points[i+1],in);
//            std::cerr<<"minus distance at"<<pd<<"\n";
//        }
        sum+=pd;
    }
    return sum;
}

double Trajectory::getMinimumDistance(const SpatialIndex::TimePoint &in) const {
    int last;
    for(last=1;m_points[last].m_startTime<in.m_startTime;last++);
    return in.getMinimumDistance(TimePoint::makemid(m_points[last-1],m_points[last],in.m_startTime));
}

double Trajectory::getMinimumDistance(const SpatialIndex::Trajectory &in) const {
    if(m_startTime()>in.m_endTime()||m_endTime()<in.m_startTime())
        return std::numeric_limits<double>::max();
    Trajectory frontTraj,midTraj,backTraj;
    Trajectory temporalTraj;
    in.getPartialTrajectory(m_startTime(),m_endTime(),temporalTraj);
    double sum=0;
    getPartialTrajectory(m_startTime(),temporalTraj.m_startTime(),frontTraj);
    getPartialTrajectory(temporalTraj.m_startTime(),temporalTraj.m_endTime(),midTraj);
    getPartialTrajectory(temporalTraj.m_endTime(),m_endTime(),backTraj);
    if(frontTraj.m_points.size()!=0){
        for(int i=0;i<frontTraj.m_points.size()-1;i++){

            sum+=0.5*(frontTraj.m_points[i].getMinimumDistance(temporalTraj.m_points[0])
                    +frontTraj.m_points[i+1].getMinimumDistance(temporalTraj.m_points[0]))
                            *(frontTraj.m_points[i+1].m_startTime-frontTraj.m_points[i].m_startTime);
        }
    }
    if(backTraj.m_points.size()!=0){
        for(int i=0;i<backTraj.m_points.size()-1;i++){
            sum+=0.5*(backTraj.m_points[i].getMinimumDistance(temporalTraj.m_points.back())
                      +backTraj.m_points[i+1].getMinimumDistance(temporalTraj.m_points.back()))
                 *(backTraj.m_points[i+1].m_startTime-backTraj.m_points[i].m_startTime);
        }
    }
    if(midTraj.m_points.size()!=0) {
        double newtime = midTraj.m_startTime(), lasttime = midTraj.m_startTime();
        auto iter1 = midTraj.m_points.begin();
        auto iter2 = temporalTraj.m_points.begin();
        TimePoint lastp1 = *iter1, lastp2 = *iter2, newp1, newp2;
        while (lasttime != temporalTraj.m_endTime()) {
            if ((iter1 + 1)->m_startTime == (iter2 + 1)->m_startTime) {
                newtime = (iter1 + 1)->m_startTime;
                newp1 = *(iter1 + 1);
                newp2 = *(iter2 + 1);
                iter1++;
                iter2++;
            } else if ((iter1 + 1)->m_startTime < (iter2 + 1)->m_startTime) {
                newtime = (iter1 + 1)->m_startTime;
                newp1 = *(iter1 + 1);
                newp2 = TimePoint::makemid(*iter2, *(iter2 + 1), newtime);
                iter1++;
            } else {
                newtime = (iter2 + 1)->m_startTime;
                newp1 = TimePoint::makemid(*iter1, *(iter1 + 1), newtime);
                newp2 = *(iter2 + 1);
                iter2++;
            }
            lasttime = newtime;
            sum += line2lineDistance(lastp1, newp1, lastp2, newp2);
            lastp1 = newp1;
            lastp2 = newp2;
        }
    }
    return sum;
}


double Trajectory::getMinimumDistance(const ShapeList &in) const {
    double sum=0;
    Trajectory tmptraj;
    double ints,inte;
    Point sPoint,ePoint;
    if(in.m_shapeType==SpatialIndex::LeafBoundByMBR) {
        ints=in.m_MBRList.front()->m_pLow[m_dimension];
        inte=in.m_MBRList.back()->m_pHigh[m_dimension];
        sPoint=Point(in.m_MBRList.front()->m_pLow,m_dimension);
        ePoint=Point(in.m_MBRList.back()->m_pHigh,m_dimension);
        for(auto &br:in.m_MBRList){
            double ts=br->m_pLow[m_dimension],te=br->m_pHigh[m_dimension];
            getPartialTrajectory(ts,te,tmptraj);
            sum+=tmptraj.getMinimumDistance(*br);
        }
    }else if(in.m_shapeType==SpatialIndex::LeafBoundByMBC){
        ints=in.m_MBCList.front()->m_startTime;
        inte=in.m_MBCList.back()->m_endTime;
        sPoint=Point(in.m_MBCList.front()->m_pLow,m_dimension);
        ePoint=Point(in.m_MBCList.back()->m_pHigh,m_dimension);
        for(auto &bc:in.m_MBCList){
            double ts=bc->m_startTime,te=bc->m_endTime;
            getPartialTrajectory(ts,te,tmptraj);
            sum+=tmptraj.getMinimumDistance(*bc);
        }
    } else throw Tools::IllegalStateException("ShapeList: missing datatype");
    if(m_startTime()<ints){
        double pLow[m_dimension+1],pHigh[m_dimension+1];
        for(int i=0;i<m_dimension;i++){
            pLow[i]=sPoint.m_pCoords[i];
            pHigh[i]=sPoint.m_pCoords[i];
        }
        pLow[m_dimension]=m_startTime();
        pHigh[m_dimension]=ints;
        Region bc(pLow,pHigh,m_dimension+1);
        getPartialTrajectory(pLow[m_dimension],pHigh[m_dimension],tmptraj);
        sum+=tmptraj.getMinimumDistance(bc);
    }
    if(m_endTime()>inte){
        double pLow[m_dimension+1],pHigh[m_dimension+1];
        for(int i=0;i<m_dimension;i++){
            pLow[i]=ePoint.m_pCoords[i];
            pHigh[i]=ePoint.m_pCoords[i];
        }
        pLow[m_dimension]=inte;
        pHigh[m_dimension]=m_endTime();
        Region bc(pLow,pHigh,m_dimension+1);
        getPartialTrajectory(pLow[m_dimension],pHigh[m_dimension],tmptraj);
        sum+=tmptraj.getMinimumDistance(bc);
    }
    return sum;
}

void Trajectory::makeInfinite(uint32_t dimension)
{
    m_points.clear();
}


void Trajectory::combineTrajectory(const Trajectory& r)
{
    throw Tools::NotSupportedException(
            "Trajectory::...: Not implemented yet!"
    );
}

bool Trajectory::containsTrajectory(const SpatialIndex::Trajectory &r) {
    throw Tools::NotSupportedException(
            "Trajectory::...: Not implemented yet!"
    );
}

void Trajectory::getCombinedTrajectory(Trajectory& out, const Trajectory& in) const
{
    out = *this;
    out.combineTrajectory(in);
}
std::ostream& SpatialIndex::operator<<(std::ostream& os, const Trajectory& r) {
    std::string s;
    s = "Trajectory length:" + std::to_string(r.m_points.size()) + "\n" +
        "m_points are" + "\n";
    for (auto p:r.m_points) {
        s += std::to_string(p.m_pCoords[0]) + "," + std::to_string(p.m_pCoords[1]) +
             "," + std::to_string(p.m_startTime) + " ";
    }
    s += "\n";
    os<<s;
    return os;
}
std::vector<std::string> split(std::string strtem,char a)
{
    std::vector<std::string> strvec;

    std::string::size_type pos1, pos2;
    pos2 = strtem.find(a);
    pos1 = 0;
    while (std::string::npos != pos2)
    {
        strvec.emplace_back(strtem.substr(pos1, pos2 - pos1));

        pos1 = pos2 + 1;
        pos2 = strtem.find(a, pos1);
    }
    strvec.emplace_back(strtem.substr(pos1));
    return strvec;
}

void Trajectory::loadFromString(std::string str) {
    std::vector<std::string> points=split(str,' ');
    for(auto p: points){
        std::vector<std::string> xyt=split(p,',');
        double xy[2];
        xy[0]=std::stod(xyt[0]);xy[1]=std::stod(xyt[1]);
        m_points.emplace_back(TimePoint(xy,std::stod(xyt[2]),std::stod(xyt[2]),2));
    }
}

void Trajectory::linkTrajectory(SpatialIndex::Trajectory other) {
    if(m_points.back().m_startTime==other.m_points.front().m_startTime){
        m_points.insert(m_points.end(),++other.m_points.begin(),other.m_points.end());
    }
    else if (other.m_points.back().m_startTime==m_points.front().m_startTime){
        m_points.insert(m_points.begin(),other.m_points.begin(),other.m_points.end()-1);
    }
    else{
        std::cerr<<m_points.back()<<" "<<other.m_points.front()<<"\n"
            <<other.m_points.back()<<" "<<m_points.front();
        throw Tools::IllegalStateException("Trajectory::linkTrajectory: the two trajectories to be linked should have a common point.");
    }
}