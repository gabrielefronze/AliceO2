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

class DeltaT {
public:
    DeltaT() : fTime(std::chrono::high_resolution_clock::now()){};
    ~DeltaT(){ fTime -= std::chrono::high_resolution_clock::now(); };

private:
    std::chrono::time_point fTime;
};

class Chrono {
public:
    void AddCall(DeltaT deltaT){ fDeltaT.emplace_back(deltaT); };
    std::chrono::duration<double, std::milli> AvgCallTime(){
        std::chrono::duration<double, std::milli> cumulus{};\
        return std::accumulate(fDeltaT.begin(),fDeltaT.end(),cumulus)/fDeltaT.size();
    };
    std::string PrintStatus(){
        std::ostringstream outputString;
        outputString << "NCalls=" << std::to_string(fDeltaT.size()) << " AvgTime=";
        outputString << AvgCallTime().count();
        return outputString.str();
    };

private:
    std::vector<DeltaT> fDeltaT;
};


#endif //CHRONO_H
