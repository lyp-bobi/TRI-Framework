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

#include <fstream>
#include <cstring>
#include <chrono>
#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE
#include <unistd.h>
#include <fcntl.h>
// For checking if a file exists - hobu
#include <sys/stat.h>

#include <spatialindex/SpatialIndex.h>
#include "DiskStorageManager.h"
#include <spatialindex/tools/Tools.h>



#ifdef WIN32
#define COPENFLG O_RDONLY
#else
//#define COPENFLG O_RDONLY
#define NEW_READ_STYLE
#define COPENFLG (O_RDONLY|O_DIRECT)
#endif

using namespace SpatialIndex;
using namespace SpatialIndex::StorageManager;

bool CheckFilesExists(Tools::PropertySet& ps)
{
    bool bExists = false;

    std::string filename("");
    std::string idx("idx");
    std::string dat("dat");

    Tools::Variant idx_name;
    Tools::Variant dat_name;
    Tools::Variant fn;

    idx_name = ps.getProperty("FileNameIdx");
    dat_name = ps.getProperty("FileNameDat");
    fn = ps.getProperty("FileName");

    if (idx_name.m_varType != Tools::VT_EMPTY) dat = std::string(idx_name.m_val.pcVal);
    if (dat_name.m_varType != Tools::VT_EMPTY) idx = std::string(dat_name.m_val.pcVal);
    if (fn.m_varType != Tools::VT_EMPTY) filename = std::string(fn.m_val.pcVal);

    conn_init(filename);

    struct stat stats;

    std::ostringstream os;
    int ret;
    os << filename <<"."<<dat;
    std::string data_name = os.str();
    ret = stat(data_name.c_str(), &stats);

    if (ret == 0) bExists = true;

    os.str("");
    os << filename <<"."<<idx;
    std::string index_name = os.str();
    ret = stat(index_name.c_str(), &stats);

    if ((ret == 0) && (bExists == true)) bExists = true;

    return bExists;
}
SpatialIndex::IStorageManager* SpatialIndex::StorageManager::returnDiskStorageManager(Tools::PropertySet& ps)
{
    IStorageManager* sm = new DiskStorageManager(ps);
    return sm;
}

SpatialIndex::IStorageManager* SpatialIndex::StorageManager::createNewDiskStorageManager(std::string& baseName, uint32_t pageSize)
{
    Tools::Variant var;
    Tools::PropertySet ps;

    var.m_varType = Tools::VT_BOOL;
    var.m_val.blVal = true;
    ps.setProperty("Overwrite", var);
    // overwrite the file if it exists.

    var.m_varType = Tools::VT_PCHAR;
    var.m_val.pcVal = const_cast<char*>(baseName.c_str());
    ps.setProperty("FileName", var);
    // .idx and .dat extensions will be added.

    var.m_varType = Tools::VT_ULONG;
    var.m_val.ulVal = pageSize;
    ps.setProperty("PageSize", var);
    // specify the page size. Since the index may also contain user defined data
    // there is no way to know how big a single node may become. The storage manager
    // will use multiple pages per node if needed. Off course this will slow down performance.

    return returnDiskStorageManager(ps);
}

SpatialIndex::IStorageManager* SpatialIndex::StorageManager::loadDiskStorageManager(std::string& baseName)
{
    Tools::Variant var;
    Tools::PropertySet ps;

    var.m_varType = Tools::VT_PCHAR;
    var.m_val.pcVal = const_cast<char*>(baseName.c_str());
    ps.setProperty("FileName", var);
    // .idx and .dat extensions will be added.

    return returnDiskStorageManager(ps);
}

