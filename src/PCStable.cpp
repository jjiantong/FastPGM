//
// Created by jjt on 2021/6/23.
//

#include "PCStable.h"

void PCStable::StructLearnCompData(Dataset *dts, bool print_struct, string topo_ord_constraint, int max_num_parents) {
    // record time
    struct timeval start, end;
    double diff;
    gettimeofday(&start,NULL);

    cout << "==================================================" << '\n'
         << "Begin structural learning with PC-stable" << endl;

    depth = (depth == -1) ? 1000 : depth; // depth = -1 means no limitation
    AssignNodeInformation(dts);

    // print time
    gettimeofday(&end,NULL);
    diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
    setlocale(LC_NUMERIC, "");
    cout << "==================================================" << '\n'
         << "The time spent to generate CPDAG with PC-stable is " << diff << " seconds" << endl;
}

//void PCStable::StructLearnByPCStable(Dataset *dts, bool print_struct) {
//
//}