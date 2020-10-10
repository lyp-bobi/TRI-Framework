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

    //line distace test
//    double v1[2]={0,1},v2[2]={1,0},v3[2]={0,1},v4[2]={0,0};
//    double pLow[3]={0,0,0},pHigh[3]={1,1,1};
//    cout<<Trajectory::line2lineIED(TimePoint(v1,0,0,2),TimePoint(v2,1,1,2),
//            TimePoint(v3,0,0,2),TimePoint(v4,1,1,2))<<endl;
//    cout<<Trajectory::line2MBRDistance(TimePoint(v1,0,0,2),TimePoint(v2,1,1,2),
//            Region(pLow,pHigh,3))<<endl;

//    Trajectory traja;
//    traja.loadFromString("17778.000000,17543.000000,485.000000 17787.789971,17567.086437,486.000000 17797.579943,17591.172875,487.000000 17807.369914,17615.259312,488.000000 17817.159885,17639.345749,489.000000 17826.949856,17663.432187,490.000000 17836.739828,17687.518624,491.000000 17840.106177,17712.658699,492.000000 17838.523750,17738.610499,493.000000 17836.941323,17764.562299,494.000000 17835.125230,17790.497241,495.000000 17832.966047,17816.407431,496.000000 17830.806865,17842.317620,497.000000 17824.247663,17867.234211,498.000000 17815.063174,17891.557968,499.000000 17805.878685,17915.881726,500.000000 17796.694195,17940.205483,501.000000 17787.509706,17964.529240,502.000000 17778.325217,17988.852997,503.000000 17769.140728,18013.176754,504.000000 17759.956239,18037.500511,505.000000 17750.771749,18061.824268,506.000000 17741.587260,18086.148025,507.000000 17737.028723,18111.571502,508.000000 17734.284364,18137.426259,509.000000 17731.540004,18163.281017,510.000000 17728.795644,18189.135774,511.000000 17726.051284,18214.990532,512.000000 17723.306925,18240.845289,513.000000 17720.562565,18266.700047,514.000000 17723.487434,18292.373955,515.000000 17727.874085,18318.001231,516.000000 17732.260735,18343.628507,517.000000 17736.647386,18369.255784,518.000000 17743.094715,18394.339212,519.000000 17751.925472,18418.793613,520.000000 17760.756228,18443.248015,521.000000 17769.586984,18467.702416,522.000000 17771.982950,18492.994123,523.000000 17644.786266,18535.148644,524.000000 17517.589582,18577.303165,525.000000 17389.031479,18614.287901,526.000000 17257.570322,18640.248718,527.000000 17125.008294,18647.900622,528.000000 16992.558917,18630.695301,529.000000 16862.708040,18597.608299,530.000000 16734.756447,18558.039141,531.000000 16607.582107,18515.817260,532.000000 16562.000000,18505.000000,533.000000");
//    double plow[3]={16083.3,16481.8,485};
//    double pHigh[3]={17853,17749.1,485};
//    Region* query=new Region(plow,pHigh,3);
//    Region bra;
//    MBC bca;
//    traja.getMBRfull(bra);
//    traja.getMBC(bca);
//    cout<<*query<<endl<<bra<<endl<<bca<<endl;
//    query->intersectsShape(bra);
//    query->intersectsShape(bca);
//    double rlow[3]={11790.8,20103.8,79};
//    double rhigh[3]={12863.3,21411.3,99};
//    double clow[2]={16521.4,21944.1};
//    double chigh[2]={12213.8,18130.7};
//    Region br(rlow,rhigh,3);
//    MBC bc(clow,chigh,54,106,2,1330.78,119.552);
//    std::cerr<<bc.intersectsRegion(br);
//    Trajectory traja,trajb;
//    traja.loadFromString("39.961386,116.397554,70335.790399 39.961386,116.397554,70635.790399");
//    trajb.loadFromString("39.958082,116.362794,69328.000000 39.958082,116.362794,69331.000000 39.958078,116.362794,69333.000000 39.958084,116.362773,69338.000000 39.958074,116.362772,69343.000000 39.958033,116.362750,69348.000000 39.958025,116.362655,69353.000000 39.958024,116.362575,69358.000000 39.958021,116.362464,69363.000000 39.958020,116.362397,69368.000000 39.957994,116.362317,69373.000000 39.957982,116.362210,69378.000000 39.957922,116.362104,69383.000000 39.957915,116.361990,69388.000000 39.957937,116.361964,69393.000000 39.957970,116.361954,69398.000000 39.958116,116.361840,69403.000000 39.958116,116.361786,69408.000000 39.958131,116.361737,69413.000000 39.958120,116.361688,69418.000000 39.958129,116.361647,69423.000000 39.958120,116.361645,69428.000000 39.958120,116.361645,69431.000000 39.958113,116.361644,69433.000000 39.958100,116.361572,69438.000000 39.958100,116.361572,69441.000000 39.958104,116.361568,69443.000000 39.958076,116.361554,69448.000000 39.958062,116.361551,69453.000000 39.958062,116.361551,69456.000000 39.958059,116.361552,69458.000000 39.958042,116.361534,69463.000000 39.958040,116.361523,69468.000000 39.958034,116.361511,69473.000000 39.958055,116.361418,69478.000000 39.958067,116.361348,69483.000000 39.958097,116.361281,69488.000000 39.958108,116.361225,69493.000000 39.958132,116.361176,69498.000000 39.958119,116.361111,69503.000000 39.958128,116.361059,69508.000000 39.958133,116.360959,69513.000000 39.958131,116.360880,69518.000000 39.958113,116.360809,69523.000000 39.958076,116.360726,69528.000000 39.958093,116.360648,69533.000000 39.958019,116.360589,69538.000000 39.957953,116.360536,69543.000000 39.957976,116.360485,69548.000000 39.957985,116.360445,69553.000000 39.957980,116.360422,69558.000000 39.957955,116.360373,69563.000000 39.957955,116.360306,69568.000000 39.957955,116.360258,69573.000000 39.957962,116.360194,69578.000000 39.957983,116.360129,69583.000000 39.957972,116.360066,69588.000000 39.957968,116.359989,69593.000000 39.957934,116.359915,69598.000000 39.957917,116.359859,69603.000000 39.957917,116.359794,69608.000000 39.957901,116.359734,69613.000000 39.957904,116.359681,69618.000000 39.957904,116.359601,69623.000000 39.957907,116.359540,69628.000000 39.957905,116.359467,69633.000000 39.957912,116.359383,69638.000000 39.957916,116.359329,69643.000000 39.957920,116.359274,69648.000000 39.957928,116.359203,69653.000000 39.957960,116.359138,69658.000000 39.957971,116.359064,69663.000000 39.957975,116.359010,69668.000000 39.957985,116.358942,69673.000000 39.957956,116.358887,69678.000000 39.957932,116.358818,69683.000000 39.957953,116.358756,69688.000000 39.957962,116.358698,69693.000000 39.957977,116.358637,69698.000000 39.957975,116.358602,69703.000000 39.957983,116.358573,69708.000000 39.957984,116.358499,69713.000000 39.958060,116.358415,69718.000000 39.958042,116.358358,69723.000000 39.958075,116.358321,69728.000000 39.958064,116.358249,69733.000000 39.958084,116.358188,69738.000000 39.958100,116.358083,69743.000000 39.958096,116.358047,69748.000000 39.958096,116.358010,69753.000000 39.958087,116.357994,69758.000000 39.958083,116.357952,69763.000000 39.958082,116.357907,69768.000000 39.958143,116.357832,69773.000000 39.958078,116.357809,69778.000000 39.958038,116.357781,69783.000000 39.958029,116.357717,69788.000000 39.958022,116.357673,69793.000000 39.958030,116.357589,69798.000000 39.958045,116.357498,69803.000000 39.958013,116.357475,69808.000000 39.957964,116.357440,69813.000000 39.957815,116.357293,69818.000000 39.957764,116.357194,69823.000000 39.957790,116.357141,69828.000000 39.957813,116.357133,69833.000000 39.957819,116.357084,69838.000000 39.957864,116.357043,69843.000000 39.957915,116.356983,69848.000000 39.957952,116.356910,69853.000000 39.957953,116.356849,69858.000000 39.957938,116.356812,69863.000000 39.957940,116.356761,69868.000000 39.957789,116.356693,69873.000000 39.957788,116.356625,69878.000000 39.957790,116.356616,69883.000000 39.957748,116.356490,69888.000000 39.957768,116.356432,69893.000000 39.957780,116.356413,69898.000000 39.957786,116.356412,69903.000000 39.957790,116.356410,69908.000000 39.957792,116.356407,69913.000000 39.957792,116.356407,69916.000000 39.957787,116.356405,69918.000000 39.957767,116.356384,69923.000000 39.957731,116.356278,69928.000000 39.957735,116.356246,69933.000000 39.957743,116.356196,69938.000000 39.957738,116.356195,69943.000000 39.957737,116.356194,69948.000000 39.957738,116.356191,69953.000000 39.957735,116.356190,69958.000000 39.957735,116.356190,69961.000000 39.957739,116.356188,69963.000000 39.957748,116.356187,69968.000000 39.957748,116.356187,69971.000000 39.957756,116.356186,69973.000000 39.957765,116.356184,69978.000000 39.957778,116.356182,69983.000000 39.957797,116.356156,69988.000000 39.957816,116.356092,69993.000000 39.957825,116.356089,69998.000000 39.957830,116.356086,70003.000000 39.957830,116.356086,70006.000000 39.957824,116.356084,70008.000000 39.957812,116.356082,70013.000000 39.957807,116.356079,70018.000000 39.957807,116.356079,70021.000000 39.957804,116.356078,70023.000000 39.957781,116.356044,70028.000000 39.957750,116.355951,70033.000000 39.957714,116.355892,70038.000000 39.957718,116.355872,70043.000000 39.957718,116.355863,70048.000000 39.957718,116.355863,70051.000000 39.957711,116.355864,70053.000000 39.957672,116.355837,70058.000000 39.957649,116.355845,70063.000000 39.957614,116.355842,70068.000000 39.957590,116.355786,70073.000000 39.957576,116.355773,70078.000000 39.957561,116.355771,70083.000000 39.957556,116.355754,70088.000000 39.957557,116.355740,70093.000000 39.957570,116.355716,70098.000000 39.957571,116.355709,70103.000000 39.957570,116.355709,70108.000000 39.957570,116.355709,70111.000000 39.957563,116.355710,70113.000000 39.957572,116.355710,70118.000000 39.957668,116.355752,70123.000000 39.957735,116.355770,70128.000000 39.957789,116.355779,70133.000000 39.957807,116.355775,70138.000000 39.957817,116.355772,70143.000000 39.957839,116.355773,70148.000000 39.957853,116.355774,70153.000000 39.957847,116.355727,70158.000000 39.957847,116.355727,70161.000000 39.957851,116.355725,70163.000000 39.957885,116.355677,70168.000000 39.957918,116.355678,70173.000000 39.957935,116.355688,70178.000000 39.957935,116.355688,70181.000000 39.957938,116.355688,70183.000000 39.957947,116.355704,70188.000000 39.957947,116.355704,70191.000000 39.957942,116.355707,70193.000000 39.957923,116.355699,70198.000000 39.957908,116.355697,70203.000000 39.957908,116.355697,70206.000000 39.957908,116.355695,70208.000000 39.957895,116.355685,70213.000000 39.957895,116.355685,70218.000000 39.957887,116.355685,70223.000000 39.957882,116.355686,70228.000000 39.957882,116.355686,70231.000000 39.957878,116.355686,70233.000000 39.957867,116.355687,70238.000000 39.957861,116.355706,70243.000000 39.957865,116.355707,70248.000000 39.957864,116.355708,70253.000000 39.957866,116.355708,70258.000000 39.957870,116.355708,70263.000000 39.957874,116.355707,70268.000000 39.957874,116.355707,70271.000000 39.957881,116.355705,70273.000000 39.957890,116.355705,70278.000000 39.957936,116.355703,70283.000000 39.957941,116.355711,70288.000000 39.957941,116.355711,70291.000000 39.957942,116.355713,70293.000000 39.957986,116.355779,70298.000000 39.958013,116.355873,70303.000000 39.958082,116.356000,70308.000000 39.958119,116.356077,70313.000000 39.958103,116.356104,70318.000000 39.958059,116.356092,70323.000000 39.958040,116.356053,70328.000000 39.958049,116.356002,70333.000000 39.958051,116.355957,70338.000000 39.958008,116.355936,70343.000000 39.958009,116.355920,70348.000000 39.957974,116.355915,70353.000000 39.957933,116.355910,70358.000000 39.957895,116.355944,70363.000000 39.957867,116.355974,70368.000000 39.957842,116.356031,70373.000000 39.957843,116.356050,70378.000000 39.957844,116.356048,70383.000000 39.957846,116.356046,70388.000000 39.957851,116.356045,70393.000000 39.957849,116.356043,70398.000000 39.957849,116.356043,70403.000000 39.957847,116.356042,70408.000000 39.957847,116.356042,70411.000000 39.957847,116.356043,70413.000000 39.957856,116.356041,70418.000000 39.957859,116.356040,70423.000000 39.957859,116.356040,70426.000000 39.957855,116.356039,70428.000000 39.957791,116.356045,70433.000000 39.957766,116.356056,70438.000000 39.957766,116.356056,70441.000000 39.957758,116.356057,70443.000000 39.957749,116.356057,70448.000000 39.957746,116.356056,70453.000000 39.957754,116.356054,70458.000000 39.957759,116.356051,70463.000000 39.957759,116.356051,70466.000000 39.957764,116.356050,70468.000000 39.957807,116.356019,70473.000000 39.957833,116.355987,70478.000000 39.957843,116.355978,70483.000000 39.957852,116.355977,70488.000000 39.957852,116.355976,70493.000000 39.957848,116.355975,70498.000000 39.957848,116.355975,70501.000000 39.957846,116.355975,70503.000000 39.957847,116.355927,70508.000000 39.957828,116.355899,70513.000000 39.957806,116.355890,70518.000000 39.957758,116.355856,70523.000000 39.957716,116.355825,70528.000000 39.957693,116.355801,70533.000000 39.957693,116.355801,70536.000000 39.957701,116.355801,70538.000000 39.957710,116.355800,70543.000000 39.957832,116.355789,70548.000000 39.957917,116.355786,70553.000000 39.957965,116.355815,70558.000000 39.957997,116.355800,70563.000000 39.958048,116.355776,70568.000000 39.958089,116.355761,70573.000000 39.958105,116.355772,70578.000000 39.958103,116.355773,70583.000000 39.958102,116.355772,70588.000000 39.958102,116.355772,70591.000000 39.958098,116.355773,70593.000000 39.958087,116.355776,70598.000000 39.958095,116.355824,70603.000000 39.958092,116.355825,70608.000000 39.958093,116.355827,70613.000000 39.958090,116.355828,70618.000000 39.958090,116.355828,70621.000000 39.958088,116.355830,70623.000000 39.958027,116.355836,70628.000000 39.957923,116.355829,70633.000000 39.957858,116.355839,70638.000000 39.957799,116.355830,70643.000000 39.957741,116.355846,70648.000000 39.957692,116.355884,70653.000000 39.957958,116.355848,70883.000000 39.957979,116.355840,70888.000000 39.958025,116.355857,70893.000000 39.958078,116.355863,70898.000000 39.958105,116.355881,70903.000000 39.958088,116.355856,70908.000000 39.958092,116.355850,70913.000000 39.958092,116.355850,70916.000000 39.958085,116.355851,70918.000000 39.958069,116.355824,70923.000000 39.958072,116.355800,70928.000000 39.958072,116.355791,70933.000000 39.958082,116.355806,70938.000000 39.958088,116.355805,70943.000000 39.958088,116.355805,70946.000000 39.958087,116.355805,70948.000000 39.958098,116.355803,70953.000000 39.958107,116.355802,70958.000000 39.958107,116.355802,70961.000000 39.958106,116.355802,70963.000000 39.958092,116.355768,70968.000000 39.958084,116.355758,70973.000000 39.958080,116.355758,70978.000000 39.958080,116.355758,70981.000000 39.958076,116.355759,70983.000000 39.958067,116.355759,70988.000000 39.958062,116.355760,70993.000000 39.958063,116.355759,70998.000000 39.958063,116.355759,71001.000000 39.958059,116.355760,71003.000000 39.958049,116.355760,71008.000000 39.958047,116.355749,71013.000000 39.958001,116.355742,71018.000000 39.958000,116.355742,71023.000000 39.958000,116.355742,71026.000000 39.958006,116.355742,71028.000000 39.957997,116.355738,71033.000000 39.957980,116.355740,71038.000000 39.957985,116.355739,71043.000000 39.957988,116.355739,71048.000000 39.957991,116.355738,71053.000000 39.957991,116.355738,71056.000000 39.957987,116.355739,71058.000000 39.957976,116.355731,71063.000000 39.957955,116.355712,71068.000000 39.957955,116.355712,71071.000000 39.957953,116.355712,71073.000000 39.957963,116.355711,71078.000000 39.957980,116.355743,71083.000000 39.957990,116.355735,71088.000000 39.957991,116.355736,71093.000000 39.957983,116.355737,71098.000000 39.957983,116.355737,71101.000000 39.958018,116.356049,72188.000000 39.958017,116.356086,72193.000000 39.958017,116.356086,72196.000000 39.958017,116.356086,72198.000000 39.958026,116.356121,72203.000000 39.958046,116.356140,72208.000000 39.958051,116.356139,72213.000000 39.958054,116.356138,72218.000000 39.958053,116.356136,72223.000000 39.958055,116.356135,72228.000000 39.958055,116.356135,72231.000000 39.958060,116.356134,72233.000000 39.958037,116.356130,72238.000000 39.958027,116.356097,72243.000000 39.958029,116.356095,72248.000000 39.958026,116.356093,72253.000000 39.958026,116.356092,72258.000000 39.958023,116.356091,72263.000000 39.958019,116.356090,72268.000000 39.958019,116.356087,72273.000000 39.958027,116.356083,72278.000000 39.958032,116.356082,72283.000000 39.958031,116.356081,72288.000000 39.958031,116.356081,72291.000000 39.958026,116.356079,72293.000000 39.957949,116.356074,72298.000000 39.957961,116.356062,72303.000000 39.957961,116.356062,72306.000000 39.957958,116.356065,72308.000000 39.957957,116.356115,72313.000000 39.957965,116.356096,72318.000000 39.957981,116.356092,72323.000000");
//    std::cerr<<traja.getMinimumDistance(trajb);//<<" "<<trajb.getMinimumDistance(traja);
    return 0;
}
