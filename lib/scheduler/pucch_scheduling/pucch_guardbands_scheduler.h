/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ue_scheduling/ue.h"

namespace srsgnb {

class cell_configuration;
struct cell_resource_allocator;
struct cell_slot_resource_allocator;

/// This class implements a scheduler that pre-reserve the PUCCH resouces on the scheduler grid, to prevent the PUSCH
/// allocator from using them. The pre-reserve the PUCCH are hereby referred as PUCCH guardbands.
class pucch_guardbands_scheduler
{
public:
  explicit pucch_guardbands_scheduler(const cell_configuration& cell_cfg_);

  /// \brief Run the PUCCH guardbands scheduler.
  ///
  /// When called for the first time, this function pre-reserves the PUCCH over the entire grid, until the last
  /// (farthest in the future) usable slot. From the second time onwards, the scheduler pre-reserves the PUCCH for the
  /// last usable slot of the grid.
  /// \param[out,in] res_alloc resource grid.
  void run_slot(cell_resource_allocator& res_alloc);

private:
  /// Pre-allocates the PUCCH guardband on the resource grid for a given slot.
  void allocate_pucch_guardbands(cell_slot_resource_allocator& slot_alloc);

  const cell_configuration& cell_cfg;

  /// Flag indicating whether \c run_slot is called for the first time or not.
  bool first_run_slot{true};

  srslog::basic_logger& logger;
};

} // namespace srsgnb
