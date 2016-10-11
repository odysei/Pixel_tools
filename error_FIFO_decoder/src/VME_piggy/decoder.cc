#ifndef VME_piggy__decoder_cc
#define VME_piggy__decoder_cc

#include "VME_piggy/decoder.h"
#include "VME_piggy/masks.h"
#include "process_file.h"

#ifdef OLD_COMPILER
/*
 * Compatibility hacks
 */

inline string to_string(const int &v)
{
    return to_string(static_cast<long long int>(v));
}

inline string to_string(const unsigned int &v)
{
    return to_string(static_cast<long long unsigned int>(v));
}

inline string to_string(const unsigned long int &v)
{
    return to_string(static_cast<long long unsigned int>(v));
}
#endif


///////////////////////////////////////////////////////////////////////////
int Decode_data(flags &fl, data &d, event &ev)
{
    // definitions
    const unsigned int ROCMAX = 16;

    const unsigned long roc = ((ev.word32 & masks::rocmsk) >> 21);
    // Check for embeded special words
    if (roc > 0 && roc < 25) { // valid ROCs go from 1-24
        return 0;///////////////////////////////////////////////////
        if (fl.PRINT_PIXELS) {
            d.print_buffer += "data ";
            d.print_buffer += String_hex(ev.word32);
        }
        
        const unsigned long channel = ((ev.word32 & masks::chnlmsk) >> 26);
        if (channel > 0 && channel < 37) { // valid channels 1-36
            const unsigned long dcol = (ev.word32 & masks::dclmsk) >> 16;
            const unsigned long pix = (ev.word32 & masks::pxlmsk) >> 8;
            const unsigned long adc = (ev.word32 & masks::plsmsk);
            
            if (fl.PRINT_PIXELS)
                cout << " Channel- " << channel << " ROC- " << roc << " DCOL- "
                     << dcol << " Pixel- " << pix << " ADC- " << adc << endl;

            if (fl.CHECK_PIXELS) {
                if (roc > ROCMAX)
                    cout << " wrong roc number " << channel << "/" << roc << "/"
                         << dcol << "/" << pix << "/" << adc << endl;
                if (dcol < 0 || dcol > 25)
                    cout << " wrong dcol number " << channel << "/" << roc
                         << "/" << dcol << "/" << pix << "/" << adc << endl;
                if (pix < 2 || pix > 181)
                    cout << " wrong pix number " << channel << "/" << roc << "/"
                         << dcol << "/" << pix << "/" << adc << endl;
            }
            
            return 0;
        }
        
        cout << "Wrong channel " << channel << endl;
        return -2;
    }
        
    return Decode_error(fl, d, ev);
}

/////////////////////////////////////////////////////////////////////////////
// Decode error FIFO
// Works for both, the error FIFO and the SLink error words. d.k. 25/04/07
int Decode_error(flags &fl, data &d, event &ev)
{
    // DUMMY WORD
    if ((ev.word32 & masks::errorMask) == masks::dummyMask)
        return DE_dummy(fl.debug, d, ev.tog0word);

    // GAP WORD
    if ((ev.word32 & masks::errorMask) == masks::gapMask)
        return DE_gap(fl.debug, d, ev.tog0word);

    // used in other words:
    const unsigned long channel = (ev.word32 & masks::channelMask) >> 26;
    const unsigned long event_nr = (ev.word32 & masks::eventNumMask) >> 13;
    const string str_event_nr = to_string(event_nr);
    const bool print = fl.PRINT_ERRORS || (channel == fl.SELECT_CHANNEL);
    
    // TIMEOUT
    if ((ev.word32 & masks::errorMask) == masks::timeOut)
        return DE_timeout(print, fl, d, ev, str_event_nr);
    
    // EVENT NUMBER ERROR
    if ((ev.word32 & masks::errorMask) == masks::eventNumError)
        return DE_ENE(print, d, ev, str_event_nr, channel);

    // TRAILER
    if (((ev.word32 & masks::errorMask) == masks::trailError))
        return DE_trailer(print, fl, d, ev, event_nr, channel);

    // FIFO
    if ((ev.word32 & masks::errorMask) == masks::fifoError)
        return DE_FIFO(d, ev, str_event_nr);
    
    // UNKOWN error
    d.print_buffer += " Unknown error?";
    d.print_buffer += String_hex(ev.word32);
    d.print_buffer += " event ";
    d.print_buffer += str_event_nr;

    return -1;
}

