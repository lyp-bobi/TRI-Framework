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
#include "Node.h"
#include "Leaf.h"
#include "Index.h"
#include "BulkLoader.h"
#include "SBRTree.h"

using namespace SpatialIndex::SBRTree;
using namespace SpatialIndex;

SpatialIndex::SBRTree::Data::Data(uint32_t len, uint8_t* pData, MBC& r, SBR &rg, id_type id)
	: m_id(id), m_mbc(r),m_sbr(rg), m_pData(0), m_dataLength(len)
{
	if (m_dataLength > 0)
	{
		m_pData = new uint8_t[m_dataLength];
		memcpy(m_pData, pData, m_dataLength);
	}
}

SpatialIndex::SBRTree::Data::Data(uint32_t len, uint8_t* pData, MBC& r, id_type id)
        : m_id(id), m_mbc(r), m_pData(0), m_dataLength(len)
{
    m_sbr.makeInfinite(2);
    if (m_dataLength > 0)
    {
        m_pData = new uint8_t[m_dataLength];
        memcpy(m_pData, pData, m_dataLength);
    }
}

SpatialIndex::SBRTree::Data::~Data()
{
	delete[] m_pData;
}

SpatialIndex::SBRTree::Data* SpatialIndex::SBRTree::Data::clone()
{
	return new Data(m_dataLength, m_pData, m_mbc,m_sbr, m_id);
}

id_type SpatialIndex::SBRTree::Data::getIdentifier() const
{
	return m_id;
}

void SpatialIndex::SBRTree::Data::getShape(IShape** out) const
{
	*out = new MBC(m_mbc);
}

void SpatialIndex::SBRTree::Data::getData(uint32_t& len, uint8_t** data) const
{
	len = m_dataLength;
	*data = 0;

	if (m_dataLength > 0)
	{
		*data = new uint8_t[m_dataLength];
		memcpy(*data, m_pData, m_dataLength);
	}
}

uint32_t SpatialIndex::SBRTree::Data::getByteArraySize() const
{
	return
		sizeof(id_type) +
		sizeof(uint32_t) +
		m_dataLength +
		m_sbr.getByteArraySize()+
		m_mbc.getByteArraySize();
}

void SpatialIndex::SBRTree::Data::loadFromByteArray(const uint8_t* ptr)
{
	memcpy(&m_id, ptr, sizeof(id_type));
	ptr += sizeof(id_type);

	delete[] m_pData;
	m_pData = 0;

	memcpy(&m_dataLength, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);

	if (m_dataLength > 0)
	{
		m_pData = new uint8_t[m_dataLength];
		memcpy(m_pData, ptr, m_dataLength);
		ptr += m_dataLength;
	}
    m_sbr.loadFromByteArray(ptr);
	ptr+=m_sbr.getByteArraySize();
	m_mbc.loadFromByteArray(ptr);
}

void SpatialIndex::SBRTree::Data::storeToByteArray(uint8_t** data, uint32_t& len)
{
	// it is thread safe this way.
	uint32_t SBRsize,SBRsize2;
	uint8_t* SBRdata = 0,*SBRdata2=0;
    m_sbr.storeToByteArray(&SBRdata, SBRsize);
	m_mbc.storeToByteArray(&SBRdata2, SBRsize2);

	len = sizeof(id_type) + sizeof(uint32_t) + m_dataLength + SBRsize;

	*data = new uint8_t[len];
	uint8_t* ptr = *data;

	memcpy(ptr, &m_id, sizeof(id_type));
	ptr += sizeof(id_type);
	memcpy(ptr, &m_dataLength, sizeof(uint32_t));
	ptr += sizeof(uint32_t);

	if (m_dataLength > 0)
	{
		memcpy(ptr, m_pData, m_dataLength);
		ptr += m_dataLength;
	}

	memcpy(ptr, SBRdata, SBRsize);
    ptr += SBRsize;
    memcpy(ptr, SBRdata2, SBRsize2);
	delete[] SBRdata;
	delete[] SBRdata2;
	// ptr += SBRsize;
}

SpatialIndex::ISpatialIndex* SpatialIndex::SBRTree::returnSBRTree(SpatialIndex::IStorageManager& sm, Tools::PropertySet& ps)
{
	SpatialIndex::ISpatialIndex* si = new SpatialIndex::SBRTree::SBRTree(sm, ps);
	return si;
}

SpatialIndex::ISpatialIndex* SpatialIndex::SBRTree::createNewSBRTree(
	SpatialIndex::IStorageManager& sm,
	double fillFactor,
	uint32_t indexCapacity,
	uint32_t leafCapacity,
	uint32_t dimension,
	SBRTreeVariant rv,
	id_type& indexIdentifier)
{
	Tools::Variant var;
	Tools::PropertySet ps;

	var.m_varType = Tools::VT_DOUBLE;
	var.m_val.dblVal = fillFactor;
	ps.setProperty("FillFactor", var);

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = indexCapacity;
	ps.setProperty("IndexCapacity", var);

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = leafCapacity;
	ps.setProperty("LeafCapacity", var);

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = dimension;
	ps.setProperty("Dimension", var);

	var.m_varType = Tools::VT_LONG;
	var.m_val.lVal = rv;
	ps.setProperty("TreeVariant", var);

	ISpatialIndex* ret = returnSBRTree(sm, ps);

	var.m_varType = Tools::VT_LONGLONG;
	var = ps.getProperty("IndexIdentifier");
	indexIdentifier = var.m_val.llVal;

	return ret;
}

SpatialIndex::ISpatialIndex* SpatialIndex::SBRTree::createAndBulkLoadNewSBRTree(
	BulkLoadMethod m,
	IDataStream& stream,
	SpatialIndex::IStorageManager& sm,
	double fillFactor,
	uint32_t indexCapacity,
	uint32_t leafCapacity,
	uint32_t dimension,
	SpatialIndex::SBRTree::SBRTreeVariant rv,
	id_type& indexIdentifier)
{
	SpatialIndex::ISpatialIndex* tree = createNewSBRTree(sm, fillFactor, indexCapacity, leafCapacity, dimension, rv, indexIdentifier);

	uint32_t bindex = static_cast<uint32_t>(std::floor(static_cast<double>(indexCapacity * fillFactor)));
	uint32_t bleaf = static_cast<uint32_t>(std::floor(static_cast<double>(leafCapacity * fillFactor)));

	SpatialIndex::SBRTree::BulkLoader bl;

	switch (m)
	{
	case BLM_STR:
		bl.bulkLoadUsingSTR(static_cast<SBRTree*>(tree), stream, bindex, bleaf, 10000, 100);
		break;
	default:
		throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Unknown bulk load method.");
		break;
	}

	return tree;
}

