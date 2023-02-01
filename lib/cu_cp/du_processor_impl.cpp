/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_processor_impl.h"
#include "../lib/f1ap/common/asn1_helpers.h"
#include "adapters/pdcp_adapters.h"
#include "adapters/rrc_ue_adapters.h"
#include "helpers/f1c_asn1_helpers.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu_factory.h"
#include "srsgnb/pdcp/pdcp_factory.h"
#include "srsgnb/ran/nr_cgi_helpers.h"

using namespace srsgnb;
using namespace srs_cu_cp;

du_processor_impl::du_processor_impl(const du_processor_config_t         du_processor_config_,
                                     du_processor_cu_cp_notifier&        cu_cp_notifier_,
                                     f1c_du_management_notifier&         f1c_du_mgmt_notifier_,
                                     f1c_message_notifier&               f1c_notifier_,
                                     du_processor_e1ap_control_notifier& e1ap_ctrl_notifier_,
                                     rrc_ue_nas_notifier&                rrc_ue_nas_pdu_notifier_,
                                     rrc_ue_control_notifier&            rrc_ue_ngc_ctrl_notifier_,
                                     du_processor_ue_task_scheduler&     task_sched_,
                                     du_processor_ue_manager&            ue_manager_,
                                     task_executor&                      ctrl_exec_) :
  cfg(du_processor_config_),
  cu_cp_notifier(cu_cp_notifier_),
  f1c_du_mgmt_notifier(f1c_du_mgmt_notifier_),
  f1c_notifier(f1c_notifier_),
  e1ap_ctrl_notifier(e1ap_ctrl_notifier_),
  rrc_ue_nas_pdu_notifier(rrc_ue_nas_pdu_notifier_),
  rrc_ue_ngc_ctrl_notifier(rrc_ue_ngc_ctrl_notifier_),
  task_sched(task_sched_),
  ue_manager(ue_manager_)
{
  context.du_index = cfg.du_index;

  // create f1c
  f1c = create_f1ap(f1c_notifier, f1c_ev_notifier, f1c_du_mgmt_notifier, ctrl_exec_);
  f1c_ev_notifier.connect_du_processor(*this);

  f1c_ue_context_notifier.connect_f1(&f1c->get_f1c_ue_context_manager());

  // create RRC
  rrc_du_creation_message rrc_creation_msg(
      cfg.rrc_cfg, rrc_ue_ev_notifier, rrc_ue_nas_pdu_notifier, rrc_ue_ngc_ctrl_notifier);
  rrc = create_rrc_du(rrc_creation_msg);
  rrc_du_adapter.connect_rrc_du(&rrc->get_rrc_du_ue_repository());

  rrc_ue_ev_notifier.connect_du_processor(*this);

  routine_mng = std::make_unique<du_processor_routine_manager>(
      e1ap_ctrl_notifier, f1c_ue_context_notifier, rrc_du_adapter, ue_manager, logger);
}

void du_processor_impl::handle_f1_setup_request(const f1_setup_request_message& msg)
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

    if (du_cell.cell_index == du_cell_index_t::invalid) {
      logger.error("Not handling F1 setup, maximum number of DU cells reached");
      send_f1_setup_failure(asn1::f1ap::cause_c::types::options::radio_network);
      return;
    }

    du_cell.pci = cell_item.served_cell_info.nr_pci;
    du_cell.cgi = cgi_from_asn1(cell_item.served_cell_info.nr_cgi);
    if (not srsgnb::config_helpers::is_valid(du_cell.cgi)) {
      logger.error("Not handling F1 setup, invalid CGI for cell {}", du_cell.cell_index);
      send_f1_setup_failure(asn1::f1ap::cause_c::types::options::radio_network);
      return;
    }
    if (not cell_item.served_cell_info.five_gs_tac_present) {
      logger.error("Not handling F1 setup, missing TAC for cell {}", du_cell.cell_index);
      send_f1_setup_failure(asn1::f1ap::cause_c::types::options::radio_network);
      return;
    }
    du_cell.tac = cell_item.served_cell_info.five_gs_tac.to_number();

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

rrc_amf_connection_handler& du_processor_impl::get_rrc_amf_connection_handler()
{
  return *rrc;
}

du_cell_index_t du_processor_impl::find_cell(uint64_t packed_nr_cell_id)
{
  for (auto& cell : cell_db) {
    if (cell.cgi.nci.packed == packed_nr_cell_id) {
      return cell.cell_index;
    }
  }
  return du_cell_index_t::invalid;
}

/// Sender for F1C messages
void du_processor_impl::send_f1_setup_response(const du_processor_context& du_ctxt)
{
  f1_setup_response_message response;
  response.success = true;
  fill_asn1_f1_setup_response(response.response, cfg.name, cfg.rrc_version, cell_db);
  f1c->handle_f1_setup_response(response);
}

void du_processor_impl::send_f1_setup_failure(asn1::f1ap::cause_c::types::options cause)
{
  f1_setup_response_message response;
  response.success = false;
  response.failure->cause->set(cause);
  f1c->handle_f1_setup_response(response);
}

