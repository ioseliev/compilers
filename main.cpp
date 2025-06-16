#include "include/cfg.hpp"
#include "include/analysis.hpp"
#include <iostream>

int main(int argc, char **argv){
    if (argc < 2) {
        std::cerr << "Usage: `" << argv[0] << " filename`." << std::endl;
        return 1;
    }

    CFG cfg = read(argv[1]);

    if (cfg.empty()) {
        std::cerr << "Empty or non-existing CFG file." << std::endl;
        return 1;
    }

    fillUseDef(cfg);
    fillGenKillReach(cfg);
    fillGenKillAvail(cfg);
    
    liveness(cfg);
    reachingDefinitions(cfg);
    available(cfg);

    std::cout << "CONTROL FLOW GRAPH SUMMARY:\n\n";
    printCFG(cfg);

    std::cout << "ANALYSES SUMMARY:\n\n";

    std::cout << "Liveness:\n";
    printInOut(cfg);

    std::cout << "Reaching definitions:\n";
    printReachingDefinitions(cfg);

    std::cout << "Available expressions:\n";
    printAvailable(cfg);
    
    return 0;
}