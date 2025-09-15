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

#include "paging_message_handler.h"
#include "../du_processor/du_processor_repository.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/tac.h"

using namespace srsran;
using namespace srs_cu_cp;

paging_message_handler::paging_message_handler(du_processor_repository& dus_) :
  dus(dus_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void paging_message_handler::handle_paging_message(const cu_cp_paging_message& msg)
{
  // Forward paging message to all DU processors
  bool paging_sent = false;
  for (const auto& du_idx : dus.get_du_processor_indexes()) {
    paging_sent |= handle_du_paging_message(du_idx, msg);
  }

  if (not paging_sent) {
    logger.warning("No DU processor was able to handle the paging message");
  }
}

static bool is_tac_in_list(span<const cu_cp_tai_list_for_paging_item> tai_list, tac_t tac)
{
  return std::any_of(tai_list.begin(), tai_list.end(), [&tac](const auto& tai) { return tai.tai.tac == tac; });
}

/// Remove recommended cells that do not match any TAC in the TAI list or that do not belong to this DU.
static void remove_non_applicable_recommended_cells(cu_cp_paging_message& msg, const du_configuration_context& du_cfg)
{
  auto& recommended_cells = msg.assist_data_for_paging.value()
                                .assist_data_for_recommended_cells.value()
                                .recommended_cells_for_paging.recommended_cell_list;

  auto is_bad_recommended_cell = [&](const cu_cp_recommended_cell_item& recommended_cell) {
    auto cell_it = std::find_if(du_cfg.served_cells.begin(),
                                du_cfg.served_cells.end(),
                                [&recommended_cell](const auto& c) { return recommended_cell.ngran_cgi == c.cgi; });
    if (cell_it == du_cfg.served_cells.end()) {
      // Recommended cell not found for this DU.
      return true;
    }
    return not is_tac_in_list(msg.tai_list_for_paging, cell_it->tac);
  };

  recommended_cells.erase(std::remove_if(recommended_cells.begin(), recommended_cells.end(), is_bad_recommended_cell),
                          recommended_cells.end());
}

bool paging_message_handler::handle_du_paging_message(du_index_t du_index, const cu_cp_paging_message& msg_before)
{
  du_processor&                   du     = dus.get_du_processor(du_index);
  const du_configuration_context* du_cfg = du.get_context();
  if (du_cfg == nullptr) {
    // DU has not completed F1 Setup.
    return false;
  }

  // Recommended cells will be added to the original paging message.
  cu_cp_paging_message msg_filtered{msg_before};
  if (not msg_filtered.assist_data_for_paging.has_value()) {
    msg_filtered.assist_data_for_paging.emplace();
  }
  if (not msg_filtered.assist_data_for_paging.value().assist_data_for_recommended_cells.has_value()) {
    msg_filtered.assist_data_for_paging.value().assist_data_for_recommended_cells.emplace();
  }
  auto& recommended_cells = msg_filtered.assist_data_for_paging.value()
                                .assist_data_for_recommended_cells.value()
                                .recommended_cells_for_paging.recommended_cell_list;

  // Clear recommended cells not matching any TAC in the tai_list_for_paging or that do not belong to this DU.
  remove_non_applicable_recommended_cells(msg_filtered, *du_cfg);

  for (const du_cell_configuration& cell : du_cfg->served_cells) {
    // Check if cell already exists in the list of recommended.
    if (std::any_of(recommended_cells.begin(), recommended_cells.end(), [&cell](const auto& c) {
          return c.ngran_cgi == cell.cgi;
        })) {
      continue;
    }
    if (not is_tac_in_list(msg_filtered.tai_list_for_paging, cell.tac)) {
      continue;
    }

    // Setup recommended cell item to add in case it doesn't exist
    cu_cp_recommended_cell_item cell_item;
    cell_item.ngran_cgi = cell.cgi;
    recommended_cells.push_back(cell_item);
  }

  if (recommended_cells.empty()) {
    logger.info("du={}: No cells with matching TAC available at this DU", du_index);
    return false;
  }

  // Forward message to F1AP of the respective DU.
  du.get_f1ap_handler().get_f1ap_paging_manager().handle_paging(msg_filtered);

  return true;
}