// DUMMY WORD
inline int DE_dummy(const bool print, data &d, unsigned long &tog0word)
{
    if (print)
        d.print_buffer += " Dummy word\n";
    
    tog0word = 0;
    return 0;
}

// GAP WORD
inline int DE_gap(const bool print, data &d, unsigned long &tog0word)
{
    if (print)
        d.print_buffer += " Gap word\n";
    
    tog0word = 0;
    return 0;
}

// EVENT NUMBER ERROR
inline int DE_ENE(const bool print, data &d, event &ev,
                  const string &event_nr,
                  const unsigned int &channel)
{
    const unsigned long tbm_event = (ev.word32 & masks::tbmEventMask);
    if (print) {
        d.print_buffer += "Event Number Error- channel: ";
        d.print_buffer += to_string(channel);
        d.print_buffer += " tbm event nr. ";
        d.print_buffer += to_string(tbm_event);
        d.print_buffer += ":event: ";
        d.print_buffer += event_nr;
        d.print_buffer +=  "\n";
    }
    
    ev.tog0word = 0;
    ++d.countErrors[channel][12];
    return -12;
}

// TIMEOUT
inline int DE_timeout(const bool print, flags &fl, data &d, event &ev,
                      const string &event_nr)
{
    if (ev.tog0word == 0) {
        ev.tog0word = ev.word32;
        
        if (print) {
            d.print_buffer += ":event: ";
            d.print_buffer += event_nr + "\n";
        }
        return -11;
    }
    
    {
        const string enbablette = String_hex((ev.tog0word & 0xf8000000) >> 27);
        d.print_buffer += "Timeout Chnl Mask in group of 4-5 0x";
        d.print_buffer += enbablette;
        d.print_buffer += " event ";
        d.print_buffer += to_string((ev.tog0word & masks::eventNumMask) >> 13);
        d.print_buffer += "\n";
        d.print_buffer += "1st channel in group : ";
        d.print_buffer += to_string(masks::offsets[((ev.word32 & masks::BlkNumMask) >> 8)] + 1);
        d.print_buffer += " of ";
        d.print_buffer += to_string(masks::offsets[((ev.word32 & masks::BlkNumMask) >> 8) + 1] -
                    masks::offsets[((ev.word32 & masks::BlkNumMask) >> 8)]);
        d.print_buffer += " channels";
        d.print_buffer += "\n";
//         cout << "Timeout Chnl Mask in group of 4-5 0x" << hex << enbablette
//              << dec << " event " << ((ev.tog0word & masks::eventNumMask) >> 13)
//              << endl;
//         cout << "1st channel in group : "
//             << (masks::offsets[((ev.word32 & masks::BlkNumMask) >> 8)] + 1) << " of ";
//         cout << (masks::offsets[((ev.word32 & masks::BlkNumMask) >> 8) + 1] -
//                     masks::offsets[((ev.word32 & masks::BlkNumMask) >> 8)])
//             << " channels" << endl;
    }
    // cout << "timeout word 0x" << hex << ev.word32 << " ev.tog0word
    // 0x" << ev.tog0word << dec << endl;
    
    // timeout word2. Has bits D26--31 for MSB and D11-12 for LSB
    if ((((ev.word32 & masks::MSB_counter) >> 24) +
        ((ev.word32 & masks::LSB_counter) >> 11)) > 0) {
        if (print)
            d.print_buffer += "Timeout Error - ";
//             cout << "Timeout Error - ";
        
        // index within a group of 4/5
        unsigned long index = (ev.word32 & masks::TO_data_chs);
        const unsigned long chip0 = (ev.word32 & masks::BlkNumMask) >> 8;
        const int offset0 = masks::offsets[chip0];
        unsigned int channel = 0;
        for (int i = 0; i < 5; ++i) {
            if ((index & 0x1) != 0) {
                channel = offset0 + i + 1;
                if (print) {
                    d.print_buffer += "channel: ";
                    d.print_buffer += to_string(channel);
                    d.print_buffer += " ";
//                     cout << "channel: " << channel << " ";
                }
            }
            index = index >> 1;
        }
        ++d.countErrors[channel][11];
        if (print) {
            d.print_buffer += " :Timeout counter?:  ";
            d.print_buffer += to_string(((ev.word32 & masks::MSB_counter) >> 24) +
                    ((ev.word32 & masks::LSB_counter) >> 11));
            d.print_buffer += " :Timeout counter?:  ";
            d.print_buffer += to_string((ev.word32 & masks::MSB_counter) >> 26);
//             cout << " :Timeout counter?:  "
//                  << (((ev.word32 & masks::MSB_counter) >> 24) +
//                     ((ev.word32 & masks::LSB_counter) >> 11));
//             cout << " :Timeout counter?:  "
//                  << (((ev.word32 & masks::MSB_counter) >> 26));
        }

        // if((ev.tog0word>0) &&
        //      (((ev.tog0word & masks::eventNumMask) >> 13) == event)) {
        if (print) {
            if ((ev.tog0word & masks::TO_PKAM))
                d.print_buffer += ":PKAM:";
            if ((ev.tog0word & masks::TO_autoreset))
                d.print_buffer += ":Autoreset:";
            if ((ev.tog0word & masks::TO_cal))
                d.print_buffer += ":CAL:";
            if ((ev.tog0word & masks::TO_stk_full))
                d.print_buffer += ":Stack FULL:";
            d.print_buffer += " :Stack Count: ";
        }

        if (ev.tog0word & 0x200) {
            if (print)
                d.print_buffer += "-" + to_string(((~ev.tog0word) & 0x1ff) + 1);
//                 cout << "-" << dec << (((~ev.tog0word) & 0x1ff) + 1);
        } else {
            if (print)
                d.print_buffer += to_string(ev.tog0word & 0x3f);
//                 cout << dec << (ev.tog0word & 0x3f);
        }
        //      }

        ev.tog0word = 0;
    } else {
        if (ev.tog0word != 0) {

            if (print)
                d.print_buffer += " Orphan Timeout Pedestal Correction value: ";
//                 cout << " Orphan Timeout Pedestal Correction value: ";

            if (ev.tog0word & 0x200) {
                if (print)
                    d.print_buffer += "-" + to_string(((~ev.tog0word) & 0x1ff) + 1);
//                     cout << "-" << dec << (((~ev.tog0word) & 0x1ff) + 1);
            } else {
                if (print)
                    d.print_buffer += to_string(ev.tog0word & 0x1ff);
//                     cout << dec << (ev.tog0word & 0x1ff);
            }
            if (print) {
                d.print_buffer += ":event: ";
                d.print_buffer += to_string((ev.tog0word & masks::eventNumMask) >> 13);
                d.print_buffer += "\n";
            }
//                 cout << ":event: " << ((ev.tog0word & masks::eventNumMask) >> 13) << endl;
            ev.tog0word = 0;
        }
        ev.tog0word = ev.word32;
    }
    
    if (print)
        d.print_buffer += ":event: " + event_nr + "\n";
//         cout << ":event: " << event_nr << endl;

    return -11;
}

