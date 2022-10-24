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
#include "../ue_scheduling/ue_configuration.h"
#include "pucch_allocator.h"
#include "pucch_scheduler.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

/// Implementation of the PUCCH scheduler interface.
class pucch_scheduler_impl final : public pucch_scheduler
{
public:
  explicit pucch_scheduler_impl(const cell_configuration& cell_cfg_, pucch_allocator& pucch_alloc_, ue_list& ues_);

  ~pucch_scheduler_impl() override;

  /// Allocate the PUCCH resource for CSI or SR opportunities.
  /// \param[out,in] res_alloc struct with scheduling results.
  /// \param[in] sl_tx slot for which the PUCCH resource should be allocated.
  void run_slot(cell_resource_allocator& res_alloc, slot_point sl_tx) override;

private:
  // Cell configuration.
  const cell_configuration& cell_cfg;
  // Ref to PUCCH resource allocator object.
  pucch_allocator&          pucch_alloc;
  ue_list&                  ues;

  srslog::basic_logger& logger;
};

} // namespace srsgnb
