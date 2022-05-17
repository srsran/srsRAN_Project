/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DATA_SCHEDULER_H
#define SRSGNB_DATA_SCHEDULER_H

#include "data_sched_input.h"
#include "data_sched_output.h"

namespace srsgnb {

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class data_scheduler
{
public:
  virtual ~data_scheduler() = default;

  /// Schedule UE DL grants for a given {slot, cell}.
  /// \param in input parameters for the given {slot, cell}. This includes list of eligible UEs. For a UE to be eligible
  ///           it has to have an active BWP in the current {slot, cell} and pending bytes.
  /// \param out scheduler output for the given {slot, cell}.
  virtual void dl_sched(const data_sched_input& in, dl_data_sched_output& out) = 0;

  /// Schedule UE UL grants for a given {slot, cell}.
  /// \param in input parameters for the given {slot, cell}. This includes list of eligible UEs. For a UE to be eligible
  ///           it has to have an active BWP in the current {slot, cell} and pending bytes.
  /// \param out scheduler output for the given {slot, cell}.
  virtual void ul_sched(const data_sched_input& in, ul_data_sched_output& out) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_DATA_SCHEDULER_H
