#include "fasta.hpp"
#include <fstream>
#include <algorithm>

read_set_t read_fasta_file(std::string file) {
    read_set_t result;

    std::ifstream infile(file);
    std::string line;
    std::string header;

    while (std::getline(infile, line)) {
        if (line[0] == '>') {
            header = line;
        } else {
            read_t r{header, line, "", ""};
            result.push_back(r);
        }
    }

    return result;
}

read_set_t read_fastq_file(std::string file) {
    // TODO
}

bool _comp_read_set_desc(read_t a, read_t b) {
    return a.seq.size() > b.seq.size();
}

void sort_read_set(read_set_t &rs) {
    std::sort(rs.begin(), rs.end(), _comp_read_set_desc);
}