DiskStorageManager::DiskStorageManager(Tools::PropertySet& ps) : m_pageSize(0), m_nextPage(-1), m_buffer(0)
{
    Tools::Variant var;

    // Open/Create flag.
    bool bOverwrite = false;
    bool bFileExists = false;
    std::streamoff length = 0;

    var = ps.getProperty("Overwrite");

    if (var.m_varType != Tools::VT_EMPTY)
    {
        if (var.m_varType != Tools::VT_BOOL)
            throw Tools::IllegalArgumentException("SpatialIndex::DiskStorageManager: Property Overwrite must be Tools::VT_BOOL");
        bOverwrite = var.m_val.blVal;
    }

    // storage filename.
    var = ps.getProperty("FileName");

    if (var.m_varType != Tools::VT_EMPTY)
    {
        if (!(var.m_varType == Tools::VT_PCHAR ||
              var.m_varType == Tools::VT_PWCHAR))
            throw Tools::IllegalArgumentException("SpatialIndex::DiskStorageManager: Property FileName must be Tools::VT_PCHAR or Tools::VT_PWCHAR");

        std::string idx("idx");
        std::string dat("dat");

        Tools::Variant idx_name = ps.getProperty("FileNameIdx");
        if (idx_name.m_varType != Tools::VT_EMPTY) idx = std::string(idx_name.m_val.pcVal);

        Tools::Variant dat_name = ps.getProperty("FileNameDat");
        if (dat_name.m_varType != Tools::VT_EMPTY) dat = std::string(dat_name.m_val.pcVal);

        std::string sIndexFile = std::string(var.m_val.pcVal) + "." + idx;
        std::string sDataFile = std::string(var.m_val.pcVal) + "." + dat;


        // check if file exists.
        bFileExists = CheckFilesExists(ps);

        // check if file can be read/written.
        if (bFileExists == true && bOverwrite == false)
        {
            std::ios_base::openmode mode = std::ios::in | std::ios::out | std::ios::binary;
            m_indexFile.rdbuf()->pubsetbuf(0,0);
            m_dataFile.rdbuf()->pubsetbuf(0,0);
            m_indexFile.open(sIndexFile.c_str(), mode);
            m_dataFile.open(sDataFile.c_str(), mode);
#ifdef NEW_READ_STYLE
            m_datafd = open(sDataFile.c_str(), COPENFLG);
            if(m_datafd == -1)
                perror("open failed:");
#endif
            if (m_indexFile.fail() || m_dataFile.fail()){
                cerr<< m_indexFile.fail() << m_dataFile.fail()<<strerror(errno);
                throw Tools::IllegalArgumentException("SpatialIndex::DiskStorageManager: Index/Data file cannot be read/writen.");
            }
        }
        else
        {
            std::ios_base::openmode mode = std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc;
            m_indexFile.rdbuf()->pubsetbuf(0,0);
            m_dataFile.rdbuf()->pubsetbuf(0,0);
            m_indexFile.open(sIndexFile.c_str(), mode);
            m_dataFile.open(sDataFile.c_str(), mode);
#ifdef NEW_READ_STYLE
            m_datafd = open(sDataFile.c_str(), COPENFLG);
            if(m_datafd == -1)
                perror("open failed:");
#endif

            if (m_indexFile.fail() || m_dataFile.fail())
                throw Tools::IllegalArgumentException("SpatialIndex::DiskStorageManager: Index/Data file cannot be created.");

        }
    }
    else
    {
        throw Tools::IllegalArgumentException("SpatialIndex::DiskStorageManager: Property FileName was not specified.");
    }

    // get current length of file
    m_indexFile.seekg (0, m_indexFile.end);
    length = m_indexFile.tellg();
    m_indexFile.seekg (0, m_indexFile.beg);

    // find page size.
    if ((bOverwrite == true) || (length == 0) || (bFileExists == false))
    {
        var = ps.getProperty("PageSize");

        if (var.m_varType != Tools::VT_EMPTY)
        {
            if (var.m_varType != Tools::VT_ULONG)
                throw Tools::IllegalArgumentException("SpatialIndex::DiskStorageManager: Property PageSize must be Tools::VT_ULONG");
            m_pageSize = var.m_val.ulVal;
            m_nextPage = 0;
        }
        else
        {
            throw Tools::IllegalArgumentException("SpatialIndex::DiskStorageManager: A new storage manager is created and property PageSize was not specified.");
        }
    }
    else
    {
        m_indexFile.read(reinterpret_cast<char*>(&m_pageSize), sizeof(uint32_t));
        if (m_indexFile.fail())
            throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Failed reading pageSize.");

        m_indexFile.read(reinterpret_cast<char*>(&m_nextPage), sizeof(id_type));
        if (m_indexFile.fail())
            throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Failed reading nextPage.");
    }

    // create buffer.
    int align = m_pageSize-1;
    const char *buff = (char *) malloc((int)m_pageSize + align);
    m_buffer = (uint8_t *)(((uintptr_t)buff + align)&~((uintptr_t)align));

    if ((bOverwrite == false) && (length > 0))
    {
        uint32_t count;
        id_type page, id;

        // load empty pages in memory.
        m_indexFile.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));
        if (m_indexFile.fail())
            throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted storage manager index file.");

        for (uint32_t cCount = 0; cCount < count; ++cCount)
        {
            m_indexFile.read(reinterpret_cast<char*>(&page), sizeof(id_type));
            if (m_indexFile.fail())
                throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted storage manager index file.");
            m_emptyPages.insert(page);
        }

        // don't load index table in memory.

    }
}

