/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid_util.h"
#include "../ue_scheduling/ue_repository.h"
#include "uci_scheduler.h"

namespace srsran {

class cell_configuration;
class uci_allocator;
struct cell_resource_allocator;
struct cell_slot_resource_allocator;

class uci_scheduler_impl final : public uci_scheduler
{
public:
  explicit uci_scheduler_impl(const cell_configuration& cell_cfg_, uci_allocator& uci_alloc_, ue_repository& ues_);

  ~uci_scheduler_impl() override;

  void run_slot(cell_resource_allocator& res_alloc, slot_point sl_tx) override;

private:
  // Helper that schedules the SR and CSI for a given user at a given slot.
  void schedule_uci(cell_slot_resource_allocator&           slot_alloc,
                    rnti_t                                  crnti,
                    const ue_cell&                          user,
                    optional<std::pair<unsigned, unsigned>> csi_period_and_offset);

  // Cell configuration.
  const cell_configuration& cell_cfg;
  // Reference to PUCCH resource allocator object.
  uci_allocator& uci_alloc;
  ue_repository& ues;

  srslog::basic_logger& logger;
};

} // namespace srsran
