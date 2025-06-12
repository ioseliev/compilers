#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <sstream> 
#include <algorithm> 
#include <iostream>
#include <fstream>

std::string trim(const std::string& s);

bool isVariable(const std::string& token);

bool isComparator(const std::string& t);

#endif