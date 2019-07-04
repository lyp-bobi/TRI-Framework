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
#include <stdio.h>
#include <cmath>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include <spatialindex/SpatialIndex.h>

#include "SBRTree.h"
#include "Leaf.h"
#include "Index.h"
#include "BulkLoader.h"

using namespace SpatialIndex;
using namespace SpatialIndex::SBRTree;

//
// ExternalSorter::Record
//
ExternalSorter::Record::Record()
: m_pData(0)
{
}

ExternalSorter::Record::Record(const SBR& r, id_type id, uint32_t len, uint8_t* pData, uint32_t s,uint32_t level,MBC *mbc)
        :  m_id(id), m_len(len), m_pData(pData), m_s(s),m_level(level),m_sbr(r)
{
    if(mbc!= nullptr){
        m_mbc=*mbc;
    }
}

ExternalSorter::Record::~Record()
{
	delete[] m_pData;
}

bool ExternalSorter::Record::operator<(const Record& r) const
{
    if (m_s != r.m_s)
        throw Tools::IllegalStateException("ExternalSorter::Record::operator<: Incompatible sorting dimensions.");

    if(m_s<m_sbr.m_dimension){
        return m_sbr.m_pLow[m_s]<r.m_sbr.m_pLow[m_s];
    }else{
        return m_sbr.m_pHigh[m_s-m_sbr.m_dimension]<r.m_sbr.m_pHigh[m_s-m_sbr.m_dimension];
    }
}

void ExternalSorter::Record::storeToFile(Tools::TemporaryFile& f)
{
	f.write(static_cast<uint64_t>(m_id));
	f.write(m_sbr.m_dimension);
	f.write(m_s);
	f.write(m_level);

	for (uint32_t i = 0; i < m_sbr.m_dimension; ++i)
	{
		f.write(m_sbr.m_pLow[i]);
		f.write(m_sbr.m_pHigh[i]);
	}
	if(m_level==0) {
        for (uint32_t i = 0; i < m_mbc.m_dimension; ++i) {
            f.write(m_mbc.m_pLow[i]);
            f.write(m_mbc.m_pHigh[i]);
        }
        f.write(m_mbc.m_startTime);
        f.write(m_mbc.m_endTime);
        f.write(m_mbc.m_rd);
        f.write(m_mbc.m_rv);
    }
	f.write(m_len);
	if (m_len > 0) f.write(m_len, m_pData);
}

void ExternalSorter::Record::loadFromFile(Tools::TemporaryFile& f)
{
	m_id = static_cast<id_type>(f.readUInt64());
	uint32_t dim = f.readUInt32();
	m_s = f.readUInt32();
	m_level=f.readUInt32();

	if (dim != m_sbr.m_dimension)
	{
		delete[] m_sbr.m_pLow;
		delete[] m_sbr.m_pHigh;
		m_sbr.m_dimension = dim;
		m_sbr.m_pLow = new double[dim];
		m_sbr.m_pHigh = new double[dim];
	}

	for (uint32_t i = 0; i < m_sbr.m_dimension; ++i)
	{
		m_sbr.m_pLow[i] = f.readDouble();
		m_sbr.m_pHigh[i] = f.readDouble();
	}
	if(m_level==0) {
        m_mbc.makeInfinite(dim - 1);
        for (uint32_t i = 0; i < m_mbc.m_dimension; ++i) {
            m_mbc.m_pLow[i] = f.readDouble();
            m_mbc.m_pHigh[i] = f.readDouble();
        }
        m_mbc.m_startTime = f.readDouble();
        m_mbc.m_endTime = f.readDouble();
        m_mbc.m_rd = f.readDouble();
        m_mbc.m_rv = f.readDouble();
    }
	m_len = f.readUInt32();
	delete[] m_pData; m_pData = 0;
	if (m_len > 0) f.readBytes(m_len, &m_pData);
}

//
// ExternalSorter
//
ExternalSorter::ExternalSorter(uint32_t u32PageSize, uint32_t u32BufferPages)
: m_bInsertionPhase(true), m_u32PageSize(u32PageSize),
  m_u32BufferPages(u32BufferPages), m_u64TotalEntries(0), m_stI(0)
{
}

