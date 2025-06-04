#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream> 
#include <algorithm> 
#include <iostream>
#include <fstream>



std::string trim(const std::string& s) {
    const auto start = s.find_first_not_of(" \t");
    const auto end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

    
bool isVariable(const std::string& token) {
    
    return !(token == "+" || token == "-" || token == "*" || token == "/" || token == "if" || token == "goto" ||  token == "<" || token == ">" || token == ">=" || token == "<=" || token == "==" ||
                std::all_of(token.begin(), token.end(), ::isdigit));
}

bool isComparator(const std::string& t) {
    auto eq_pos = t.find('=');
    if (eq_pos != std::string::npos) {
        if ((eq_pos > 0 && (t[eq_pos - 1] == '<' || t[eq_pos - 1] == '>')) ||
            (eq_pos + 1 < t.size() && t[eq_pos + 1] == '=')) {
            return true;
        }
    }
    return false;
}

struct Instruction {
    std::string text; 
    std::string var_def;
    std::set<std::string> var_use;

    Instruction(const std::string& t) : text(t) {
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
    }

    

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
    std::set<std::string> gen;
    std::set<std::string> kill;
    std::set<std::string> in_reach;
    std::set<std::string> out_reach;

    // Expressões disponíveis
    std::set<std::string> in_avail;
    std::set<std::string> out_avail;

    BasicBlock() : id(-1) {}
    BasicBlock(int i) : id(i) {}
};




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
}


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
}

void liveness(std::map<int, BasicBlock>& CFG) {
    bool changed;
    do {
        changed = false;
        for (auto& [id, block] : CFG) {
            std::set<std::string> old_in = block.in_live;
            std::set<std::string> old_out = block.out_live;

            // OUT = união dos IN dos sucessores
            block.out_live.clear();
            for (int succ : block.successors) {
                block.out_live.insert(CFG[succ].in_live.begin(), CFG[succ].in_live.end());
            }

            // IN = USE união (OUT - DEF)
            block.in_live = block.use;
            for (const auto& v : block.out_live) {
                if (block.def.find(v) == block.def.end())
                    block.in_live.insert(v);
            }

            if (block.in_live != old_in || block.out_live != old_out)
                changed = true;
        }
    } while (changed);
}

void fillGenKill(std::map<int, BasicBlock>& CFG) {
    for (auto& [id, block] : CFG) {
        for (const auto& instr : block.instructions) {
            if (instr.var_def != "") {
                auto rhs = trim(instr.text.substr(instr.text.find('=') + 1));
                block.kill.insert(instr.var_def);
                if (rhs.find(instr.var_def) == std::string::npos) {
                    block.gen.insert(rhs);
                }
            }
        }
    }
}

void available(std::map<int, BasicBlock>& CFG) {
    for (auto& [id, block] : CFG) {
        block.in_avail = id != 1 ? block.gen : std::set<std::string>{};
        block.out_avail = block.gen;
    }

    bool changed;
    do {
        changed = false;
        for (auto& [id, block] : CFG) {
            auto in = id != 1 ? CFG[*block.predecessors.begin()].out_avail : std::set<std::string>{};
            for (auto v = in.begin(); v != in.end(); ) {
                for (const auto pred : block.predecessors) {
                    if (CFG[pred].out_avail.find(*v) == CFG[pred].out_avail.end()) {
                        v = in.erase(v);
                    } else {
                        ++v;
                    }
                }
            }
            
            auto out = block.gen;
            for (const auto& v : in) {
                if (block.kill.find(v) == block.kill.end()) {
                    out.insert(v);
                }
            }

            if (in != block.in_avail || out != block.out_avail) {
                changed = true;
                block.in_avail = in;
                block.out_avail = out;
            }
        }
    } while (changed);
}

void printCFG(std::map<int, BasicBlock> CFG){
    for (const auto& [id, block] : CFG) {
        std::cout << "Block " << id << ":\n";
        std::cout << "  Instructions:\n";
        for (const auto& instr : block.instructions) {
            std::cout << "    " << instr.text << "\n";
        }
        std::cout << "  Use: ";
        for (const auto& v : block.use) {
            std::cout << v << " ";
        }
        std::cout << "\n  Def: ";
        for (const auto& v : block.def) {
            std::cout << v << " ";
        }
        std::cout << "\n  Gen: ";
        for (const auto& v : block.gen) {
            std::cout << '[' << v << "] ";
        }
        std::cout << "\n  Kill: ";
        for (const auto& v : block.kill) {
            std::cout << v << " ";
        }
        std::cout << "\n\n";
    }
}


void printInOut(std::map<int, BasicBlock>& CFG) {
    for (const auto& [id, block] : CFG) {
        std::cout << "Block " << id << ":\n";
        std::cout << "  In Live: ";
        for (const auto& v : block.in_live) {
            std::cout << v << " ";
        }
        std::cout << "\n  Out Live: ";
        for (const auto& v : block.out_live) {
            std::cout << v << " ";
        }
        std::cout << "\n\n";
    }
}

void printAvailable(std::map<int, BasicBlock>& CFG) {
    for (const auto& [id, block] : CFG) {
        std::cout << "Block " << id << ":\n";
        std::cout << "  In Available: ";
        for (const auto& v : block.in_avail) {
            std::cout << '[' << v << "] ";
        }
        std::cout << "\n  Out Available: ";
        for (const auto& v : block.out_avail) {
            std::cout << '[' << v << "] ";
        }
        std::cout << "\n\n";
    }
}

int main(){

    std::map<int, BasicBlock> CFG;

    read("exemplos/codigo.txt", CFG);

    fillUseDef(CFG);
    fillGenKill(CFG);
    available(CFG);
    printCFG(CFG); 
    printAvailable(CFG);

    

    

    return 0;
}