du_cell_index_t du_processor_impl::get_next_du_cell_index()
{
  for (int du_cell_idx_int = du_cell_index_to_uint(du_cell_index_t::min); du_cell_idx_int < MAX_NOF_DU_CELLS;
       du_cell_idx_int++) {
    du_cell_index_t cell_idx = uint_to_du_cell_index(du_cell_idx_int);
    if (!cell_db.contains(cell_idx)) {
      return cell_idx;
    }
  }
  logger.error("No DU cell index available");
  return du_cell_index_t::invalid;
}

ue_creation_complete_message du_processor_impl::handle_ue_creation_request(const ue_creation_message& msg)
{
  ue_creation_complete_message ue_creation_complete_msg = {};
  ue_creation_complete_msg.ue_index                     = ue_index_t::invalid;

  // Check that creation message is valid
  du_cell_index_t pcell_index = find_cell(msg.cgi.nci.packed);
  if (pcell_index == du_cell_index_t::invalid) {
    logger.error("Could not find cell with cell_id={}", msg.cgi.nci.packed);
    return ue_creation_complete_msg;
  }

  // Create new UE context
  ue_context* ue_ctxt = ue_manager.add_ue(msg.c_rnti);
  if (ue_ctxt == nullptr) {
    logger.error("Could not create UE context");
    return ue_creation_complete_msg;
  }

  // Set parameters from creation message
  ue_ctxt->pcell_index = pcell_index;
  ue_ctxt->c_rnti      = msg.c_rnti;

  // Create a UE task scheduler notifier.
  ue_ctxt->task_sched = std::make_unique<rrc_to_du_ue_task_scheduler>(ue_ctxt->ue_index, *this);

  // Create new RRC UE entity
  rrc_ue_creation_message rrc_ue_create_msg{};
  rrc_ue_create_msg.ue_index = ue_ctxt->ue_index;
  rrc_ue_create_msg.c_rnti   = msg.c_rnti;
  rrc_ue_create_msg.cell.cgi = msg.cgi;
  rrc_ue_create_msg.cell.tac = cell_db[pcell_index].tac;
  for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
    ue_ctxt->srbs.emplace(int_to_srb_id(i));
    rrc_ue_create_msg.srbs[i].pdu_notifier = ue_ctxt->srbs[int_to_srb_id(i)].rrc_tx_notifier.get();
  }
  rrc_ue_create_msg.du_to_cu_container = std::move(msg.du_to_cu_rrc_container);
  rrc_ue_create_msg.ue_task_sched      = ue_ctxt->task_sched.get();
  auto* rrc_ue                         = rrc_du_adapter.on_ue_creation_request(std::move(rrc_ue_create_msg));
  if (rrc_ue == nullptr) {
    logger.error("Could not create RRC UE");
    return ue_creation_complete_msg;
  }

  ue_ctxt->rrc_ue_notifier =
      std::make_unique<du_processor_rrc_ue_adapter>(rrc_ue->get_rrc_ue_control_message_handler());

  // Notifiy CU-CP about the creation of the RRC UE
  cu_cp_notifier.on_rrc_ue_created(context.du_index, ue_ctxt->ue_index, rrc_ue);

  // Create SRB0 bearer and notifier
  srb_creation_message srb0_msg{};
  srb0_msg.srb_id   = srb_id_t::srb0;
  srb0_msg.ue_index = ue_ctxt->ue_index;
  create_srb(srb0_msg);

  logger.info("UE Created (ue_index={}, c-rnti={})", ue_ctxt->ue_index, msg.c_rnti);

  ue_creation_complete_msg.ue_index = ue_ctxt->ue_index;
  for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
    ue_creation_complete_msg.srbs[i] = ue_ctxt->srbs[int_to_srb_id(i)].rx_notifier.get();
  }

  return ue_creation_complete_msg;
}