// TRAILER
inline int DE_trailer(const bool print, const flags &fl, data &d,
                      const event &ev, const unsigned int &event_nr,
                      const unsigned int &channel)
{
    int status = -1;
    const unsigned long tbm_status = (ev.word32 & masks::TBM_status);

    if (print) {
        d.print_buffer += "Trailer Error- ";
        d.print_buffer += "channel: ";
        d.print_buffer += to_string(channel) + " - ";
    }
    
    if (ev.word32 & masks::TE_overflow) {
        if (print)
            d.print_buffer += "Overflow Error";
        status = -10;
        ++d.countErrors[channel][10];
    }

    if (ev.word32 & masks::TE_ROC_error) {
        if (print)
            d.print_buffer += "Number of Rocs Error";
        status = -14;
        ++d.countErrors[channel][14];
    }
    if (ev.word32 & masks::TE_FSM_error) {
        if (print) {
            if (ev.word32 & masks::TE_PKAM)
                d.print_buffer += ", PKAM";
            if (ev.word32 & masks::TE_autoreset)
                d.print_buffer += ", Autoreset";
        }
        status = -15;
        ++d.countErrors[channel][15];
    }

    // trailer errors/messages
//     const unsigned long non_TBM = ((ev.word32 & masks::TE_ROC_error) |
//                                    (ev.word32 & masks::TE_FSM_error) |
//                                    (ev.word32 & masks::TE_overflow));
    if (tbm_status)
        DE_trailer_TBM(print, fl, d, tbm_status, event_nr, channel, status);
    if (print)
        d.print_buffer += ":event: " + to_string(event_nr) + "\n";
        
    return status;
}

