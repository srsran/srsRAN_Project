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
#include "adapters/pdcp_adapters.h"
#include "adapters/rrc_ue_adapters.h"
#include "f1c_asn1_helpers.h"
#include "srsgnb/f1_interface/cu/f1ap_cu_factory.h"
#include "srsgnb/pdcp/pdcp_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

du_processor::du_processor(const du_processor_config_t& cfg_) : cfg(cfg_), ue_mng(cfg_.logger)
{
  const size_t number_of_pending_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_UES; ++i) {
    ue_ctrl_loop.emplace(i, number_of_pending_procedures);
  }

  // create f1ap
  f1ap = create_f1ap(*cfg.f1c_notifier, f1ap_ev_notifier, *cfg.f1c_du_mgmt_notifier);
  f1ap_ev_notifier.connect_du_processor(*this);

  // create RRC
  rrc_entity_creation_message rrc_creation_msg(cfg.rrc_cfg, cfg.ngap_entity, rrc_ue_ev_notifier);
  rrc = create_rrc_entity(rrc_creation_msg);
  rrc_ue_ev_notifier.connect_du_processor(*this);
}

void du_processor::handle_f1_setup_request(const f1_setup_request_message& msg)
{
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
    f1ap->add_ue_index_to_context(int_to_f1ap_ue_id(msg.tmp_ue_id), INVALID_UE_INDEX);
    return;
  }

  // 2. Set parameters from initiating message
  ue_ctxt->pcell_index = msg.pcell_index;
  ue_ctxt->c_rnti      = msg.c_rnti;

  // 3. Create a UE task scheduler notifier.
  ue_ctxt->task_sched = std::make_unique<rrc_to_du_ue_task_scheduler>(ue_ctxt->ue_index, *this);

  // 4. Create new RRC entity
  ue_creation_message ue_create_msg{};
  ue_create_msg.ue_index = ue_ctxt->ue_index;
  ue_create_msg.c_rnti   = ue_ctxt->c_rnti;
  for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
    ue_create_msg.srbs[i] = ue_ctxt->srbs[i].rrc_tx_notifier.get();
  }
  ue_create_msg.du_to_cu_container = std::move(msg.du_to_cu_rrc_container);
  ue_create_msg.ue_task_sched      = ue_ctxt->task_sched.get();
  ue_ctxt->rrc                     = rrc->add_user(std::move(ue_create_msg));
  if (ue_ctxt->rrc == nullptr) {
    logger.error("Could not create RRC entity");
    f1ap->add_ue_index_to_context(int_to_f1ap_ue_id(msg.tmp_ue_id), INVALID_UE_INDEX);
    return;
  }

  logger.info("UE Created (ue_index={}, c-rnti={})", ue_ctxt->ue_index, ue_ctxt->c_rnti);
  f1ap->add_ue_index_to_context(int_to_f1ap_ue_id(msg.tmp_ue_id), ue_ctxt->ue_index);

  // 5. Create SRB0 bearer and notifier
  srb_creation_message srb0_msg{};
  srb0_msg.ue_index = ue_ctxt->ue_index;
  srb0_msg.srb_id   = srb_id_t::srb0;
  create_srb(srb0_msg);

  // 6. Determine LCID and payload
  lcid_t                                 srb_lcid      = LCID_SRB0;
  const asn1::unbounded_octstring<true>* rrc_container = &msg.rrc_container;
  if (msg.rrc_container_rrc_setup_complete.has_value()) {
    // RRC setup complete over SRB1
    srb_lcid      = LCID_SRB1;
    rrc_container = &msg.rrc_container_rrc_setup_complete.value();
  }

  // 7. Pass container to RRC
  ue_ctxt->srbs[srb_lcid].rx_notifier->on_new_rrc_message({*rrc_container});

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

  // 2. Notify upper layers about reception
  ue_ctxt->srbs[msg.srbid].rx_notifier->on_new_rrc_message({msg.rrc_container});
}

