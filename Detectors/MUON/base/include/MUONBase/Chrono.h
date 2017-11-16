//
// Created by Gabriele Gaetano Fronzé on 16/11/2017.
//

#ifndef CHRONO_H
#define CHRONO_H


#include <cstdint>
#include <ratio>
#include <chrono>
#include <vector>
#include <numeric>
#include <string>
#include <sstream>

class Chrono {
public:
    void AddCall(double deltaT){ fDeltaTs.emplace_back(deltaT); };

    double AvgCallTime(){
        return std::accumulate(fDeltaTs.begin(),fDeltaTs.end(),0.)/fDeltaTs.size();
    };

    std::string PrintStatus(){
        std::ostringstream outputString;
        outputString << "NCalls=" << std::to_string(fDeltaTs.size()) << " AvgTime=";
        outputString << AvgCallTime();
        return outputString.str();
    };

private:
    std::vector<double> fDeltaTs;
};

class DeltaT {
public:
    DeltaT(Chrono *chrono) : fStart(std::chrono::high_resolution_clock::now()),fChrono(chrono){};
    ~DeltaT(){ fChrono->AddCall(std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - fStart).count()); };

private:
    std::chrono::high_resolution_clock::time_point fStart;
    Chrono *fChrono;
};


#endif //CHRONO_H
