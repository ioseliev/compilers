#include "cfg.h"
#include "utils.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream> 
#include <algorithm> 
#include <iostream>
#include <fstream>

Instruction::Instruction(const std::string& t) : text(t) {
    auto eq_pos = t.find('=');
        if (eq_pos != std::string::npos && !isComparator(t)) {
            
            var_def = trim(t.substr(0, eq_pos));

            std::string rhs = t.substr(eq_pos + 1);
            std::istringstream iss(rhs);
            std::string token;
            while (iss >> token) {
                if (isVariable(token)) {
                    var_use.insert(token);
                }
            }
        }else{
            var_def = "";
            std::istringstream iss(t);
            std::string token;
            while (iss >> token) {
                if (isVariable(token)) {
                    var_use.insert(token);
                }
            }
        }
};

BasicBlock::BasicBlock() : id(-1) {}
BasicBlock::BasicBlock(int i) : id(i) {}

void read(const std::string& filename, std::map<int, BasicBlock>& CFG) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    while (true) {
        int block_id, num_instr;
        file >> block_id >> num_instr;
        if (!file) break;
        file.ignore();

        BasicBlock block(block_id);

        for (int i = 0; i < num_instr; i++) {
            std::string line;
            std::getline(file, line);
            if (!line.empty()) {
                block.instructions.emplace_back(line);
            }
        }

        std::string succ_line;
        std::getline(file, succ_line);
        std::istringstream iss(succ_line);
        int succ;
        while (iss >> succ) {
            if (succ != 0) {
                block.successors.insert(succ);
            }
        }

        CFG[block_id] = block;
    }

    for (auto& [id, block] : CFG) {
        for (const auto succ_id : block.successors) {
            CFG[succ_id].predecessors.insert(id);
        }
    }
};

void fillUseDef(std::map<int, BasicBlock>& CFG) {
    for (auto& [id, block] : CFG) {
        std::set<std::string> defined;
        for (const auto& instr : block.instructions) {
            
            for (const auto& v : instr.var_use) {
                if (defined.find(v) == defined.end())
                    block.use.insert(v);
            }
            if (!instr.var_def.empty()) {
                block.def.insert(instr.var_def);
                defined.insert(instr.var_def);
            }
        }
    }
};

void printCFG(std::map<int, BasicBlock> CFG){
    for (const auto& [id, block] : CFG) {
        std::cout << "Block " << id << ":\n";
        std::cout << "  Instructions:\n";
        for (const auto& instr : block.instructions) {
            std::cout << "    " << instr.text << "\n";
        }
        std::cout << "  Predecessors: ";
        for (const auto pred : block.predecessors) {
            std::cout << pred << ' ';
        }
        std::cout << "\n  Successors: ";
        for (const auto succ : block.successors) {
            std::cout << succ << ' ';
        }
        std::cout << "\n  Use: ";
        for (const auto& v : block.use) {
            std::cout << v << " ";
        }
        std::cout << "\n  Def: ";
        for (const auto& v : block.def) {
            std::cout << v << " ";
        }
        std::cout << "\n  Gen Reach: ";
        for (const auto& def : block.gen_reach) {
            std::cout << def << " ";
        }
        std::cout << "\n  Kill Reach: ";
        for (const auto& def : block.kill_reach) {
            std::cout << def << " ";
        }
        std::cout << "\n  Gen Avail.: ";
        for (const auto& v : block.gen_avail) {
            std::cout << '[' << v << "] ";
        }
        std::cout << "\n  Kill Avail.: ";
        for (const auto& v : block.kill_avail) {
            std::cout << '[' << v << "] ";
        }
        std::cout << "\n\n";
    }
}