void du_processor::create_srb(const srb_creation_message& msg)
{
  ue_context* ue_ctxt = ue_mng.find_ue(msg.ue_index);
  srsgnb_assert(ue_ctxt != nullptr, "Could not find UE context");

  // create entry for SRB
  cu_srb_context& srb = ue_ctxt->srbs[msg.srb_id];

  // create adapter objects and PDCP bearer as needed
  if (msg.srb_id == srb_id_t::srb0) {
    // create direct connection with UE manager to RRC adapter
    srb.rx_notifier     = std::make_unique<du_processor_rrc_ue_adapter>(ue_ctxt->rrc->get_ul_ccch_pdu_handler());
    srb.rrc_tx_notifier = std::make_unique<rrc_ue_f1ap_adapter>(*f1ap, ue_ctxt->ue_index);

    // update notifier in RRC
    ue_ctxt->rrc->connect_srb_notifier(msg.srb_id, *ue_ctxt->srbs[msg.srb_id].rrc_tx_notifier.get());
    // TODO: update notifier in F1AP
  } else if (msg.srb_id <= srb_id_t::srb2) {
    // create PDCP context for this SRB
    srb.pdcp_context.emplace();

    // add adapter for PDCP to talk to F1AP (Tx) and RRC (Rx)
    srb.pdcp_context->pdcp_tx_notifier =
        std::make_unique<pdcp_du_processor_adapter>(*f1ap, ue_ctxt->ue_index, msg.srb_id);
    srb.pdcp_context->rrc_rx_notifier = std::make_unique<pdcp_rrc_ue_adapter>(ue_ctxt->rrc->get_ul_dcch_pdu_handler());

    // prepare PDCP creation message
    pdcp_entity_creation_message srb_pdcp{};
    srb_pdcp.ue_index    = msg.ue_index;
    srb_pdcp.lcid        = srb_id_to_lcid(msg.srb_id);
    srb_pdcp.config      = {}; // TODO: writer converter from ASN1 for msg.pdcp_cfg;
    srb_pdcp.tx_lower    = srb.pdcp_context->pdcp_tx_notifier.get();
    srb_pdcp.tx_upper_cn = nullptr; // TODO: add CN handler
    srb_pdcp.rx_upper_dn = srb.pdcp_context->rrc_rx_notifier.get();
    srb_pdcp.rx_upper_cn = nullptr; // TODO: add CN handler
    srb_pdcp.timers      = nullptr; // TODO: add timers

    // create PDCP entity
    srb.pdcp_context->pdcp_bearer = create_pdcp_entity(srb_pdcp);

    // created adapters between F1AP to PDCP (Rx) and RRC to PDCP (Tx)
    srb.rx_notifier = std::make_unique<f1ap_pdcp_adapter>(srb.pdcp_context->pdcp_bearer->get_rx_lower_interface());
    srb.rrc_tx_notifier =
        std::make_unique<rrc_ue_pdcp_adapter>(srb.pdcp_context->pdcp_bearer->get_tx_upper_data_interface());

    // update notifier in RRC
    ue_ctxt->rrc->connect_srb_notifier(msg.srb_id, *ue_ctxt->srbs[msg.srb_id].rrc_tx_notifier.get());
    // TODO: update notifier in F1AP
  } else {
    logger.error("Couldn't create SRB{}.", msg.srb_id);
  }
}

void du_processor::handle_ue_context_release_command(const ue_context_release_command_message& msg)
{
  f1ap_ue_context_release_command_message f1ap_msg = {};
  f1ap_msg.ue_index                                = msg.ue_index;
  f1ap_msg.cause.set_radio_network();

  ue_ctrl_loop[msg.ue_index].schedule([this, f1ap_msg](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    ue_index_t result_idx;

    CORO_AWAIT_VALUE(result_idx, f1ap->handle_ue_context_release_command(f1ap_msg));

    if (result_idx == f1ap_msg.ue_index) {
      logger.info("Removed UE(ue_index={}) from F1AP.", f1ap_msg.ue_index);
    }

    // TODO: remove UE from RRC

    CORO_RETURN();
  });

  // Remove UE from UE database
  logger.info("Removing UE (id={})", msg.ue_index);
  ue_mng.remove_ue(msg.ue_index);
}
