#ifndef dump_error_files_h
#define dump_error_files_h

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class flags
{
  public:
    bool wait;
    bool halt;
    const bool debug;
    const bool DumpRaw;
    const bool skipResetMessage;
    const bool CHECK_PIXELS;    // Decode_data
    const bool PRINT_PIXELS;    // Decode_data
    const bool PRINT_ERRORS;    // Decode_error
    const bool printFirstReset; // Decode_error
    const bool all_TBM_errors;  // Decode_error
    const unsigned int SELECT_CHANNEL;
    
    flags();
};

class data
{
  public:
    bool tmp_catcher;
    
    static const unsigned int ch_nr = 49;
    static const unsigned int error_nr = 20;
    static const unsigned int buffer_sz = 1000000;

    unsigned long count;
    double time;
    double time0;
    
    unsigned int countErrors[ch_nr][error_nr];
    
    string print_buffer;
    
    void Print();

    data();
};

class event
{
  public:
    int status;
    unsigned long tog0word;
    unsigned long word32;

    event();
};

#endif
