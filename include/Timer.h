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

using namespace std;

class Timer {
public:
    map<string, struct timeval> start_time;
    map<string, struct timeval> stop_time;
    map<string, double> time;

    void Start (string name) {
        if (!time.count(name)) {
            time[name] = 0.0;
        }
        gettimeofday(&start_time[name], NULL);
    }

    void Stop (string name) {
        gettimeofday(&stop_time[name], NULL);
        time[name] += (stop_time[name].tv_sec - start_time[name].tv_sec) * 1000000.0 +
                      (stop_time[name].tv_usec - start_time[name].tv_usec);
    }

    void Print (string name) {
        cout << "The time spent for " << name << " is " << time[name] / CLOCKS_PER_SEC << " seconds" << endl;
    }

    double GetTime (string name) {
        return time[name];
    }

    void SetTime (string name, double time) {
        this->time[name] = time;
    }
};


#endif //BAYESIANNETWORK_TIMER_H
