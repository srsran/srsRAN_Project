/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ue_scheduling/ue.h"
#include "uci_scheduler.h"

namespace srsran {

class cell_configuration;
class uci_allocator;
struct cell_resource_allocator;

class uci_scheduler_impl final : public uci_scheduler
{
public:
  explicit uci_scheduler_impl(const cell_configuration& cell_cfg_, uci_allocator& uci_alloc_, ue_list& ues_);

  ~uci_scheduler_impl() override;

  void run_slot(cell_resource_allocator& res_alloc, slot_point sl_tx) override;

private:
  // Cell configuration.
  const cell_configuration& cell_cfg;
  // Reference to PUCCH resource allocator object.
  uci_allocator& uci_alloc;
  ue_list&       ues;

  srslog::basic_logger& logger;
};

} // namespace srsran
