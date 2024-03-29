//
// Created by Chuang on 2019/8/19.
//

#include "testFuncs.h"

int main() {

    try {
        vector<pair<id_type, Trajectory> > trajs = loadDumpedFiledToTrajs("/root/glfilter.txt");

        int maxseg = 0;
        double segLenParas[] = {100, 200, 300, 400, 500, 750, 1000, 1500, 2000, 2500, 3000};
        double queryLenParas[] = {3600};
        std::cerr << "Starting range test\n" << "Segmentation lengths are:";
        for (auto p:segLenParas) std::cerr << p << "\t";
        std::cerr << "\nQuery lengths are:";
        for (auto p:queryLenParas) std::cerr << p << "\t";
        std::cerr << "\n";
        vector<vector<IShape *>> querySet;
        for (auto queryLen:queryLenParas) {
            vector<IShape *> queries;
            for (int i = 0; i < 2*testtime; i++) {
                auto p =trajs[int(random(0,trajs.size()-1))].second.randomPoint();
                Cylinder *rg = new Cylinder(p.m_pCoords, 0.05, p.m_time - queryLen/2, p.m_time + queryLen/2, 2);
                queries.emplace_back(rg);
            }
            querySet.emplace_back(queries);
            queries.clear();
        }
        for (double segLen:segLenParas) {
            maxseg = 3000;
            string name0 = "name0", name1 = "name1", name2 = "name2";
            id_type indexIdentifier0, indexIdentifier1, indexIdentifier2;
            IStorageManager *diskfile0 = StorageManager::createNewDiskStorageManager(name0, 4096),
                    *diskfile1 = StorageManager::createNewDiskStorageManager(name1, 4096),
                    *diskfile2 = StorageManager::createNewDiskStorageManager(name2, 4096);
            // Create a new storage manager with the provided base name and a 4K page size.
            StorageManager::IBuffer *file0 = StorageManager::createNewRandomEvictionsBuffer(*diskfile0, 10, false),
                    *file1 = StorageManager::createNewRandomEvictionsBuffer(*diskfile1, 10, false),
                    *file2 = StorageManager::createNewRandomEvictionsBuffer(*diskfile2, 10, false);

            maxseg = std::max(3000, Trajectory::cutTrajsIntoFile(trajs, segLen));
            std::cerr << "maxseg is " << maxseg << "\n";

            TrajStore *ts1 = new TrajStore(name1, diskfile1, 4096, maxseg + 1);
            TrajStore *ts2 = new TrajStore(name2, diskfile2, 4096, maxseg + 1);

            ts1->loadSegments(subTrajFile, true);
            ISpatialIndex *r = MBCRTree::createAndBulkLoadNewRTreeWithTrajStore(ts1, 4096, 3, indexIdentifier1);

            ts2->loadSegments(subTrajFile, true);
            ISpatialIndex *rc = MBCRTree::createAndBulkLoadNewMBCRTreeWithTrajStore(ts2, 4096, 3, indexIdentifier2);

            ts1->flush();
            ts2->flush();
            std::cerr << "Seg len:" << segLen << "\n";
            bUsingSBBD = true;
            for (const auto &qs:querySet) {
//                std::cerr<<"SBBD off\n";
//                bUsingSBBD = false;
//                rangeQueryBatch(r, qs, ts1);
//                rangeQueryBatch(rc, qs, ts2);
//                std::cerr<<"SBBD on\n";
                rangeQueryBatch(r, qs, ts1);
                rangeQueryBatch(rc, qs, ts2);
            }
//            std:cerr<<*r<<*rc<<endl;

            delete r;
            delete ts1;
            delete rc;
            delete ts2;
            delete file0;
            delete file1;
            delete file2;
            delete diskfile0;
            delete diskfile1;
            delete diskfile2;
        }
        for (auto &qs:querySet) {
            for (auto &shape:qs) {
                delete (shape);
            }
        }
    }
    catch (Tools::Exception &e) {
        cerr << "******ERROR******" << endl;
        std::string s = e.what();
        cerr << s << endl;
        return -1;
    }
    catch (...) {

    }



    return 0;
}