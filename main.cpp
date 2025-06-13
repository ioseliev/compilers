#include "cfg.h"
#include "utils.h"
#include "analysis.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream> 
#include <algorithm> 
#include <iostream>
#include <fstream>

int main(int argc, char **argv){
    if (argc < 2) {
        std::cerr << "Usage: `" << argv[0] << " filename`." << std::endl;
        return 1;
    }

    std::map<int, BasicBlock> CFG;
    read(argv[1], CFG);

    fillUseDef(CFG);
    fillGenKillReach(CFG);
    fillGenKillAvail(CFG);
    
    liveness(CFG);
    reachingDefinitions(CFG);
    available(CFG);

    std::cerr << "CONTROL FLOW GRAPH SUMMARY:\n\n";
    printCFG(CFG);

    std::cout << "ANALYSES SUMMARY:\n\n";

    std::cout << "Liveness:\n";
    printInOut(CFG);

    std::cout << "Reaching definitions:\n";
    printReachingDefinitions(CFG);

    std::cout << "Available expressions:\n";
    printAvailable(CFG);
    
    return 0;
}