SpatialIndex::ISpatialIndex* SpatialIndex::SBRTree::createAndBulkLoadNewSBRTree(
	BulkLoadMethod m,
	IDataStream& stream,
	SpatialIndex::IStorageManager& sm,
	Tools::PropertySet& ps,
	id_type& indexIdentifier)
{
	Tools::Variant var;
	SBRTreeVariant rv(RV_LINEAR);
	double fillFactor(0.0);
	uint32_t indexCapacity(0);
	uint32_t leafCapacity(0);
	uint32_t dimension(0);
	uint32_t pageSize(0);
	uint32_t numberOfPages(0);

	// tree variant
	var = ps.getProperty("TreeVariant");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (
			var.m_varType != Tools::VT_LONG ||
			(var.m_val.lVal != RV_LINEAR &&
			var.m_val.lVal != RV_QUADRATIC &&
			var.m_val.lVal != RV_RSTAR))
			throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property TreeVariant must be Tools::VT_LONG and of SBRTreeVariant type");

		rv = static_cast<SBRTreeVariant>(var.m_val.lVal);
	}

	// fill factor
	// it cannot be larger than 50%, since linear and quadratic split algorithms
	// require assigning to both nodes the same number of entries.
	var = ps.getProperty("FillFactor");
	if (var.m_varType != Tools::VT_EMPTY)
	{
	    if (var.m_varType != Tools::VT_DOUBLE)
            throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property FillFactor was not of type Tools::VT_DOUBLE");

        if (var.m_val.dblVal <= 0.0)
            throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property FillFactor was less than 0.0");

        if (((rv == RV_LINEAR || rv == RV_QUADRATIC) && var.m_val.dblVal > 0.5))
            throw Tools::IllegalArgumentException( "createAndBulkLoadNewSBRTree: Property FillFactor must be in range (0.0, 0.5) for LINEAR or QUADRATIC index types");
        if ( var.m_val.dblVal >= 1.0)
            throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property FillFactor must be in range (0.0, 1.0) for RSTAR index type");
		fillFactor = var.m_val.dblVal;
	}

	// index capacity
	var = ps.getProperty("IndexCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG || var.m_val.ulVal < 4)
			throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property IndexCapacity must be Tools::VT_ULONG and >= 4");

		indexCapacity = var.m_val.ulVal;
	}

	// leaf capacity
	var = ps.getProperty("LeafCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG )// || var.m_val.ulVal < 4)
			throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property LeafCapacity must be Tools::VT_ULONG and >= 4");

		leafCapacity = var.m_val.ulVal;
	}

	// dimension
	var = ps.getProperty("Dimension");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property Dimension must be Tools::VT_ULONG");
		if (var.m_val.ulVal <= 1)
			throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property Dimension must be greater than 1");

		dimension = var.m_val.ulVal;
	}

	// page size
	var = ps.getProperty("ExternalSortBufferPageSize");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property ExternalSortBufferPageSize must be Tools::VT_ULONG");
		if (var.m_val.ulVal <= 1)
			throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property ExternalSortBufferPageSize must be greater than 1");

		pageSize = var.m_val.ulVal;
	}

	// number of pages
	var = ps.getProperty("ExternalSortBufferTotalPages");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property ExternalSortBufferTotalPages must be Tools::VT_ULONG");
		if (var.m_val.ulVal <= 1)
			throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Property ExternalSortBufferTotalPages must be greater than 1");

		numberOfPages = var.m_val.ulVal;
	}

	SpatialIndex::ISpatialIndex* tree = createNewSBRTree(sm, fillFactor, indexCapacity, leafCapacity, dimension, rv, indexIdentifier);

	uint32_t bindex = static_cast<uint32_t>(std::floor(static_cast<double>(indexCapacity * fillFactor)));
	uint32_t bleaf = static_cast<uint32_t>(std::floor(static_cast<double>(leafCapacity * fillFactor)));

	SpatialIndex::SBRTree::BulkLoader bl;

	switch (m)
	{
	case BLM_STR:
		bl.bulkLoadUsingSTR(static_cast<SBRTree*>(tree), stream, bindex, bleaf, pageSize, numberOfPages);
		break;
	default:
		throw Tools::IllegalArgumentException("createAndBulkLoadNewSBRTree: Unknown bulk load method.");
		break;
	}

	return tree;
}

SpatialIndex::ISpatialIndex* SpatialIndex::SBRTree::loadSBRTree(IStorageManager& sm, id_type indexIdentifier)
{
	Tools::Variant var;
	Tools::PropertySet ps;

	var.m_varType = Tools::VT_LONGLONG;
	var.m_val.llVal = indexIdentifier;
	ps.setProperty("IndexIdentifier", var);

	return returnSBRTree(sm, ps);
}

SpatialIndex::SBRTree::SBRTree::SBRTree(IStorageManager& sm, Tools::PropertySet& ps) :
	m_pStorageManager(&sm),
	m_rootID(StorageManager::NewPage),
	m_headerID(StorageManager::NewPage),
	m_treeVariant(RV_RSTAR),
	m_fillFactor(0.7),
	m_indexCapacity(100),
	m_leafCapacity(100),
	m_nearMinimumOverlapFactor(32),
	m_splitDistributionFactor(0.4),
	m_sbreinsertFactor(0.3),
	m_dimension(2),
	m_bTightSBRs(true),
	m_pointPool(500),
	m_SBRPool(1000),
    m_mbcPool(1000),
	m_indexPool(100),
	m_leafPool(100)
{
#ifdef HAVE_PTHREAD_H
	pthread_mutex_init(&m_lock, NULL);
#endif

	Tools::Variant var = ps.getProperty("IndexIdentifier");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType == Tools::VT_LONGLONG) m_headerID = var.m_val.llVal;
		else if (var.m_varType == Tools::VT_LONG) m_headerID = var.m_val.lVal;
			// for backward compatibility only.
		else throw Tools::IllegalArgumentException("SBRTree: Property IndexIdentifier must be Tools::VT_LONGLONG");

		initOld(ps);
	}
	else
	{
		initNew(ps);
		var.m_varType = Tools::VT_LONGLONG;
		var.m_val.llVal = m_headerID;
		ps.setProperty("IndexIdentifier", var);
	}
}

SpatialIndex::SBRTree::SBRTree::~SBRTree()
{
#ifdef HAVE_PTHREAD_H
	pthread_mutex_destroy(&m_lock);
#endif

	storeHeader();
}

//
// ISpatialIndex interface
//

void SpatialIndex::SBRTree::SBRTree::insertData(uint32_t len, const uint8_t* pData, const IShape& shape, id_type id)
{
//	if (shape.getDimension() != m_dimension) throw Tools::IllegalArgumentException("insertData: Shape has the wrong number of dimensions.");
//
//#ifdef HAVE_PTHREAD_H
//	Tools::LockGuard lock(&m_lock);
//#endif
//
//	// convert the shape into a SBR (R-Trees index SBRs only; i.e., approximations of the shapes).
//	SBRPtr sbr = m_SBRPool.acquire();
//	shape.getSBR(*sbr);
//
//	uint8_t* buffer = 0;
//
//	if (len > 0)
//	{
//		buffer = new uint8_t[len];
//		memcpy(buffer, pData, len);
//	}
//
//	insertData_impl(len, buffer, *sbr, id);
//		// the buffer is stored in the tree. Do not delete here.
    throw Tools::NotSupportedException("SBRTree::insertData");
}