ExternalSorter::~ExternalSorter()
{
	for (m_stI = 0; m_stI < m_buffer.size(); ++m_stI) delete m_buffer[m_stI];
}

void ExternalSorter::insert(Record* r)
{
	if (m_bInsertionPhase == false)
		throw Tools::IllegalStateException("ExternalSorter::insert: Input has already been sorted.");

	m_buffer.emplace_back(r);
	++m_u64TotalEntries;

	// this will create the initial, sorted buckets before the
	// external merge sort.
	if (m_buffer.size() >= m_u32PageSize * m_u32BufferPages)
	{
		std::sort(m_buffer.begin(), m_buffer.end(), Record::SortAscending());
		Tools::TemporaryFile* tf = new Tools::TemporaryFile();
		for (size_t j = 0; j < m_buffer.size(); ++j)
		{
			m_buffer[j]->storeToFile(*tf);
			delete m_buffer[j];
		}
		m_buffer.clear();
		tf->rewindForReading();
		m_runs.emplace_back(Tools::SmartPointer<Tools::TemporaryFile>(tf));
	}
}

void ExternalSorter::sort()
{
	if (m_bInsertionPhase == false)
		throw Tools::IllegalStateException("ExternalSorter::sort: Input has already been sorted.");

	if (m_runs.empty())
	{
		// The data fits in main memory. No need to store to disk.
		std::sort(m_buffer.begin(), m_buffer.end(), Record::SortAscending());
		m_bInsertionPhase = false;
		return;
	}

	if (m_buffer.size() > 0)
	{
		// Whatever remained in the buffer (if not filled) needs to be stored
		// as the final bucket.
		std::sort(m_buffer.begin(), m_buffer.end(), Record::SortAscending());
		Tools::TemporaryFile* tf = new Tools::TemporaryFile();
		for (size_t j = 0; j < m_buffer.size(); ++j)
		{
			m_buffer[j]->storeToFile(*tf);
			delete m_buffer[j];
		}
		m_buffer.clear();
		tf->rewindForReading();
		m_runs.emplace_back(Tools::SmartPointer<Tools::TemporaryFile>(tf));
	}

	if (m_runs.size() == 1)
	{
		m_sortedFile = m_runs.front();
	}
	else
	{
		Record* r = 0;

		while (m_runs.size() > 1)
		{
			Tools::SmartPointer<Tools::TemporaryFile> tf(new Tools::TemporaryFile());
			std::vector<Tools::SmartPointer<Tools::TemporaryFile> > buckets;
			std::vector<std::queue<Record*> > buffers;
			std::priority_queue<PQEntry, std::vector<PQEntry>, PQEntry::SortAscending> pq;

			// initialize buffers and priority queue.
			std::list<Tools::SmartPointer<Tools::TemporaryFile> >::iterator it = m_runs.begin();
			for (uint32_t i = 0; i < (std::min)(static_cast<uint32_t>(m_runs.size()), m_u32BufferPages); ++i)
			{
				buckets.emplace_back(*it);
				buffers.emplace_back(std::queue<Record*>());

				r = new Record();
				r->loadFromFile(**it);
					// a run cannot be empty initially, so this should never fail.
				pq.push(PQEntry(r, i));

				for (uint32_t j = 0; j < m_u32PageSize - 1; ++j)
				{
					// fill the buffer with the rest of the page of records.
					try
					{
						r = new Record();
						r->loadFromFile(**it);
						buffers.back().push(r);
					}
					catch (Tools::EndOfStreamException)
					{
						delete r;
						break;
					}
				}
				++it;
			}

			// exhaust buckets, buffers, and priority queue.
			while (! pq.empty())
			{
				PQEntry e = pq.top(); pq.pop();
				e.m_sbr->storeToFile(*tf);
				delete e.m_sbr;

				if (! buckets[e.m_u32Index]->eof() && buffers[e.m_u32Index].empty())
				{
					for (uint32_t j = 0; j < m_u32PageSize; ++j)
					{
						try
						{
							r = new Record();
							r->loadFromFile(*buckets[e.m_u32Index]);
							buffers[e.m_u32Index].push(r);
						}
						catch (Tools::EndOfStreamException)
						{
							delete r;
							break;
						}
					}
				}

				if (! buffers[e.m_u32Index].empty())
				{
					e.m_sbr = buffers[e.m_u32Index].front();
					buffers[e.m_u32Index].pop();
					pq.push(e);
				}
			}

			tf->rewindForReading();

			// check if another pass is needed.
			uint32_t u32Count = std::min(static_cast<uint32_t>(m_runs.size()), m_u32BufferPages);
			for (uint32_t i = 0; i < u32Count; ++i)
			{
				m_runs.pop_front();
			}

			if (m_runs.size() == 0)
			{
				m_sortedFile = tf;
				break;
			}
			else
			{
				m_runs.emplace_back(tf);
			}
		}
	}

	m_bInsertionPhase = false;
}

