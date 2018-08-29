#ifndef _flipper_fl_h
#define _flipper_fl_h

#include <string>
#include "fasta.hpp"

read_set_t get_fl_reads(const read_set_t &reads, std::string a5, std::string a3, bool is_fq, int n_threads);

#endif