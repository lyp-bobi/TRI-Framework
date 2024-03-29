//
// Created by Chuang on 2020/12/10.
//
#include "testFuncs.h"
#include "random"
#include "../../src/xrtree/BulkLoader.h"

int main(){
    try {
        string target = "tdfilter.txt";
        testtime=400;
        double seglens[] = {600,900,1200,1800,2700,3600};
        cerr<<"seglen: ";
        for(auto len:seglens){cerr<<len<<" ";}
        cerr<<endl;
        xStore x(target, testFileName(target), true);
        for(double qt=300;qt<=5400;qt+=600) {
            cerr<<"qt is " << qt<<endl;
            vector<xTrajectory> queries;
//            xTrajectory tj;
//            tj.loadFromString("116.467128,39.939488,265079.000000 116.497360,39.961560,265185.000000 116.356050,39.867990,265229.000000 116.360633,39.872935,265379.000000");
//            queries.emplace_back(tj);
            fillQuerySet(queries,x,qt);
//            {
//                MTQ q;
//                q.prepareTrees(&x, [](auto x) { return buildTBTreeWP(x); });
//                q.appendQueries(queries);
//                std::cerr << q.runQueries().toString();
//            }
//            {
//                MTQ q;
//                q.prepareTrees(&x, [](auto x) { return buildSTRTreeWP(x); });
//                q.appendQueries(queries);
//                std::cerr << q.runQueries().toString();
//            }
            for (auto len:seglens) {
                MTQ q;
                q.prepareTrees(&x, [&len](auto x) { return buildMBCRTreeWP(x, xTrajectory::ISS, len); });
                q.appendQueries(queries);
                std::cerr << q.runQueries().toString();
            }
            bulkloadt=true;
            for (auto len:seglens) {
                MTQ q;
                q.prepareTrees(&x, [&len](auto x) { return buildMBCRTreeWP(x, xTrajectory::ISS, len,"T"); });
                q.appendQueries(queries);
                std::cerr << q.runQueries().toString();
            }
        }
        cerr<<"mission complete.\n";
    }catch (Tools::Exception &e) {
        cerr << "******ERROR******" << endl;
        std::string s = e.what();
        cerr << s << endl;
        return -1;
    }
    return 0;
}