bool SpatialIndex::SBRTree::SBRTree::deleteData(const IShape& shape, id_type id)
{
	throw Tools::NotSupportedException("SBRTree::deleteData");
}


void SpatialIndex::SBRTree::SBRTree::containsWhatQuery(const IShape& query, IVisitor& v)
{
	if (query.getDimension() != m_dimension) throw Tools::IllegalArgumentException("containsWhatQuery: Shape has the wrong number of dimensions.");

#ifdef HAVE_PTHREAD_H
	Tools::LockGuard lock(&m_lock);
#endif

	try
	{
		std::stack<NodePtr> st;
		NodePtr root = readNode(m_rootID);
		st.push(root);

		while (! st.empty())
		{
			NodePtr n = st.top(); st.pop();

			if(n->m_level == 0)
			{
				v.visitNode(*n);

				for (uint32_t cChild = 0; cChild < n->m_children; ++cChild)
				{
					if(query.containsShape(*(n->m_ptrMBC[cChild])))
					{
						Data data = Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBC[cChild]), n->m_pIdentifier[cChild]);
						v.visitData(data);
						++(m_stats.m_u64QueryResults);
					}
				}
			}
			else //not a leaf
			{
				if(query.containsShape(n->m_nodeSBR))
				{
					visitSubTree(n, v);
				}
				else if(query.intersectsShape(n->m_nodeSBR))
				{
					v.visitNode(*n);

					for (uint32_t cChild = 0; cChild < n->m_children; ++cChild)
					{
						st.push(readNode(n->m_pIdentifier[cChild]));
					}
				}
			}
		}

	}
	catch (...)
	{
		throw;
	}
}
void SpatialIndex::SBRTree::SBRTree::intersectsWithQuery(const IShape& query, IVisitor& v)
{
//	if (query.getDimension() != m_dimension)
//	    throw Tools::IllegalArgumentException("intersectsWithQuery: Shape has the wrong number of dimensions.");
	rangeQuery(IntersectionQuery, query, v);
}

void SpatialIndex::SBRTree::SBRTree::pointLocationQuery(const Point& query, IVisitor& v)
{
	if (query.m_dimension != m_dimension) throw Tools::IllegalArgumentException("pointLocationQuery: Shape has the wrong number of dimensions.");
	Region r(query, query);
	rangeQuery(IntersectionQuery, r, v);
}

void SpatialIndex::SBRTree::SBRTree::nearestNeighborQuery(uint32_t k, const IShape& query, IVisitor& v, INearestNeighborComparator& nnc)
{
//	if (query.getDimension() != m_dimension) throw Tools::IllegalArgumentException("nearestNeighborQuery: Shape has the wrong number of dimensions.");
    const Trajectory *queryTraj;
    if(m_DataType==TrajectoryType)
        queryTraj= dynamic_cast<const Trajectory*>(&query);
    if(queryTraj == nullptr||queryTraj->m_points.size()<2) {
        std::cerr << "bad query traj\n";
        return;
    }
#ifdef HAVE_PTHREAD_H
	Tools::LockGuard lock(&m_lock);
#endif

	std::priority_queue<NNEntry*, std::vector<NNEntry*>, NNEntry::ascending> queue;

	queue.push(new NNEntry(m_rootID, 0, 0.0));

	uint32_t count = 0;
	double knearest = 0.0;
    int iternum=0;
    std::map<id_type ,int> insertedTrajId;
    while (! queue.empty())
    {
        iternum++;
        NNEntry* pFirst = queue.top();

        // report all nearest neighbors with equal greatest distances.
        // (neighbors can be more than k, if many happen to have the same greatest distance).
        if (count >= k && pFirst->m_minDist > knearest)	break;

        queue.pop();

        if (pFirst->m_pEntry == nullptr)
        {
            // n is a leaf or an index.
            NodePtr n = readNode(pFirst->m_id);
            v.visitNode(*n);

            for (uint32_t cChild = 0; cChild < n->m_children; ++cChild)
            {
                if (n->m_level == 0)
                {
                    Data* e = new Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBC[cChild]), n->m_pIdentifier[cChild]);
                    // we need to compare the query with the actual data entry here, so we call the
                    // appropriate getMinimumDistance method of NearestNeighborComparator.
                    if(m_DataType==TrajectoryType&&m_bUsingTrajStore== false){
                        Trajectory traj;
                        traj.loadFromByteArray(e->m_pData);
                        queue.push(new NNEntry(n->m_pIdentifier[cChild], e, nnc.getMinimumDistance(*queryTraj, traj)));
                    }else{
//                        queue.push(new NNEntry(n->m_pIdentifier[cChild], e, queryTraj->getMinimumDistance(e->m_mbc)));
                        queue.push(new NNEntry(n->m_pIdentifier[cChild], e, queryTraj->getPeriodMinimumDistance(e->m_mbc,m_ts->m_maxVelocity)));
                    }

                }
                else
                {
                    queue.push(new NNEntry(n->m_pIdentifier[cChild], nullptr, nnc.getMinimumDistance(*queryTraj, *(n->m_ptrSBR[cChild]))));
                }
            }
        }
        else
        {
            if(m_bUsingTrajStore&&pFirst->m_type==0){
                //load ShapeList<MBC>, aka retrieve MBCs
                id_type trajId=m_ts->getTrajId(pFirst->m_id);
                if(insertedTrajId[trajId]==1){}
                else {
                    ShapeList bcs = m_ts->getMBCsByTime(pFirst->m_id, queryTraj->m_points.front().m_time,
                                                        queryTraj->m_points.back().m_time);
                    queue.push(new NNEntry(pFirst->m_id, pFirst->m_pEntry, nnc.getMinimumDistance(*queryTraj, bcs), 1));
                    insertedTrajId[trajId]=1;
                }
//                std::cerr<<nnc.getMinimumDistance(query, bcs)<<"\n";
            }
            else if(m_bUsingTrajStore&&pFirst->m_type==1){
                //load Trajectory
                Trajectory traj=m_ts->getTrajByTime(pFirst->m_id,queryTraj->m_points.front().m_time,queryTraj->m_points.back().m_time);
                queue.push(new NNEntry(pFirst->m_id, pFirst->m_pEntry, nnc.getMinimumDistance(*queryTraj, traj),2));
            }
            else {
                Data *e=static_cast<Data*>(pFirst->m_pEntry);
                v.visitData(*(static_cast<IData *>(pFirst->m_pEntry)));
                ++(m_stats.m_u64QueryResults);
                ++count;
                knearest = pFirst->m_minDist;
//            std::cout<<"knearest is"<<knearest<<std::endl;
                delete pFirst->m_pEntry;
            }
        }
		delete pFirst;
	}

	while (! queue.empty())
	{
		NNEntry* e = queue.top(); queue.pop();
		if (e->m_pEntry != 0) delete e->m_pEntry;
		delete e;
	}
