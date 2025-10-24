/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/slotted_vector.h"
#include "srsran/mac/mac_cell_rach_handler.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"
#include "srsran/srslog/logger.h"

namespace srsran {

class scheduler_configurator;
class rnti_manager;
class mac_rach_handler;
struct sched_cell_configuration_request_message;

/// Handler of RACH indications for a given cell in the MAC.
class mac_cell_rach_handler_impl final : public mac_cell_rach_handler
{
public:
  mac_cell_rach_handler_impl(mac_rach_handler& parent_, const sched_cell_configuration_request_message& sched_cfg_);

  /// Handle detected RACH indication.
  void handle_rach_indication(const mac_rach_indication& rach_ind) override;

  /// Reserve Contention-free RACH preamble for a given UE.
  [[nodiscard]] bool handle_cfra_allocation(uint8_t preamble_idx, du_ue_index_t ue_idx, rnti_t crnti);

  /// Deallocate Contention-free RACH preamble that was previously given to a UE.
  void handle_cfra_deallocation(du_ue_index_t ue_idx);

private:
  // Whether the preamble provided is reserved for CFRA.
  bool is_cfra_preamble(unsigned ra_preamble_id) const;

  // Map of preamble ID to entry in \c preambles vector.
  unsigned get_cfra_index(unsigned ra_preamble_id) const;

  mac_rach_handler&     parent;
  const du_cell_index_t cell_index;
  const unsigned        nof_cb_preambles;

  std::vector<std::atomic<rnti_t>> preambles;
};

/// Handler of RACH indications for multiple cells.
class mac_rach_handler
{
public:
  mac_rach_handler(scheduler_configurator& sched_, rnti_manager& rnti_mng_, srslog::basic_logger& logger_);

  /// Create new handler of RACH indications for a cell.
  mac_cell_rach_handler_impl& add_cell(const sched_cell_configuration_request_message& sched_cfg);

  void rem_cell(du_cell_index_t cell_index);

private:
  friend class mac_cell_rach_handler_impl;

  struct cfra_ue_context {
    du_cell_index_t cell_idx;
    uint8_t         preamble_id;
  };

  scheduler_configurator& sched;
  rnti_manager&           rnti_mng;
  srslog::basic_logger&   logger;

  slotted_id_vector<du_cell_index_t, std::unique_ptr<mac_cell_rach_handler_impl>> cell_map;
  std::vector<cfra_ue_context>                                                    ue_map;
};

} // namespace srsran
