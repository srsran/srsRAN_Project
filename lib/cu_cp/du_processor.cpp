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
#include "rrc_asn1_helpers.h"

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

void du_processor::send_rrc_setup(const f1ap_initial_ul_rrc_msg& msg)
{
  f1ap_dl_rrc_msg dl_rrc_msg = {};

  // GNB DU UE F1AP ID
  dl_rrc_msg.msg->gnb_du_ue_f1_ap_id.value = msg.msg->gnb_du_ue_f1_ap_id.value;

  // GNB CU UE F1AP ID
  // TODO: set real GNB CU UE F1AP ID
  dl_rrc_msg.msg->gnb_cu_ue_f1_ap_id.value = 22;

  // fill rrc setup
  asn1::rrc_nr::dl_ccch_msg_s dl_ccch_msg = {};
  dl_ccch_msg.msg.set_c1().set_rrc_setup();
  asn1::rrc_nr::rrc_setup_s& rrc_setup = dl_ccch_msg.msg.c1().rrc_setup();
  fill_asn1_rrc_setup_msg(rrc_setup, msg);

  if (cfg.logger.debug.enabled()) {
    asn1::json_writer js;
    rrc_setup.to_json(js);
    cfg.logger.debug("Containerized RRCSetup: {}", js.to_string());
  }

  // pack DL CCCH msg
  byte_buffer   byte_buf{};
  asn1::bit_ref packed_dl_ccch_msg{byte_buf};
  dl_ccch_msg.pack(packed_dl_ccch_msg);

  dl_rrc_msg.msg->rrc_container.value.resize(byte_buf.length());
  std::copy(byte_buf.begin(), byte_buf.end(), dl_rrc_msg.msg->rrc_container.value.begin());

  // send to f1ap
  cfg.f1ap_rrc_msg_proc_handler->handle_dl_rrc_message_transfer(dl_rrc_msg);
}