ExternalSorter::Record* ExternalSorter::getNextRecord()
{
	if (m_bInsertionPhase == true)
		throw Tools::IllegalStateException("ExternalSorter::getNextRecord: Input has not been sorted yet.");

	Record* ret;

	if (m_sortedFile.get() == 0)
	{
		if (m_stI < m_buffer.size())
		{
			ret = m_buffer[m_stI];
			m_buffer[m_stI] = 0;
			++m_stI;
		}
		else
			throw Tools::EndOfStreamException("");
	}
	else
	{
		ret = new Record();
		ret->loadFromFile(*m_sortedFile);
	}

	return ret;
}

inline uint64_t ExternalSorter::getTotalEntries() const
{
	return m_u64TotalEntries;
}

//
// BulkLoader
//
void BulkLoader::bulkLoadUsingSTR(
	SpatialIndex::SBRTree::SBRTree* pTree,
	IDataStream& stream,
	uint32_t bindex,
	uint32_t bleaf,
	uint32_t pageSize,
	uint32_t numberOfPages
) {
	if (! stream.hasNext())
		throw Tools::IllegalArgumentException(
			"SBRTree::BulkLoader::bulkLoadUsingSTR: Empty data stream given."
		);

	NodePtr n = pTree->readNode(pTree->m_rootID);
	pTree->deleteNode(n.get());

	#ifndef NDEBUG
	std::cerr << "SBRTree::BulkLoader: Sorting data." << std::endl;
	#endif

	Tools::SmartPointer<ExternalSorter> es = Tools::SmartPointer<ExternalSorter>(new ExternalSorter(pageSize, numberOfPages));

	while (stream.hasNext())
	{
		Data* d = reinterpret_cast<Data*>(stream.getNext());
		if (d == 0)
			throw Tools::IllegalArgumentException(
				"bulkLoadUsingSTR: SBRTree bulk load expects SpatialIndex::SBRTree::Data entries."
			);
		es->insert(new ExternalSorter::Record(d->m_sbr, d->m_id, d->m_dataLength, d->m_pData, 0,0,&d->m_mbc));
		d->m_pData = 0;
		delete d;
	}
	es->sort();

	pTree->m_stats.m_u64Data = es->getTotalEntries();

	// create index levels.
	uint32_t level = 0;

	while (true)
	{
		#ifndef NDEBUG
		std::cerr << "SBRTree::BulkLoader: Building level " << level << std::endl;
		#endif

		pTree->m_stats.m_nodesInLevel.emplace_back(0);

		Tools::SmartPointer<ExternalSorter> es2 = Tools::SmartPointer<ExternalSorter>(new ExternalSorter(pageSize, numberOfPages));
		createLevel(pTree, es, 0, bleaf, bindex, level++, es2, pageSize, numberOfPages);
		es = es2;

		if (es->getTotalEntries() == 1) break;
		es->sort();
	}

	pTree->m_stats.m_u32TreeHeight = level;
	pTree->storeHeader();
}

