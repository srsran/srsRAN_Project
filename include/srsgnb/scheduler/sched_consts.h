/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SCHED_CONSTS_H
#define SRSGNB_SCHED_CONSTS_H

#include "srsgnb/ran/resource_block.h"
#include <cstddef>

namespace srsgnb {

/// Maximum number of layers (implementation-defined)
const size_t MAX_NOF_LAYERS = 2;

const size_t MAX_NOF_RBGS = 18;

/// SSB constants
/// This is the max num of SSB in a list of SSB to be passed from SCHED to MAC
const size_t DEFAULT_SSB_PERIODICITY = 5;
/// FR1 = [ 410 MHz – 7125 MHz] (TS 38.101, Section 5.1) and ARFCN corresponding to 7.125GHz is 875000
const unsigned FR1_MAX_FREQUENCY_ARFCN = 875000;
/// The cutoff frequency for case A, B and C paired is 3GHz, corresponding to 600000 ARFCN (TS 38.213, Section 4.1).
const unsigned CUTOFF_FREQ_ARFCN_CASE_A_B_C = 600000;
/// The cutoff frequency for case C unpaired is 1.88GHz, corresponding to 376000 ARFCN (TS 38.213, Section 4.1).
const unsigned CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED = 376000;
const unsigned NOF_SSB_OFDM_SYMBOLS              = 4;
const unsigned NOF_SSB_PRBS                      = 20;

} // namespace srsgnb

#endif // SRSGNB_SCHED_CONSTS_H
