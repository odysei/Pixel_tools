#ifndef VME_piggy__decoder_h
#define VME_piggy__decoder_h

#include <iostream>
#include <fstream>
#include <sstream>
#include "dump_error_files.h"

using namespace std;

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

#endif
