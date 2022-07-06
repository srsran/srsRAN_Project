/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_processor.h"
#include "../lib/f1_interface/common/asn1_helpers.h"

using namespace srsgnb;
using namespace srs_cu_cp;

du_processor::du_processor(cu_cp_manager_config_t& cfg_) : logger(cfg_.logger), cfg(cfg_), ue_mng(cfg_) {}

void du_processor::handle_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg)
{
  // Reject request without served cells
  if (not msg.msg->duto_currc_container_present) {
    cfg.logger.error("Not handling Initial UL RRC message transfer without DU to CU container");
    /// Assume the DU can't serve the UE. Ignoring the message.
    return;
  }

  nr_cell_global_identity cgi = cgi_from_asn1(msg.msg->nrcgi.value);

  logger.info("Received Initial UL RRC message transfer nr_cgi={}, crnti={}", cgi.nci.packed, msg.msg->c_rnti.value);
  logger.debug("plmn={}", cgi.plmn);

  if (msg.msg->sul_access_ind_present) {
    logger.debug("Ignoring SUL access indicator");
  }

  du_cell_index_t pcell_index = find_cell(cgi.nci.packed);
  ue_mng.handle_initial_ul_rrc_message_transfer(pcell_index, msg);
}

du_cell_index_t du_processor::find_cell(uint64_t packed_nr_cell_id)
{
  for (auto& cell : cell_db) {
    if (cell.cgi.nci.packed == packed_nr_cell_id) {
      return cell.cell_index;
    }
  }
  return MIN_DU_CELL_INDEX;
}
