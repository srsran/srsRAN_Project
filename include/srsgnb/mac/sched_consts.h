
#ifndef SRSGNB_SCHED_CONSTS_H
#define SRSGNB_SCHED_CONSTS_H

#include <cstddef>
#include <cstdint>

namespace srsgnb {

/// Maximum number of layers (implementation-defined)
const size_t MAX_NOF_LAYERS = 2;

const size_t MAX_NOF_PRBS = 275;
const size_t MAX_NOF_RBGS = 18;

/// SSB constants
/// This is the max num of SSB in a list of SSB to be passed from SCHED to MAC
const size_t MAX_NOF_SSB  = 4;
const size_t DEFAULT_SSB_PERIODICITY = 5;
/// FR1 = [ 410 MHz – 7125 MHz] (TS 38.101, Section 5.1) and ARFCN corresponding to 7.125GHz is 875000
const uint32_t FR1_MAX_FREQUENCY_ARFCN = 875000;

} // namespace srsgnb

#endif // SRSGNB_SCHED_CONSTS_H