//    std::cout<<"knearest is"<<knearest<<std::endl;
//    std::cerr<<"iternum is "<<iternum<<"\n";
    m_stats.m_doubleExactQueryResults+=knearest;
}

void SpatialIndex::SBRTree::SBRTree::nearestNeighborQuery(uint32_t k, const IShape& query, IVisitor& v)
{
//	if (query.getDimension() != m_dimension) throw Tools::IllegalArgumentException("nearestNeighborQuery: Shape has the wrong number of dimensions.");
	NNComparator nnc;
	nearestNeighborQuery(k, query, v, nnc);
}


void SpatialIndex::SBRTree::SBRTree::selfJoinQuery(const IShape& query, IVisitor& v)
{
    throw Tools::NotSupportedException("SBRTree::deleteData");
//	if (query.getDimension() != m_dimension)
//		throw Tools::IllegalArgumentException("selfJoinQuery: Shape has the wrong number of dimensions.");
//
//#ifdef HAVE_PTHREAD_H
//	Tools::LockGuard lock(&m_lock);
//#endif
//
//	SBRPtr sbr = m_SBRPool.acquire();
//	query.getSBR(*sbr);
//	selfJoinQuery(m_rootID, m_rootID, *sbr, v);
}

void SpatialIndex::SBRTree::SBRTree::queryStrategy(IQueryStrategy& qs)
{
#ifdef HAVE_PTHREAD_H
	Tools::LockGuard lock(&m_lock);
#endif

	id_type next = m_rootID;
	bool hasNext = true;

	while (hasNext)
	{
		NodePtr n = readNode(next);
		qs.getNextEntry(*n, next, hasNext);
	}
}

void SpatialIndex::SBRTree::SBRTree::getIndexProperties(Tools::PropertySet& out) const
{
	Tools::Variant var;

	// dimension
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = m_dimension;
	out.setProperty("Dimension", var);

	// index capacity
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = m_indexCapacity;
	out.setProperty("IndexCapacity", var);

	// leaf capacity
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = m_leafCapacity;
	out.setProperty("LeafCapacity", var);

	// R-tree variant
	var.m_varType = Tools::VT_LONG;
	var.m_val.lVal = m_treeVariant;
	out.setProperty("TreeVariant", var);

	// fill factor
	var.m_varType = Tools::VT_DOUBLE;
	var.m_val.dblVal = m_fillFactor;
	out.setProperty("FillFactor", var);

	// near minimum overlap factor
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = m_nearMinimumOverlapFactor;
	out.setProperty("NearMinimumOverlapFactor", var);

	// split distribution factor
	var.m_varType = Tools::VT_DOUBLE;
	var.m_val.dblVal = m_splitDistributionFactor;
	out.setProperty("SplitDistributionFactor", var);

	// reinsert factor
	var.m_varType = Tools::VT_DOUBLE;
	var.m_val.dblVal = m_sbreinsertFactor;
	out.setProperty("ReinsertFactor", var);

	// tight SBRs
	var.m_varType = Tools::VT_BOOL;
	var.m_val.blVal = m_bTightSBRs;
	out.setProperty("EnsureTightSBRs", var);

	// index pool capacity
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = m_indexPool.getCapacity();
	out.setProperty("IndexPoolCapacity", var);

	// leaf pool capacity
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = m_leafPool.getCapacity();
	out.setProperty("LeafPoolCapacity", var);

	// SBR pool capacity
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = m_SBRPool.getCapacity();
	out.setProperty("SBRPoolCapacity", var);

	// point pool capacity
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = m_pointPool.getCapacity();
	out.setProperty("PointPoolCapacity", var);
}

void SpatialIndex::SBRTree::SBRTree::addCommand(ICommand* pCommand, CommandType ct)
{
	switch (ct)
	{
		case CT_NODEREAD:
			m_sbreadNodeCommands.emplace_back(Tools::SmartPointer<ICommand>(pCommand));
			break;
		case CT_NODEWRITE:
			m_writeNodeCommands.emplace_back(Tools::SmartPointer<ICommand>(pCommand));
			break;
		case CT_NODEDELETE:
			m_deleteNodeCommands.emplace_back(Tools::SmartPointer<ICommand>(pCommand));
			break;
	}
}

bool SpatialIndex::SBRTree::SBRTree::isIndexValid()
{
	bool ret = true;
	std::stack<ValidateEntry> st;
	NodePtr root = readNode(m_rootID);

	if (root->m_level != m_stats.m_u32TreeHeight - 1)
	{
		std::cerr << "Invalid tree height." << std::endl;
		return false;
	}

	std::map<uint32_t, uint32_t> nodesInLevel;
	nodesInLevel.insert(std::pair<uint32_t, uint32_t>(root->m_level, 1));

	ValidateEntry e(root->m_nodeSBR, root);
	st.push(e);

	while (! st.empty())
	{
		e = st.top(); st.pop();

		SBR tmpSBR;
		tmpSBR = m_infiniteSBR;

		for (uint32_t cDim = 0; cDim < tmpSBR.m_dimension; ++cDim)
		{
			tmpSBR.m_pLow[cDim] = std::numeric_limits<double>::max();
			tmpSBR.m_pHigh[cDim] = -std::numeric_limits<double>::max();

			for (uint32_t cChild = 0; cChild < e.m_pNode->m_children; ++cChild)
			{
				tmpSBR.m_pLow[cDim] = std::min(tmpSBR.m_pLow[cDim], e.m_pNode->m_ptrSBR[cChild]->m_pLow[cDim]);
				tmpSBR.m_pHigh[cDim] = std::max(tmpSBR.m_pHigh[cDim], e.m_pNode->m_ptrSBR[cChild]->m_pHigh[cDim]);
			}
		}

		if (! (tmpSBR == e.m_pNode->m_nodeSBR))
		{
			std::cerr << "Invalid parent information." << std::endl;
			ret = false;
		}
		else if (! (tmpSBR == e.m_parentSBR))
		{
			std::cerr << "Error in parent." << std::endl;
			ret = false;
		}

		if (e.m_pNode->m_level != 0)
		{
			for (uint32_t cChild = 0; cChild < e.m_pNode->m_children; ++cChild)
			{
				NodePtr ptrN = readNode(e.m_pNode->m_pIdentifier[cChild]);
				ValidateEntry tmpEntry(*(e.m_pNode->m_ptrSBR[cChild]), ptrN);

				std::map<uint32_t, uint32_t>::iterator itNodes = nodesInLevel.find(tmpEntry.m_pNode->m_level);

				if (itNodes == nodesInLevel.end())
				{
					nodesInLevel.insert(std::pair<uint32_t, uint32_t>(tmpEntry.m_pNode->m_level, 1l));
				}
				else
				{
					nodesInLevel[tmpEntry.m_pNode->m_level] = nodesInLevel[tmpEntry.m_pNode->m_level] + 1;
				}

				st.push(tmpEntry);
			}
		}
	}

	uint32_t nodes = 0;
	for (uint32_t cLevel = 0; cLevel < m_stats.m_u32TreeHeight; ++cLevel)
	{
		if (nodesInLevel[cLevel] != m_stats.m_nodesInLevel[cLevel])
		{
			std::cerr << "Invalid nodesInLevel information." << std::endl;
			ret = false;
		}

		nodes += m_stats.m_nodesInLevel[cLevel];
	}

	if (nodes != m_stats.m_u32Nodes)
	{
		std::cerr << "Invalid number of nodes information." << std::endl;
		ret = false;
	}

	return ret;
}

