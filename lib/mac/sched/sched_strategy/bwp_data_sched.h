
#ifndef SRSGNB_BWP_DATA_SCHED_H
#define SRSGNB_BWP_DATA_SCHED_H

#include "bwp_sched_input.h"
#include "bwp_sched_output.h"

namespace srsgnb {

/// Interface to allocate UE DL and UL grants in a specific BWP
class bwp_data_sched
{
public:
  virtual ~bwp_data_sched()                                                  = default;
  virtual void dl_sched(const bwp_sched_input& in, bwp_dl_sched_output& out) = 0;
  virtual void ul_sched(const bwp_sched_input& in, bwp_ul_sched_output& out) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_BWP_DATA_SCHED_H
