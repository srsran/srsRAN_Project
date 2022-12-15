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

class pucch_guardbands_sched
{
public:
  pucch_guardbands_sched(const cell_configuration& cell_cfg_);

  void run_slot(cell_resource_allocator& res_alloc);

private:
  void allocate_pucch_guardbands(cell_slot_resource_allocator& slot_alloc);

  const cell_configuration& cell_cfg;
  /// Flag indicating whether \c run_slot is called for the first time or not.
  bool first_run_slot{true};
};

} // namespace srsgnb
