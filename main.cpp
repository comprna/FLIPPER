#include "fasta.hpp"
#include "fl.hpp"
#include "argagg.hpp"

#include <iostream>

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
        read_set_t fl_reads = get_fl_reads(reads, args["a5"], args["a3"], args["fastq"], args["threads"]);        

        for (auto read : fl_reads) {
            std::cout << read.header << std::endl;
            std::cout << read.seq << std::endl;
        }

        return EXIT_SUCCESS;
    }
}