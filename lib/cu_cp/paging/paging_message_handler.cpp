/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "paging_message_handler.h"
#include "../du_processor/du_processor_repository.h"
#include "srsran/cu_cp/cu_cp_types.h"

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
  for (unsigned i = 0; i != dus.get_nof_dus(); ++i) {
    paging_sent |= handle_du_paging_message(uint_to_du_index(i), msg);
  }

  if (not paging_sent) {
    logger.warning("No DU processor was able to handle the paging message");
  }
}

bool paging_message_handler::handle_du_paging_message(du_index_t du_index, const cu_cp_paging_message& msg_before)
{
  du_processor&                   du     = dus.get_du_processor(du_index);
  const du_configuration_context& du_cfg = du.get_context();

  // Recommended cells will be added to the original paging message.
  cu_cp_paging_message msg_expanded{msg_before};
  if (not msg_expanded.assist_data_for_paging.has_value()) {
    msg_expanded.assist_data_for_paging.emplace();
  }
  if (not msg_expanded.assist_data_for_paging.value().assist_data_for_recommended_cells.has_value()) {
    msg_expanded.assist_data_for_paging.value().assist_data_for_recommended_cells.emplace();
  }
  auto& recommended_cells = msg_expanded.assist_data_for_paging.value()
                                .assist_data_for_recommended_cells.value()
                                .recommended_cells_for_paging.recommended_cell_list;

  for (const auto& tai_list_item : msg_expanded.tai_list_for_paging) {
    for (const du_cell_configuration& cell : du_cfg.served_cells) {
      if (cell.tac != tai_list_item.tai.tac) {
        // Only add cells with matching TAC.
        continue;
      }

      // Check if cell already exists in the list of recommended.
      for (auto& recommended_cell : recommended_cells) {
        if (recommended_cell.ngran_cgi == cell.cgi) {
          // NR CGI for TAC is already present. Continue with next cell.
          continue;
          break;
        }
      }

      // Setup recommended cell item to add in case it doesn't exist
      cu_cp_recommended_cell_item cell_item;
      cell_item.ngran_cgi = cell.cgi;
      recommended_cells.push_back(cell_item);
    }
  }

  if (recommended_cells.empty()) {
    logger.info("du={}: No cells with matching TAC available at this DU", du_index);
    return false;
  }

  // Forward message to F1AP of the respective DU.
  du.get_f1ap_handler().get_f1ap_paging_manager().handle_paging(msg_expanded);

  return true;
}