void SpatialIndex::SBRTree::SBRTree::getStatistics(IStatistics** out) const
{
	*out = new Statistics(m_stats);
}

void SpatialIndex::SBRTree::SBRTree::initNew(Tools::PropertySet& ps)
{
	Tools::Variant var;

	// tree variant
	var = ps.getProperty("TreeVariant");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (
			var.m_varType != Tools::VT_LONG ||
			(var.m_val.lVal != RV_LINEAR &&
			var.m_val.lVal != RV_QUADRATIC &&
			var.m_val.lVal != RV_RSTAR))
			throw Tools::IllegalArgumentException("initNew: Property TreeVariant must be Tools::VT_LONG and of SBRTreeVariant type");

		m_treeVariant = static_cast<SBRTreeVariant>(var.m_val.lVal);
	}

	// fill factor
	// it cannot be larger than 50%, since linear and quadratic split algorithms
	// require assigning to both nodes the same number of entries.
	var = ps.getProperty("FillFactor");
	if (var.m_varType != Tools::VT_EMPTY)
	{
	    if (var.m_varType != Tools::VT_DOUBLE)
            throw Tools::IllegalArgumentException("initNew: Property FillFactor was not of type Tools::VT_DOUBLE");

        if (var.m_val.dblVal <= 0.0)
            throw Tools::IllegalArgumentException("initNew: Property FillFactor was less than 0.0");

        if (((m_treeVariant == RV_LINEAR || m_treeVariant == RV_QUADRATIC) && var.m_val.dblVal > 0.5))
            throw Tools::IllegalArgumentException(  "initNew: Property FillFactor must be in range "
                                                    "(0.0, 0.5) for LINEAR or QUADRATIC index types");
        if ( var.m_val.dblVal >= 1.0)
            throw Tools::IllegalArgumentException(  "initNew: Property FillFactor must be in range "
                                                    "(0.0, 1.0) for RSTAR index type");
		m_fillFactor = var.m_val.dblVal;
	}

	// index capacity
	var = ps.getProperty("IndexCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG || var.m_val.ulVal < 4)
			throw Tools::IllegalArgumentException("initNew: Property IndexCapacity must be Tools::VT_ULONG and >= 4");

		m_indexCapacity = var.m_val.ulVal;
	}

	// leaf capacity
	var = ps.getProperty("LeafCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG )//|| var.m_val.ulVal < 4)
			throw Tools::IllegalArgumentException("initNew: Property LeafCapacity must be Tools::VT_ULONG and >= 4");

		m_leafCapacity = var.m_val.ulVal;
	}

	// near minimum overlap factor
	var = ps.getProperty("NearMinimumOverlapFactor");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (
			var.m_varType != Tools::VT_ULONG ||
			var.m_val.ulVal < 1 ||
			var.m_val.ulVal > m_indexCapacity ||
			var.m_val.ulVal > m_leafCapacity)
			throw Tools::IllegalArgumentException("initNew: Property NearMinimumOverlapFactor must be Tools::VT_ULONG and less than both index and leaf capacities");

		m_nearMinimumOverlapFactor = var.m_val.ulVal;
	}

	// split distribution factor
	var = ps.getProperty("SplitDistributionFactor");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (
			var.m_varType != Tools::VT_DOUBLE ||
			var.m_val.dblVal <= 0.0 ||
			var.m_val.dblVal >= 1.0)
			throw Tools::IllegalArgumentException("initNew: Property SplitDistributionFactor must be Tools::VT_DOUBLE and in (0.0, 1.0)");

		m_splitDistributionFactor = var.m_val.dblVal;
	}

	// reinsert factor
	var = ps.getProperty("ReinsertFactor");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (
			var.m_varType != Tools::VT_DOUBLE ||
			var.m_val.dblVal <= 0.0 ||
			var.m_val.dblVal >= 1.0)
			throw Tools::IllegalArgumentException("initNew: Property ReinsertFactor must be Tools::VT_DOUBLE and in (0.0, 1.0)");

		m_sbreinsertFactor = var.m_val.dblVal;
	}

	// dimension
	var = ps.getProperty("Dimension");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw Tools::IllegalArgumentException("initNew: Property Dimension must be Tools::VT_ULONG");
		if (var.m_val.ulVal <= 1)
			throw Tools::IllegalArgumentException("initNew: Property Dimension must be greater than 1");

		m_dimension = var.m_val.ulVal;
	}

	// tight SBRs
	var = ps.getProperty("EnsureTightSBRs");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_BOOL)
			throw Tools::IllegalArgumentException("initNew: Property EnsureTightSBRs must be Tools::VT_BOOL");

		m_bTightSBRs = var.m_val.blVal;
	}

	// index pool capacity
	var = ps.getProperty("IndexPoolCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw Tools::IllegalArgumentException("initNew: Property IndexPoolCapacity must be Tools::VT_ULONG");

		m_indexPool.setCapacity(var.m_val.ulVal);
	}

	// leaf pool capacity
	var = ps.getProperty("LeafPoolCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw Tools::IllegalArgumentException("initNew: Property LeafPoolCapacity must be Tools::VT_ULONG");

		m_leafPool.setCapacity(var.m_val.ulVal);
	}

	// SBR pool capacity
	var = ps.getProperty("SBRPoolCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw Tools::IllegalArgumentException("initNew: Property SBRPoolCapacity must be Tools::VT_ULONG");

		m_SBRPool.setCapacity(var.m_val.ulVal);
	}

	// point pool capacity
	var = ps.getProperty("PointPoolCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw Tools::IllegalArgumentException("initNew: Property PointPoolCapacity must be Tools::VT_ULONG");

		m_pointPool.setCapacity(var.m_val.ulVal);
	}

	m_infiniteSBR.makeInfinite(m_dimension);

	m_stats.m_u32TreeHeight = 1;
	m_stats.m_nodesInLevel.emplace_back(0);

	Leaf root(this, -1);
	m_rootID = writeNode(&root);

	storeHeader();
}

