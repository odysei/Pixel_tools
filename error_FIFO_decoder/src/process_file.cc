#ifndef process_file_cc
#define process_file_cc

#include "process_file.h"
#include <bitset>

#ifdef OLD_COMPILER
/*
 * Compatibility hacks
 */
inline string to_string(const unsigned long int &v)
{
    return to_string(static_cast<long long unsigned int>(v));
}

inline string to_string(const double &v)
{
    return to_string(static_cast<long double>(v));
}
#endif

flags::flags() : debug(true),
                 DumpRaw(true),
                 skipResetMessage(false),
                 CHECK_PIXELS(false),
                 PRINT_PIXELS(true),
                 PRINT_ERRORS(true),
                 printFirstReset(true),
                 all_TBM_errors(true),
                 SELECT_CHANNEL(999)
{
    halt = false;   // can change
    wait = false;   // can change
}

data::data() : countErrors{{0}}
{
    tmp_catcher = false;
    count = 0;
    time = 0;
    time0 = 0;
    print_buffer.reserve(buffer_sz);
}

inline void data::Print()
{
    cout << print_buffer;
    print_buffer.clear();
    print_buffer.reserve(buffer_sz);
}

event::event()
{
    status = 0;
    tog0word = 0;
    word32 = 0;
}

// big-endian input: x0 < x1
unsigned long Convert_32bit(const char *in, const int x0)
{
    return bitset<8>(in[x0]).to_ulong() +
           (bitset<8>(in[x0 + 1]).to_ulong() << 8) +
           (bitset<8>(in[x0 + 2]).to_ulong() << 16) +
           (bitset<8>(in[x0 + 3]).to_ulong() << 24);
}

void Process_file(flags &fl, ifstream &in_file, data &d)
{
    // read the header, i.e., run number
    {
        char buffer[4];
        in_file.read(buffer, 4);
        
        const unsigned long runnr = Convert_32bit(buffer, 0);
        if (in_file.eof()) {
            cout << "End of input file" << endl;
            return;
        }
        cout << " run number = " << runnr;
        if (fl.skipResetMessage)
            cout << " - tbm trailer messages suppressed ";
        cout << endl;
    }
    
    // Event loop
    const unsigned long max_evs = 5E+10;
    unsigned long ev_nr = 0;
    while (ev_nr < max_evs) {
        if (Process_file_(fl, in_file, d, ev_nr) != 0)
            break;
    }

    cout << " Summary, events " << ev_nr << endl;
}

inline int Process_file_(flags &fl, ifstream &in_file, data &d,
                          unsigned long &ev_nr)
{
    if (Process_file_timestamp(in_file, d) != 0)    // read a time stamp
        return -1;
    Process_file_count(in_file, d); // read an event count
    
    if (ev_nr == 0) 
        d.time0 = d.time;
    const double dtime = d.time - d.time0;
//     if (dtime >= 26939) {
        d.print_buffer += " time in sec. = " + to_string(dtime) + ", count = " +
                          to_string(d.count) + "\n";
//         cout << " time in sec. = " << dtime
//             << ", count = " << d.count << endl;
//     }
    
    // fifo loop
    event ev;
    char *buffer = new char[4 * d.count];
    in_file.read(buffer, 4 * d.count);
    if (in_file.eof()) {
        cout << "End of input file" << endl;
        d.Print();
        
        delete buffer;
        return -1;
    }
    for (unsigned int i = 0; i < d.count; ++i) {
        ++ev_nr;
//         if (dtime < 26939)///////
//             continue;
//         if (!d.tmp_catcher) {
//             if (dtime == 26939 && d.count == 290)
//                 d.tmp_catcher = true;
//             else
//                 continue;
//         }
//         if (dtime != 26939 || (d.count != 290 && d.count != 193))
//             continue;
//         if (dtime > 333)
//             continue;
        /////
        ev.word32 = Convert_32bit(buffer, 4 * i);

        if (fl.DumpRaw)
            d.print_buffer += String_hex(ev.word32) + "\n";
//             cout << hex << ev.word32 << dec << endl;
        if (ev.word32 != 0)
            ev.status = Decode_data(fl, d, ev);
        // if (ev.status == -12)
        //     fl.wait = true;  // halt on ENE

        if (fl.halt || fl.wait) {
            int dummy = 0;
            d.Print();
            cout << " Enter 0 to continue, -1 to stop, 999 continous ";
            cin >> dummy;
            if (dummy == 999)
                fl.halt = false;
            if (dummy == -1)
                break;
            fl.wait = false;
        }
    }
    d.Print();
    
    delete buffer;
    return 0;
}

// read a time stamp
inline int Process_file_timestamp(ifstream &in_file, data &d)
{
    in_file.read(reinterpret_cast<char *>(&d.time), 8);
    if (in_file.eof()) {
        cout << "End of input file" << endl;
        
        return -1;
    }
    
    return 0;
}

// read an event count
inline void Process_file_count(ifstream &in_file, data &d)
{
    char buffer[4];
    in_file.read(buffer, 4);
    d.count = Convert_32bit(buffer, 0);
}

void Print_summary(const unsigned int countErrors[][data::error_nr])
{
    const string errorString[data::error_nr] = {" ", // 0
                                    " ", // 1
                                    " ", // 2
                                    " ", // 3
                                    " ", // 4
                                    " ", // 5
                                    " ", // 6
                                    " ", // 7
                                    " ", // 8
                                    " ", // 9
                                    "overflow    ", // 10 - overflow ()
                                    "timeout     ", // 11 - timeout (29)
                                    "ENE         ", // 12 - ENE (31)
                                    "fifo        ", // 13 - fifo (28)
                                    "num-of-rocs ", // 14 - num of rocs (36)
                                    "pkam/autorst", // 15 - fsm error ()
                                    "trailer     ", // 16 - trailer  (30)
                                    " ",
                                    " ",
                                    " "};
    int countAllChanErrors[data::error_nr] = {0};
    cout << " channel error count " << endl;
    
    for (unsigned int ichan = 0; ichan < data::ch_nr; ++ichan)
        for (unsigned int ierr = 0; ierr < data::error_nr; ++ierr) {
            const unsigned int nr = countErrors[ichan][ierr];
            if (nr > 0) {
                cout << ichan << " " << errorString[ierr] << " " << nr << endl;
                countAllChanErrors[ierr] += nr;
            }
        }
    
    for (unsigned int ierr = 0; ierr < data::error_nr; ++ierr)
        if (countAllChanErrors[ierr] > 0)
            cout << "Tot. " << errorString[ierr] << " - "
                 << countAllChanErrors[ierr] << endl;
}

#endif
