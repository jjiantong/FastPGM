//
// Created by jjt on 2021/7/19.
//

#ifndef BAYESIANNETWORK_TIMER_H
#define BAYESIANNETWORK_TIMER_H

#include <iostream>
#include <map>
#include <string>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

using namespace std;

class Timer {
public:

//    double start;
//    double end;
//    start = omp_get_wtime();
//    ... work to be timed ...
//    end = omp_get_wtime();
//    printf("Work took %f seconds\n", end - start);


//    map<string, struct timeval> start_time;
//    map<string, struct timeval> stop_time;
    map<string, double> start_time;
    map<string, double> stop_time;
    map<string, double> time;

    void Start (string name) {
        if (!time.count(name)) {
            time[name] = 0.0;
        }
//        gettimeofday(&start_time[name], NULL);
        start_time[name] = omp_get_wtime();
    }

    void Stop (string name) {
//        gettimeofday(&stop_time[name], NULL);
        stop_time[name] = omp_get_wtime();
//        time[name] += (stop_time[name].tv_sec - start_time[name].tv_sec) * 1000000.0 +
//                      (stop_time[name].tv_usec - start_time[name].tv_usec);
        time[name] += stop_time[name] - start_time[name];
    }

    void Print (string name) {
//        cout << name << ": " << time[name] / CLOCKS_PER_SEC << " s";
        cout << name << ": " << time[name] << " s " << endl;
    }

    double GetTime (string name) {
        return time[name];
    }

    void SetTime (string name, double time) {
        this->time[name] = time;
    }
};


#endif //BAYESIANNETWORK_TIMER_H