void SpatialIndex::SBRTree::SBRTree::initOld(Tools::PropertySet& ps)
{
	loadHeader();

	// only some of the properties may be changed.
	// the rest are just ignored.

	Tools::Variant var;

	// tree variant
	var = ps.getProperty("TreeVariant");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (
			var.m_varType != Tools::VT_LONG ||
			(var.m_val.lVal != RV_LINEAR &&
			 var.m_val.lVal != RV_QUADRATIC &&
			 var.m_val.lVal != RV_RSTAR))
			throw Tools::IllegalArgumentException("initOld: Property TreeVariant must be Tools::VT_LONG and of SBRTreeVariant type");

		m_treeVariant = static_cast<SBRTreeVariant>(var.m_val.lVal);
	}

	// near minimum overlap factor
	var = ps.getProperty("NearMinimumOverlapFactor");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (
			var.m_varType != Tools::VT_ULONG ||
			var.m_val.ulVal < 1 ||
			var.m_val.ulVal > m_indexCapacity ||
			var.m_val.ulVal > m_leafCapacity)
			throw Tools::IllegalArgumentException("initOld: Property NearMinimumOverlapFactor must be Tools::VT_ULONG and less than both index and leaf capacities");

		m_nearMinimumOverlapFactor = var.m_val.ulVal;
	}

	// split distribution factor
	var = ps.getProperty("SplitDistributionFactor");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_DOUBLE || var.m_val.dblVal <= 0.0 || var.m_val.dblVal >= 1.0)
			throw Tools::IllegalArgumentException("initOld: Property SplitDistributionFactor must be Tools::VT_DOUBLE and in (0.0, 1.0)");

		m_splitDistributionFactor = var.m_val.dblVal;
	}

	// reinsert factor
	var = ps.getProperty("ReinsertFactor");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_DOUBLE || var.m_val.dblVal <= 0.0 || var.m_val.dblVal >= 1.0)
			throw Tools::IllegalArgumentException("initOld: Property ReinsertFactor must be Tools::VT_DOUBLE and in (0.0, 1.0)");

		m_sbreinsertFactor = var.m_val.dblVal;
	}

	// tight SBRs
	var = ps.getProperty("EnsureTightSBRs");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_BOOL) throw Tools::IllegalArgumentException("initOld: Property EnsureTightSBRs must be Tools::VT_BOOL");

		m_bTightSBRs = var.m_val.blVal;
	}

	// index pool capacity
	var = ps.getProperty("IndexPoolCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) throw Tools::IllegalArgumentException("initOld: Property IndexPoolCapacity must be Tools::VT_ULONG");

		m_indexPool.setCapacity(var.m_val.ulVal);
	}

	// leaf pool capacity
	var = ps.getProperty("LeafPoolCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) throw Tools::IllegalArgumentException("initOld: Property LeafPoolCapacity must be Tools::VT_ULONG");

		m_leafPool.setCapacity(var.m_val.ulVal);
	}

	// SBR pool capacity
	var = ps.getProperty("SBRPoolCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) throw Tools::IllegalArgumentException("initOld: Property SBRPoolCapacity must be Tools::VT_ULONG");

		m_SBRPool.setCapacity(var.m_val.ulVal);
	}

	// point pool capacity
	var = ps.getProperty("PointPoolCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG) throw Tools::IllegalArgumentException("initOld: Property PointPoolCapacity must be Tools::VT_ULONG");

		m_pointPool.setCapacity(var.m_val.ulVal);
	}

	m_infiniteSBR.makeInfinite(m_dimension);
}

