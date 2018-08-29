#include "fl.hpp"
#include "adapters.hpp"
#include "utils.hpp"
#include "polya.hpp"

#include <algorithm>
#include <future>
#include <mutex>

read_set_t get_fl_reads(const read_set_t &reads, std::string a5, std::string a3, bool is_fq, int n_threads) {
    read_set_t fl_reads;
    
    std::mutex mu;
    std::vector<std::future<void>> tasks;

    for (int t = 0; t < n_threads; ++t) {
        tasks.emplace_back(std::async(std::launch::async, [t, &mu, &reads, n_threads, &fl_reads, a5, a3, is_fq] {
            for (int i = t; i < reads.size(); i += n_threads) {
                auto read = reads[i];

                // TODO: analyze small reads as well
                if (read.seq.size() < 151) {
                    continue;
                }

                std::string seq = "";
                std::string qt = "";

                int pos5 = adapter_pos_start(read.seq, a5);
                int pos3 = adapter_pos_end(read.seq, a3);

                if (pos5 != -1 && pos3 != -1) {
                    seq = read.seq.substr(pos5, read.seq.size()-150+pos3-pos5);

                    if (is_fq) {
                        qt = read.quality.substr(pos5, read.quality.size()-150+pos3-pos5);
                    }
                } else {
                    pos5 = adapter_pos_start(read.seq, reverse_complement(a5));
                    pos3 = adapter_pos_end(read.seq, reverse_complement(a3));
        
                    if (pos5 != -1 && pos3 != -1) {
                        seq = read.seq.substr(pos5, read.seq.size()-150+pos3-pos5);
        
                        if (is_fq) {
                            qt = read.quality.substr(pos5, read.quality.size()-150+pos3-pos5);
                        }
                    }
                }

                if (seq != "") {                        
                    auto polya_info = get_polya(seq);
                    if (polya_info.tail_pos > -1) {
                        seq = seq.substr(0, polya_info.tail_pos);
                        
                        read_t fl_read;
                        fl_read.header = read.header;
                        fl_read.seq = seq;
                        
                        if (is_fq) {
                            qt = qt.substr(0, polya_info.tail_pos);
                            
                            fl_read.quality = qt;
                            fl_read.ann = read.ann;
                        }

                        std::lock_guard<std::mutex> lock(mu);
                        fl_reads.push_back(fl_read);
                    } else {
                        seq = reverse_complement(seq);
                        if (is_fq) {
                            std::reverse(qt.begin(), qt.end());
                        }

                        polya_info = get_polya(seq);
                        if (polya_info.tail_pos > -1) {
                            seq = seq.substr(0, polya_info.tail_pos);
                            
                            read_t fl_read;
                            fl_read.header = read.header;
                            fl_read.seq = seq;
                            
                            if (is_fq) {
                                qt = qt.substr(0, polya_info.tail_pos);
                                
                                fl_read.quality = qt;
                                fl_read.ann = read.ann;
                            }
        
                            std::lock_guard<std::mutex> lock(mu);
                            fl_reads.push_back(fl_read);
                        } 
                    }
                }
            }
        }));
    }

    for (auto &&task : tasks) {
        task.get();
    }

    return fl_reads;
}