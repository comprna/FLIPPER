#include "nw.hpp"

#include <vector>
#include <algorithm>

int nw_score(std::string seq1, std::string seq2) {
    std::vector<std::vector<int>> F(seq1.size()+1, std::vector<int>(seq2.size()+1));

    for (int i = 1; i <= seq1.size(); ++i) {
        for (int j = 1; j <= seq2.size(); ++j) {
            int eq = -1;
            if (seq1[i-1] == seq2[j-1]) eq = 1;
            
            int mat = F[i-1][j-1] + eq;
            int del = F[i-1][j] - 1;
            int ins = F[i][j-1] - 1;

            F[i][j] = std::max(std::max(mat, del), ins);
        }
    }

    return F[seq1.size()][seq2.size()];
}