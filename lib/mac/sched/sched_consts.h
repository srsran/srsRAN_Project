
#ifndef SRSGNB_SCHED_CONSTS_H
#define SRSGNB_SCHED_CONSTS_H

#include <cstddef>

namespace srsgnb {

const size_t MAX_NOF_PRBS = 275;
const size_t MAX_NOF_RBGS = 18;

/// SSB constants
/// This is the max num of SSB in a list of SSB to be passed from SCHED to MAC
const size_t MAX_NOF_SSB  = 4;
const size_t DEFAULT_SSB_PERIODICITY = 5;

} // namespace srsgnb

#endif // SRSGNB_SCHED_CONSTS_H
