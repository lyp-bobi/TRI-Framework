//
// Created by Chuang on 2020/10/28.
//
#include "testFuncs.h"

int main(){
    struct str{
        char a[100];
    };
    cerr<<sizeof(str)<<endl;
    string target = "glexpand.datas";
    xStore x(target, testFileName(target), true);
    double seglen[]={600};//900,1200,1800
    xCylinder query(xPoint(40,116.327,6516),0.0001,6516,9516,2);
    xTrajectory tj,tj2;
//    tj.getMinimumDistance(tj2);
//    x.loadTraj(tj, xStoreEntry(4337,0,1000));
//    tj.intersectsxCylinder(query);
//    cerr<<tj.intersectsxCylinder(query);
    tjstat->bt=50;
    xSBB s;
    s.loadFromString("0 1 0 39.984438 116.347402 9194.000000 39.998213 116.365543 10194.000000 0.043406 0.000149");
    query.checkRel(s);
    MyVisitor vis;
    vector<xTrajectory> queries;
    for(int i=0;i<500;i++)
    {
        queries.emplace_back(x.randomSubtraj(4300));
    }


    try {
        for(int i=0;i<10;i++) {
            tj = x.randomSubtraj(3000);
//            tj.loadFromString("116.329232,39.879030,5029569.186606 116.329230,39.879030,5029643.000000 116.329243,39.879049,5032569.186606");
//            cerr<<tj.toString()<<endl;
//            tj.loadFromString(
//                    "39.899691,116.377862,72791.035208 39.899686,116.377800,72795.000000 39.899677,116.377846,72800.000000 39.899641,116.377865,72805.000000 39.899595,116.377922,72810.000000 39.899525,116.377938,72815.000000 39.899440,116.377893,72820.000000 39.899346,116.377845,72825.000000 39.899275,116.377863,72830.000000 39.899227,116.377888,72835.000000 39.899178,116.377847,72840.000000 39.899124,116.377916,72845.000000 39.899050,116.377946,72850.000000 39.898996,116.377945,72855.000000 39.898946,116.377970,72860.000000 39.898908,116.378006,72865.000000 39.898891,116.378110,72870.000000 39.898891,116.378214,72875.000000 39.898879,116.378289,72880.000000 39.898881,116.378365,72885.000000 39.898879,116.378426,72890.000000 39.898858,116.378566,72895.000000 39.898864,116.378668,72900.000000 39.898887,116.378801,72905.000000 39.898899,116.378896,72910.000000 39.898885,116.379009,72915.000000 39.898884,116.379068,72920.000000 39.898893,116.379129,72925.000000 39.898900,116.379188,72930.000000 39.898883,116.379236,72935.000000 39.898886,116.379246,72940.000000 39.898886,116.379254,72945.000000 39.898886,116.379254,72948.000000 39.898889,116.379262,72950.000000 39.898885,116.379299,72955.000000 39.898876,116.379306,72960.000000 39.898887,116.379309,72965.000000 39.898882,116.379357,72970.000000 39.898864,116.379341,72975.000000 39.898873,116.379357,72980.000000 39.898873,116.379357,72983.000000 39.898876,116.379354,72985.000000 39.898884,116.379381,72990.000000 39.898658,116.379307,73295.000000 39.898561,116.379176,73300.000000 39.898552,116.378908,73305.000000 39.898607,116.378557,73310.000000 39.898577,116.378381,73315.000000 39.893856,116.357093,73440.000000 39.894710,116.359564,73445.000000 39.898502,116.368890,73458.000000 39.898495,116.368890,73460.000000 39.898512,116.368887,73465.000000 39.898514,116.368889,73470.000000 39.898516,116.368891,73475.000000 39.898515,116.368894,73480.000000 39.898509,116.368897,73485.000000 39.898505,116.368904,73490.000000 39.898505,116.368904,73493.000000 39.898503,116.368904,73495.00000039.898502,116.368879,73500.000000 39.898503,116.368857,73505.000000 39.898503,116.368857,73508.000000 39.898503,116.368861,73510.000000 39.898500,116.368842,73515.000000 39.898468,116.368656,73520.000000 39.898417,116.368320,73525.000000 39.898358,116.367851,73530.000000 39.898303,116.367330,73535.000000 39.898289,116.366719,73540.000000 39.898271,116.366053,73545.000000 39.898272,116.365368,73550.000000 39.898314,116.364751,73555.000000 39.898350,116.364298,73560.000000 39.898336,116.363989,73565.000000 39.898330,116.363693,73570.000000 39.898340,116.363438,73575.000000 39.898325,116.363224,73580.000000 39.898323,116.362967,73585.000000 39.898335,116.362765,73590.000000 39.898323,116.362671,73595.000000 39.898312,116.362616,73600.000000 39.898288,116.362481,73605.000000 39.898098,116.359508,73635.000000 39.898035,116.359746,73640.000000 39.898011,116.359695,73645.000000 39.898040,116.359598,73650.000000 39.898050,116.359451,73655.000000 39.898002,116.359143,73660.000000 39.897986,116.359017,73665.000000 39.897946,116.358845,73670.000000 39.897867,116.358716,73675.000000 39.897845,116.358653,73680.000000 39.898031,116.359913,73685.000000 39.898183,116.360433,73690.000000 39.898179,116.360493,73695.000000 39.898250,116.360568,73700.000000 39.898265,116.360499,73705.000000 39.898300,116.360227,73710.000000 39.898289,116.360003,73715.000000 39.898270,116.359934,73720.000000 39.898261,116.359874,73725.000000 39.898279,116.359816,73730.000000 39.898265,116.359769,73735.000000 39.898253,116.359685,73740.000000 39.898238,116.359563,73745.000000 39.898224,116.359390,73750.000000 39.898189,116.359176,73755.000000 39.898227,116.358939,73760.000000 39.898227,116.358822,73765.000000 39.898316,116.358688,73770.000000 39.898316,116.358688,73773.000000 39.898322,116.358683,73775.000000 39.898301,116.358699,73780.000000 39.898291,116.358734,73785.000000 39.898316,116.358743,73790.000000 39.898394,116.358714,73795.000000 39.898415,116.358713,73800.000000 39.898398,116.358739,73805.000000 39.898372,116.358751,73810.00000039.898364,116.358760,73815.000000 39.898199,116.358869,73820.000000 39.898261,116.358692,73825.000000 39.898307,116.358321,73830.000000 39.898268,116.357883,73835.000000 39.898184,116.357394,73840.000000 39.898139,116.356973,73845.000000 39.898117,116.356614,73850.000000 39.898096,116.356179,73855.000000 39.898099,116.355653,73860.000000 39.898136,116.355269,73865.000000 39.898141,116.355210,73870.000000 39.898116,116.355211,73875.000000 39.898070,116.355065,73880.000000 39.897976,116.354853,73885.000000 39.897999,116.354462,73890.000000 39.898294,116.353749,73895.000000 39.898605,116.353063,73900.000000 39.898966,116.352387,73905.000000 39.899362,116.351815,73910.000000 39.899782,116.351365,73915.000000 39.900191,116.351080,73920.000000 39.900585,116.350870,73925.000000 39.900968,116.350676,73930.000000 39.901332,116.350619,73935.000000 39.901639,116.350636,73940.000000 39.901950,116.350641,73945.000000 39.902285,116.350639,73950.000000 39.902565,116.350683,73955.000000 39.902674,116.350744,73960.000000 39.902789,116.350737,73965.000000 39.903081,116.350654,73970.000000 39.903418,116.350640,73975.000000 39.903732,116.350600,73980.000000 39.904087,116.350549,73985.000000 39.904495,116.350512,73990.000000 39.904973,116.350447,73995.000000 39.905493,116.350416,74000.000000 39.906046,116.350426,74005.000000 39.906557,116.350422,74010.000000 39.907017,116.350440,74015.000000 39.907416,116.350444,74020.000000 39.907597,116.350625,74025.000000 39.907692,116.350971,74030.000000 39.909147,116.350777,74050.000000 39.909295,116.350835,74055.000000 39.909569,116.350661,74060.000000 39.909620,116.350657,74065.000000 39.909678,116.350680,74070.000000 39.909689,116.350676,74075.000000 39.909683,116.350661,74080.000000 39.909673,116.350658,74085.000000 39.909709,116.350677,74090.000000 39.909791,116.350686,74095.000000 39.909880,116.350696,74100.000000 39.910042,116.350729,74105.000000 39.910316,116.350767,74110.000000 39.910657,116.350790,74115.000000 39.910985,116.350733,74120.000000 39.911210,116.350724,74125.00000039.911432,116.350628,74130.000000 39.911647,116.350582,74135.000000 39.911892,116.350593,74140.000000 39.912221,116.350577,74145.000000 39.912551,116.350518,74150.000000 39.912926,116.350458,74155.000000 39.913363,116.350402,74160.000000 39.913819,116.350395,74165.000000 39.914300,116.350369,74170.000000 39.914809,116.350339,74175.000000 39.915379,116.350336,74180.000000 39.915880,116.350319,74185.000000 39.916284,116.350293,74190.000000 39.916606,116.350330,74195.000000 39.916976,116.350284,74200.000000 39.917393,116.350236,74205.000000 39.917828,116.350203,74210.000000 39.918233,116.350220,74215.000000 39.918571,116.350221,74220.000000 39.918911,116.350191,74225.000000 39.919181,116.350082,74230.000000 39.919430,116.349978,74235.000000 39.919691,116.349903,74240.000000 39.919972,116.349831,74245.000000 39.920211,116.349906,74260.000000 39.920541,116.349930,74265.000000 39.921006,116.349895,74270.000000 39.921712,116.349713,74275.000000 39.922216,116.349753,74280.000000 39.922649,116.349708,74285.000000 39.923240,116.350016,74295.000000 39.923572,116.350023,74300.000000 39.923866,116.350047,74305.000000 39.924058,116.350049,74310.000000 39.924211,116.350029,74315.000000 39.924282,116.350034,74320.000000 39.924232,116.350050,74325.000000 39.923852,116.351188,74370.000000 39.924007,116.350945,74375.000000 39.924386,116.350624,74380.000000 39.924706,116.350484,74385.000000 39.925044,116.350371,74390.000000 39.925395,116.350349,74395.000000 39.925896,116.350242,74400.000000 39.926407,116.350164,74405.000000 39.926878,116.350128,74410.000000 39.930222,116.349838,74450.000000 39.930416,116.349695,74455.000000 39.930538,116.349930,74460.000000 39.930492,116.349943,74465.000000 39.930482,116.349908,74470.000000 39.930421,116.349982,74475.000000 39.930524,116.349973,74480.000000 39.930657,116.349999,74485.000000 39.930840,116.349872,74490.000000 39.931024,116.349510,74495.000000 39.931114,116.348975,74500.000000 39.931150,116.348363,74505.000000 39.931295,116.347609,74510.000000 39.931340,116.347022,74515.00000039.931299,116.346703,74520.000000 39.931225,116.346589,74525.000000 39.931130,116.346646,74530.000000 39.930945,116.346739,74535.000000 39.930928,116.346701,74540.000000 39.930940,116.346517,74545.000000 39.930951,116.346510,74550.000000 39.930951,116.346510,74553.000000 39.930952,116.346518,74555.000000 39.930959,116.346490,74560.000000 39.931019,116.346315,74565.000000 39.931050,116.346052,74570.000000 39.931108,116.345635,74575.000000 39.931177,116.345091,74580.000000 39.931221,116.344590,74585.000000 39.931208,116.344107,74590.000000 39.931210,116.343833,74595.000000 39.931208,116.343607,74600.000000 39.931184,116.343457,74605.000000 39.931140,116.343461,74610.000000 39.931121,116.343451,74615.000000 39.931101,116.343443,74620.000000 39.931113,116.343427,74625.000000 39.931123,116.343417,74630.000000 39.931123,116.343417,74633.000000 39.931129,116.343424,74635.000000 39.931116,116.343437,74640.000000 39.931096,116.343448,74645.000000 39.931082,116.343450,74650.000000 39.931073,116.343452,74655.000000 39.931057,116.343463,74660.000000 39.931057,116.343463,74663.000000 39.931056,116.343462,74665.000000 39.931065,116.343448,74670.000000 39.931085,116.343349,74675.000000 39.931104,116.343102,74680.000000 39.931136,116.342726,74685.000000 39.931201,116.342489,74690.000000 39.931213,116.342389,74695.000000 39.931221,116.342316,74700.000000 39.931248,116.342213,74705.000000 39.931259,116.341976,74710.000000 39.931252,116.341657,74715.000000 39.931267,116.341195,74720.000000 39.931300,116.340526,74725.000000 39.931308,116.339726,74730.000000 39.931274,116.339074,74735.000000 39.931234,116.338536,74740.000000 39.931243,116.338178,74745.000000 39.931260,116.338030,74750.000000 39.931269,116.337854,74755.000000 39.930836,116.335244,74770.000000 39.930964,116.336016,74775.000000 39.931335,116.336465,74780.000000 39.931854,116.337529,74785.000000 39.932388,116.337766,74790.000000 39.932958,116.338235,74795.000000 39.933516,116.338172,74800.000000 39.934020,116.338148,74805.000000 39.935514,116.338162,74830.00000039.935621,116.338019,75035.000000 39.935754,116.338002,75040.000000 39.935746,116.337991,75045.000000 39.935746,116.337991,75048.000000 39.935751,116.337993,75050.000000 39.935779,116.338005,75055.000000 39.935813,116.338033,75060.000000 39.935813,116.338033,75063.000000 39.935813,116.338025,75065.000000 39.935835,116.338045,75070.000000 39.935864,116.338047,75075.000000 39.935861,116.338032,75080.000000 39.935871,116.338035,75085.000000 39.935936,116.338026,75090.000000 39.936059,116.338082,75095.000000 39.936040,116.338250,75100.000000 39.936048,116.338558,75105.000000 39.936060,116.338972,75110.000000 39.936048,116.339334,75115.000000 39.936100,116.339517,75120.000000 39.936250,116.339566,75125.000000 39.936468,116.339600,75130.000000 39.936586,116.339526,75135.000000 39.936864,116.339775,75140.000000 39.936936,116.340184,75145.000000 39.936932,116.340631,75150.000000 39.937199,116.341041,75155.000000 39.937411,116.341427,75160.000000 39.938009,116.341870,75165.000000 39.938179,116.342330,75170.000000 39.938338,116.342780,75175.000000 39.938343,116.343130,75180.000000 39.938357,116.343409,75185.000000 39.938380,116.343514,75190.000000 39.938373,116.343527,75195.000000 39.938395,116.343533,75200.000000 39.938426,116.343532,75205.000000 39.938441,116.343566,75210.000000 39.938442,116.343575,75215.000000 39.938438,116.343565,75220.000000 39.938448,116.343612,75225.000000 39.938522,116.343894,75230.000000 39.938610,116.344240,75235.000000 39.938672,116.344613,75240.000000 39.938739,116.344967,75245.000000 39.938763,116.345215,75250.000000 39.938746,116.345308,75255.000000 39.938759,116.345356,75260.000000 39.938766,116.345539,75265.000000 39.938789,116.345810,75270.000000 39.938872,116.346036,75275.000000 39.938922,116.346163,75280.000000 39.938977,116.346179,75285.000000 39.939069,116.346042,75290.000000 39.939241,116.345961,75295.000000 39.939501,116.345861,75300.000000 39.939934,116.345870,75305.000000 39.940226,116.345762,75310.000000 39.940337,116.345826,75315.000000 39.940318,116.345813,75320.00000039.940311,116.345793,75325.000000 39.940297,116.345783,75330.000000 39.940282,116.345770,75335.000000 39.940264,116.345768,75340.000000 39.940475,116.345724,75345.000000 39.940734,116.345756,75350.000000 39.940963,116.345615,75355.000000 39.941140,116.345735,75360.000000 39.941310,116.345639,75365.000000 39.941549,116.345660,75370.000000 39.941827,116.345600,75375.000000 39.942243,116.345352,75380.000000 39.942528,116.345051,75385.000000 39.942706,116.344683,75390.000000 39.942877,116.344266,75395.000000 39.943061,116.343712,75400.000000 39.943215,116.343349,75405.000000 39.943274,116.343210,75410.000000 39.943376,116.343092,75415.000000 39.943544,116.342728,75420.000000 39.943646,116.342328,75425.000000 39.943753,116.342049,75430.000000 39.943764,116.341967,75435.000000 39.943764,116.341967,75438.000000 39.943762,116.341967,75440.000000 39.943771,116.341873,75445.000000 39.943890,116.341596,75450.000000 39.943985,116.341198,75455.000000 39.944137,116.340791,75460.000000 39.944279,116.340381,75465.000000 39.944490,116.339925,75470.000000 39.944657,116.339498,75475.000000 39.944793,116.339086,75480.000000 39.944918,116.338643,75485.000000 39.945012,116.338232,75490.000000 39.945153,116.337843,75495.000000 39.945294,116.337498,75500.000000 39.945384,116.337210,75505.000000 39.945404,116.337110,75510.000000 39.945425,116.337128,75515.000000 39.945425,116.337128,75518.000000 39.945429,116.337121,75520.000000 39.945446,116.337102,75525.000000 39.945481,116.337081,75530.000000 39.945524,116.337031,75535.000000 39.945574,116.336936,75540.000000 39.945609,116.336876,75545.000000 39.945656,116.336838,75550.000000 39.945719,116.336791,75555.000000 39.945769,116.336747,75560.000000 39.945794,116.336675,75565.000000 39.945822,116.336602,75570.000000 39.945869,116.336529,75575.000000 39.945913,116.336443,75580.000000 39.945951,116.336341,75585.000000 39.946008,116.336241,75590.000000 39.946055,116.336189,75595.000000 39.946079,116.336123,75600.000000 39.946101,116.336093,75605.000000 39.946134,116.336056,75610.00000039.946148,116.336053,75615.000000 39.946195,116.336029,75620.000000 39.946266,116.336010,75625.000000 39.946331,116.335950,75630.000000 39.946365,116.335950,75635.000000 39.946424,116.335954,75640.000000 39.946464,116.335934,75645.000000 39.946457,116.335890,75650.000000 39.946445,116.335882,75655.000000 39.946463,116.335868,75660.000000 39.946526,116.335845,75665.000000 39.946617,116.335806,75670.000000 39.946744,116.335803,75675.000000 39.946813,116.335798,75680.000000 39.946841,116.335799,75685.000000 39.946914,116.335835,75690.000000 39.947017,116.335854,75695.000000 39.947144,116.335840,75700.000000 39.947256,116.335866,75705.000000 39.947316,116.335809,75710.000000 39.947359,116.335768,75715.000000 39.947391,116.335639,75720.000000 39.947479,116.335556,75725.000000 39.947534,116.335504,75730.000000 39.947588,116.335494,75735.000000 39.947642,116.335472,75740.000000 39.947713,116.335455,75745.000000 39.947754,116.335436,75750.000000 39.947745,116.335439,75755.000000 39.947763,116.335433,75760.000000 39.947852,116.335419,75765.000000 39.947972,116.335363,75770.000000 39.948087,116.335287,75775.000000 39.948155,116.335281,75780.000000 39.948196,116.335268,75785.000000 39.948299,116.335185,75790.000000 39.948312,116.335165,75791.035208"
//                    );
            double res1 = 0;
            double res2 = 0;
//            for (auto querylen:seglen) {
//                tjstat->bt = querylen;
//                auto r = buildMBCRTreeWP(&x, xTrajectory::ISS, querylen);
////            r->intersectsWithQuery(query, vis);
//                r->nearestNeighborQuery(6, tj, vis);
//
//                std::cerr<<"Hoo " << vis.m_resultGet << " " << vis.m_lastResult << endl;
////                if(res1==0&&res2==0) {
////                    res1 = vis.m_resultGet;
////                    res2 = vis.m_lastResult;
////                }else{
////                    if(vis.m_resultGet!=res1 || vis.m_lastResult!=res2){
////                        cerr<<tj<<endl;
////                        return 1;
////                    }
////                }
//                vis.clear();
////            auto r2 = buildMBRRTreeWP(&x, xTrajectory::ISS,querylen);
////            r2->intersectsWithQuery(query, vis);
////            r2->nearestNeighborQuery(6, tj, vis);
////            std::cerr << vis.m_resultGet << " " << vis.m_lastResult << endl;
////            vis.clear();
//            }
//        {
//            auto r = buildTBTreeWP(&x);
////            r->intersectsWithQuery(query, vis);
//            r->nearestNeighborQuery(6, tj, vis);
//            std::cerr <<"Hoo " << vis.m_resultGet << " " << vis.m_lastResult << endl;
//            vis.clear();
//        }
        {
            auto r = buildSTRTreeWP(&x);
//            r->intersectsWithQuery(query, vis);
//            r->findid(3134);
            r->nearestNeighborQuery(6, tj, vis);
            std::cerr << vis.m_resultGet << " " << vis.m_lastResult << endl;
            vis.clear();
            delete r;
        }
//        for (auto querylen:querylens) {
//            tjstat->bt = querylen;
//            auto r = buildMBCRTreeWP(&x, xTrajectory::ISS,querylen);
//            r->intersectsWithQuery(query, vis);
//            r->nearestNeighborQuery(6, tj, vis);
//            std::cerr << vis.m_resultGet << " " << vis.m_lastResult << endl;
//            vis.clear();
//            auto r2 = buildMBRRTreeWP(&x, xTrajectory::ISS,querylen);
//            r2->intersectsWithQuery(query, vis);
//            r2->nearestNeighborQuery(6, tj, vis);
//            std::cerr << vis.m_resultGet << " " << vis.m_lastResult << endl;
//            vis.clear();
//        }
//        {
//            auto r = buildTBTreeWP(&x);
//            r->intersectsWithQuery(query, vis);
//            r->nearestNeighborQuery(6, tj, vis);
//            std::cerr << vis.m_resultGet << " " << vis.m_lastResult << endl;
//            vis.clear();
//        }
//        {
//            auto r = buildSTRTreeWP(&x);
//            r->intersectsWithQuery(query, vis);
//            r->nearestNeighborQuery(6, tj, vis);
//            std::cerr << vis.m_resultGet << " " << vis.m_lastResult << endl;
//            vis.clear();
//        }
        }
    }
    catch (Tools::Exception &e) {
        cerr << "******ERROR******" << endl;
        std::string s = e.what();
        cerr << s << endl;
        return -1;
    }
}