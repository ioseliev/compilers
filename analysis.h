#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "cfg.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream> 
#include <algorithm> 
#include <iostream>
#include <fstream>

 void liveness(std::map<int, BasicBlock>& CFG);

 void fillGenKillReach(std::map<int, BasicBlock>& CFG);

 void reachingDefinitions(std::map<int, BasicBlock>& CFG);

 void fillGenKillAvail(std::map<int, BasicBlock>& CFG);

 void available(std::map<int, BasicBlock>& CFG);

 void printInOut(std::map<int, BasicBlock>& CFG);

 void printReachingDefinitions(const std::map<int, BasicBlock>& CFG);

 void printAvailable(std::map<int, BasicBlock>& CFG);

#endif