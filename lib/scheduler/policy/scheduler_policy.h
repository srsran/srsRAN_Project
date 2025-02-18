/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "slice_allocator.h"

namespace srsran {

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class scheduler_policy
{
public:
  virtual ~scheduler_policy() = default;

  /// Schedule UE DL grants for a given slot and one or more cells.
  virtual void dl_sched(slice_dl_sched_context& dl_ctxt) = 0;

  /// Schedule UE UL grants for a given {slot, cell}.
  virtual void ul_sched(slice_ul_sched_context& ul_ctxt) = 0;
};

} // namespace srsran