void SpatialIndex::SBRTree::SBRTree::storeHeader()
{
	const uint32_t headerSize =
		sizeof(id_type) +						// m_rootID
		sizeof(SBRTreeVariant) +					// m_treeVariant
		sizeof(double) +						// m_fillFactor
		sizeof(uint32_t) +						// m_indexCapacity
		sizeof(uint32_t) +						// m_leafCapacity
		sizeof(uint32_t) +						// m_nearMinimumOverlapFactor
		sizeof(double) +						// m_splitDistributionFactor
		sizeof(double) +						// m_sbreinsertFactor
		sizeof(uint32_t) +						// m_dimension
		sizeof(char) +							// m_bTightSBRs
		sizeof(uint32_t) +						// m_stats.m_nodes
		sizeof(uint64_t) +						// m_stats.m_data
		sizeof(uint32_t) +						// m_stats.m_treeHeight
		m_stats.m_u32TreeHeight * sizeof(uint32_t);	// m_stats.m_nodesInLevel

	uint8_t* header = new uint8_t[headerSize];
	uint8_t* ptr = header;

	memcpy(ptr, &m_rootID, sizeof(id_type));
	ptr += sizeof(id_type);
	memcpy(ptr, &m_treeVariant, sizeof(SBRTreeVariant));
	ptr += sizeof(SBRTreeVariant);
	memcpy(ptr, &m_fillFactor, sizeof(double));
	ptr += sizeof(double);
	memcpy(ptr, &m_indexCapacity, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	memcpy(ptr, &m_leafCapacity, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	memcpy(ptr, &m_nearMinimumOverlapFactor, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	memcpy(ptr, &m_splitDistributionFactor, sizeof(double));
	ptr += sizeof(double);
	memcpy(ptr, &m_sbreinsertFactor, sizeof(double));
	ptr += sizeof(double);
	memcpy(ptr, &m_dimension, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	char c = (char) m_bTightSBRs;
	memcpy(ptr, &c, sizeof(char));
	ptr += sizeof(char);
	memcpy(ptr, &(m_stats.m_u32Nodes), sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	memcpy(ptr, &(m_stats.m_u64Data), sizeof(uint64_t));
	ptr += sizeof(uint64_t);
	memcpy(ptr, &(m_stats.m_u32TreeHeight), sizeof(uint32_t));
	ptr += sizeof(uint32_t);

	for (uint32_t cLevel = 0; cLevel < m_stats.m_u32TreeHeight; ++cLevel)
	{
		memcpy(ptr, &(m_stats.m_nodesInLevel[cLevel]), sizeof(uint32_t));
		ptr += sizeof(uint32_t);
	}

	m_pStorageManager->storeByteArray(m_headerID, headerSize, header);

	delete[] header;
}

void SpatialIndex::SBRTree::SBRTree::loadHeader()
{
	uint32_t headerSize;
	uint8_t* header = 0;
	m_pStorageManager->loadByteArray(m_headerID, headerSize, &header);

	uint8_t* ptr = header;

	memcpy(&m_rootID, ptr, sizeof(id_type));
	ptr += sizeof(id_type);
	memcpy(&m_treeVariant, ptr, sizeof(SBRTreeVariant));
	ptr += sizeof(SBRTreeVariant);
	memcpy(&m_fillFactor, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_indexCapacity, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	memcpy(&m_leafCapacity, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	memcpy(&m_nearMinimumOverlapFactor, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	memcpy(&m_splitDistributionFactor, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_sbreinsertFactor, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_dimension, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	char c;
	memcpy(&c, ptr, sizeof(char));
	m_bTightSBRs = (c != 0);
	ptr += sizeof(char);
	memcpy(&(m_stats.m_u32Nodes), ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	memcpy(&(m_stats.m_u64Data), ptr, sizeof(uint64_t));
	ptr += sizeof(uint64_t);
	memcpy(&(m_stats.m_u32TreeHeight), ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);

	for (uint32_t cLevel = 0; cLevel < m_stats.m_u32TreeHeight; ++cLevel)
	{
		uint32_t cNodes;
		memcpy(&cNodes, ptr, sizeof(uint32_t));
		ptr += sizeof(uint32_t);
		m_stats.m_nodesInLevel.emplace_back(cNodes);
	}

	delete[] header;
}

void SpatialIndex::SBRTree::SBRTree::insertData_impl(uint32_t dataLength, uint8_t* pData, SBR& sbr, id_type id)
{
	assert(sbr.getDimension() == m_dimension);

	std::stack<id_type> pathBuffer;
	uint8_t* overflowTable = 0;

	try
	{
		NodePtr root = readNode(m_rootID);

		overflowTable = new uint8_t[root->m_level];
		memset(overflowTable, 0, root->m_level);

		NodePtr l = root->chooseSubtree(sbr, 0, pathBuffer);
		if (l.get() == root.get())
		{
			assert(root.unique());
			root.relinquish();
		}
		l->insertData(dataLength, pData, sbr, id, pathBuffer, overflowTable);

		delete[] overflowTable;
		++(m_stats.m_u64Data);
	}
	catch (...)
	{
		delete[] overflowTable;
		throw;
	}
}

void SpatialIndex::SBRTree::SBRTree::insertData_impl(uint32_t dataLength, uint8_t* pData, SBR& sbr, id_type id, uint32_t level, uint8_t* overflowTable)
{
	assert(sbr.getDimension() == m_dimension);

	std::stack<id_type> pathBuffer;
	NodePtr root = readNode(m_rootID);
	NodePtr n = root->chooseSubtree(sbr, level, pathBuffer);

	assert(n->m_level == level);

	if (n.get() == root.get())
	{
		assert(root.unique());
		root.relinquish();
	}
	n->insertData(dataLength, pData, sbr, id, pathBuffer, overflowTable);
}

bool SpatialIndex::SBRTree::SBRTree::deleteData_impl(const SBR& sbr, id_type id)
{
	assert(sbr.m_dimension == m_dimension);

	std::stack<id_type> pathBuffer;
	NodePtr root = readNode(m_rootID);
	NodePtr l = root->findLeaf(sbr, id, pathBuffer);
	if (l.get() == root.get())
	{
		assert(root.unique());
		root.relinquish();
	}

	if (l.get() != 0)
	{
		Leaf* pL = static_cast<Leaf*>(l.get());
		pL->deleteData(id, pathBuffer);
		--(m_stats.m_u64Data);
		return true;
	}

	return false;
}

SpatialIndex::id_type SpatialIndex::SBRTree::SBRTree::writeNode(Node* n)
{
	uint8_t* buffer;
	uint32_t dataLength;
	n->storeToByteArray(&buffer, dataLength);

	id_type page;
	if (n->m_identifier < 0) page = StorageManager::NewPage;
	else page = n->m_identifier;

	try
	{
		m_pStorageManager->storeByteArray(page, dataLength, buffer);
		delete[] buffer;
	}
	catch (InvalidPageException& e)
	{
		delete[] buffer;
		std::cerr << e.what() << std::endl;
		throw;
	}

	if (n->m_identifier < 0)
	{
		n->m_identifier = page;
		++(m_stats.m_u32Nodes);

#ifndef NDEBUG
		try
		{
			m_stats.m_nodesInLevel[n->m_level] = m_stats.m_nodesInLevel.at(n->m_level) + 1;
		}
		catch(...)
		{
			throw Tools::IllegalStateException("writeNode: writing past the end of m_nodesInLevel.");
		}
#else
		m_stats.m_nodesInLevel[n->m_level] = m_stats.m_nodesInLevel[n->m_level] + 1;
#endif
	}

	++(m_stats.m_u64Writes);

	for (size_t cIndex = 0; cIndex < m_writeNodeCommands.size(); ++cIndex)
	{
		m_writeNodeCommands[cIndex]->execute(*n);
	}

	return page;
}

SpatialIndex::SBRTree::NodePtr SpatialIndex::SBRTree::SBRTree::readNode(id_type page)
{
	uint32_t dataLength;
	uint8_t* buffer;

	try
	{
		m_pStorageManager->loadByteArray(page, dataLength, &buffer);
	}
	catch (InvalidPageException& e)
	{
		std::cerr << e.what() << std::endl;
		throw;
	}

	try
	{
		uint32_t nodeType;
		memcpy(&nodeType, buffer, sizeof(uint32_t));

		NodePtr n;

		if (nodeType == PersistentIndex) n = m_indexPool.acquire();
		else if (nodeType == PersistentLeaf) n = m_leafPool.acquire();
		else throw Tools::IllegalStateException("readNode: failed reading the correct node type information");

		if (n.get() == 0)
		{
			if (nodeType == PersistentIndex) n = NodePtr(new Index(this, -1, 0), &m_indexPool);
			else if (nodeType == PersistentLeaf) n = NodePtr(new Leaf(this, -1), &m_leafPool);
		}

		//n->m_pTree = this;
		n->m_identifier = page;
		n->loadFromByteArray(buffer);

		++(m_stats.m_u64Reads);

		for (size_t cIndex = 0; cIndex < m_sbreadNodeCommands.size(); ++cIndex)
		{
			m_sbreadNodeCommands[cIndex]->execute(*n);
		}

		delete[] buffer;
		return n;
	}
	catch (...)
	{
		delete[] buffer;
		throw;
	}
}

void SpatialIndex::SBRTree::SBRTree::deleteNode(Node* n)
{
	try
	{
		m_pStorageManager->deleteByteArray(n->m_identifier);
	}
	catch (InvalidPageException& e)
	{
		std::cerr << e.what() << std::endl;
		throw;
	}

	--(m_stats.m_u32Nodes);
	m_stats.m_nodesInLevel[n->m_level] = m_stats.m_nodesInLevel[n->m_level] - 1;

	for (size_t cIndex = 0; cIndex < m_deleteNodeCommands.size(); ++cIndex)
	{
		m_deleteNodeCommands[cIndex]->execute(*n);
	}
}

void SpatialIndex::SBRTree::SBRTree::rangeQuery(RangeQueryType type, const IShape& query, IVisitor& v)
{
#ifdef HAVE_PTHREAD_H
    Tools::LockGuard lock(&m_lock);
#endif

    std::stack<NodePtr> st;
    NodePtr root = readNode(m_rootID);

    if (root->m_children > 0 && root->m_nodeSBR.intersectsShape(query)) st.push(root);

    while (! st.empty())
    {

        NodePtr n = st.top(); st.pop();

        if (n->m_level == 0)
        {
            v.visitNode(*n);
            for (uint32_t cChild = 0; cChild < n->m_children; ++cChild)
            {
                bool b;
                if (type == ContainmentQuery) b = n->m_ptrMBC[cChild]->containsShape(query);
                else b = n->m_ptrMBC[cChild]->intersectsShape(query);
                if (b)
                {
                    Data data = Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBC[cChild]), n->m_pIdentifier[cChild]);
                    ++(m_stats.m_u64QueryResults);
                    if(m_DataType==TrajectoryType){
                        //check if the timed slice is included in query
                        const Region *querybr= dynamic_cast<const Region*>(&query);
                        Region spatialbr(querybr->m_pLow,querybr->m_pHigh,2);
                        Region bcbr;
                        data.m_mbc.getMBRAtTime(querybr->m_pLow[2],bcbr);
                        if(spatialbr.containsRegion(bcbr)){
                            m_stats.m_doubleExactQueryResults += 1;
                            v.visitData(data);
                        }else {
                            if (m_bUsingTrajStore == true) {
                                Trajectory segtraj = m_ts->getTraj(n->m_pIdentifier[cChild]);
                                if (query.intersectsShape(segtraj)) {
                                    m_stats.m_doubleExactQueryResults += 1;
                                    v.visitData(data);
                                }
                            } else {
                                Trajectory traj;
                                traj.loadFromByteArray(data.m_pData);
//                            v.visitData(data);
                                if (traj.intersectsShape(query)) {
                                    m_stats.m_doubleExactQueryResults += 1;
                                    v.visitData(data);
                                }
                            }
                        }
                    }else{
                        v.visitData(data);
                    }
                }
            }
        }
        else
        {
            v.visitNode(*n);
            for (uint32_t cChild = 0; cChild < n->m_children; ++cChild)
            {
                if (query.intersectsShape(*(n->m_ptrSBR[cChild])))
                {
                    st.push(readNode(n->m_pIdentifier[cChild]));
                }
            }
        }
    }
}

void SpatialIndex::SBRTree::SBRTree::selfJoinQuery(id_type id1, id_type id2, const SBR& r, IVisitor& vis)
{
    throw Tools::NotSupportedException("SBRTree::selfJoinQuery");
//	NodePtr n1 = readNode(id1);
//	NodePtr n2 = readNode(id2);
//	vis.visitNode(*n1);
//	vis.visitNode(*n2);
//
//	for (uint32_t cChild1 = 0; cChild1 < n1->m_children; ++cChild1)
//	{
//		if (r.intersectsSBR(*(n1->m_ptrSBR[cChild1])))
//		{
//			for (uint32_t cChild2 = 0; cChild2 < n2->m_children; ++cChild2)
//			{
//				if (
//					r.intersectsSBR(*(n2->m_ptrSBR[cChild2])) &&
//					n1->m_ptrSBR[cChild1]->intersectsSBR(*(n2->m_ptrSBR[cChild2])))
//				{
//					if (n1->m_level == 0)
//					{
//						if (n1->m_pIdentifier[cChild1] != n2->m_pIdentifier[cChild2])
//						{
//							assert(n2->m_level == 0);
//
//							std::vector<const IData*> v;
//							Data e1(n1->m_pDataLength[cChild1], n1->m_pData[cChild1], *(n1->m_ptrMBC[cChild1]), n1->m_pIdentifier[cChild1]);
//							Data e2(n2->m_pDataLength[cChild2], n2->m_pData[cChild2], *(n2->m_ptrMBC[cChild2]), n2->m_pIdentifier[cChild2]);
//							v.emplace_back(&e1);
//							v.emplace_back(&e2);
//							vis.visitData(v);
//						}
//					}
//					else
//					{
//						SBR rr = r.getIntersectingSBR(n1->m_ptrSBR[cChild1]->getIntersectingSBR(*(n2->m_ptrSBR[cChild2])));
//						selfJoinQuery(n1->m_pIdentifier[cChild1], n2->m_pIdentifier[cChild2], rr, vis);
//					}
//				}
//			}
//		}
//	}
}

void SpatialIndex::SBRTree::SBRTree::visitSubTree(NodePtr subTree, IVisitor& v)
{
	std::stack<NodePtr> st;
	st.push(subTree);

	while (! st.empty())
	{
		NodePtr n = st.top(); st.pop();
		v.visitNode(*n);

		if(n->m_level == 0)
		{
			for (uint32_t cChild = 0; cChild < n->m_children; ++cChild)
			{
				Data data = Data(n->m_pDataLength[cChild], n->m_pData[cChild], *(n->m_ptrMBC[cChild]), n->m_pIdentifier[cChild]);
				v.visitData(data);
				++(m_stats.m_u64QueryResults);
			}
		}
		else
		{
			for (uint32_t cChild = 0; cChild < n->m_children; ++cChild)
			{
				st.push(readNode(n->m_pIdentifier[cChild]));
			}
		}
	}
}

std::ostream& SpatialIndex::SBRTree::operator<<(std::ostream& os, const SBRTree& t)
{
	os	<< "Dimension: " << t.m_dimension << std::endl
		<< "Fill factor: " << t.m_fillFactor << std::endl
		<< "Index capacity: " << t.m_indexCapacity << std::endl
		<< "Leaf capacity: " << t.m_leafCapacity << std::endl
		<< "Tight SBRs: " << ((t.m_bTightSBRs) ? "enabled" : "disabled") << std::endl;

	if (t.m_treeVariant == RV_RSTAR)
	{
		os	<< "Near minimum overlap factor: " << t.m_nearMinimumOverlapFactor << std::endl
			<< "Reinsert factor: " << t.m_sbreinsertFactor << std::endl
			<< "Split distribution factor: " << t.m_splitDistributionFactor << std::endl;
	}

	if (t.m_stats.getNumberOfNodesInLevel(0) > 0)
		os	<< "Utilization: " << 100 * t.m_stats.getNumberOfData() / (t.m_stats.getNumberOfNodesInLevel(0) * t.m_leafCapacity) << "%" << std::endl
			<< t.m_stats;

	#ifndef NDEBUG
	os	<< "Leaf pool hits: " << t.m_leafPool.m_hits << std::endl
		<< "Leaf pool misses: " << t.m_leafPool.m_misses << std::endl
		<< "Index pool hits: " << t.m_indexPool.m_hits << std::endl
		<< "Index pool misses: " << t.m_indexPool.m_misses << std::endl
		<< "SBR pool hits: " << t.m_SBRPool.m_hits << std::endl
		<< "SBR pool misses: " << t.m_SBRPool.m_misses << std::endl
        << "Point pool hits: " << t.m_pointPool.m_hits << std::endl
        << "Point pool misses: " << t.m_pointPool.m_misses << std::endl;
#endif
    return os;
}