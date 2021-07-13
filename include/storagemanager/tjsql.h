//
// Created by Chuang on 2021/4/8.
//

#ifndef SPATIALINDEX_TJSQL_H
#define SPATIALINDEX_TJSQL_H

#include <string>
#include "xStore.h"
using std::string;

/**
 * page table
 * | PAGEID | DATA |
 */
/**
* traj table
* | TRAJID | PAGEID | LENGTH |
*/

class db_thread{
public:
    db_thread();
    void release();
    ~db_thread();
};

extern thread_local db_thread dbt;

SIDX_DLL bool db_insert_page(int64_t &pageid, uint32_t len, const uint8_t* const data);

SIDX_DLL bool db_load_page(int64_t page, uint32_t& len, uint8_t** data);

SIDX_DLL int64_t db_last_pageid(void);

SIDX_DLL bool db_insert_traj(int64_t id, int64_t pageid, uint32_t npoint);

SIDX_DLL xTrajEntry db_load_traj_entry(int64_t trajid);

SIDX_DLL int64_t db_last_trajid(void);

SIDX_DLL bool db_create_table();
SIDX_DLL bool db_drop_table(string name);

SIDX_DLL bool conn_init(string path);



#endif //SPATIALINDEX_TJSQL_H
