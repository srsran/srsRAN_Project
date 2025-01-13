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

#include "mac_test_mode_ue_repository.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

mac_test_mode_ue_repository::mac_test_mode_ue_repository(rnti_t rnti_start_, uint16_t nof_ues_, uint16_t nof_cells) :
  rnti_start(static_cast<unsigned>(rnti_start_)), nof_ues(nof_ues_), rnti_end(rnti_start + nof_ues * nof_cells)
{
  cells.reserve(nof_cells);
  for (unsigned i = 0, e = nof_cells; i < e; ++i) {
    cells.emplace_back(std::make_unique<cell_info>(128U));
  }
}

unsigned mac_test_mode_ue_repository::get_cell_index(du_ue_index_t ue_index) const
{
  unsigned cell_idx = static_cast<unsigned>(ue_index) / nof_ues;
  srsran_assert(cell_idx < cells.size(), "Invalid UE index {}", fmt::underlying(ue_index));
  return cell_idx;
}

unsigned mac_test_mode_ue_repository::get_cell_index(rnti_t rnti) const
{
  unsigned rnti_idx = static_cast<unsigned>(rnti) - static_cast<unsigned>(rnti_start);
  unsigned cell_idx = rnti_idx / nof_ues;
  srsran_assert(cell_idx < cells.size(), "Invalid RNTI {}", rnti);
  return cell_idx;
}

du_ue_index_t mac_test_mode_ue_repository::rnti_to_du_ue_idx(rnti_t rnti) const
{
  unsigned cell_idx = get_cell_index(rnti);
  auto     it       = cells[cell_idx]->rnti_to_ue_info_lookup.find(rnti);
  if (it == cells[cell_idx]->rnti_to_ue_info_lookup.end()) {
    return INVALID_DU_UE_INDEX;
  }
  return it->second.ue_idx;
}

const sched_ue_config_request& mac_test_mode_ue_repository::get_sched_ue_cfg_request(rnti_t rnti) const
{
  return cells[get_cell_index(rnti)]->rnti_to_ue_info_lookup.at(rnti).sched_ue_cfg_req;
}

const sched_ue_config_request* mac_test_mode_ue_repository::find_sched_ue_cfg_request(rnti_t rnti) const
{
  unsigned cell_idx = get_cell_index(rnti);
  auto     it       = cells[cell_idx]->rnti_to_ue_info_lookup.find(rnti);
  return it != cells[cell_idx]->rnti_to_ue_info_lookup.end() ? &it->second.sched_ue_cfg_req : nullptr;
}

bool mac_test_mode_ue_repository::is_msg4_rxed(rnti_t rnti) const
{
  unsigned cell_idx = get_cell_index(rnti);
  if (cells[cell_idx]->rnti_to_ue_info_lookup.count(rnti) > 0) {
    return cells[cell_idx]->rnti_to_ue_info_lookup.at(rnti).msg4_rx_flag;
  }
  return false;
}

void mac_test_mode_ue_repository::msg4_rxed(rnti_t rnti, bool msg4_rx_flag_)
{
  unsigned cell_idx = get_cell_index(rnti);
  if (cells[cell_idx]->rnti_to_ue_info_lookup.count(rnti) > 0) {
    cells[cell_idx]->rnti_to_ue_info_lookup.at(rnti).msg4_rx_flag = msg4_rx_flag_;
  }
}

void mac_test_mode_ue_repository::add_ue(rnti_t                         rnti,
                                         du_ue_index_t                  ue_idx,
                                         const sched_ue_config_request& sched_ue_cfg_req)
{
  if (not is_test_ue(rnti) or not is_test_ue(ue_idx)) {
    return;
  }
  unsigned rnti_idx = static_cast<unsigned>(rnti) - static_cast<unsigned>(rnti_start);
  srsran_assert(rnti_idx == static_cast<unsigned>(ue_idx),
                "rnti={}, ue={}: Invalid test mode UE identifiers",
                rnti,
                fmt::underlying(ue_idx));

  unsigned cell_idx = get_cell_index(ue_idx);

  // Dispatch creation of UE to du_cell thread.
  while (not cells[cell_idx]->pending_tasks.try_push([this, rnti, ue_idx, sched_ue_cfg_req]() {
    unsigned idx = get_cell_index(ue_idx);
    cells[idx]->rnti_to_ue_info_lookup[rnti] =
        test_ue_info{.ue_idx = ue_idx, .sched_ue_cfg_req = sched_ue_cfg_req, .msg4_rx_flag = false};
  })) {
    srslog::fetch_basic_logger("MAC").warning("Failed to add test mode UE. Retrying...");
  }
}

void mac_test_mode_ue_repository::remove_ue(rnti_t rnti)
{
  unsigned cell_idx = get_cell_index(rnti);
  while (not cells[cell_idx]->pending_tasks.try_push([this, rnti]() {
    unsigned idx = get_cell_index(rnti);
    if (cells[idx]->rnti_to_ue_info_lookup.count(rnti) > 0) {
      cells[idx]->rnti_to_ue_info_lookup.erase(rnti);
    }
  })) {
    srslog::fetch_basic_logger("MAC").warning("Failed to remove test mode UE. Retrying...");
  }
}

void mac_test_mode_ue_repository::process_pending_tasks(du_cell_index_t cell_index)
{
  unique_task task;
  while (cells[cell_index]->pending_tasks.try_pop(task)) {
    task();
  }
}
