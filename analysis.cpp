#include "include/utils.hpp"
#include "include/cfg.hpp"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>

// Liveness analysis

void liveness(CFG& cfg) {
    bool changed;
    do {
        changed = false;
        for (auto& [id, block] : cfg) {
            std::set<std::string> old_in = block.in_live;
            std::set<std::string> old_out = block.out_live;

            // OUT = união dos IN dos sucessores
            block.out_live.clear();
            for (int succ : block.successors) {
                block.out_live.insert(cfg[succ].in_live.begin(), cfg[succ].in_live.end());
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

// Reaching definitions analysis

void fillGenKillReach(CFG& cfg) {
    std::map<std::string, std::set<std::string>> var_definitions;
    unsigned int count = 0;

    for (auto& [block_id, block] : cfg) {
        std::map<std::string, std::string> gen;
        for (size_t i = 0; i < block.instructions.size(); ++i) {
            const auto& instr = block.instructions[i];
            if (!instr.var_def.empty()) {
                std::string def_id = instr.var_def + "[d" + std::to_string(++count) + ']';
                gen[instr.var_def] = def_id;
                var_definitions[instr.var_def].insert(def_id);
            }
        }
        for (const auto& [_, def] : gen) {
            block.gen_reach.insert(def);
        }
    }

    count = 0;
    for (auto& [block_id, block] : cfg) {
        std::set<std::string> kill;

        for (size_t i = 0; i < block.instructions.size(); ++i) {
            const auto& instr = block.instructions[i];
            if (!instr.var_def.empty()) {
                std::string def_id = instr.var_def + "[d" + std::to_string(++count) + ']';
                for (const auto& other_def_id : var_definitions[instr.var_def]) {
                    if (other_def_id != def_id && block.gen_reach.find(other_def_id) == block.gen_reach.end()) {
                        kill.insert(other_def_id);
                    }
                }
            }
        }

        block.kill_reach = kill;
    }
}

void reachingDefinitions(CFG& cfg) {
    for (auto& [id, block] : cfg) {
        block.in_reach.clear();
        block.out_reach.clear();
    }

    bool changed;
    do {
        changed = false;
        for (auto& [id, block] : cfg) {
            std::set<std::string> in_new;
            if (block.predecessors.empty()) {
                in_new.clear();
            } else {
                for (int pred : block.predecessors) {
                    in_new.insert(cfg[pred].out_reach.begin(), cfg[pred].out_reach.end());
                }
            }

            // OUT[B] = GEN[B] ∪ (IN[B] - KILL[B])
            std::set<std::string> out_new = block.gen_reach;

            for (const auto& def : in_new) {
                if (block.kill_reach.find(def) == block.kill_reach.end()) {
                    out_new.insert(def);
                }
            }

            if (block.in_reach != in_new || block.out_reach != out_new) {
                changed = true;
                block.in_reach = in_new;
                block.out_reach = out_new;
            }
        }
    } while (changed);
}

// Available expressions analysis

void fillGenKillAvail(CFG& cfg) {
    std::set<std::string> all_exprs{};
    for (const auto& [_, block] : cfg) {
        for (const auto& instr : block.instructions) {
            if (instr.var_def != "") {
                auto rhs = trim(instr.text.substr(instr.text.find('=') + 1));
                all_exprs.insert(rhs);
            }
        }
    }
    
    for (auto& [id, block] : cfg) {
        for (const auto& instr : block.instructions) {
            if (instr.var_def != "") {
                auto rhs = trim(instr.text.substr(instr.text.find('=') + 1));
                block.gen_avail.insert(rhs);
                for (auto v = block.gen_avail.begin(); v != block.gen_avail.end(); ) {
                    if (v->find(instr.var_def) != std::string::npos) {
                        v = block.gen_avail.erase(v);
                    } else {
                        ++v;
                    }
                }
            }
        }

        for (const auto& expr : all_exprs) {
            for (const auto& def : block.def) {
                if (expr.find(def) != std::string::npos && block.gen_avail.find(expr) == block.gen_avail.end()) {
                    block.kill_avail.insert(expr);
                }
            }
        }
    }
}

void available(CFG& cfg) {    
    for (auto& [id, block] : cfg) {
        block.in_avail = std::set<std::string>{};
        block.out_avail = std::set<std::string>{};
    }

    bool changed;
    do {
        changed = false;
        for (auto& [id, block] : cfg) {
            auto in = block.predecessors.size() > 0 ? cfg[*block.predecessors.begin()].out_avail : std::set<std::string>{};
            for (const auto pred : block.predecessors) {
                for (auto v = in.begin(); v != in.end(); ) {
                    if (cfg[pred].out_avail.find(*v) == cfg[pred].out_avail.end()) {
                        v = in.erase(v);
                    } else {
                        ++v;
                    }
                }
            }
            
            auto out = block.gen_avail;
            for (const auto& expr : in) {
                if (block.kill_avail.find(expr) == block.kill_avail.end()) {
                    out.insert(expr);
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

// Analyses' printing & debugging

void printInOut(const CFG& cfg) {
    for (const auto& [id, block] : cfg) {
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

void printReachingDefinitions(const CFG& cfg) {
    for (const auto& [block_id, block] : cfg) {
        std::cout << "Block " << block_id << ":\n";

        std::cout << "  In Reach: ";
        for (const auto& def : block.in_reach) {
            std::cout << def << " ";
        }

        std::cout << "\n  Out Reach: ";
        for (const auto& def : block.out_reach) {
            std::cout << def << " ";
        }
        std::cout << "\n\n";
    }
}

void printAvailable(const CFG& cfg) {
    for (const auto& [id, block] : cfg) {
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

