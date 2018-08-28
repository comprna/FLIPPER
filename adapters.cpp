#include "adapters.hpp"
#include "nw.hpp"

int adapter_pos_start(std::string seq, std::string adapter) {
    std::string seq_start = seq.substr(0, 150);
    int max_score = adapter.size() * 1.15;
    int best_score_adapter = 0;
    int best_window_pos = -1;

    for (int i = 0; i < seq_start.size()-max_score; ++i) {
        int score_adapter = nw_score(seq_start.substr(i, max_score), adapter);
        if (score_adapter > best_score_adapter) {
            best_score_adapter = score_adapter;
            best_window_pos = i;
        }
    }

    if (best_score_adapter >= 9) {
        return best_window_pos + adapter.size();
    }

    return -1;
}

int adapter_pos_end(std::string seq, std::string adapter) {
    std::string seq_end = seq.substr(seq.size()-150, 150);

    int max_score = adapter.size() * 1.15;
    int best_score_adapter = 0;
    int best_window_pos = -1;

    for (int i = 0; i < seq_end.size()-max_score; ++i) {
        int score_adapter = nw_score(seq_end.substr(i, max_score), adapter);
        if (score_adapter > best_score_adapter) {
            best_score_adapter = score_adapter;
            best_window_pos = i;
        }
    }

    if (best_score_adapter >= 9) {
        return best_window_pos;
    }

    return -1;
}