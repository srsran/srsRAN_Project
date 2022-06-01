/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SCHEDULER_TIME_RR_H
#define SRSGNB_SCHEDULER_TIME_RR_H

#include "scheduler_policy.h"

namespace srsgnb {

class scheduler_time_rr : public scheduler_policy
{
public:
  void dl_sched(const ue_list& ues, ue_pdsch_allocator& pdsch_alloc) override;

  void ul_sched(const ue_list& ues, ue_pusch_allocator& pusch_alloc) override;

private:
  unsigned rr_count = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHEDULER_TIME_RR_H
