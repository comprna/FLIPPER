#ifndef _flipper_utils_h
#define _flipper_utils_h

#include <string>
#include <unordered_map>

const std::unordered_map<char, char> base_complements = {
    {'A', 'T'},
    {'C', 'G'},
    {'T', 'A'},
    {'G', 'C'}
};

std::string reverse_complement(std::string seq);

#endif