// Trailer TBM part
inline void DE_trailer_TBM(const bool print, const flags &fl, data &d,
                      const unsigned long &tbm_status,
                      const unsigned int &event_nr,
                      const unsigned int &channel, int &status)
{
    if (fl.all_TBM_errors) {
        if (print) {
            if (tbm_status & masks::TBM_NTP)
                d.print_buffer += ", no token pass";
            if (tbm_status & masks::TBM_only_reset)
                d.print_buffer += ", TBM reset";
            if (tbm_status & masks::TBM_ROC_reset)
                d.print_buffer += ", ROC reset";
            if (tbm_status & masks::TBM_sync_err)
                d.print_buffer += ", SYNC error";
            if (tbm_status & masks::TBM_sync_trg)
                d.print_buffer += ", SYNC trigger";
            if (tbm_status & masks::TBM_clr_trg)
                d.print_buffer += ", CLEAR trigger cnt";
            if (tbm_status & masks::TBM_cal_trg)
                d.print_buffer += ", CAL trigger";
            if (tbm_status & masks::TBM_stk_full)
                d.print_buffer += ", STACK full";
            d.print_buffer += " ";
        }
        
        return;
    }
    
    if (event_nr > 1 || fl.printFirstReset) {
        if (tbm_status == masks::TBM_reset) {
            if (!fl.skipResetMessage) {
                d.print_buffer += " Trailer Message";
                d.print_buffer += " TBM status:0x";
                d.print_buffer += String_hex(tbm_status);
                d.print_buffer += " TBM-Reset received ";
            }
        } else {
            if (print) {
                d.print_buffer += " TBM status:0x";
                d.print_buffer += String_hex(tbm_status);
                d.print_buffer += " ";
            }
            ++d.countErrors[channel][16];
            status = -16;
        }
    }
}

// FIFO
inline int DE_FIFO(data &d, event &ev,
                   const string &event_nr)
{
    ev.tog0word = 0;
        
    if (ev.word32 & masks::Fif2NFMask)
        d.print_buffer += "A fifo 2 is Nearly full- ";
    
    if (ev.word32 & masks::TrigNFMask)
        d.print_buffer += "The trigger fifo is nearly Full - ";
    
    if (ev.word32 & masks::ChnFifMask) {
        d.print_buffer += "fifo-1 is nearly full for channel(s) of this FPGA//";
        if (ev.word32 & masks::ChnFifMask0)
            d.print_buffer += " 1 //";
        if (ev.word32 & masks::ChnFifMask1)
            d.print_buffer += " 2 //";
        if (ev.word32 & masks::ChnFifMask2)
            d.print_buffer += " 3 ";
        if (ev.word32 & masks::ChnFifMask3)
            d.print_buffer += " 4 ";
        if (ev.word32 & masks::ChnFifMask4)
            d.print_buffer += " 5 ";
    }
    d.print_buffer += ":event: " + event_nr;
    d.print_buffer += "\n";
    
    ++d.countErrors[0][13];
    return -13;
}

#endif
