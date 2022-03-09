
#ifndef SRSGNB_DATA_SCHEDULER_H
#define SRSGNB_DATA_SCHEDULER_H

#include "data_sched_input.h"
#include "data_sched_output.h"

namespace srsgnb {

/// Interface to allocate UE DL and UL grants in a specific BWP
class data_scheduler
{
public:
  virtual ~data_scheduler()                                                  = default;
  virtual void dl_sched(const data_sched_input& in, dl_data_sched_output& out) = 0;
  virtual void ul_sched(const data_sched_input& in, ul_data_sched_output& out) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_DATA_SCHEDULER_H
