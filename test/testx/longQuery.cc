//
// Created by Chuang on 2020/12/10.
//
#include "testFuncs.h"
#include "random"

int main(int argc,char *argv[]){
    try {
        bulkloadt =true;
        vector<double> seglens;
        string target;
        target = "tdexpand.data";
        seglens = {3600,7200,10800,14400,20000,30000};
        testtime=400;
        cerr<<"seglen: ";
        for(auto len:seglens){cerr<<len<<" ";}
        cerr<<endl;
        xStore x(target, testFileName(target), true);
        for(double qt=43200;qt<=100000;qt+=43200) {
            cerr<<"qt is " << qt<<endl;
            vector<xTrajectory> queries;
//            xTrajectory tj;
//            tj.loadFromString("39.977928,116.329393,372330.000000 39.977922,116.329412,372335.000000 39.977922,116.329408,372340.000000 39.977922,116.329408,372343.000000 39.977922,116.329405,372345.000000 39.977925,116.329435,372350.000000 39.977940,116.329437,372355.000000 39.977940,116.329440,372360.000000 39.977938,116.329442,372365.000000 39.977938,116.329445,372370.000000 39.977938,116.329445,372373.000000 39.977937,116.329447,372375.000000 39.977937,116.329437,372380.000000 39.977937,116.329435,372385.000000 39.977937,116.329435,372388.000000 39.977937,116.329442,372390.000000 39.977928,116.329488,372395.000000 39.977947,116.329627,372400.000000 39.977947,116.329682,372405.000000 39.977947,116.329682,372408.000000 39.977948,116.329675,372410.000000 39.977965,116.329813,372415.000000 39.977998,116.329873,372420.000000 39.977998,116.329873,372423.000000 39.978002,116.329865,372425.000000 39.978083,116.329853,372430.000000 39.978257,116.329670,372435.000000 39.978287,116.329712,372440.000000 39.978300,116.329660,372445.000000 39.978307,116.329642,372450.000000 39.978337,116.329503,372455.000000 39.978315,116.329445,372460.000000 39.978317,116.329430,372465.000000 39.978318,116.329387,372470.000000 39.978300,116.329408,372475.000000 39.978300,116.329408,372478.000000 39.978298,116.329417,372480.000000 39.978223,116.329650,372485.000000 39.978073,116.329947,372490.000000 39.978095,116.330050,372495.000000 39.978107,116.330173,372500.000000 39.978080,116.330293,372505.000000 39.978063,116.330397,372510.000000 39.978022,116.330512,372515.000000 39.977962,116.330608,372520.000000 39.977927,116.330702,372525.000000 39.977877,116.330725,372530.000000 39.977823,116.330788,372535.000000 39.977767,116.330870,372540.000000 39.977757,116.330940,372545.000000 39.977795,116.330965,372550.000000 39.977850,116.330958,372555.000000 39.977895,116.330930,372560.000000 39.977932,116.330880,372565.000000 39.977978,116.330817,372570.000000 39.978033,116.330783,372575.000000 39.978070,116.330742,372580.000000 39.978115,116.330712,372585.000000 39.978127,116.330685,372590.000000 39.978113,116.330693,372595.000000 39.978067,116.330732,372600.000000 39.978060,116.330722,372605.000000 39.978107,116.330640,372610.000000 39.978242,116.330497,372615.000000 39.978347,116.330288,372620.000000 39.978380,116.330178,372625.000000 39.978375,116.330167,372630.000000");
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
                q.prepareTrees(&x, [&len](auto x) { return buildMBRRTreeWP(x, xTrajectory::OPTS, len); });
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