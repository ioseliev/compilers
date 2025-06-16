#ifndef CFG_H
#define CFG_H

#include <string>
#include <vector>
#include <set>
#include <map>

struct Instruction {
    std::string text; 
    std::string var_def;
    std::set<std::string> var_use;

    Instruction(const std::string& t);
};

struct BasicBlock {
    int id;
    std::vector<Instruction> instructions;
    std::set<int> successors;
    std::set<int> predecessors; 

    // Análise de Longevidade
    std::set<std::string> use;
    std::set<std::string> def;
    std::set<std::string> in_live;
    std::set<std::string> out_live;

    // definições alcançantes
    std::set<std::string> gen_reach;
    std::set<std::string> kill_reach;
    std::set<std::string> in_reach;
    std::set<std::string> out_reach;

    // Expressões disponíveis
    std::set<std::string> gen_avail;
    std::set<std::string> kill_avail;
    std::set<std::string> in_avail;
    std::set<std::string> out_avail;

    BasicBlock();
    BasicBlock(int i);
};

using CFG = std::map<int, BasicBlock>;

CFG read(const std::string& filename);

void fillUseDef(CFG& cfg);

void printCFG(const CFG& cfg);


#endif