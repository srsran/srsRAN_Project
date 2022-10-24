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

#include "../cell/resource_grid.h"
#include "../ue_scheduling/ue.h"
#include "../ue_scheduling/ue_configuration.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

/// PUCCH scheduling interface.
class pucch_scheduler
{
public:
  virtual ~pucch_scheduler() = default;

  /// Allocate the PUCCH resource all UEs's SR opportunities.
  /// \param[out,in] res_alloc struct with scheduling results.
  /// \param[in] sl_tx slot for which the SR should be allocated.
  virtual void run_slot(cell_resource_allocator& res_alloc, slot_point sl_tx) = 0;
};

} // namespace srsgnb