DiskStorageManager::~DiskStorageManager()
{
    flush();
    m_indexFile.close();
    m_dataFile.close();
#ifdef NEW_READ_STYLE
    close(m_datafd);
#endif
//    delete[] m_rawbuffer;
}

void DiskStorageManager::flush()
{
    if(m_isro) return;
    m_indexFile.seekp(0, std::ios_base::beg);
    if (m_indexFile.fail())
        throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted storage manager index file.");

    m_indexFile.write(reinterpret_cast<const char*>(&m_pageSize), sizeof(uint32_t));
    if (m_indexFile.fail())
        throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted storage manager index file.");

    m_indexFile.write(reinterpret_cast<const char*>(&m_nextPage), sizeof(id_type));
    if (m_indexFile.fail())
        throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted storage manager index file.");

    uint32_t count = static_cast<uint32_t>(m_emptyPages.size());
    m_indexFile.write(reinterpret_cast<const char*>(&count), sizeof(uint32_t));
    if (m_indexFile.fail())
        throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted storage manager index file.");

    for (std::set<id_type>::const_iterator it = m_emptyPages.begin(); it != m_emptyPages.end(); ++it)
    {
        m_indexFile.write(reinterpret_cast<const char*>(&(*it)), sizeof(id_type));
        if (m_indexFile.fail())
            throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted storage manager index file.");
    }

    m_indexFile.flush();
    m_dataFile.flush();
    m_indexFile.sync();
    m_dataFile.sync();
}

void DiskStorageManager::loadByteArray(const id_type page, uint32_t& len, uint8_t** data)
{
    auto pages = db_load_page(page);

    uint32_t cNext = 0;
    uint32_t cTotal = static_cast<uint32_t>(pages.size());

    len = m_pageSize;
    *data = new uint8_t[len];

    uint8_t *ptr = *data;
    uint32_t cLen;
    uint32_t cRem = len;

    do {
#ifndef NEW_READ_STYLE
        m_dataFile.seekg(pages[cNext] * m_pageSize, std::ios_base::beg);
        if (m_dataFile.fail())
            throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted data file.");
        m_dataFile.read(reinterpret_cast<char *>(m_buffer), m_pageSize);
        if (m_dataFile.fail())
            throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted data file.");
#else
        double size = 0;
//            size = lseek64(m_datafd,pages[cNext] * m_pageSize,SEEK_SET);
//            if(size == -1)
//                perror("seek error");
//            size = read(m_datafd, m_buffer,m_pageSize);
        size = pread(m_datafd,m_buffer, m_pageSize, pages[cNext] * m_pageSize);
        if(size == -1)
            perror("read error");
#endif

        cLen = (cRem > m_pageSize) ? m_pageSize : cRem;
        memcpy(ptr, m_buffer, cLen);

        ptr += cLen;
        cRem -= cLen;
        ++cNext;
    } while (cNext < cTotal);
}

