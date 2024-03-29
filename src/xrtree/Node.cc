/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Marios Hadjieleftheriou, mhadji@gmail.com
 ******************************************************************************
 * Copyright (c) 2002, Marios Hadjieleftheriou
 *
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
******************************************************************************/

#include <cstring>
#include <cmath>
#include <limits>

#include <spatialindex/SpatialIndex.h>

#include "xRTree.h"
#include "Node.h"
#include "Index.h"

using namespace SpatialIndex;
using namespace SpatialIndex::xRTreeNsp;

//
// Tools::IObject interface
//
Tools::IObject* Node::clone()
{
	throw Tools::NotSupportedException("IObject::clone should never be called.");
}

//
// Tools::ISerializable interface
//
uint32_t Node::getByteArraySize() const
{
    return PageSizeDefault; //i'm lazy.
}



void Node::loadFromByteArray(const uint8_t* ptr)
{
    m_nodeMBR.makeInfinite(2);
    memcpy(&m_level, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    memcpy(&m_children, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    if(m_level >0) {//inner
        for (uint32_t u32Child = 0; u32Child < m_children; ++u32Child) {
            memcpy(&(m_pIdentifier[u32Child]), ptr, sizeof(id_type));
            ptr += sizeof(id_type);
            m_ptrMBR[u32Child] = m_pTree->m_xMBRPool.acquire();
            (m_ptrMBR[u32Child])->loadFromByteArray(ptr);
            ptr += (m_ptrMBR[u32Child])->getByteArraySize();
        }
    }else{//leaf
        double rv;
        if(bCompactMBC && m_pTree->m_bUsingMBC)
        {
            memcpy(&rv, ptr, sizeof(double));
            ptr += sizeof(double);
        }
        for (uint32_t u32Child = 0; u32Child < m_children; ++u32Child) {
            memcpy(&(m_pIdentifier[u32Child]), ptr, sizeof(id_type));
            ptr += sizeof(id_type);
            memcpy(&(m_se[u32Child]), ptr, sizeof(xStoreEntry));
            ptr += sizeof(xStoreEntry);
            if(m_pTree->m_bStoringLinks) {
                memcpy(&(m_prevNode[u32Child]), ptr, sizeof(id_type));
                ptr += sizeof(id_type);
                memcpy(&(m_nextNode[u32Child]), ptr, sizeof(id_type));
                ptr += sizeof(id_type);
            }
            m_ptrxSBB[u32Child] = m_pTree->m_xSBBPool.acquire();
            m_ptrxSBB[u32Child]->hasbr=m_ptrxSBB[u32Child]->hasbc=m_ptrxSBB[u32Child]->hasbl=false;
            if(m_pTree->m_bUsingMBR){
                m_ptrxSBB[u32Child]->loadFromByteArray(1,ptr);
                ptr+=m_ptrxSBB[u32Child]->br.getByteArraySize();
            }else if(m_pTree->m_bUsingMBC){
                m_ptrxSBB[u32Child]->loadFromByteArray(2,ptr);
                ptr+=m_ptrxSBB[u32Child]->bc.getByteArraySize();
                if(bCompactMBC)
                {
                    m_ptrxSBB[u32Child]->bc.m_rv = rv;
                }
            }else if(m_pTree->m_bUsingMBL){
                m_ptrxSBB[u32Child]->loadFromByteArray(3,ptr);
                ptr+=m_ptrxSBB[u32Child]->bl.getByteArraySize();
            }
        }
    }
    m_nodeMBR.loadFromByteArray(ptr);
//    ptr+= m_nodeMBR.getByteArraySize();
}

void Node::storeToByteArray(uint8_t** data, uint32_t& len)
{
    len = getByteArraySize();

    *data = new uint8_t[len];
    uint8_t* ptr = *data;

    memcpy(ptr, &m_level, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    memcpy(ptr, &m_children, sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    if(m_level >0) {//inner
        for (uint32_t u32Child = 0; u32Child < m_children; ++u32Child) {
            memcpy(ptr, &(m_pIdentifier[u32Child]), sizeof(id_type));
            ptr += sizeof(id_type);
            m_ptrMBR[u32Child]->storeToByteArrayE(&ptr,len);
            ptr += (m_ptrMBR[u32Child])->getByteArraySize();
        }
    }else{//leaf
        double rv = 0;
        if(m_pTree->m_bUsingMBC && bCompactMBC) {
            for (uint32_t u32Child = 0; u32Child < m_children; ++u32Child) {
                rv = std::max(rv, m_ptrxSBB[u32Child]->bc.m_rv);
            }
            memcpy(ptr,&rv,  sizeof(double));
            ptr += sizeof(double);
        }
        for (uint32_t u32Child = 0; u32Child < m_children; ++u32Child) {
            memcpy(ptr,&(m_pIdentifier[u32Child]),  sizeof(id_type));
            ptr += sizeof(id_type);
            memcpy(ptr,&(m_se[u32Child]),  sizeof(xStoreEntry));
            ptr += sizeof(xStoreEntry);
            if(m_pTree->m_bStoringLinks) {
                memcpy(ptr, &(m_prevNode[u32Child]), sizeof(id_type));
                ptr += sizeof(id_type);
                memcpy(ptr, &(m_nextNode[u32Child]), sizeof(id_type));
                ptr += sizeof(id_type);
            }
            if(m_pTree->m_bUsingMBR){
                m_ptrxSBB[u32Child]->br.storeToByteArrayE(&ptr,len);
                ptr+=m_ptrxSBB[u32Child]->br.getByteArraySize();
            }else if(m_pTree->m_bUsingMBC){
                m_ptrxSBB[u32Child]->bc.storeToByteArrayE(&ptr,len);
                ptr+=m_ptrxSBB[u32Child]->bc.getByteArraySize();
            }else if(m_pTree->m_bUsingMBL){
                m_ptrxSBB[u32Child]->bl.storeToByteArrayE(&ptr,len);
                ptr+=m_ptrxSBB[u32Child]->bl.getByteArraySize();
            }
        }
    }
    m_nodeMBR.storeToByteArrayE(&ptr,len);
    ptr+= m_nodeMBR.getByteArraySize();
    assert(ptr-*data<PageSizeDefault);
    len = ptr - *data;
//    assert(len == (ptr - *data)+m_pTree->m_dimension*sizeof(double));
}

//
// SpatialIndex::IEntry interface
//
SpatialIndex::id_type Node::getIdentifier() const
{
	return m_identifier;
}

void Node::getShape(IShape** out) const
{
	*out = new xMBR(m_nodeMBR);
}

//
// SpatialIndex::INode interface
//
uint32_t Node::getChildrenCount() const
{
	return m_children;
}

SpatialIndex::id_type Node::getChildIdentifier(uint32_t index) const
{
	if (index >= m_children) throw Tools::IndexOutOfBoundsException(index);

	return m_pIdentifier[index];
}

void Node::getChildShape(uint32_t index, IShape** out) const
{
	if (index >= m_children) throw Tools::IndexOutOfBoundsException(index);

	*out = new xMBR(*(m_ptrMBR[index]));
}

void Node::getChildData(uint32_t index, uint32_t &len, uint8_t **data) const {
    *data= nullptr;
}


uint32_t Node::getLevel() const
{
	return m_level;
}

bool Node::isLeaf() const
{
	return (m_level == 0);
}

bool Node::isIndex() const
{
	return (m_level != 0);
}

//
// Internal
//
Node::Node()=default;

Node::Node(SpatialIndex::xRTreeNsp::xRTree* pTree, id_type id, uint32_t level, uint32_t capacity) :
	m_pTree(pTree),
	m_level(level),
	m_identifier(id),
	m_children(0),
	m_capacity(capacity),
    m_prevNode(nullptr),
    m_nextNode(nullptr),
    m_se(nullptr),
	m_ptrMBR(nullptr),
    m_ptrxSBB(nullptr),
	m_pIdentifier(nullptr)
{
	m_nodeMBR.makeInfinite(m_pTree->m_dimension);

	try
	{
        m_ptrMBR = new xMBRPtr[m_capacity + 1];
        m_ptrxSBB = new xSBBPtr[m_capacity + 1];
		if(m_level==0){
		    m_prevNode=new id_type[m_capacity+1];
		    m_nextNode=new id_type[m_capacity+1];
            m_se=new xStoreEntry[m_capacity+1];
		    for(int i=0;i<m_capacity+1;i++){
		        m_prevNode[i]=-1;
                m_nextNode[i]=-1;
		    }
		}
		m_pIdentifier = new id_type[m_capacity + 1];
	}
	catch (...)
	{
		delete[] m_ptrMBR;
        delete[] m_ptrxSBB;
		delete[] m_pIdentifier;
		delete[] m_prevNode;
		delete[] m_nextNode;
        delete[] m_se;
		throw;
	}
}

Node::~Node()
{
    delete[] m_ptrMBR;
    delete[] m_ptrxSBB;
    delete[] m_pIdentifier;
    delete[] m_prevNode;
    delete[] m_nextNode;
    delete[] m_se;
}

Node& Node::operator=(const Node&)
{
	throw Tools::IllegalStateException("operator =: This should never be called.");
}


void Node::insertEntry(xMBR& mbr,id_type id, xSBBData* sbd) {
    assert(m_children < m_capacity);
    m_ptrMBR[m_children] = m_pTree->m_xMBRPool.acquire();
    *(m_ptrMBR[m_children]) = mbr;
    m_pIdentifier[m_children] = id;
    if(sbd!= nullptr) {    //for leaf
        m_ptrxSBB[m_children] = m_pTree->m_xSBBPool.acquire();
        *(m_ptrxSBB[m_children]) = sbd->m_b;
        m_se[m_children] = sbd->m_se;
        m_prevNode[m_children] = sbd->m_hasPrev;
        m_nextNode[m_children] = sbd->m_hasNext;
    }
    ++m_children;
    m_nodeMBR.combinexMBR(mbr);
}



void Node::deleteEntry(uint32_t index)
{
	assert(index >= 0 && index < m_children);

	// cache it, since I might need it for "touches" later.
	xMBRPtr ptrR = m_ptrMBR[index];


	if (m_children > 1 && index != m_children - 1)
	{
		m_ptrMBR[index] = m_ptrMBR[m_children - 1];
		m_pIdentifier[index] = m_pIdentifier[m_children - 1];
	}

	--m_children;

	// WARNING: index has now changed. Do not use it below here.

	if (m_children == 0)
	{
		m_nodeMBR.makeInfinite(2);
	}
	else if (m_pTree->m_bTightMBRs && m_nodeMBR.touchesxMBR(*ptrR))
	{
		for (uint32_t cDim = 0; cDim < 3; ++cDim)
		{
			m_nodeMBR.m_pLow(cDim) = std::numeric_limits<double>::max();
			m_nodeMBR.m_pHigh(cDim) = -std::numeric_limits<double>::max();

			for (uint32_t u32Child = 0; u32Child < m_children; ++u32Child)
			{
				m_nodeMBR.m_pLow(cDim) = std::min(m_nodeMBR.m_pLow(cDim), m_ptrMBR[u32Child]->m_pLow(cDim));
				m_nodeMBR.m_pHigh(cDim) = std::max(m_nodeMBR.m_pHigh(cDim), m_ptrMBR[u32Child]->m_pHigh(cDim));
			}
		}
	}
}

bool Node::insertData(xMBR& mbr, id_type id, std::stack<id_type>& pathBuffer, uint8_t* overflowTable)
{
	if (m_children < m_capacity)
	{
		bool adjusted = false;

		// this has to happen before insertEntry modifies m_nodeMBR.
		bool b = m_nodeMBR.containsxMBR(mbr);

		insertEntry(mbr, id);
		m_pTree->writeNode(this);

		if ((! b) && (! pathBuffer.empty()))
		{
			id_type cParent = pathBuffer.top(); pathBuffer.pop();
			NodePtr ptrN = m_pTree->readNode(cParent);
			Index* p = static_cast<Index*>(ptrN.get());
			p->adjustTree(this, pathBuffer);
			adjusted = true;
		}

		return adjusted;
	}
	else if (m_pTree->m_treeVariant == RV_RSTAR && (! pathBuffer.empty()) && overflowTable[m_level] == 0)
	{
		overflowTable[m_level] = 1;

		std::vector<uint32_t> vReinsert, vKeep;
		reinsertData(mbr, id, vReinsert, vKeep);

		uint32_t lReinsert = static_cast<uint32_t>(vReinsert.size());
		uint32_t lKeep = static_cast<uint32_t>(vKeep.size());

		uint8_t** reinsertdata = 0;
		xMBRPtr* reinsertmbr = 0;
		id_type* reinsertid = 0;
		uint32_t* reinsertlen = 0;
		uint8_t** keepdata = 0;
		xMBRPtr* keepmbr = 0;
		id_type* keepid = 0;
		uint32_t* keeplen = 0;

		try
		{
			reinsertdata = new uint8_t*[lReinsert];
			reinsertmbr = new xMBRPtr[lReinsert];
			reinsertid = new id_type[lReinsert];
			reinsertlen = new uint32_t[lReinsert];

			keepdata = new uint8_t*[m_capacity + 1];
			keepmbr = new xMBRPtr[m_capacity + 1];
			keepid = new id_type[m_capacity + 1];
			keeplen = new uint32_t[m_capacity + 1];
		}
		catch (...)
		{
			delete[] reinsertdata;
			delete[] reinsertmbr;
			delete[] reinsertid;
			delete[] reinsertlen;
			delete[] keepdata;
			delete[] keepmbr;
			delete[] keepid;
			delete[] keeplen;
			throw;
		}

		uint32_t cIndex;

		for (cIndex = 0; cIndex < lReinsert; ++cIndex)
		{
			reinsertmbr[cIndex] = m_ptrMBR[vReinsert[cIndex]];
			reinsertid[cIndex] = m_pIdentifier[vReinsert[cIndex]];
		}

		for (cIndex = 0; cIndex < lKeep; ++cIndex)
		{

			keepmbr[cIndex] = m_ptrMBR[vKeep[cIndex]];
			keepid[cIndex] = m_pIdentifier[vKeep[cIndex]];
		}

		delete[] m_ptrMBR;
		delete[] m_pIdentifier;
        delete[] m_prevNode;
        delete[] m_nextNode;
        delete[] m_se;

		m_ptrMBR = keepmbr;
		m_pIdentifier = keepid;
		m_children = lKeep;

		for (uint32_t cDim = 0; cDim < 3; ++cDim)
		{
			m_nodeMBR.m_pLow(cDim) = std::numeric_limits<double>::max();
			m_nodeMBR.m_pHigh(cDim) = -std::numeric_limits<double>::max();

			for (uint32_t u32Child = 0; u32Child < m_children; ++u32Child)
			{
				m_nodeMBR.m_pLow(cDim) = std::min(m_nodeMBR.m_pLow(cDim), m_ptrMBR[u32Child]->m_pLow(cDim));
				m_nodeMBR.m_pHigh(cDim) = std::max(m_nodeMBR.m_pHigh(cDim), m_ptrMBR[u32Child]->m_pHigh(cDim));
			}
		}

		m_pTree->writeNode(this);

		// Divertion from R*-Tree algorithm here. First adjust
		// the path to the root, then start reinserts, to avoid complicated handling
		// of changes to the same node from multiple insertions.
		id_type cParent = pathBuffer.top(); pathBuffer.pop();
		NodePtr ptrN = m_pTree->readNode(cParent);
		Index* p = static_cast<Index*>(ptrN.get());
		p->adjustTree(this, pathBuffer);

		for (cIndex = 0; cIndex < lReinsert; ++cIndex)
		{
			m_pTree->insertData_impl(
				*(reinsertmbr[cIndex]), reinsertid[cIndex],
				m_level, overflowTable);
		}

		delete[] reinsertdata;
		delete[] reinsertmbr;
		delete[] reinsertid;
		delete[] reinsertlen;

		return true;
	}
	else
	{
		NodePtr n;
		NodePtr nn;
		split(mbr, id, n, nn);

		if (pathBuffer.empty())
		{
			n->m_level = m_level;
			nn->m_level = m_level;
			n->m_identifier = -1;
			nn->m_identifier = -1;
			m_pTree->writeNode(n.get());
			m_pTree->writeNode(nn.get());

			NodePtr ptrR = m_pTree->m_indexPool.acquire();
			if (ptrR.get() == 0)
			{
				ptrR = NodePtr(new Index(m_pTree, m_pTree->m_rootID, m_level + 1), &(m_pTree->m_indexPool));
			}
			else
			{
				//ptrR->m_pTree = m_pTree;
				ptrR->m_identifier = m_pTree->m_rootID;
				ptrR->m_level = m_level + 1;
				ptrR->m_nodeMBR.makeInfinite(2);
			}

			ptrR->insertEntry(n->m_nodeMBR, n->m_identifier);
			ptrR->insertEntry(nn->m_nodeMBR, nn->m_identifier);

			m_pTree->writeNode(ptrR.get());

			m_pTree->m_stats.m_nodesInLevel[m_level] = 2;
			m_pTree->m_stats.m_nodesInLevel.emplace_back(1);
			m_pTree->m_stats.m_u32TreeHeight = m_level + 2;
		}
		else
		{
			n->m_level = m_level;
			nn->m_level = m_level;
			n->m_identifier = m_identifier;
			nn->m_identifier = -1;

			m_pTree->writeNode(n.get());
			m_pTree->writeNode(nn.get());

			id_type cParent = pathBuffer.top(); pathBuffer.pop();
			NodePtr ptrN = m_pTree->readNode(cParent);
			Index* p = static_cast<Index*>(ptrN.get());
			p->adjustTree(n.get(), nn.get(), pathBuffer, overflowTable);
		}

		return true;
	}
}

void Node::reinsertData(xMBR& mbr, id_type id, std::vector<uint32_t>& reinsert, std::vector<uint32_t>& keep)
{
	ReinsertEntry** v = new ReinsertEntry*[m_capacity + 1];

	m_ptrMBR[m_children] = m_pTree->m_xMBRPool.acquire();
	*(m_ptrMBR[m_children]) = mbr;
	m_pIdentifier[m_children] = id;

	xPointPtr nc = m_pTree->m_xPointPool.acquire();
	m_nodeMBR.getCenter(*nc);
	xPointPtr c = m_pTree->m_xPointPool.acquire();

	for (uint32_t u32Child = 0; u32Child < m_capacity + 1; ++u32Child)
	{
		try
		{
			v[u32Child] = new ReinsertEntry(u32Child, 0.0);
		}
		catch (...)
		{
			for (uint32_t i = 0; i < u32Child; ++i) delete v[i];
			delete[] v;
			throw;
		}

		m_ptrMBR[u32Child]->getCenter(*c);

		// calculate relative distance of every entry from the node MBR (ignore square root.)
		for (uint32_t cDim = 0; cDim < 3; ++cDim)
		{
			double d = nc->m_pCoords(cDim) - c->m_pCoords(cDim);
			v[u32Child]->m_dist += d * d;
		}
	}

	// sort by increasing order of distances.
	::qsort(v, m_capacity + 1, sizeof(ReinsertEntry*), ReinsertEntry::compareReinsertEntry);

	uint32_t cReinsert = static_cast<uint32_t>(std::floor((m_capacity + 1) * m_pTree->m_reinsertFactor));

	uint32_t cCount;

	for (cCount = 0; cCount < cReinsert; ++cCount)
	{
		reinsert.emplace_back(v[cCount]->m_index);
		delete v[cCount];
	}

	for (cCount = cReinsert; cCount < m_capacity + 1; ++cCount)
	{
		keep.emplace_back(v[cCount]->m_index);
		delete v[cCount];
	}

	delete[] v;
}

void Node::xRTreeSplit(xMBR& mbr, id_type id, std::vector<uint32_t>& group1, std::vector<uint32_t>& group2)
{
	uint32_t u32Child;
	uint32_t minimumLoad = static_cast<uint32_t>(std::floor(m_capacity * m_pTree->m_fillFactor));

	// use this mask array for marking visited entries.
	uint8_t* mask = new uint8_t[m_capacity + 1];
	memset(mask, 0, m_capacity + 1);

	// insert new data in the node for easier manipulation. Data arrays are always
	// by one larger than node capacity.
	m_ptrMBR[m_capacity] = m_pTree->m_xMBRPool.acquire();
	*(m_ptrMBR[m_capacity]) = mbr;
	m_pIdentifier[m_capacity] = id;
	// m_totalDataLength does not need to be increased here.

	// initialize each group with the seed entries.
	uint32_t seed1, seed2;
	pickSeeds(seed1, seed2);

	group1.emplace_back(seed1);
	group2.emplace_back(seed2);

	mask[seed1] = 1;
	mask[seed2] = 1;

	// find MBR of each group.
	xMBRPtr mbr1 = m_pTree->m_xMBRPool.acquire();
	*mbr1 = *(m_ptrMBR[seed1]);
	xMBRPtr mbr2 = m_pTree->m_xMBRPool.acquire();
	*mbr2 = *(m_ptrMBR[seed2]);

	// count how many entries are left unchecked (exclude the seeds here.)
	uint32_t cRemaining = m_capacity + 1 - 2;

	while (cRemaining > 0)
	{
		if (minimumLoad - group1.size() == cRemaining)
		{
			// all remaining entries must be assigned to group1 to comply with minimun load requirement.
			for (u32Child = 0; u32Child < m_capacity + 1; ++u32Child)
			{
				if (mask[u32Child] == 0)
				{
					group1.emplace_back(u32Child);
					mask[u32Child] = 1;
					--cRemaining;
				}
			}
		}
		else if (minimumLoad - group2.size() == cRemaining)
		{
			// all remaining entries must be assigned to group2 to comply with minimun load requirement.
			for (u32Child = 0; u32Child < m_capacity + 1; ++u32Child)
			{
				if (mask[u32Child] == 0)
				{
					group2.emplace_back(u32Child);
					mask[u32Child] = 1;
					--cRemaining;
				}
			}
		}
		else
		{
			// For all remaining entries compute the difference of the cost of grouping an
			// entry in either group. When done, choose the entry that yielded the maximum
			// difference. In case of linear split, select any entry (e.g. the first one.)
			uint32_t sel;
			double md1 = 0.0, md2 = 0.0;
			double m = -std::numeric_limits<double>::max();
			double d1, d2, d;
			double a1 = mbr1->getArea();
			double a2 = mbr2->getArea();

			xMBRPtr a = m_pTree->m_xMBRPool.acquire();
			xMBRPtr b = m_pTree->m_xMBRPool.acquire();

			for (u32Child = 0; u32Child < m_capacity + 1; ++u32Child)
			{
				if (mask[u32Child] == 0)
				{
					mbr1->getCombinedxMBR(*a, *(m_ptrMBR[u32Child]));
					d1 = a->getArea() - a1;
					mbr2->getCombinedxMBR(*b, *(m_ptrMBR[u32Child]));
					d2 = b->getArea() - a2;
					d = std::abs(d1 - d2);

					if (d > m)
					{
						m = d;
						md1 = d1; md2 = d2;
						sel = u32Child;
						if (m_pTree->m_treeVariant== RV_LINEAR || m_pTree->m_treeVariant == RV_RSTAR) break;
					}
				}
			}

			// determine the group where we should add the new entry.
			int32_t group = -1;

			if (md1 < md2)
			{
				group1.emplace_back(sel);
				group = 1;
			}
			else if (md2 < md1)
			{
				group2.emplace_back(sel);
				group = 2;
			}
			else if (a1 < a2)
			{
				group1.emplace_back(sel);
				group = 1;
			}
			else if (a2 < a1)
			{
				group2.emplace_back(sel);
				group = 2;
			}
			else if (group1.size() < group2.size())
			{
				group1.emplace_back(sel);
				group = 1;
			}
			else if (group2.size() < group1.size())
			{
				group2.emplace_back(sel);
				group = 2;
			}
			else
			{
				group1.emplace_back(sel);
				group = 1;
			}
			mask[sel] = 1;
			--cRemaining;
			if (group == 1)
			{
				mbr1->combinexMBR(*(m_ptrMBR[sel]));
			}
			else
			{
				mbr2->combinexMBR(*(m_ptrMBR[sel]));
			}
		}
	}

	delete[] mask;
}

void Node::rstarSplit(xMBR& mbr, id_type id, std::vector<uint32_t>& group1, std::vector<uint32_t>& group2)
{
	RstarSplitEntry** dataLow = 0;
	RstarSplitEntry** dataHigh = 0;

	try
	{
		dataLow = new RstarSplitEntry*[m_capacity + 1];
		dataHigh = new RstarSplitEntry*[m_capacity + 1];
	}
	catch (...)
	{
		delete[] dataLow;
		throw;
	}

	m_ptrMBR[m_capacity] = m_pTree->m_xMBRPool.acquire();
	*(m_ptrMBR[m_capacity]) = mbr;
	m_pIdentifier[m_capacity] = id;
	// m_totalDataLength does not need to be increased here.

	uint32_t nodeSPF = static_cast<uint32_t>(
		std::floor((m_capacity + 1) * m_pTree->m_splitDistributionFactor));
	uint32_t splitDistribution = (m_capacity + 1) - (2 * nodeSPF) + 2;

	uint32_t u32Child = 0, cDim, cIndex;

	for (u32Child = 0; u32Child <= m_capacity; ++u32Child)
	{
		try
		{
			dataLow[u32Child] = new RstarSplitEntry(m_ptrMBR[u32Child].get(), u32Child, 0);
		}
		catch (...)
		{
			for (uint32_t i = 0; i < u32Child; ++i) delete dataLow[i];
			delete[] dataLow;
			delete[] dataHigh;
			throw;
		}

		dataHigh[u32Child] = dataLow[u32Child];
	}

	double minimumMargin = std::numeric_limits<double>::max();
	uint32_t splitAxis = std::numeric_limits<uint32_t>::max();
	uint32_t sortOrder = std::numeric_limits<uint32_t>::max();

	// chooseSplitAxis.
	for (cDim = 0; cDim < m_pTree->m_dimension; ++cDim)
	{
		::qsort(dataLow, m_capacity + 1, sizeof(RstarSplitEntry*), RstarSplitEntry::compareLow);
		::qsort(dataHigh, m_capacity + 1, sizeof(RstarSplitEntry*), RstarSplitEntry::compareHigh);

		// calculate sum of margins and overlap for all distributions.
		double marginl = 0.0;
		double marginh = 0.0;

		xMBR bbl1, bbl2, bbh1, bbh2;

		for (u32Child = 1; u32Child <= splitDistribution; ++u32Child)
		{
			uint32_t l = nodeSPF - 1 + u32Child;

			bbl1 = *(dataLow[0]->m_pxMBR);
			bbh1 = *(dataHigh[0]->m_pxMBR);

			for (cIndex = 1; cIndex < l; ++cIndex)
			{
				bbl1.combinexMBR(*(dataLow[cIndex]->m_pxMBR));
				bbh1.combinexMBR(*(dataHigh[cIndex]->m_pxMBR));
			}

			bbl2 = *(dataLow[l]->m_pxMBR);
			bbh2 = *(dataHigh[l]->m_pxMBR);

			for (cIndex = l + 1; cIndex <= m_capacity; ++cIndex)
			{
				bbl2.combinexMBR(*(dataLow[cIndex]->m_pxMBR));
				bbh2.combinexMBR(*(dataHigh[cIndex]->m_pxMBR));
			}

			marginl += bbl1.getMargin() + bbl2.getMargin();
			marginh += bbh1.getMargin() + bbh2.getMargin();
		} // for (u32Child)

		double margin = std::min(marginl, marginh);

		// keep minimum margin as split axis.
		if (margin < minimumMargin)
		{
			minimumMargin = margin;
			splitAxis = cDim;
			sortOrder = (marginl < marginh) ? 0 : 1;
		}

		// increase the dimension according to which the data entries should be sorted.
		for (u32Child = 0; u32Child <= m_capacity; ++u32Child)
		{
			dataLow[u32Child]->m_sortDim = cDim + 1;
		}
	} // for (cDim)

	for (u32Child = 0; u32Child <= m_capacity; ++u32Child)
	{
		dataLow[u32Child]->m_sortDim = splitAxis;
	}

	::qsort(dataLow, m_capacity + 1, sizeof(RstarSplitEntry*), (sortOrder == 0) ? RstarSplitEntry::compareLow : RstarSplitEntry::compareHigh);

	double ma = std::numeric_limits<double>::max();
	double mo = std::numeric_limits<double>::max();
	uint32_t splitxPoint = std::numeric_limits<uint32_t>::max();

	xMBR bb1, bb2;

	for (u32Child = 1; u32Child <= splitDistribution; ++u32Child)
	{
		uint32_t l = nodeSPF - 1 + u32Child;

		bb1 = *(dataLow[0]->m_pxMBR);

		for (cIndex = 1; cIndex < l; ++cIndex)
		{
			bb1.combinexMBR(*(dataLow[cIndex]->m_pxMBR));
		}

		bb2 = *(dataLow[l]->m_pxMBR);

		for (cIndex = l + 1; cIndex <= m_capacity; ++cIndex)
		{
			bb2.combinexMBR(*(dataLow[cIndex]->m_pxMBR));
		}

		double o = bb1.getIntersectingArea(bb2);

		if (o < mo)
		{
			splitxPoint = u32Child;
			mo = o;
			ma = bb1.getArea() + bb2.getArea();
		}
		else if (o == mo)
		{
			double a = bb1.getArea() + bb2.getArea();

			if (a < ma)
			{
				splitxPoint = u32Child;
				ma = a;
			}
		}
	} // for (u32Child)

	uint32_t l1 = nodeSPF - 1 + splitxPoint;

	for (cIndex = 0; cIndex < l1; ++cIndex)
	{
		group1.emplace_back(dataLow[cIndex]->m_index);
		delete dataLow[cIndex];
	}

	for (cIndex = l1; cIndex <= m_capacity; ++cIndex)
	{
		group2.emplace_back(dataLow[cIndex]->m_index);
		delete dataLow[cIndex];
	}

	delete[] dataLow;
	delete[] dataHigh;
}

void Node::pickSeeds(uint32_t& index1, uint32_t& index2)
{
	double separation = -std::numeric_limits<double>::max();
	double inefficiency = -std::numeric_limits<double>::max();
	uint32_t cDim, u32Child, cIndex;

	switch (m_pTree->m_treeVariant)
	{
		case RV_LINEAR:
		case RV_RSTAR:
			for (cDim = 0; cDim < m_pTree->m_dimension; ++cDim)
			{
				double leastLower = m_ptrMBR[0]->m_pLow(cDim);
				double greatestUpper = m_ptrMBR[0]->m_pHigh(cDim);
				uint32_t greatestLower = 0;
				uint32_t leastUpper = 0;
				double width;

				for (u32Child = 1; u32Child <= m_capacity; ++u32Child)
				{
					if (m_ptrMBR[u32Child]->m_pLow(cDim) > m_ptrMBR[greatestLower]->m_pLow(cDim)) greatestLower = u32Child;
					if (m_ptrMBR[u32Child]->m_pHigh(cDim) < m_ptrMBR[leastUpper]->m_pHigh(cDim)) leastUpper = u32Child;

					leastLower = std::min(m_ptrMBR[u32Child]->m_pLow(cDim), leastLower);
					greatestUpper = std::max(m_ptrMBR[u32Child]->m_pHigh(cDim), greatestUpper);
				}

				width = greatestUpper - leastLower;
				if (width <= 0) width = 1;

				double f = (m_ptrMBR[greatestLower]->m_pLow(cDim) - m_ptrMBR[leastUpper]->m_pHigh(cDim)) / width;

				if (f > separation)
				{
					index1 = leastUpper;
					index2 = greatestLower;
					separation = f;
				}
			}  // for (cDim)

			if (index1 == index2)
			{
				if (index2 == 0) ++index2;
				else --index2;
			}

			break;
		case RV_QUADRATIC:
			// for each pair of xMBRs (account for overflow xMBR too!)
			for (u32Child = 0; u32Child < m_capacity; ++u32Child)
			{
				double a = m_ptrMBR[u32Child]->getArea();

				for (cIndex = u32Child + 1; cIndex <= m_capacity; ++cIndex)
				{
					// get the combined MBR of those two entries.
					xMBR r;
					m_ptrMBR[u32Child]->getCombinedxMBR(r, *(m_ptrMBR[cIndex]));

					// find the inefficiency of grouping these entries together.
					double d = r.getArea() - a - m_ptrMBR[cIndex]->getArea();

					if (d > inefficiency)
					{
						inefficiency = d;
						index1 = u32Child;
						index2 = cIndex;
					}
				}  // for (cIndex)
			} // for (u32Child)

			break;
		default:
			throw Tools::NotSupportedException("Node::pickSeeds: Tree variant not supported.");
	}
}

void Node::condenseTree(std::stack<NodePtr>& toReinsert, std::stack<id_type>& pathBuffer, NodePtr& ptrThis)
{
	uint32_t minimumLoad = static_cast<uint32_t>(std::floor(m_capacity * m_pTree->m_fillFactor));

	if (pathBuffer.empty())
	{
		// eliminate root if it has only one child.
		if (m_level != 0 && m_children == 1)
		{
			NodePtr ptrN = m_pTree->readNode(m_pIdentifier[0]);
			m_pTree->deleteNode(ptrN.get());
			ptrN->m_identifier = m_pTree->m_rootID;
			m_pTree->writeNode(ptrN.get());

			m_pTree->m_stats.m_nodesInLevel.pop_back();
			m_pTree->m_stats.m_u32TreeHeight -= 1;
			// HACK: pending deleteNode for deleted child will decrease nodesInLevel, later on.
			m_pTree->m_stats.m_nodesInLevel[m_pTree->m_stats.m_u32TreeHeight - 1] = 2;
		}
	}
	else
	{
		id_type cParent = pathBuffer.top(); pathBuffer.pop();
		NodePtr ptrParent = m_pTree->readNode(cParent);
		Index* p = static_cast<Index*>(ptrParent.get());

		// find the entry in the parent, that xPoints to this node.
		uint32_t child;

		for (child = 0; child != p->m_children; ++child)
		{
			if (p->m_pIdentifier[child] == m_identifier) break;
		}

		if (m_children < minimumLoad)
		{
			// used space less than the minimum
			// 1. eliminate node entry from the parent. deleteEntry will fix the parent's MBR.
			p->deleteEntry(child);
			// 2. add this node to the stack in order to reinsert its entries.
			toReinsert.push(ptrThis);
		}
		else
		{
			// adjust the entry in 'p' to contain the new bounding xMBR of this node.
			*(p->m_ptrMBR[child]) = m_nodeMBR;

			// global recalculation necessary since the MBR can only shrink in size,
			// due to data removal.
			if (m_pTree->m_bTightMBRs)
			{
				for (uint32_t cDim = 0; cDim < 3; ++cDim)
				{
					p->m_nodeMBR.m_pLow(cDim) = std::numeric_limits<double>::max();
					p->m_nodeMBR.m_pHigh(cDim) = -std::numeric_limits<double>::max();

					for (uint32_t u32Child = 0; u32Child < p->m_children; ++u32Child)
					{
						p->m_nodeMBR.m_pLow(cDim) = std::min(p->m_nodeMBR.m_pLow(cDim), p->m_ptrMBR[u32Child]->m_pLow(cDim));
						p->m_nodeMBR.m_pHigh(cDim) = std::max(p->m_nodeMBR.m_pHigh(cDim), p->m_ptrMBR[u32Child]->m_pHigh(cDim));
					}
				}
			}
		}

		// write parent node back to storage.
		m_pTree->writeNode(p);

		p->condenseTree(toReinsert, pathBuffer, ptrParent);
	}
}


string Node::toString() const{
    stringstream os;
    os<<m_identifier<<" "<<m_level<<" "<<m_children<<"\n";

    if(m_level >0) {//inner
        for (uint32_t u32Child = 0; u32Child < m_children; u32Child++) {
            os<<(m_pIdentifier[u32Child])<<" "<<*m_ptrMBR[u32Child]<<"\n";
        }
    }else{//leaf
        for (uint32_t u32Child = 0; u32Child < m_children; u32Child++) {
            os<<"idprevnext "<<(m_pIdentifier[u32Child])<<" "<<m_prevNode[u32Child]<<" "
            <<m_nextNode[u32Child]<<"\n";
            os<<"storeEntry"<<m_se[u32Child].m_id<<" "<<m_se[u32Child].m_s<<" "
                <<m_se[u32Child].m_e<<"\n";
            os<<"SBB "<<m_ptrxSBB[u32Child]->toString()<<"\n";
        }
    }
    os<< m_nodeMBR<<"\n";
    return os.str();
}