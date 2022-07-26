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
#include "adapters/rrc_adapters.h"
#include "f1c_asn1_helpers.h"
#include "srsgnb/f1_interface/cu/f1ap_cu_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

du_processor::du_processor(const du_processor_config_t& cfg_) : cfg(cfg_), ue_mng(cfg_.logger)
{
  // create f1ap
  f1ap = create_f1ap(*cfg.f1c_notifier, f1ap_ev_notifier, f1ap_ev_notifier, *cfg.f1c_du_mgmt_notifier);
  f1ap_ev_notifier.connect(*this);

  // Connect DU processor to F1AP adapter
  du_processor_ev_notifier.connect(*f1ap);

  // create RRC
  rrc_entity_creation_message rrc_creation_msg(cfg.rrc_cfg, cfg.ngap_entity, *this);
  rrc = create_rrc_entity(rrc_creation_msg);
}

void du_processor::handle_f1_setup_request(const f1_setup_request_message& msg)
{
  // TODO: add handling
  logger.debug("Received F1 setup request");

  // Reject request without served cells
  if (not msg.request->gnb_du_served_cells_list_present) {
    logger.error("Not handling F1 setup without served cells");
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
      logger.error("Not handling served cells without system information");
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

rrc_amf_connection_handler& du_processor::get_amf_connection_handler()
{
  return *rrc;
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

du_index_t du_processor::get_du_index()
{
  return context.du_index;
};

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

void du_processor::handle_initial_ul_rrc_message_transfer(const initial_ul_rrc_message& msg)
{
  // 1. Create new UE context
  ue_context* ue_ctxt = ue_mng.add_ue(msg.c_rnti);
  if (ue_ctxt == nullptr) {
    logger.error("Could not create UE context");
    du_processor_ev_notifier.on_ue_creation(msg.tmp_ue_id, INVALID_UE_INDEX);
    return;
  }

  // 2. Set parameters from initiating message
  ue_ctxt->pcell_index = msg.pcell_index;
  ue_ctxt->c_rnti      = msg.c_rnti;

  // 3. Create new RRC entity
  ue_ctxt->rrc = rrc->add_user(*ue_ctxt, msg.du_to_cu_rrc_container);
  if (ue_ctxt->rrc == nullptr) {
    logger.error("Could not create RRC entity");
    du_processor_ev_notifier.on_ue_creation(msg.tmp_ue_id, INVALID_UE_INDEX);
    return;
  }

  logger.info("UE Created (ue_index={}, c-rnti={})", ue_ctxt->ue_index, ue_ctxt->c_rnti);
  du_processor_ev_notifier.on_ue_creation(msg.tmp_ue_id, ue_ctxt->ue_index);

  // 4. Create SRB0 bearer and notifier
  srb_creation_message srb0_msg{};
  srb0_msg.ue_index = ue_ctxt->ue_index;
  srb0_msg.srb_id   = srb_id_t::srb0;
  create_srb(srb0_msg);

  // 5. Pass container to RRC
  if (msg.rrc_container_rrc_setup_complete.has_value()) {
    // check that SRB1 is present
    if (ue_ctxt->srbs.contains(LCID_SRB0)) {
      ue_ctxt->srbs[LCID_SRB1].rx_notifier->on_new_rrc_message(msg.rrc_container_rrc_setup_complete.value());
    } else {
      logger.error("SRB1 not present - dropping PDU");
    }
  } else {
    // pass UL-CCCH to RRC
    ue_ctxt->srbs[LCID_SRB0].rx_notifier->on_new_rrc_message(msg.rrc_container);
  }

  return;
}

void du_processor::handle_ul_rrc_message_transfer(const ul_rrc_message& msg)
{
  // 1. Find UE context
  ue_context* ue_ctxt = ue_mng.find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("Could not find UE context");
    return;
  }

  // 2. Pass to SRB accordingly
  if (ue_ctxt->srbs.contains(msg.srbid)) {
    ue_ctxt->srbs[msg.srbid].rx_notifier->on_new_rrc_message(msg.rrc_container);
  } else {
    logger.error("SRB{} not present - dropping PDU", msg.srbid);
  }
}

void du_processor::create_srb(const srb_creation_message& msg)
{
  ue_context* ue_ctxt = ue_mng.find_ue(msg.ue_index);
  srsgnb_assert(ue_ctxt != nullptr, "Could not find UE context");

  const lcid_t lcid = srb_id_to_lcid(msg.srb_id);
  srsgnb_assert(!ue_ctxt->srbs.contains(lcid), "SRB already present");

  ue_ctxt->srbs.emplace(lcid);
  cu_srb_context& srb = ue_ctxt->srbs[lcid];
  srb.lcid            = lcid;

  if (lcid == LCID_SRB0) {
    // create direct connection with UE manager to RRC adapter
    srb.rx_notifier = std::make_unique<rrc_ul_ccch_message_indicator>(*ue_ctxt->rrc->get_ul_ccch_pdu_handler());
    srb.tx_notifier = std::make_unique<du_processor_dl_message_indicator>(*f1ap, ue_ctxt->ue_index);
  } else {
    logger.error("Couldn't create notifier for SRB{}. Removing entry again.", lcid);
    ue_ctxt->srbs.erase(lcid);
  }
}