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
#include "adapters/du_processor_adapters.h"
#include "f1c_asn1_helpers.h"
#include "srsgnb/f1_interface/cu/f1ap_cu_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

du_processor::du_processor(cu_cp_manager_config_t& cfg_) : logger(cfg_.logger), cfg(cfg_), ue_mng(cfg_)
{
  // create f1ap
  f1ap = create_f1ap(*cfg.f1c_notifier, f1ap_ev_notifier, f1ap_ev_notifier, *cfg.f1c_du_mgmt_notifier);
  f1ap_ev_notifier.connect(*this);

  // create RRC
  rrc_entity_creation_message rrc_creation_msg(cfg.rrc_cfg);
  rrc = create_rrc_entity(rrc_creation_msg);
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

du_cell_index_t du_processor::find_cell(uint64_t packed_nr_cell_id)
{
  for (auto& cell : cell_db) {
    if (cell.cgi.nci.packed == packed_nr_cell_id) {
      return cell.cell_index;
    }
  }
  return INVALID_DU_CELL_INDEX;
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
  for (int du_cell_idx_int = MIN_DU_CELL_INDEX; du_cell_idx_int < MAX_NOF_DUS; du_cell_idx_int++) {
    du_cell_index_t cell_idx = int_to_du_cell_index(du_cell_idx_int);
    if (!cell_db.contains(cell_idx)) {
      return cell_idx;
    }
  }
  logger.error("No DU cell index available");
  return INVALID_DU_CELL_INDEX;
}

ue_index_t du_processor::handle_initial_ul_rrc_message_transfer(const initial_ul_rrc_message& msg)
{
  // 1. Create new UE context
  ue_context* ue_ctxt = ue_mng.add_ue(msg.c_rnti);
  if (ue_ctxt == nullptr) {
    logger.error("Could not create UE context");
    return INVALID_UE_INDEX;
  }

  // 2. Set parameters from initiating message
  ue_ctxt->pcell_index            = msg.pcell_index;
  ue_ctxt->c_rnti                 = msg.c_rnti;
  const auto& du_to_cu_rrc_cont   = msg.du_to_cu_rrc_container;
  ue_ctxt->du_to_cu_rrc_container = byte_buffer{du_to_cu_rrc_cont.begin(), du_to_cu_rrc_cont.end()};

  // 3. Create new RRC entity
  ue_ctxt->rrc = rrc->add_user(*ue_ctxt);
  if (ue_ctxt->rrc == nullptr) {
    logger.error("Could not create RRC entity");
    return INVALID_UE_INDEX;
  }

  // 4. Create SRB0 bearer and notifier
  create_srb0(*ue_ctxt);

  // 5. Pass container to RRC
  if (msg.rrc_container_rrc_setup_complete.has_value()) {
    // check that SRB1 is present
    if (ue_ctxt->srbs.contains(LCID_SRB0)) {
      ue_ctxt->srbs[LCID_SRB1].rx_notifier->on_new_rrc_message(msg.rrc_container_rrc_setup_complete.value());
    } else {
      cfg.logger.error("SRB1 not present - dropping PDU");
    }
  } else {
    // pass UL-CCCH to RRC
    ue_ctxt->srbs[LCID_SRB0].rx_notifier->on_new_rrc_message(msg.rrc_container);
  }

  logger.info("UE Created (ue_index={}, c-rnti={})", ue_ctxt->ue_index, ue_ctxt->c_rnti);

  return ue_ctxt->ue_index;
}

void du_processor::handle_ul_rrc_message_transfer(const ul_rrc_message& msg)
{
  // 1. Find UE context
  ue_context* ue_ctxt = ue_mng.find_ue(msg.ue_idx);
  if (ue_ctxt == nullptr) {
    logger.error("Could not find UE context");
    return;
  }

  // 2. Pass to SRB accordingly
  if (ue_ctxt->srbs.contains(msg.srbid)) {
    ue_ctxt->srbs[msg.srbid].rx_notifier->on_new_rrc_message(msg.rrc_container);
  } else {
    cfg.logger.error("SR {} not present - dropping PDU", msg.srbid);
  }
}

void du_processor::create_srb0(ue_context& ue_ctxt)
{
  ue_ctxt.srbs.emplace(LCID_SRB0);
  cu_srb_context& srb0 = ue_ctxt.srbs[LCID_SRB0];
  srb0.lcid            = LCID_SRB0;

  // create UE manager to RRC adapter
  srb0.rx_notifier = std::make_unique<rrc_ul_ccch_adapter>(*ue_ctxt.rrc->get_ul_ccch_pdu_handler());
}