void BulkLoader::createLevel(
	SpatialIndex::SBRTree::SBRTree* pTree,
	Tools::SmartPointer<ExternalSorter> es,
	uint32_t dimension,
	uint32_t bleaf,
	uint32_t bindex,
	uint32_t level,
	Tools::SmartPointer<ExternalSorter> es2,
	uint32_t pageSize,
	uint32_t numberOfPages
) {
	uint64_t b = (level == 0) ? bleaf : bindex;
    uint64_t P = static_cast<uint64_t>(std::ceil(static_cast<double>(es->getTotalEntries()) / static_cast<double>(b)));
    int remainDim;
    remainDim=2*pTree->m_dimension-dimension;
    uint64_t S = static_cast<uint64_t>(ceil(pow(static_cast<double>(P),1.0/remainDim)));
    std::cerr<<"crtLvl at"<<b<<" "<<P<<" "<<S<<" "<<level<<" "<<dimension<<" \n";

	if (S == 1 || remainDim==1 || S * b == es->getTotalEntries())
	{
		std::vector<ExternalSorter::Record*> node;
		ExternalSorter::Record* r;

		while (true)
		{
			try { r = es->getNextRecord(); } catch (Tools::EndOfStreamException) { break; }
			node.emplace_back(r);

			if (node.size() == b)
			{
				Node* n = createNode(pTree, node, level);
				node.clear();
				pTree->writeNode(n);
				es2->insert(new ExternalSorter::Record(n->m_nodeSBR, n->m_identifier, 0, 0, 0,level));
				pTree->m_rootID = n->m_identifier;
					// special case when the root has exactly bindex entries.
				delete n;
			}
		}

		if (! node.empty())
		{
			Node* n = createNode(pTree, node, level);
			pTree->writeNode(n);
			es2->insert(new ExternalSorter::Record(n->m_nodeSBR, n->m_identifier, 0, 0, 0,level));
			pTree->m_rootID = n->m_identifier;
			delete n;
		}
	}
	else
	{
		bool bMore = true;

		while (bMore)
		{
			ExternalSorter::Record* pR;
			Tools::SmartPointer<ExternalSorter> es3 = Tools::SmartPointer<ExternalSorter>(new ExternalSorter(pageSize, numberOfPages));

			for (uint64_t i = 0; i < b*floor(1.0*P/S); ++i)
//            for (uint64_t i = 0; i < S*b; ++i)
			{
				try { pR = es->getNextRecord(); }
				catch (Tools::EndOfStreamException) { bMore = false; break; }
				pR->m_s = dimension + 1;
				es3->insert(pR);
			}
			es3->sort();
			createLevel(pTree, es3, dimension + 1, bleaf, bindex, level, es2, pageSize, numberOfPages);
		}
	}
}

Node* BulkLoader::createNode(SpatialIndex::SBRTree::SBRTree* pTree, std::vector<ExternalSorter::Record*>& e, uint32_t level)
{
	Node* n;
	if (level == 0) n = new Leaf(pTree, -1);
	else n = new Index(pTree, -1, level);
//	n->m_nodeSBR.m_startTime=e[0]->m_sbr.m_startTime;
//    n->m_nodeSBR.m_endTime=e[0]->m_sbr.m_endTime;
//    n->m_nodeSBR.m_pLow[0]=e[0]->m_sbr.m_pLow[0];
//    n->m_nodeSBR.m_pLow[1]=e[0]->m_sbr.m_pLow[1];
//    n->m_nodeSBR.m_pHigh[0]=e[0]->m_sbr.m_pHigh[0];
//    n->m_nodeSBR.m_pHigh[1]=e[0]->m_sbr.m_pHigh[1];
    std::vector<SBR> sbrs;
	for (size_t cChild = 0; cChild < e.size(); ++cChild)
	{
        if (level == 0) {
            n->insertEntry(e[cChild]->m_len, e[cChild]->m_pData,e[cChild]->m_sbr, e[cChild]->m_mbc, e[cChild]->m_id);
            std::cerr<<"inserted entry\n"<<e[cChild]->m_mbc<<"\n"<<e[cChild]->m_sbr<<"\n";
        }
        else n->insertEntry(e[cChild]->m_len, e[cChild]->m_pData, e[cChild]->m_sbr, e[cChild]->m_id);
        sbrs.push_back(e[cChild]->m_sbr);
	    e[cChild]->m_pData = 0;
		delete e[cChild];
	}
	n->m_nodeSBR=SBR::getSBR(sbrs);
    std::cerr<<"node sbr\n"<<n->m_nodeSBR<<"\n";
	return n;
}