void DiskStorageManager::storeByteArray(id_type& page, const uint32_t len, const uint8_t* const data)
{
    if(m_isro)
        throw Tools::IllegalStateException("DiskStorageManger:can't write a ro file");
    if (page == NewPage)
    {
//        std::cout<<"page id"<<page<<std::endl;
        Entry* e = new Entry();
        e->m_length = len;

        const uint8_t* ptr = data;
        id_type cPage;
        uint32_t cRem = len;
        uint32_t cLen;

        while (cRem > 0)
        {
            if (! m_emptyPages.empty())
            {
                cPage = *m_emptyPages.begin();
                m_emptyPages.erase(m_emptyPages.begin());
            }
            else
            {
                cPage = m_nextPage;
                ++m_nextPage;
            }

            cLen = (cRem > m_pageSize) ? m_pageSize : cRem;
            memcpy(m_buffer, ptr, cLen);

            m_dataFile.seekp(cPage * m_pageSize, std::ios_base::beg);
            if (m_dataFile.fail())
                throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted data file.");

            m_dataFile.write(reinterpret_cast<const char*>(m_buffer), m_pageSize);
            if (m_dataFile.fail())
                throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted data file.");

            ptr += cLen;
            cRem -= cLen;
            e->m_pages.emplace_back(cPage);
        }

        page = e->m_pages[0];
        db_insert_page(page, e->m_pages);
    }
    else
    {
        auto oldpages = db_load_page(page);
        assert(oldpages.size()>=1);

        Entry* e = new Entry();
        e->m_length = len;

        const uint8_t* ptr = data;
        id_type cPage;
        uint32_t cRem = len;
        uint32_t cLen, cNext = 0;

        while (cRem > 0)
        {
            if (cNext < oldpages.size())
            {
                cPage = oldpages[cNext];
                ++cNext;
            }
            else if (! m_emptyPages.empty())
            {
                cPage = *m_emptyPages.begin();
                m_emptyPages.erase(m_emptyPages.begin());
            }
            else
            {
                cPage = m_nextPage;
                ++m_nextPage;
            }

            cLen = (cRem > m_pageSize) ? m_pageSize : cRem;
            memcpy(m_buffer, ptr, cLen);

            m_dataFile.seekp(cPage * m_pageSize, std::ios_base::beg);
            if (m_dataFile.fail())
                throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted data file.");

            m_dataFile.write(reinterpret_cast<const char*>(m_buffer), m_pageSize);
            if (m_dataFile.fail())
                throw Tools::IllegalStateException("SpatialIndex::DiskStorageManager: Corrupted data file.");

            ptr += cLen;
            cRem -= cLen;
            e->m_pages.emplace_back(cPage);
        }

        while (cNext < oldpages.size())
        {
            m_emptyPages.insert(oldpages[cNext]);
            ++cNext;
        }
        db_insert_page(page, e->m_pages);
//        m_pageIndex.insert(std::pair<id_type, Entry*>(page, e));
    }
}

void DiskStorageManager::deleteByteArray(const id_type page)
{
    if(m_isro)
        throw Tools::IllegalStateException("DiskStorageManger:can't write a ro file");

    auto pages = db_load_page(page);

    for (uint32_t cIndex = 0; cIndex < pages.size(); ++cIndex)
    {
        m_emptyPages.insert(pages[cIndex]);
    }

}


DiskStorageManager::DiskStorageManager(DiskStorageManager &r,string &name) {
    assert(!r.m_isro);
    std::string sIndexFile = name + ".idx";
    std::string sDataFile = name + ".dat";
    std::ios_base::openmode mode = std::ios::in | std::ios::out | std::ios::binary;
    m_indexFile.rdbuf()->pubsetbuf(0,0);
    m_dataFile.rdbuf()->pubsetbuf(0,0);
    m_indexFile.open(sIndexFile.c_str(), mode);
    m_dataFile.open(sDataFile.c_str(), mode);
#ifdef NEW_READ_STYLE
    m_datafd = open(sDataFile.c_str(), COPENFLG);
    if(m_datafd == -1)
        perror("open failed:");
#endif
    if(m_indexFile.fail()){
        cerr<<strerror(errno);
    }
    if(m_dataFile.fail()){
        cerr<<strerror(errno);
    }
    m_pageSize=r.m_pageSize;
    m_isro=true;
    m_nextPage = r.m_nextPage;
    // create buffer.
//    m_buffer = new uint8_t[m_pageSize];
//    memset(m_buffer, 0, m_pageSize);
    int align = m_pageSize-1;
    m_rawbuffer = (char *) malloc((int)m_pageSize + align);
    m_buffer = (uint8_t *)(((uintptr_t)m_rawbuffer + align)&~((uintptr_t)align));
}