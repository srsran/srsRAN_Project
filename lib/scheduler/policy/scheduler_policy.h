/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SCHEDULER_POLICY_H
#define SRSGNB_SCHEDULER_POLICY_H

#include "ue_allocator.h"

namespace srsgnb {

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class scheduler_policy
{
public:
  virtual ~scheduler_policy() = default;

  /// Schedule UE DL grants for a given {slot, cell}.
  /// \param in input parameters for the given {slot, cell}. This includes list of eligible UEs. For a UE to be eligible
  ///           it has to have an active BWP in the current {slot, cell} and pending bytes.
  /// \param out scheduler output for the given {slot, cell}.
  virtual void dl_sched(const ue_list& ues, ue_pdsch_allocator& pdsch_alloc) = 0;

  /// Schedule UE UL grants for a given {slot, cell}.
  /// \param in input parameters for the given {slot, cell}. This includes list of eligible UEs. For a UE to be eligible
  ///           it has to have an active BWP in the current {slot, cell} and pending bytes.
  /// \param out scheduler output for the given {slot, cell}.
  virtual void ul_sched(const ue_list& ues, ue_pusch_allocator& pusch_alloc) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SCHEDULER_POLICY_H
