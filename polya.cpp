#include "polya.hpp"

#include <limits>
#include <math.h>

std::vector<int> viterbi(std::string seq) {
    auto v = std::vector<std::vector<double>>(states.size());
    auto ptr = std::vector<std::vector<int>>(seq.size()+1);

    for (int i = 0; i < states.size(); ++i) {
        v[i] = std::vector<double>(seq.size()+1);
        v[i][0] = -std::numeric_limits<double>::infinity();
    }

    v[0][0] = 0;

    for (int i = 0; i < seq.size(); ++i) {
        ptr[i+1] = std::vector<int>(states.size());

        for (int j = 0; j < states.size(); ++j) {
            double max = -std::numeric_limits<double>::infinity();
            int max_k = -1;

            for (int k = 0; k < states.size(); ++k) {
                double p = v[k][i] + log(transitions[k][j]);
                if (p > max) {
                    max = p;
                    max_k = k;
                }
            }

            v[j][i+1] = log(emissions[j][alphabet.find(seq[i])->second]) + max;
            ptr[i+1][j] = max_k;
        }
    }

    int max_k = -1;
    double max = -std::numeric_limits<double>::infinity();
    
    for (int k = 0; k < states.size(); ++k) {
        if (v[k][seq.size()] > max) {
            max = v[k][seq.size()];
            max_k = k;
        }
    }

    auto pi = std::vector<int>(seq.size()+1);
    pi[seq.size()] = max_k;
    for (int i = seq.size(); i >= 1; --i) {
        pi[i-1] = ptr[i][pi[i]];
    }

    return pi;
}

polya_res_t get_polya(std::string seq) {
    auto hmm_res = viterbi(seq);
    int pos = -1;
    int pas_pos = -1;
    int length = -1;

    for (int i = 0; i < hmm_res.size(); ++i) {
        if (states[hmm_res[i]] == "polyA") {
            if (pos == -1) {
                pos = i;
                length = 1;
            } else {
                length++;
            }
        } else if (states[hmm_res[i]] == "pAs1") {
            pas_pos = i;
        }
    }

    return polya_res_t{pas_pos, pos, length};
}