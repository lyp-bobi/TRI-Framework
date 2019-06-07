#include <spatialindex/SpatialIndex.h>

using namespace SpatialIndex;
using namespace std;

/* 
 * Test the Geometry
 * Nowhere near complete, but it's something
 */
int main(int argc, char** argv) {
    //define points
    double c1[2] = {1.0, 0.0};
    double c2[2] = {3.0, 2.0};
    double c3[2] = {2.0, 0.0};
    double c4[2] = {2.0, 4.0};
    double c5[2] = {1.0, 1.0};
    double c6[2] = {2.5, 3.0};
    double c7[2] = {1.0, 2.0};
    double c8[2] = {0.0, -1.0};
    double c9[2] = {4.0, 3.0};
    Point p1 = Point(&c1[0], 2); 
    Point p2 = Point(&c2[0], 2); 
    Point p3 = Point(&c3[0], 2); 
    Point p4 = Point(&c4[0], 2); 
    Point p5 = Point(&c5[0], 2); 
    Point p6 = Point(&c6[0], 2); 
    Point p7 = Point(&c7[0], 2); 
    Point p8 = Point(&c8[0], 2); 
    Point p9 = Point(&c9[0], 2); 
    
    double c3a[2] = {2.0, 3.0};
    Point p3a = Point(&c3a[0], 2); 
    
    //Now Test LineSegment intersection
    LineSegment ls1 = LineSegment(p1, p2);
    LineSegment ls2 = LineSegment(p3, p4);
    LineSegment ls3 = LineSegment(p3a, p4);

    if (!ls1.intersectsShape(ls2)) {
        cerr << "Test failed:  intersectsShape returned false, but should be true." << endl;
        cerr << ls1 << ", " << ls2 << endl;
        return -1;
    }

    if (ls1.intersectsShape(ls3)) {
        cerr << "Test failed:  intersectsShape returned true, but should be false." << endl;
        cerr << ls1 << ", " << ls3 << endl;
        return -1;
    }

    //Now LineSegment Region intersection
    Region r1 = Region(p5, p6);
    Region r2 = Region(p7, p6);
    Region r3 = Region(p8, p9);
    
    if (!r1.intersectsShape(ls1) || !ls1.intersectsShape(r1)) {
        cerr << "Test failed:  intersectsShape returned false, but should be true." << endl;
        cerr << r1 << ", " << ls1 << endl;
        return -1;
    }

    if (r2.intersectsShape(ls1) || ls1.intersectsShape(r2)) {
        cerr << "Test failed:  intersectsShape returned true, but should be false." << endl;
        cerr << r2 << ", " << ls1 << endl;
        return -1;
    }

    // This is the contains test
    if (!r3.intersectsShape(ls1) || !ls1.intersectsShape(r3)) {
        cerr << "Test failed:  intersectsShape returned false, but should be true." << endl;
        cerr << r3 << ", " << ls1 << endl;
        return -1;
    }

    //Mbbc test
    double sLow[2]={39.996512,116.322447};
    double sHigh[2]={39.996512,116.322447};
    double eLow[2]={40.006551,116.329964};
    double eHigh[2]={40.006551,116.329964};
    double vLow[2]={0.000001,-0.000008};
    double vHigh[2]={0.000030,0.000017};
    double pLow[2]={39.996534,116.322148};
    double pHigh[2]={39.999769,116.324886};
    double oLow[2]={39.996534,116.322148};
    double oHigh[2]={39.999769,116.324886};
    //just for test, should use delete to free pointers
    Mbbc m=Mbbc(Region(sLow,sHigh,2),Region(eLow,eHigh,2),
                     Region(vLow,vHigh,2),Region(pLow,pHigh,2),0.0,900.0);
    Region r(pLow,pHigh,2);
    TimeRegion t(oLow,oHigh,0.5,0.5,2);
//    cout<<m.intersectsTimeRegion(t);
    uint8_t* d;
    uint32_t l;
//    r.storeToByteArray(&d,l);
//    cout<<d[30]<<endl;
//    r.loadFromByteArray(d);
    m.storeToByteArray(&d,l);

    m.loadFromByteArray(d);
//    cout<<m.toString();
    double pc[2];
    vector<TimePoint> points;
    pc[0]=39.990873;pc[1]=116.317939;points.emplace_back(TimePoint(pc,680,680,2));
    pc[0]=39.991146;pc[1]=116.317736;points.emplace_back(TimePoint(pc,685,685,2));
    pc[0]=39.991179;pc[1]=116.317958;points.emplace_back(TimePoint(pc,690,690,2));
    pc[0]=39.991291;pc[1]=116.318188;points.emplace_back(TimePoint(pc,695,695,2));
    pc[0]=39.991339;pc[1]=116.318216;points.emplace_back(TimePoint(pc,700,700,2));
    pc[0]=39.99135;pc[1]=116.318202;points.emplace_back(TimePoint(pc,705,705,2));
    pc[0]=39.991341;pc[1]=116.318231;points.emplace_back(TimePoint(pc,710,710,2));
    pc[0]=39.991326;pc[1]=116.318289;points.emplace_back(TimePoint(pc,715,715,2));
    pc[0]=39.991324;pc[1]=116.318438;points.emplace_back(TimePoint(pc,720,720,2));
    pc[0]=39.991322;pc[1]=116.31864;points.emplace_back(TimePoint(pc,725,725,2));
    pc[0]=39.991327;pc[1]=116.318837;points.emplace_back(TimePoint(pc,730,730,2));
    pc[0]=39.991313;pc[1]=116.318953;points.emplace_back(TimePoint(pc,735,735,2));
    pc[0]=39.991345;pc[1]=116.319096;points.emplace_back(TimePoint(pc,740,740,2));
    pc[0]=39.991377;pc[1]=116.31919;points.emplace_back(TimePoint(pc,745,745,2));
    pc[0]=39.991434;pc[1]=116.31931;points.emplace_back(TimePoint(pc,750,750,2));
    pc[0]=39.991416;pc[1]=116.319416;points.emplace_back(TimePoint(pc,755,755,2));
    pc[0]=39.991453;pc[1]=116.31961;points.emplace_back(TimePoint(pc,760,760,2));
    pc[0]=39.991559;pc[1]=116.319749;points.emplace_back(TimePoint(pc,765,765,2));
    pc[0]=39.991624;pc[1]=116.319851;points.emplace_back(TimePoint(pc,770,770,2));
    pc[0]=39.991617;pc[1]=116.319928;points.emplace_back(TimePoint(pc,775,775,2));
    pc[0]=39.991737;pc[1]=116.320008;points.emplace_back(TimePoint(pc,780,780,2));
    pc[0]=39.99179;pc[1]=116.320071;points.emplace_back(TimePoint(pc,785,785,2));
    pc[0]=39.991922;pc[1]=116.320241;points.emplace_back(TimePoint(pc,790,790,2));
    pc[0]=39.992099;pc[1]=116.320387;points.emplace_back(TimePoint(pc,795,795,2));
    pc[0]=39.992271;pc[1]=116.320474;points.emplace_back(TimePoint(pc,800,800,2));
    pc[0]=39.992403;pc[1]=116.320479;points.emplace_back(TimePoint(pc,805,805,2));
    pc[0]=39.992503;pc[1]=116.320392;points.emplace_back(TimePoint(pc,810,810,2));
    pc[0]=39.992621;pc[1]=116.320306;points.emplace_back(TimePoint(pc,815,815,2));
    pc[0]=39.992755;pc[1]=116.320391;points.emplace_back(TimePoint(pc,820,820,2));
    pc[0]=39.992876;pc[1]=116.32036;points.emplace_back(TimePoint(pc,825,825,2));
    pc[0]=39.993004;pc[1]=116.320402;points.emplace_back(TimePoint(pc,830,830,2));
    pc[0]=39.993094;pc[1]=116.320398;points.emplace_back(TimePoint(pc,835,835,2));
    pc[0]=39.993234;pc[1]=116.320376;points.emplace_back(TimePoint(pc,840,840,2));
    pc[0]=39.993469;pc[1]=116.320318;points.emplace_back(TimePoint(pc,845,845,2));
    pc[0]=39.993673;pc[1]=116.320364;points.emplace_back(TimePoint(pc,850,850,2));
    pc[0]=39.993807;pc[1]=116.320333;points.emplace_back(TimePoint(pc,855,855,2));
    pc[0]=39.993969;pc[1]=116.320326;points.emplace_back(TimePoint(pc,860,860,2));
    pc[0]=39.994174;pc[1]=116.320295;points.emplace_back(TimePoint(pc,865,865,2));
    pc[0]=39.994401;pc[1]=116.320241;points.emplace_back(TimePoint(pc,870,870,2));
    pc[0]=39.994525;pc[1]=116.320202;points.emplace_back(TimePoint(pc,875,875,2));
    pc[0]=39.994604;pc[1]=116.320265;points.emplace_back(TimePoint(pc,880,880,2));
    pc[0]=39.994705;pc[1]=116.32041;points.emplace_back(TimePoint(pc,885,885,2));
    pc[0]=39.994748;pc[1]=116.320628;points.emplace_back(TimePoint(pc,890,890,2));
    pc[0]=39.994779;pc[1]=116.320831;points.emplace_back(TimePoint(pc,895,895,2));

    Trajectory traj(points);
//    cout<<"m and r is"<<m.toString()<<r.toString();
    double qLow[2]={39.993017,116.320135};
    double qHigh[2]={39.994017,116.321135};
    TimeRegion tm=TimeRegion(qLow,qHigh,855,855,2);
//    cout<<traj.getMinimumDistance(m);
//    cout<<"end here\n\n\n\n";
//    cout<<traj.getMinimumDistance(r);

    Trajectory traja,trajb;
    string s1="11979.000000,23595.000000,0.000000 11844.161443,23823.826393,1.000000 11566.996640,23714.368362,2.000000 11293.529319,23"
              "596.158248,3.000000 11317.706793,23606.708419,4.000000 11514.395488,23637.382848,5.000000 11694.590570,23206.667867,6.00"
              "0000 11886.920657,22740.267408,7.000000 12035.933386,22398.460392,8.000000 12170.283153,22090.489387,9.000000 12309.9805"
              "54,21784.917180,10.000000 12761.509409,21896.533371,11.000000 13265.883023,21885.241425,12.000000 13770.256636,21873.949"
              "478,13.000000 14273.796395,21847.364776,14.000000 14776.407557,21803.749757,15.000000 15278.700824,21811.281931,16.00000"
              "0 15780.731016,21861.141094,17.000000 16275.737818,21940.342485,18.000000 16745.934175,22123.196624,19.000000 17214.9679"
              "59,22308.969769,20.000000 17682.129644,22499.443417,21.000000 17900.494433,22709.897371,22.000000 17726.542293,22951.857"
              "395,23.000000 17667.667190,23224.664222,24.000000 18001.656559,23602.779749,25.000000 18337.790290,23978.989211,26.00000"
              "0 18399.485809,24172.233422,27.000000 18300.565201,24259.633155,28.000000 18201.673471,24347.065478,29.000000 18103.0044"
              "22,24434.749107,30.000000 18004.335373,24522.432736,31.000000 17905.666324,24610.116364,32.000000 17806.997274,24697.799"
              "993,33.000000 17709.132604,24765.682038,34.000000 17617.900673,24670.284266,35.000000 17526.668743,24574.886494,36.00000"
              "0 17438.492324,24513.203300,37.000000 17357.175583,24409.224516,38.000000 17275.858841,24305.245731,39.000000 17192.8293"
              "14,24202.653365,40.000000 17101.502430,24108.254773,41.000000 17179.785600,24187.080768,42.000000 17263.344905,24289.244"
              "305,43.000000 17344.661646,24393.223089,44.000000 17425.978388,24497.201873,45.000000 17512.628946,24560.205610,46.00000"
              "0 17603.860876,24655.603382,47.000000 17695.092806,24751.001154,48.000000 17791.812970,24711.293738,49.000000 17890.4820"
              "19,24623.610109,50.000000 17989.151068,24535.926480,51.000000 18087.820117,24448.242851,52.000000 18186.489166,24360.559"
              "223,53.000000 18285.342184,24273.083211,54.000000 18384.262791,24185.683478,55.000000 18389.727470,24036.697189,56.00000"
              "0 18053.209400,23660.830837,57.000000 17719.065237,23282.852899,58.000000 17445.260538,22980.077644,59.000000 17112.2302"
              "70,22601.171061,60.000000 16789.814441,22230.405050,61.000000 16470.403519,21902.152891,62.000000 16069.398624,21666.278"
              "478,63.000000 15603.996888,21471.842083,64.000000 15135.674238,21284.259322,65.000000 14725.993968,21122.102047,66.00000"
              "0 14414.608278,20995.897526,67.000000 14104.266506,20867.119697,68.000000 13635.691312,20680.190532,69.000000 13220.9841"
              "23,20500.526656,70.000000 13000.650831,20258.928740,71.000000 12842.522239,19983.506062,72.000000 13106.967593,19593.167"
              "027,73.000000 13449.132127,19266.899803,74.000000 13321.737887,18836.052957,75.000000 13127.840066,18566.171476,76.00000"
              "0 12671.587780,18472.504611,77.000000 12337.392750,18437.724132,78.000000 12194.309822,18204.085620,79.000000 12014.6328"
              "78,18155.696003,80.000000 11797.605034,17763.773926,81.000000 11721.894767,17390.197625,82.000000 11721.557123,17257.857"
              "549,83.000000 11752.987669,17068.448520,84.000000 11811.785529,16884.211589,85.000000 12210.794343,16715.632579,86.00000"
              "0 12562.316956,16640.411573,87.000000 12848.333115,16487.211065,88.000000 13141.952434,16320.977101,89.000000 13531.1540"
              "58,16156.045472,90.000000 14027.898682,16070.902086,91.000000 14466.489554,15918.464124,92.000000 14741.848961,15743.952"
              "776,93.000000 15046.941547,15516.604003,94.000000 15455.545127,15220.722878,95.000000 15855.638411,14915.993509,96.00000"
              "0 15892.214822,14243.883341,97.000000 15786.006174,13949.960053,98.000000 16054.774986,13845.885697,99.000000 15664.4906"
              "56,13970.965684,100.000000 15381.650002,13659.005359,101.000000 15082.393595,13325.621104,102.000000 14889.987426,13115."
              "548277,103.000000 14739.661476,12949.482349,104.000000 14450.451759,12758.827097,105.000000 14168.581336,12662.115855,10"
              "6.000000 13886.710912,12565.404613,107.000000 13604.842458,12468.687633,108.000000 13475.402115,12292.128164,109.000000"
              "13618.327689,11988.492143,110.000000 13687.665546,11603.345419,111.000000 13252.505199,11512.618565,112.000000 12901.616"
              "340,11465.237849,113.000000 12574.417958,11493.353632,114.000000 12300.865379,11482.771847,115.000000 11374.923460,11447"
              ".190459,116.000000 11267.439799,11249.341095,117.000000 11053.067623,10855.960226,118.000000 10837.723509,10463.117748,1"
              "19.000000 10665.080326,10058.138040,120.000000 10632.574370,9611.318879,121.000000 10631.496117,9164.792435,122.000000 1"
              "0971.168778,9098.536951,123.000000 11300.807367,9146.972481,124.000000 11634.612901,9263.488803,125.000000 11945.384704,"
              "9272.879270,126.000000 12092.730083,9013.855449,127.000000 12167.368317,9487.124271,128.000000 12374.250023,9573.007678,"
              "129.000000 12736.039067,9739.841470,130.000000 12937.112256,9838.561138,131.000000 13275.236029,10024.056221,132.000000"
              "13470.335205,10134.112167,133.000000 13812.597748,10327.810584,134.000000 13863.350341,10647.278845,135.000000 13858.000"
              "000,10983.235700,136.000000 13860.367073,11414.626831,137.000000 13755.405823,11728.175813,138.000000 13598.782361,12025"
              ".411096,139.000000 13459.585279,12330.791539,140.000000 13335.422973,12643.003222,141.000000 13395.289613,13008.229891,1"
              "42.000000 13861.741155,13200.436054,143.000000 14306.008033,13436.940570,144.000000 14718.238197,13726.537033,145.000000"
              " 15123.709376,14021.907359,146.000000 15625.863458,14070.502915,147.000000 16127.012221,14080.748167,148.000000 16060.91"
              "1062,14442.619839,149.000000 15863.981561,14906.904234,150.000000 15465.783842,15213.607839,151.000000 15119.036316,1546"
              "3.616728,152.000000 14833.594570,15675.449387,153.000000 14713.623260,15763.046872,154.000000 14835.175284,15849.548507,"
              "155.000000 14956.560427,15935.958270,156.000000 15136.856678,16082.062976,157.000000 15335.148608,16300.113872,158.00000"
              "0 15522.551656,16177.760629,159.000000 15680.789327,15977.370267,160.000000 15519.807871,16189.147336,161.000000 15324.2"
              "80253,16297.680464,162.000000 15129.021349,16073.357055,163.000000 14951.789494,15932.562013,164.000000 14830.404351,158"
              "46.152250,165.000000 14708.194253,15759.288329,166.000000 14378.320588,15977.490010,167.000000 14021.871179,16071.811897"
              ",168.000000 13689.674317,16122.219532,169.000000 13365.184338,16206.251543,170.000000 13066.815092,16360.670895,171.0000"
              "00 12776.273607,16529.433433,172.000000 12483.403642,16662.284617,173.000000 12102.570875,16778.982903,174.000000 11773."
              "242099,16897.614478,175.000000 11559.273174,16890.121952,176.000000 11336.705597,16864.830181,177.000000 11132.994106,17"
              "184.621844,178.000000 11024.448785,17391.177149,179.000000 10918.115826,17588.314890,180.000000 10809.878172,17784.42865"
              "9,181.000000 10742.336733,17990.208470,182.000000 10725.349852,18138.237000,183.000000 10695.571984,18360.996112,184.000"
              "000 10652.732573,18655.899753,185.000000 10453.504775,18874.839505,186.000000 10410.156933,19122.702865,187.000000 10377"
              ".410365,19294.006327,188.000000 10369.228463,19405.707072,189.000000 10361.046561,19517.407818,190.000000 10359.000000,1"
              "9824.794892,191.000000 10361.703217,20048.775841,192.000000 10363.728451,20272.766686,193.000000 10365.753685,20496.7575"
              "30,194.000000 10364.413306,20720.750019,195.000000 10351.017069,20956.608427,196.000000 10143.025218,21083.978504,197.00"
              "0000 10090.951367,21080.345444,198.000000 10256.421714,20959.640330,199.000000 10368.267320,20902.864488,200.000000";
    string s34="11600.000000,17854.000000,469.000000 11602.381930,17837.167697,470.000000 11604.763859,17820.335395,471.000000 11607.145\n"
               "789,17803.503092,472.000000 11609.527718,17786.670790,473.000000 11611.909648,17769.838487,474.000000 11614.291578,17753\n"
               ".006184,475.000000 11610.712375,17736.852176,476.000000 11604.665066,17721.789014,477.000000 11620.869201,17716.648391,4\n"
               "78.000000 11637.073337,17711.507769,479.000000 11653.277472,17706.367147,480.000000 11669.481607,17701.226525,481.000000\n"
               " 11685.685743,17696.085902,482.000000 11701.889878,17690.945280,483.000000 11718.094013,17685.804658,484.000000 11734.29\n"
               "8149,17680.664036,485.000000 11747.949349,17667.815981,486.000000 11736.616679,17579.540444,487.000000 11729.525211,1749\n"
               "0.966471,488.000000 11723.333302,17402.305293,489.000000 11718.763786,17313.724277,490.000000 11723.708492,17224.890746,\n"
               "491.000000 11738.830208,17137.184796,492.000000 11759.735751,17051.015977,493.000000 11779.664956,16992.525147,494.00000\n"
               "0 11804.968853,16907.198054,495.000000 11857.431047,16841.652441,496.000000 11936.554926,16802.684244,497.000000 12020.7\n"
               "58466,16774.077791,498.000000 12129.047234,16763.484546,499.000000 12244.691173,16695.790533,500.000000 12370.067705,166\n"
               "63.338905,501.000000 12504.061908,16662.092447,502.000000 12627.308055,16611.477824,503.000000 12749.897856,16557.369222\n"
               ",504.000000 12851.043075,16485.623198,505.000000 12966.963713,16418.404867,506.000000 13082.968458,16351.331034,507.0000\n"
               "00 13202.180326,16290.151802,508.000000 13321.096996,16228.393252,509.000000 13445.668360,16180.579484,510.000000 13574.\n"
               "468887,16143.614310,511.000000 13706.135882,16119.661316,512.000000 13838.606326,16099.474517,513.000000 13971.105404,16\n"
               "079.474656,514.000000 14103.629591,16059.643786,515.000000 14236.219558,16040.255631,516.000000 14357.889314,15989.11470\n"
               "0,517.000000 14470.038325,15915.987644,518.000000 14579.994759,15839.400886,519.000000 14690.195678,15763.166003,520.000\n"
               "000 14798.200947,15701.876626,521.000000 14905.572497,15621.705869,522.000000 15013.092431,15541.734407,523.000000 15120\n"
               ".948842,15462.218683,524.000000 15229.127660,15383.140599,525.000000 15337.741627,15304.661777,526.000000 15446.792868,1\n"
               "5226.804956,527.000000 15518.190877,15217.446542,528.000000 15560.690083,15258.358102,529.000000 15606.681110,15295.3151\n"
               "78,530.000000 15641.561887,15342.374068,531.000000 15673.079778,15392.220409,532.000000 15714.749191,15422.000000,533.00\n"
               "0000 15761.862763,15452.373160,534.000000 15795.000000,15483.000000,535.000000";
    traja.loadFromString(s34);
    MBC mbc;
    traja.getMBC(mbc);
    double pLow2[]={11165.959655,16956.083865,469.000000};
    double pHigh2[]={12405.940672,18760.741050,469.000000};
    Region query(pLow2,pHigh2,3);
    cout<<query.intersectsShape(mbc);
    return 0;
}
