// #ifndef dump_error_files_h
// #define dump_error_files_h

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

int Decode_data(flags &, data &, event &);
int Decode_error(flags &, data &, event &);
inline int DE_dummy(const bool print, data &d, unsigned long &tog0word);
inline int DE_gap(const bool print, data &d, unsigned long &tog0word);
inline int DE_ENE(const bool print, data &d, event &ev,
                  const string &str_event_nr,  const unsigned int &channel);
inline int DE_timeout(const bool print, flags &fl, data &d, event &ev,
                      const string &str_event_nr);
inline int DE_trailer(const bool print, const flags &fl, data &d,
                      const event &ev, const unsigned int &event_nr,
                      const unsigned int &channel);
inline void DE_trailer_TBM(const bool print, const flags &fl, data &d,
                      const unsigned long &tbm_status,
                      const unsigned int &event_nr,
                      const unsigned int &channel, int &status);
inline int DE_FIFO(data &d, event &ev, const string &event_nr);

// #endif
