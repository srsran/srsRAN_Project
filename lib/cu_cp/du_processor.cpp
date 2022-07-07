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
#include "f1c_asn1_helpers.h"
#include "srsgnb/f1_interface/cu/f1ap_cu_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

du_processor::du_processor(cu_cp_manager_config_t& cfg_) : logger(cfg_.logger), cfg(cfg_), ue_mng(cfg_)
{
  // create f1ap
  f1ap = create_f1ap(*cfg.f1c_notifier, f1ap_ev_notifier, ue_manager_f1ap_ev_notifier, *cfg.f1c_du_mgmt_notifier);
  f1ap_ev_notifier.connect(*this);
  ue_manager_f1ap_ev_notifier.connect(ue_mng);
}

void du_processor::handle_f1_setup_request(const f1_setup_request_message& msg)
{
  // TODO: add handling
  cfg.logger.debug("Received F1 setup request");

  // Reject request without served cells
  if (not msg.request->gnb_du_served_cells_list_present) {
    cfg.logger.error("Not handling F1 setup without served cells");
    send_f1_setup_failure(asn1::f1ap::cause_c::types::options::radio_network);
    return;
  }

  // Set DU context
  context.id = msg.request->gnb_du_id.value;
  if (msg.request->gnb_du_name_present) {
    context.name = msg.request->gnb_du_name.value.to_string();
  }

  for (const auto& served_cell : msg.request->gnb_du_served_cells_list.value) {
    const auto&     cell_item = served_cell.value().gnb_du_served_cells_item();
    du_cell_context du_cell;
    du_cell.cell_index = get_next_du_cell_index();
    du_cell.pci        = cell_item.served_cell_info.nrpci;
    du_cell.cgi        = cgi_from_asn1(cell_item.served_cell_info.nrcgi);

    if (not cell_item.gnb_du_sys_info_present) {
      cfg.logger.error("Not handling served cells without system information");
      send_f1_setup_failure(asn1::f1ap::cause_c::types::options::radio_network);
      return;
    }

    // Store and unpack system information
    du_cell.sys_info.packed_mib =
        byte_buffer(cell_item.gnb_du_sys_info.mib_msg.begin(), cell_item.gnb_du_sys_info.mib_msg.end());
    du_cell.sys_info.packed_sib1 =
        byte_buffer(cell_item.gnb_du_sys_info.sib1_msg.begin(), cell_item.gnb_du_sys_info.sib1_msg.end());

    // TODO: add unpacking

    // add cell to DU context
    du_cell_index_t cell_index = du_cell.cell_index;
    cell_db.emplace(cell_index, std::move(du_cell));
  }

  // send setup response
  send_f1_setup_response(context);
}

void du_processor::handle_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg)
{
  // Reject request without served cells
  if (not msg.msg->duto_currc_container_present) {
    logger.error("Not handling Initial UL RRC message transfer without DU to CU container");
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
  if (pcell_index == MAX_NOF_DU_CELLS) {
    logger.error("Could not find cell with cell_id={}", cgi.nci.packed);
    return;
  }

  ue_mng.handle_initial_ul_rrc_message_transfer(pcell_index, msg);
}

du_cell_index_t du_processor::find_cell(uint64_t packed_nr_cell_id)
{
  for (auto& cell : cell_db) {
    if (cell.cgi.nci.packed == packed_nr_cell_id) {
      return cell.cell_index;
    }
  }
  // this is never a valid index
  return MAX_NOF_DU_CELLS;
}

/// Sender for F1AP messages
void du_processor::send_f1_setup_response(const du_context& du_ctxt)
{
  f1_setup_response_message response;
  response.success = true;
  fill_asn1_f1_setup_response(response.response, cfg.name, cfg.rrc_version, cell_db);
  f1ap->handle_f1ap_setup_response(response);
}

void du_processor::send_f1_setup_failure(asn1::f1ap::cause_c::types::options cause)
{
  f1_setup_response_message response;
  response.success = false;
  response.failure->cause->set(cause);
  f1ap->handle_f1ap_setup_response(response);
}

du_cell_index_t du_processor::get_next_du_cell_index()
{
  du_cell_index_t new_index;
  do {
    new_index = int_to_du_cell_index(next_du_cell_index.fetch_add(1, std::memory_order_relaxed));
  } while (cell_db.contains(new_index));
  return new_index;
}
