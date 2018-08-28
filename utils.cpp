#include "utils.hpp"
#include <iostream>

std::string reverse_complement(std::string seq) {      
    std::string res = seq;
    int len = res.length();

    for (int i = 0; i < len; ++i) {
        res[i] = base_complements.find(seq[len-1-i])->second;
    }

    return res;
}