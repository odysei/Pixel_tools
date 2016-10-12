#ifndef uTCA__masks_h
#define uTCA__masks_h

namespace masks
{
typedef const unsigned long type;

/*
 * Data-word masks
 */
type pulse_height = 0xff;   // pulse height
type pixel = 0xff00;    // pixel index
type dclmsk = 0x1f0000;
type rocmsk = 0x3e00000;
type channel = 0xfc000000;

/*
 * Error-word masks
 */
type errorMask = 0x3e00000;     // (0x1f << 21)
// type dummyMask = 0x03600000;    // (0x1b << 21)
// type gapMask = 0x03400000;      // (0x1a << 21)
type timeOut = 0x3a00000;       // (0x1d << 21)
type eventNumError = 0x3e00000; // (0x1f << 21)
type trailError = 0x3c00000;    // (0x1e << 21)
// type fifoError = 0x3800000;     // (0x1c << 21) nearly full
type event_number = 0x1fe000;   // event number mask

/* 
 * Event-number-error-word masks
 */
type ENE_TBM_event_nr = 0xff;   // tbm event number

/* 
 * Trailer-error-word masks
 */
type TE_ROC_error = 0x800;  // pointer to #Roc errors
type TE_FSM_error = 0x600;  // pointer to FSM errors
type TE_autoreset = 0x400;  // under FSM bits
type TE_PKAM = 0x200;   // under FSM bits
type TE_overflow = 0x100;   // data overflow
type TBM_status = 0xff; // TBM trailer info
type TBM_NTP = 0x80;    // no token pass
type TBM_reset = 0x60;  // TBM + ROC reset
type TBM_only_reset = 0x40; // TBM-only reset
type TBM_ROC_reset = 0x20;  // ROC reset
type TBM_sync_err = 0x10;   // synchronization error
type TBM_sync_trg = 0x8;    // sync trigger
type TBM_clr_trg = 0x4;     // clear trigger counter
type TBM_cal_trg = 0x2;     // calibration trigger
type TBM_stk_full = 0x1;    // stack full

/* 
 * Timeout-error-word masks
 */
type TO_w1_stack = 0x3f;    // TBM stack count from prev. ev.
type TO_w2_channel = 0x3f;  // channel index
}

#endif
