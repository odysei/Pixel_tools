#ifndef process_file_h
#define process_file_h

#include <iostream>
#include <fstream>
#include <sstream>
#include "dump_error_files.h"

using namespace std;

static ostringstream os_converter;

// inline string String_dec(const double &c) {
//     ostringstream os;
//     os << c << std::ends;
//     return os.str();
// }
// 
// inline string String_dec(unsigned int &c) {
//     ostringstream os;
//     os << c << std::ends;
//     return os.str();
// }
// 
inline string String_hex(const unsigned long &c) {
    os_converter.clear();
    os_converter.str("");
    os_converter.seekp(0); // for outputs: seek put ptr to start
    os_converter << hex << c;
    return os_converter.str();
}

void Process_file(flags &, ifstream &in_file, data &);
inline int Process_file_(flags &, ifstream &in_file, data &,
                         unsigned long &event);
inline int Process_file_timestamp(ifstream &in_file, data &);
inline void Process_file_count(ifstream &in_file, data &);

void Print_summary(const unsigned int countErrors[][data::error_nr]);

/*
 * Decode_XYZ defined in decoder.cc
 */
int Decode_data(flags &, data &, event &);
int Decode_error(flags &, data &, event &);

#endif
