/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

  void run_slot(cell_resource_allocator& res_alloc) override;

  void add_ue(const ue_cell_configuration& ue_cfg);

  void reconf_ue(const ue_cell_configuration& new_ue_cfg, const ue_cell_configuration& old_ue_cfg);

  void rem_ue(const ue_cell_configuration& ue_cfg);

private:
  /// Information on currently configured UE UCI resource to periodically schedule.
  struct periodic_uci_info {
    rnti_t   rnti        = rnti_t::INVALID_RNTI;
    unsigned sr_counter  = 0;
    unsigned csi_counter = 0;
  };

  // Helper to fetch a UE cell config.
  const ue_cell_configuration* get_ue_cfg(rnti_t rnti) const;
  // Helper that schedules the SR and CSI for a given slot.
  void schedule_slot_ucis(cell_slot_resource_allocator& slot_alloc);
  // Helper that schedules the SR and CSI for UEs that were recently updated.
  void schedule_updated_ues_ucis(cell_resource_allocator& res_alloc);

  void add_resource(rnti_t crnti, unsigned offset, unsigned period, bool is_sr);
  void rem_resource(rnti_t crnti, unsigned offset, unsigned period, bool is_sr);

  // Cell configuration.
  const cell_configuration& cell_cfg;
  // Reference to PUCCH resource allocator object.
  uci_allocator& uci_alloc;
  ue_repository& ues;

  srslog::basic_logger& logger;

  // Storage of the periodic UCIs to be scheduled in the resource grid. Each position of the vector represents a slot
  // in a ring-like structure (ie slot % WHEEL_SIZE). Each of these vector indexes/slots contains a list of periodic
  // UCI information to be scheduled in the respective slot.
  std::vector<static_vector<periodic_uci_info, MAX_PUCCH_PDUS_PER_SLOT>> periodic_uci_slot_wheel;

  // UEs whose configuration has been updated in between the last and current slot indications.
  std::vector<rnti_t> updated_ues;
};

} // namespace srsran
