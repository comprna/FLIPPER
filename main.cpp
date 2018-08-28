#include "polya.hpp"
#include "fasta.hpp"
#include "utils.hpp"
#include "adapters.hpp"
#include "argagg.hpp"

#include <iostream>
#include <algorithm>
#include <future>
#include <mutex>

int main(int argc, char *argv[]) {
    argagg::parser argparser {{
        { "help", {"-h", "--help"},
          "shows this help message", 0},
        { "input", {"-i", "--input"},
          "input fasta/fastq file (required)", 1},
        { "threads", {"-t", "--threads"},
          "number of threads to use", 1},
        { "fastq", {"--fastq"},
          "whether input and output should be in fastq format (instead of fasta)", 0},
        { "fl", {"--fl"},
          "output only full-length reads (default: outputs all possibly oriented reads)", 0},
        { "a5", {"--a5"},
          "expected adapter at 5'", 1},
        { "a3", {"--a3"},
          "expected adapter at 3'", 1},
    }};

    argagg::parser_results args;
    try {
        args = argparser.parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (args["help"]) {
        std::cerr << argparser;
        return EXIT_SUCCESS;
    }

    if (!args["input"]) {
        std::cerr << "ERROR: No input file provided" << std::endl;
        std::cerr << argparser;
        return EXIT_FAILURE;
    }

    if (args["fl"]) {
        if (!args["a3"] || !args["a5"]) {
            std::cerr << "ERROR: In full-length mode, both ends adapters must be provided" << std::endl;
            std::cerr << argparser;
            return EXIT_FAILURE;
        }
    } else {
        if (!args["a3"]) {
            std::cerr << "ERROR: In non-full-length mode, 3' end adapter must be provided" << std::endl;
            std::cerr << argparser;
            return EXIT_FAILURE;
        }
    }

    // TODO: handle non-existing file
    read_set_t reads;
    if (args["fastq"]) {
        reads = read_fastq_file(args["input"]);
    } else {
        reads = read_fasta_file(args["input"]);
    }
    
    if (args["fl"]) {
        read_set_t fl_reads;

        std::mutex mu;
        std::vector<std::future<void>> tasks;
        int n_threads = args["threads"];

        for (int t = 0; t < n_threads; ++t) {
            tasks.emplace_back(std::async(std::launch::async, [t, &mu, &reads, &args, n_threads, &fl_reads] {
                for (int i = t; i < reads.size(); i += n_threads) {
                    auto read = reads[i];

                    // TODO: analyze small reads as well
                    if (read.seq.size() < 151) {
                        continue;
                    }

                    std::string seq = "";
                    std::string qt = "";

                    int pos5 = adapter_pos_start(read.seq, args["a5"]);
                    int pos3 = adapter_pos_end(read.seq, args["a3"]);

                    if (pos5 != -1 && pos3 != -1) {
                        seq = read.seq.substr(pos5, read.seq.size()-150+pos3-pos5);

                        if (args["fastq"]) {
                            qt = read.quality.substr(pos5, read.quality.size()-150+pos3-pos5);
                        }
                    } else {
                        pos5 = adapter_pos_start(read.seq, reverse_complement(args["a5"]));
                        pos3 = adapter_pos_end(read.seq, reverse_complement(args["a3"]));
            
                        if (pos5 != -1 && pos3 != -1) {
                            seq = read.seq.substr(pos5, read.seq.size()-150+pos3-pos5);
            
                            if (args["fastq"]) {
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
                            
                            if (args["fastq"]) {
                                qt = qt.substr(0, polya_info.tail_pos);
                                
                                fl_read.quality = qt;
                                fl_read.ann = read.ann;
                            }

                            std::lock_guard<std::mutex> lock(mu);
                            fl_reads.push_back(fl_read);
                        } else {
                            seq = reverse_complement(seq);
                            if (args["fastq"]) {
                                std::reverse(qt.begin(), qt.end());
                            }

                            polya_info = get_polya(seq);
                            if (polya_info.tail_pos > -1) {
                                seq = seq.substr(0, polya_info.tail_pos);
                                
                                read_t fl_read;
                                fl_read.header = read.header;
                                fl_read.seq = seq;
                                
                                if (args["fastq"]) {
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

        for (auto read : fl_reads) {
            std::cout << read.header << std::endl;
            std::cout << read.seq << std::endl;
        }

        return EXIT_SUCCESS;
    }
}