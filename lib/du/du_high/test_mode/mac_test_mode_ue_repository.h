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

#include "mac_test_mode_event_handler.h"
#include "srsran/adt/flat_map.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/adt/unique_function.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {

/// \brief Handles information related to the test UE(s).
class mac_test_mode_ue_repository
{
public:
  mac_test_mode_ue_repository(mac_test_mode_event_handler& event_handler,
                              rnti_t                       rnti_start_,
                              uint16_t                     nof_ues_,
                              uint16_t                     nof_cells_);

  bool is_test_ue(du_ue_index_t ue_idx) const { return ue_idx < nof_ues * cells.size(); }

  bool is_test_ue(rnti_t rnti) const
  {
    uint16_t rnti_val = static_cast<unsigned>(rnti);
    return rnti_val >= rnti_start and rnti_val < rnti_end;
  }

  bool is_cell_test_ue(du_cell_index_t cell_idx, rnti_t rnti) const
  {
    unsigned rnti_val = static_cast<unsigned>(rnti) - rnti_start;
    return rnti_val / nof_ues == cell_idx;
  }

  const sched_ue_config_request& get_sched_ue_cfg_request(rnti_t rnti) const;

  const sched_ue_config_request* find_sched_ue_cfg_request(rnti_t rnti) const;

  bool is_msg4_rxed(rnti_t rnti) const;

  void msg4_rxed(rnti_t rnti, bool msg4_rx_flag_);

  void add_ue(rnti_t rnti, du_ue_index_t ue_idx_, const sched_ue_config_request& sched_ue_cfg_req_);

  void remove_ue(rnti_t rnti);

  du_ue_index_t rnti_to_du_ue_idx(rnti_t rnti) const;

private:
  using cell_event_queue =
      concurrent_queue<unique_task, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

  struct test_ue_info {
    du_ue_index_t           ue_idx;
    sched_ue_config_request sched_ue_cfg_req;
    bool                    msg4_rx_flag;
  };

  struct cell_info {
    // Mapping between UE RNTI and test UE information.
    flat_map<rnti_t, test_ue_info> rnti_to_ue_info_lookup;

    cell_info() { rnti_to_ue_info_lookup.reserve(MAX_NOF_DU_UES); }
  };

  unsigned get_cell_index(rnti_t rnti) const;

  // Parameters received from configuration.
  mac_test_mode_event_handler& event_handler;
  const uint16_t               rnti_start;
  const uint16_t               nof_ues;

  // Derived
  uint16_t rnti_end;

  std::vector<std::unique_ptr<cell_info>> cells;
};

} // namespace srsran