void du_processor_impl::create_srb(const srb_creation_message& msg)
{
  ue_context* ue_ctxt = ue_manager.find_ue(msg.ue_index);
  srsgnb_assert(ue_ctxt != nullptr, "Could not find UE context");

  // create entry for SRB
  ue_ctxt->srbs.emplace(msg.srb_id);
  cu_srb_context& srb = ue_ctxt->srbs[msg.srb_id];

  // create adapter objects and PDCP bearer as needed
  if (msg.srb_id == srb_id_t::srb0) {
    // create direct connection with UE manager to RRC adapter
    srb.rx_notifier     = std::make_unique<f1c_rrc_ue_adapter>(rrc->find_ue(msg.ue_index)->get_ul_ccch_pdu_handler());
    srb.rrc_tx_notifier = std::make_unique<rrc_ue_f1ap_pdu_adapter>(*f1c, msg.ue_index);

    // update notifier in RRC
    rrc->find_ue(msg.ue_index)
        ->connect_srb_notifier(msg.srb_id, *ue_ctxt->srbs[msg.srb_id].rrc_tx_notifier, nullptr, nullptr);
  } else if (msg.srb_id <= srb_id_t::srb2) {
    // create PDCP context for this SRB
    srb.pdcp_context.emplace();

    // add adapter for PDCP to talk to F1C (Tx), RRC data (Rx) and RRC control (Tx/Rx)
    srb.pdcp_context->pdcp_tx_notifier = std::make_unique<pdcp_du_processor_adapter>(*f1c, msg.ue_index, msg.srb_id);
    srb.pdcp_context->rrc_tx_control_notifier =
        std::make_unique<pdcp_tx_control_rrc_ue_adapter>(); // TODO: pass actual RRC handler
    srb.pdcp_context->rrc_rx_data_notifier =
        std::make_unique<pdcp_rrc_ue_adapter>(rrc->find_ue(msg.ue_index)->get_ul_dcch_pdu_handler());
    srb.pdcp_context->rrc_rx_control_notifier =
        std::make_unique<pdcp_rx_control_rrc_ue_adapter>(); // TODO: pass actual RRC handler

    // prepare PDCP creation message
    pdcp_entity_creation_message srb_pdcp{};
    srb_pdcp.ue_index           = ue_index_to_uint(msg.ue_index);
    srb_pdcp.rb_id              = msg.srb_id;
    srb_pdcp.config             = pdcp_make_default_srb_config(); // TODO: allow non-default PDCP SRB configs
    srb_pdcp.config.tx.rb_type  = pdcp_rb_type::srb;
    srb_pdcp.config.tx.rlc_mode = pdcp_rlc_mode::am;
    srb_pdcp.config.rx.rb_type  = pdcp_rb_type::srb;
    srb_pdcp.config.rx.rlc_mode = pdcp_rlc_mode::am;
    srb_pdcp.tx_lower           = srb.pdcp_context->pdcp_tx_notifier.get();
    srb_pdcp.tx_upper_cn        = srb.pdcp_context->rrc_tx_control_notifier.get();
    srb_pdcp.rx_upper_dn        = srb.pdcp_context->rrc_rx_data_notifier.get();
    srb_pdcp.rx_upper_cn        = srb.pdcp_context->rrc_rx_control_notifier.get();
    srb_pdcp.timers             = &timer_db;

    // create PDCP entity
    srb.pdcp_context->pdcp_bearer = create_pdcp_entity(srb_pdcp);

    // created adapters between F1C to PDCP (Rx) and RRC to PDCP (Tx)
    srb.rx_notifier = std::make_unique<f1c_pdcp_adapter>(srb.pdcp_context->pdcp_bearer->get_rx_lower_interface());
    srb.rrc_tx_notifier =
        std::make_unique<rrc_ue_pdcp_pdu_adapter>(srb.pdcp_context->pdcp_bearer->get_tx_upper_data_interface());

    srb.pdcp_context->rrc_tx_sec_notifier = std::make_unique<rrc_ue_pdcp_tx_security_adapter>(
        srb.pdcp_context->pdcp_bearer->get_tx_upper_control_interface());

    srb.pdcp_context->rrc_rx_sec_notifier = std::make_unique<rrc_ue_pdcp_rx_security_adapter>(
        srb.pdcp_context->pdcp_bearer->get_rx_upper_control_interface());

    // update notifier in RRC
    rrc->find_ue(msg.ue_index)
        ->connect_srb_notifier(msg.srb_id,
                               *ue_ctxt->srbs[msg.srb_id].rrc_tx_notifier,
                               ue_ctxt->srbs[msg.srb_id].pdcp_context->rrc_tx_sec_notifier.get(),
                               ue_ctxt->srbs[msg.srb_id].pdcp_context->rrc_rx_sec_notifier.get());

    // update notifier in F1C
    f1c->connect_srb_notifier(msg.ue_index, msg.srb_id, *ue_ctxt->srbs[msg.srb_id].rx_notifier);
  } else {
    logger.error("Couldn't create SRB{}.", msg.srb_id);
  }
}

void du_processor_impl::handle_ue_context_release_command(const cu_cp_ue_context_release_command& cmd)
{
  task_sched.schedule_async_task(cmd.ue_index, routine_mng->start_ue_context_release_routine(cmd));
}

async_task<cu_cp_pdu_session_resource_setup_response>
du_processor_impl::handle_new_pdu_session_resource_setup_request(const cu_cp_pdu_session_resource_setup_request& msg)
{
  ue_context* ue_ctxt = ue_manager.find_ue(msg.ue_index);
  srsgnb_assert(ue_ctxt != nullptr, "Could not find UE context");

  return routine_mng->start_pdu_session_resource_setup_routine(msg,
                                                               rrc->find_ue(msg.ue_index)->get_rrc_ue_secutity_config(),
                                                               *ue_ctxt->rrc_ue_notifier.get(),
                                                               rrc->find_ue(msg.ue_index)->get_rrc_ue_drb_manager());
}

void du_processor_impl::handle_new_ue_context_release_command(const cu_cp_ue_context_release_command& cmd)
{
  ue_context* ue_ctxt = ue_manager.find_ue(cmd.ue_index);
  srsgnb_assert(ue_ctxt != nullptr, "Could not find UE context");

  // Call RRC UE notifier to release the UE
  ue_ctxt->rrc_ue_notifier->on_rrc_ue_release();

  handle_ue_context_release_command(cmd);
}