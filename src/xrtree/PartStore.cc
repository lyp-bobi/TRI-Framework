//
// Created by Chuang on 2021/2/3.
//

#include "xRTree.h"

using namespace xRTreeNsp;

void PartsStore::Parts::insert(xSBB &r, id_type prev, id_type next, xStoreEntry &entry) {
    if(m_sbbs.empty()) m_sbbs.emplace_back(r);
    else{
        auto j=m_sbbs.begin();
        for(;j!=m_sbbs.end()&&j->startTime()<r.startTime();j++);
        m_sbbs.insert(j,r);
    }
    m_loadedTime+=r.endTime()-r.startTime();
    if(r.endTime()>m_maxtime){
        m_maxtime=r.endTime();
        if(next==-1) {
            m_hasNext=false;
            m_loadedTime+=m_ps->m_query.m_endTime()-m_maxtime;
        }
    }
    if(r.startTime()<m_mintime){
        m_mintime=r.startTime();
        if(prev==-1) {
            m_hasPrev=false;
            m_loadedTime+=m_mintime-m_ps->m_query.m_startTime();
        }
    }
    if(prev>=0&&m_loadedLeaf.count(prev)==0) m_missingLeaf.insert(prev);
    if(next>=0&&m_loadedLeaf.count(next)==0) m_missingLeaf.insert(next);
    m_ses[r.startTime()]=entry;
}

void PartsStore::insert(id_type id, xSBB &b, id_type prev, id_type next, xStoreEntry &entry) {
    if(m_parts.count(id)==0){
        m_parts[id]=Parts(this);
        m_parts[id].m_loadedTime = min(b.endTime(),m_query.m_endTime())-
                                   max(b.startTime(),m_query.m_startTime());
    }
    m_parts[id].insert(b,prev,next,entry);
}

DISTE PartsStore::updateValue(id_type id) {
    Parts *parts = &m_parts[id];
    double computedTime = 0;
    DISTE pd;
    DISTE res;
    std::pair<double, double> timeInterval;
    //inferred distance(front dist, back dist and mid dist) should be stored as negative values
    //front dist
    if (parts->m_mintime > m_query.m_startTime()) {
        timeInterval.first=m_query.m_startTime();
        timeInterval.second=parts->m_mintime;
        if (parts->m_computedDist.count(timeInterval) > 0) {
            pd= parts->m_computedDist[timeInterval];
        } else {
            if (parts->m_hasPrev) {
                pd = m_query.frontDist(parts->m_sbbs.front(),stat->vmax);
                parts->m_computedDist[timeInterval] = pd;
            } else {
                pd = m_query.frontDistStatic(parts->m_sbbs.front());
                parts->m_computedDist[timeInterval] = pd;
                computedTime += timeInterval.second - timeInterval.first;
            }
        }
        //we don't know if it has sbbs, so can't use
        if(tjstat->regular&&parts->m_computedDist[timeInterval].infer&&!m_nodespq.empty()){
            pd.opt = std::max(pd.opt, m_nodespq.top()->m_dist.opt *
                                      (timeInterval.second - timeInterval.first) /
                                      (m_query.m_endTime() - m_query.m_startTime()));
            pd.pes = max(pd.opt,pd.pes);
        }
        res = res + pd;
    }
    //mid dist
    const xSBB *prev= nullptr;
    for (const auto &box:parts->m_sbbs) {
        //this box
        timeInterval.first=box.startTime();
        timeInterval.second=box.endTime();
        if (parts->m_computedDist.count(timeInterval) > 0) {
            pd = parts->m_computedDist[timeInterval];
        } else {
            pd = DISTE(m_query.sbbDist(box));
            parts->m_computedDist[timeInterval] = pd;
        }
        res = res + pd;
        computedTime += timeInterval.second - timeInterval.first;
        //the gap
        if (box.startTime() != parts->m_sbbs.front().startTime()) {//not first
            if (prev->endTime() < box.startTime()) {
                timeInterval.first=prev->endTime();
                timeInterval.second=box.startTime();
                if (parts->m_computedDist.count(timeInterval) > 0) {
                    pd= parts->m_computedDist[timeInterval];
                } else {
                    pd = m_query.gapDist(*prev, box, stat->vmax);
                    parts->m_computedDist[timeInterval] = pd;
                }
                if(parts->m_computedDist[timeInterval].infer&&!m_nodespq.empty()){
                    pd.opt = std::max(pd.opt, m_nodespq.top()->m_dist.opt *
                                              (timeInterval.second - timeInterval.first) /
                                              (m_query.m_endTime() - m_query.m_startTime()));
                    pd.pes = max(pd.opt,pd.pes);
                }
                res = res + pd;
            }
        }
        prev = &box;
    }
    //backdist
    if (parts->m_maxtime < m_query.m_endTime()) {
        timeInterval.first=parts->m_maxtime;
        timeInterval.second=m_query.m_endTime();
        if (parts->m_computedDist.count(timeInterval) > 0) {
            pd= parts->m_computedDist[timeInterval];
        } else {
            if (parts->m_hasNext) {
                pd = m_query.backDist(parts->m_sbbs.back(),stat->vmax);
                parts->m_computedDist[timeInterval] = pd;
            } else {
                pd = m_query.backDistStatic(parts->m_sbbs.back());
                parts->m_computedDist[timeInterval] = pd;
                computedTime += timeInterval.second - timeInterval.first;
            }
        }
        //we don't know if it has sbbs, so can't use
        if(tjstat->regular&&parts->m_computedDist[timeInterval].infer&&!m_nodespq.empty()){
            pd.opt = std::max(pd.opt, m_nodespq.top()->m_dist.opt *
                                      (timeInterval.second - timeInterval.first) /
                                      (m_query.m_endTime() - m_query.m_startTime()));
            pd.pes = max(pd.opt,pd.pes);
        }
        res = res + pd;
    }
    parts->m_calcMin = res;
    parts->m_computedTime = computedTime;
    int type = 2;
    if (parts->m_missingLeaf.empty()) type = 3;
    res.opt -= m_error;
    res.pes += m_error;
    m_mpq.updateValue(m_handlers[id], id, res, type);
    return res;
}


