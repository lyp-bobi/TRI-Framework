//
// Created by Chuang on 2019/6/10.
//

#pragma once


#include <spatialindex/SpatialIndex.h>
using namespace SpatialIndex;
using namespace SpatialIndex::StorageManager;
using std::vector;

namespace SpatialIndex
{
    class XZ3Enocder{
    private:
        XZ3Enocder();
        static XZ3Enocder* singleton;
    public:
        long encode(double x,double y,double z);
        double m_xmin,m_xmax,m_ymin,m_ymax,m_zmin,m_zmax;
        uint32_t m_length;
        static XZ3Enocder* instance();
    };
    namespace StorageManager {
        class tsExternalSorter
        {
        public:
            class Record
            {
            public:
                Record();
                Record(const Region& shape, id_type id,id_type pvId,id_type ntId, uint32_t len,
                        uint8_t* pData, uint32_t s,uint32_t level,MBC* mbc= nullptr);
                ~Record();

                bool operator<(const Record& r) const;

                void storeToFile(Tools::TemporaryFile& f);
                void loadFromFile(Tools::TemporaryFile& f);

                struct SortAscending : public std::binary_function<Record* const, Record* const, bool>
                {
                    bool operator()(Record* const r1, Record* const r2)
                    {
                        if (*r1 < *r2) return true;
                        else return false;
                    }
                };

            public:
                uint32_t m_level;
                Region m_r;
                MBC m_mbc;
                id_type m_id,m_pvId,m_ntId;
                uint32_t m_len;
                uint8_t* m_pData;
                uint32_t m_s;
            };

        public:
            tsExternalSorter(uint32_t u32PageSize, uint32_t u32BufferPages);
            virtual ~tsExternalSorter();

            void insert(Record* r);
            void sort();
            Record* getNextRecord();
            uint64_t getTotalEntries() const;

        private:
            class PQEntry
            {
            public:
                PQEntry(Record* r, uint32_t u32Index) : m_r(r), m_u32Index(u32Index) {}

                struct SortAscending : public std::binary_function<const PQEntry&, const PQEntry&, bool>
                {
                    bool operator()(const PQEntry& e1, const PQEntry& e2)
                    {
                        if (*(e1.m_r) < *(e2.m_r)) return true;
                        else return false;
                    }
                };

                Record* m_r;
                uint32_t m_u32Index;
            };

        private:
            bool m_bInsertionPhase;
            uint32_t m_u32PageSize;
            uint32_t m_u32BufferPages;
            Tools::SmartPointer<Tools::TemporaryFile> m_sortedFile;
            std::list<Tools::SmartPointer<Tools::TemporaryFile> > m_runs;
            std::vector<Record*> m_buffer;
            uint64_t m_u64TotalEntries;
            uint32_t m_stI;
        };//tsExternalSorter

        class TrajStore:public IStorageManager{
        public:
            TrajStore(IStorageManager *store,uint32_t pageSize);
            void flush(){m_pStorageManager->flush();}
            void loadByteArray(const id_type page, uint32_t& len, uint8_t** data){
                m_pStorageManager->loadByteArray(page,len,data);
                m_IO+=1;
            }//for inner nodes
            void storeByteArray(id_type& page, const uint32_t len, const uint8_t* const data){
                m_pStorageManager->storeByteArray(page,len,data);
            }//for inner nodes
            void deleteByteArray(const id_type page){
                m_pStorageManager->deleteByteArray(page);
            }//should not be used in bulkload mode i guess
            class Entry{
            public:
                id_type m_page;
                uint32_t m_start;
                uint32_t m_len;
                id_type m_pvId;
                id_type m_ntId;
                Entry(id_type page,uint32_t start,uint32_t len,id_type pvId,id_type ntId);
            };
            id_type getTrajId(id_type id){return id/m_maxTrajSegs;}
            id_type getSegId(id_type id,uint32_t segnum){return id*m_maxTrajSegs+segnum;}
            void loadSegments(vector<std::pair<id_type, vector<Trajectory>> > &trajs);
            const Trajectory getTraj(id_type &id);
            const Trajectory getTrajByTime(id_type &id,double tstart,double tend);
            const ShapeList getMBRsByTime(id_type &id,double tstart,double tend);
            const ShapeList getMBCsByTime(id_type &id,double tstart,double tend);
            std::map<id_type, Entry*> m_entries;//map from seg id to entry
            std::map<id_type, MBC> m_entryMbcs;
            std::map<id_type, Region> m_entryMbrs;
            IStorageManager* m_pStorageManager;
            uint32_t m_pageSize;
            uint32_t m_maxTrajSegs=100;
            uint32_t m_IO=0;
        };
    }//namespace StorageManager
    class baseSegmentStream:public IDataStream{
    public:
        std::map<id_type, MBC>::iterator iter;
        std::map<id_type, Region> *m_brs;
        std::map<id_type, MBC> *m_bcs;
        baseSegmentStream(TrajStore *ts)
            :m_brs(&ts->m_entryMbrs),m_bcs(&ts->m_entryMbcs),iter(ts->m_entryMbcs.begin()){}
        virtual bool hasNext() override
        {
            return iter!=m_bcs->end();
        }
        virtual IData* constructData(id_type id,Region mbr,MBC mbc)=0;
        virtual IData* getNext() override{
            uint8_t *data;
            uint32_t len;
            auto d=constructData(iter->first,(*m_brs)[iter->first],iter->second);
            iter++;
            return d;
        }
        virtual uint32_t size()
        {
            return m_bcs->size();
        }

        virtual void rewind(){iter=m_bcs->begin();}
    };
}