#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "./cfg.hpp"

 void liveness(CFG& cfg);

 void fillGenKillReach(CFG& cfg);

 void reachingDefinitions(CFG& cfg);

 void fillGenKillAvail(CFG& cfg);

 void available(CFG& cfg);

 void printInOut(const CFG& cfg);

 void printReachingDefinitions(const CFG& cfg);

 void printAvailable(const CFG& cfg);

#endif