void PartsStore::loadLeaf(const Node &n, double dist) {
//                    std::cerr<<"load leaf"<<n.m_nodeMBR<<"\n";
//                    std::cerr<<"leaf dist"<<m_query.getNodeMinimumDistance(n.m_nodeMBR,100)/(m_query.m_endTime()-m_query.m_startTime())<<"\n";
//                    std::cerr<<"load leaf"<<n.m_identifier<<"\n";
    loadedLeaf.insert(n.m_identifier);
    std::set<id_type > relatedIds;
    for(int i=0;i<n.m_children;i++){
        id_type trajid=n.m_se[i].m_id;
        xStoreEntry entry= n.m_se[i];
        double bts=n.m_ptrxSBB[i]->startTime(),bte=n.m_ptrxSBB[i]->endTime();
        if(bts>=m_query.m_endTime()||
           bte<=m_query.m_startTime()){}
        else {
            insert(trajid, *n.m_ptrxSBB[i],
                   (m_query.m_startTime()<bts)?n.m_prevNode[i]:-1
                    , (m_query.m_endTime()>bte)?n.m_nextNode[i]:-1,
                   entry);
            relatedIds.insert(trajid);
//                        if(m_parts.count(trajid)>0){
//                            auto s = m_parts[trajid];
//                            std::cerr<<"";
//                        }
        }
    }
    for(const auto &rid:relatedIds){
        m_parts[rid].m_missingLeaf.erase(n.m_identifier);
        m_parts[rid].m_loadedLeaf.insert(n.m_identifier);
        if(m_handlers.count(rid)==0){
            auto handle = m_mpq.push(new NNEntry(rid, DISTE(dist), 2));
            m_handlers[rid] = handle;
        }
    }
}

NNEntry* PartsStore::top() {
    if(!m_mpq.empty()&&m_mpq.top()->m_type==2){
        id_type lastid=m_mpq.top()->m_id;
        updateValue(lastid);
        m_mpq.updateOrder(m_handlers[lastid]);
        while(m_mpq.top()->m_type==2&&m_mpq.top()->m_id!=lastid){
            lastid=m_mpq.top()->m_id;
            updateValue(lastid);
            m_mpq.updateOrder(m_handlers[lastid]);
        }
    }
    if(!m_mpq.empty()&&(m_nodespq.empty()|| m_mpq.top()->m_dist < m_nodespq.top()->m_dist))
        return m_mpq.top();
    else
        return m_nodespq.top();
}

NNEntry* PartsStore::pop() {
    if(!m_mpq.empty()&&(m_nodespq.empty()|| m_mpq.top()->m_dist < m_nodespq.top()->m_dist))
        return m_mpq.pop();
    else
        return m_nodespq.pop();
}

