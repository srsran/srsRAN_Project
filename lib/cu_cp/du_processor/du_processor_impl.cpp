/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "du_processor_impl.h"
#include "../adapters/pdcp_adapters.h"
#include "../adapters/rrc_ue_adapters.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_factory.h"
#include "srsran/pdcp/pdcp_factory.h"
#include "srsran/ran/nr_cgi_helpers.h"
#include "srsran/ran/pci_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

du_processor_impl::du_processor_impl(const du_processor_config_t&        du_processor_config_,
                                     du_processor_cu_cp_notifier&        cu_cp_notifier_,
                                     f1ap_du_management_notifier&        f1ap_du_mgmt_notifier_,
                                     f1ap_message_notifier&              f1ap_notifier_,
                                     du_processor_e1ap_control_notifier& e1ap_ctrl_notifier_,
                                     du_processor_ngap_control_notifier& ngap_ctrl_notifier_,
                                     rrc_ue_nas_notifier&                rrc_ue_nas_pdu_notifier_,
                                     rrc_ue_control_notifier&            rrc_ue_ngap_ctrl_notifier_,
                                     rrc_ue_reestablishment_notifier&    rrc_ue_cu_cp_notifier_,
                                     du_processor_ue_task_scheduler&     task_sched_,
                                     du_processor_ue_manager&            ue_manager_,
                                     cell_meas_manager&                  cell_meas_mng_,
                                     task_executor&                      ctrl_exec_) :
  cfg(du_processor_config_),
  cu_cp_notifier(cu_cp_notifier_),
  f1ap_du_mgmt_notifier(f1ap_du_mgmt_notifier_),
  f1ap_notifier(f1ap_notifier_),
  e1ap_ctrl_notifier(e1ap_ctrl_notifier_),
  ngap_ctrl_notifier(ngap_ctrl_notifier_),
  rrc_ue_nas_pdu_notifier(rrc_ue_nas_pdu_notifier_),
  rrc_ue_ngap_ctrl_notifier(rrc_ue_ngap_ctrl_notifier_),
  rrc_ue_cu_cp_notifier(rrc_ue_cu_cp_notifier_),
  task_sched(task_sched_),
  ue_manager(ue_manager_),
  ctrl_exec(ctrl_exec_)
{
  context.du_index = cfg.du_index;

  // create f1ap
  f1ap = create_f1ap(f1ap_notifier, f1ap_ev_notifier, f1ap_du_mgmt_notifier, ctrl_exec_);
  f1ap_ev_notifier.connect_du_processor(get_du_processor_f1ap_interface());

  f1ap_ue_context_notifier.connect_f1(f1ap->get_f1ap_ue_context_manager());

  // create RRC
  rrc_du_creation_message rrc_creation_msg(cfg.rrc_cfg,
                                           rrc_ue_ev_notifier,
                                           rrc_ue_nas_pdu_notifier,
                                           rrc_ue_ngap_ctrl_notifier,
                                           rrc_ue_cu_cp_notifier,
                                           cell_meas_mng_);
  rrc = create_rrc_du(rrc_creation_msg);
  rrc_du_adapter.connect_rrc_du(rrc->get_rrc_du_cell_manager(), rrc->get_rrc_du_ue_repository());

  rrc_ue_ev_notifier.connect_du_processor(get_du_processor_rrc_ue_interface());

  routine_mng = std::make_unique<du_processor_routine_manager>(
      e1ap_ctrl_notifier, f1ap_ue_context_notifier, rrc_du_adapter, ue_manager, logger);
}

void du_processor_impl::handle_f1_setup_request(const f1ap_f1_setup_request& request)
{
  logger.debug("Received F1 setup request");

  // Reject request without served cells
  if (request.gnb_du_served_cells_list.size() == 0) {
    logger.error("Not handling F1 setup without served cells");
    send_f1_setup_failure(cause_t::radio_network);
    return;
  }

  // Set DU context
  context.id = request.gnb_du_id;
  if (request.gnb_du_name.has_value()) {
    context.name = request.gnb_du_name.value();
  }

  // Forward serving cell list to RRC DU
  // TODO: How to handle missing optional freq and timing in meas timing config?
  if (!rrc_du_adapter.on_new_served_cell_list(request.gnb_du_served_cells_list)) {
    send_f1_setup_failure(cause_t::protocol);
    return;
  }

  for (const auto& served_cell : request.gnb_du_served_cells_list) {
    du_cell_context du_cell;
    du_cell.du_index   = context.du_index;
    du_cell.cell_index = get_next_du_cell_index();

    if (du_cell.cell_index == du_cell_index_t::invalid) {
      logger.error("Not handling F1 setup, maximum number of DU cells reached");
      send_f1_setup_failure(cause_t::radio_network);
      return;
    }

    du_cell.cgi = served_cell.served_cell_info.nr_cgi;
    if (not srsran::config_helpers::is_valid(du_cell.cgi)) {
      logger.error("Not handling F1 setup, invalid CGI for cell {}", du_cell.cell_index);
      send_f1_setup_failure(cause_t::radio_network);
      return;
    }

    du_cell.pci = served_cell.served_cell_info.nr_pci;
    if (not srsran::config_helpers::is_valid(du_cell.pci)) {
      logger.error("Not handling F1 setup, invalid PCI for cell {}", du_cell.pci);
      send_f1_setup_failure(cause_t::radio_network);
      return;
    }

    if (not served_cell.served_cell_info.five_gs_tac.has_value()) {
      logger.error("Not handling F1 setup, missing TAC for cell {}", du_cell.cell_index);
      send_f1_setup_failure(cause_t::radio_network);
      return;
    } else {
      du_cell.tac = served_cell.served_cell_info.five_gs_tac.value();
    }

    if (not served_cell.gnb_du_sys_info.has_value()) {
      logger.error("Not handling served cells without system information");
      send_f1_setup_failure(cause_t::radio_network);
      return;
    } else {
      // Store and unpack system information
      du_cell.sys_info.packed_mib  = served_cell.gnb_du_sys_info.value().mib_msg.copy();
      du_cell.sys_info.packed_sib1 = served_cell.gnb_du_sys_info.value().sib1_msg.copy();
    }

    // Add band information.
    if (served_cell.served_cell_info.nr_mode_info.fdd.has_value()) {
      for (const auto& band : served_cell.served_cell_info.nr_mode_info.fdd.value().dl_nr_freq_info.freq_band_list_nr) {
        du_cell.bands.push_back(uint_to_nr_band(band.freq_band_ind_nr));
      }
    } else if (served_cell.served_cell_info.nr_mode_info.tdd.has_value()) {
      for (const auto& band : served_cell.served_cell_info.nr_mode_info.tdd.value().nr_freq_info.freq_band_list_nr) {
        du_cell.bands.push_back(uint_to_nr_band(band.freq_band_ind_nr));
      }
    }

    // TODO: add unpacking of sys_info

    // add cell to DU context
    du_cell_index_t cell_index = du_cell.cell_index;
    cell_db.emplace(cell_index, std::move(du_cell));

    // Add cell to lookup
    tac_to_nr_cgi.emplace(cell_db.at(cell_index).tac, cell_db.at(cell_index).cgi);
  }

  // send setup response
  send_f1_setup_response(context);

  // connect paging f1ap paging adapter
  f1ap_paging_notifier.connect_f1(f1ap->get_f1ap_paging_manager());
}

ue_index_t du_processor_impl::get_new_ue_index()
{
  return ue_manager.allocate_new_ue_index(context.du_index);
}

du_cell_index_t du_processor_impl::find_cell(uint64_t packed_nr_cell_id)
{
  for (auto& cell_pair : cell_db) {
    auto& cell = cell_pair.second;
    if (cell.cgi.nci == packed_nr_cell_id) {
      return cell.cell_index;
    }
  }
  return du_cell_index_t::invalid;
}

/// Sender for F1AP messages
void du_processor_impl::send_f1_setup_response(const du_processor_context& du_ctxt)
{
  f1ap_f1_setup_response response;
  response.success = true;

  // fill CU common info
  response.gnb_cu_name        = cfg.name;
  response.gnb_cu_rrc_version = cfg.rrc_version;

  // activate all DU cells
  for (const auto& du_cell_pair : cell_db) {
    const auto& du_cell = du_cell_pair.second;

    f1ap_cells_to_be_activ_list_item resp_cell;
    resp_cell.nr_cgi = du_cell.cgi;
    resp_cell.nr_pci = du_cell.pci;

    response.cells_to_be_activ_list.push_back(resp_cell);
  }

  f1ap->handle_f1_setup_response(response);
}

void du_processor_impl::send_f1_setup_failure(cause_t cause)
{
  f1ap_f1_setup_response response;
  response.success = false;
  response.cause   = cause;
  f1ap->handle_f1_setup_response(response);
}

du_cell_index_t du_processor_impl::get_next_du_cell_index()
{
  for (int du_cell_idx_int = du_cell_index_to_uint(du_cell_index_t::min); du_cell_idx_int < MAX_NOF_DU_CELLS;
       du_cell_idx_int++) {
    du_cell_index_t cell_idx = uint_to_du_cell_index(du_cell_idx_int);
    if (cell_db.find(cell_idx) == cell_db.end()) {
      return cell_idx;
    }
  }
  logger.error("No DU cell index available");
  return du_cell_index_t::invalid;
}

bool du_processor_impl::create_rrc_ue(du_ue&                     ue,
                                      rnti_t                     c_rnti,
                                      const nr_cell_global_id_t& cgi,
                                      byte_buffer                du_to_cu_rrc_container)
{
  // Create and connect RRC UE task schedulers
  rrc_ue_task_scheds.emplace(ue.get_ue_index(), rrc_to_du_ue_task_scheduler{ue.get_ue_index(), ctrl_exec});
  rrc_ue_task_scheds.at(ue.get_ue_index()).connect_du_processor(get_du_processor_ue_task_handler());

  // Set task schedulers
  ue.set_task_sched(rrc_ue_task_scheds.at(ue.get_ue_index()));

  // Create new RRC UE entity
  rrc_ue_creation_message rrc_ue_create_msg{};
  rrc_ue_create_msg.ue_index   = ue.get_ue_index();
  rrc_ue_create_msg.c_rnti     = c_rnti;
  rrc_ue_create_msg.cell.cgi   = cgi;
  rrc_ue_create_msg.cell.tac   = cell_db.at(ue.get_pcell_index()).tac;
  rrc_ue_create_msg.cell.pci   = cell_db.at(ue.get_pcell_index()).pci;
  rrc_ue_create_msg.cell.bands = cell_db.at(ue.get_pcell_index()).bands;

  for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
    ue.get_srbs()[int_to_srb_id(i)] = {};
  }
  rrc_ue_create_msg.du_to_cu_container = std::move(du_to_cu_rrc_container);
  rrc_ue_create_msg.ue_task_sched      = &ue.get_task_sched();
  auto* rrc_ue = rrc_du_adapter.on_ue_creation_request(ue.get_up_resource_manager(), std::move(rrc_ue_create_msg));
  if (rrc_ue == nullptr) {
    logger.error("Could not create RRC UE");
    return false;
  }

  // Create and connect DU Processor to RRC UE adapter
  rrc_ue_adapters[ue.get_ue_index()] = {};
  rrc_ue_adapters[ue.get_ue_index()].connect_rrc_ue(rrc_ue->get_rrc_ue_control_message_handler());
  ue.set_rrc_ue_notifier(rrc_ue_adapters.at(ue.get_ue_index()));

  // Notifiy CU-CP about the creation of the RRC UE
  cu_cp_notifier.on_rrc_ue_created(context.du_index, ue.get_ue_index(), *rrc_ue);

  // Create SRB0 bearer and notifier
  srb_creation_message srb0_msg{};
  srb0_msg.srb_id   = srb_id_t::srb0;
  srb0_msg.ue_index = ue.get_ue_index();
  create_srb(srb0_msg);

  return true;
}

ue_creation_complete_message du_processor_impl::handle_ue_creation_request(const cu_cp_ue_creation_message& msg)
{
  ue_creation_complete_message ue_creation_complete_msg = {};
  ue_creation_complete_msg.ue_index                     = ue_index_t::invalid;

  // Check that creation message is valid
  du_cell_index_t pcell_index = find_cell(msg.cgi.nci);
  if (pcell_index == du_cell_index_t::invalid) {
    logger.error("Could not find cell with cell_id={}", msg.cgi.nci);
    return ue_creation_complete_msg;
  }

  // Create new UE context
  du_ue* ue = ue_manager.add_ue(msg.ue_index, cell_db.at(pcell_index).pci, msg.c_rnti);
  if (ue == nullptr) {
    logger.error("Could not create UE context");
    return ue_creation_complete_msg;
  }

  // Set parameters from creation message
  ue->set_pcell_index(pcell_index);

  // Create RRC UE only if all RRC-related values are available already.
  if (!msg.du_to_cu_rrc_container.empty()) {
    if (create_rrc_ue(*ue, msg.c_rnti, msg.cgi, msg.du_to_cu_rrc_container.copy()) == false) {
      logger.error("Could not create RRC UE object");
      return ue_creation_complete_msg;
    }

    for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
      ue_creation_complete_msg.srbs[i] = ue->get_srbs().at(int_to_srb_id(i)).rx_notifier.get();
    }
  }

  logger.info("ue={} UE Created (c-rnti={})", ue->get_ue_index(), msg.c_rnti);

  ue_creation_complete_msg.ue_index = ue->get_ue_index();

  return ue_creation_complete_msg;
}

ue_update_complete_message du_processor_impl::handle_ue_update_request(const ue_update_message& msg)
{
  du_ue* ue = ue_manager.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "Could not find DU UE");

  ue_update_complete_message ue_update_complete_msg = {};

  // Create RRC object if it doesn't already exist.
  if (rrc_ue_adapters.find(ue->get_ue_index()) != rrc_ue_adapters.end()) {
    if (!msg.cell_group_cfg.empty() && msg.c_rnti != INVALID_RNTI) {
      if (!create_rrc_ue(*ue, msg.c_rnti, msg.cgi, msg.cell_group_cfg.copy())) {
        logger.error("Could not create RRC UE object");
        return ue_update_complete_msg;
      }

      for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
        ue_update_complete_msg.srbs[i] = ue->get_srbs().at(int_to_srb_id(i)).rx_notifier.get();
      }
    }
  }

  ue_update_complete_msg.ue_index = ue->get_ue_index();

  return ue_update_complete_msg;
}

void du_processor_impl::handle_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& request)
{
  du_ue* ue = ue_manager.find_du_ue(request.ue_index);
  srsran_assert(ue != nullptr, "Could not find DU UE");

  cu_cp_ue_context_release_request ue_context_release_request;
  ue_context_release_request.ue_index = request.ue_index;
  ue_context_release_request.cause    = request.cause;

  // Add PDU Session IDs
  auto& up_resource_manager                                   = ue->get_up_resource_manager();
  ue_context_release_request.pdu_session_res_list_cxt_rel_req = up_resource_manager.get_pdu_sessions();

  ngap_ctrl_notifier.on_ue_context_release_request(ue_context_release_request);
}

void du_processor_impl::create_srb(const srb_creation_message& msg)
{
  du_ue* ue = ue_manager.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "Could not find UE");

  // create entry for SRB
  ue->get_srbs()[msg.srb_id] = {};
  cu_srb_context& srb        = ue->get_srbs().at(msg.srb_id);

  // create adapter objects and PDCP bearer as needed
  if (msg.srb_id == srb_id_t::srb0) {
    // create direct connection with UE manager to RRC adapter
    srb.rx_notifier     = std::make_unique<f1ap_rrc_ue_adapter>(rrc->find_ue(msg.ue_index)->get_ul_ccch_pdu_handler());
    srb.rrc_tx_notifier = std::make_unique<rrc_ue_f1ap_pdu_adapter>(*f1ap, msg.ue_index);

    // update notifier in RRC
    rrc->find_ue(msg.ue_index)
        ->connect_srb_notifier(msg.srb_id, *ue->get_srbs().at(msg.srb_id).rrc_tx_notifier, nullptr, nullptr);
  } else if (msg.srb_id <= srb_id_t::srb2) {
    // create PDCP context for this SRB
    srb.pdcp_context.emplace();

    // add adapter for PDCP to talk to F1AP (Tx), RRC data (Rx) and RRC control (Tx/Rx)
    srb.pdcp_context->pdcp_tx_notifier =
        std::make_unique<pdcp_du_processor_adapter>(*f1ap, msg.ue_index, msg.srb_id, msg.old_ue_index);
    srb.pdcp_context->rrc_tx_control_notifier =
        std::make_unique<pdcp_tx_control_rrc_ue_adapter>(); // TODO: pass actual RRC handler
    srb.pdcp_context->rrc_rx_data_notifier =
        std::make_unique<pdcp_rrc_ue_adapter>(rrc->find_ue(msg.ue_index)->get_ul_dcch_pdu_handler());
    srb.pdcp_context->rrc_rx_control_notifier =
        std::make_unique<pdcp_rx_control_rrc_ue_adapter>(); // TODO: pass actual RRC handler

    // prepare PDCP creation message
    pdcp_entity_creation_message srb_pdcp{};
    srb_pdcp.ue_index    = ue_index_to_uint(msg.ue_index);
    srb_pdcp.rb_id       = msg.srb_id;
    srb_pdcp.config      = pdcp_make_default_srb_config(); // TODO: allow non-default PDCP SRB configs
    srb_pdcp.tx_lower    = srb.pdcp_context->pdcp_tx_notifier.get();
    srb_pdcp.tx_upper_cn = srb.pdcp_context->rrc_tx_control_notifier.get();
    srb_pdcp.rx_upper_dn = srb.pdcp_context->rrc_rx_data_notifier.get();
    srb_pdcp.rx_upper_cn = srb.pdcp_context->rrc_rx_control_notifier.get();
    srb_pdcp.timers      = timer_factory{timer_db, ctrl_exec};

    // create PDCP entity
    srb.pdcp_context->entity = create_pdcp_entity(srb_pdcp);

    // created adapters between F1AP to PDCP (Rx) and RRC to PDCP (Tx)
    srb.rx_notifier = std::make_unique<f1ap_pdcp_adapter>(srb.pdcp_context->entity->get_rx_lower_interface());
    srb.rrc_tx_notifier =
        std::make_unique<rrc_ue_pdcp_pdu_adapter>(srb.pdcp_context->entity->get_tx_upper_data_interface());

    srb.pdcp_context->rrc_tx_sec_notifier =
        std::make_unique<rrc_ue_pdcp_tx_security_adapter>(srb.pdcp_context->entity->get_tx_upper_control_interface());

    srb.pdcp_context->rrc_rx_sec_notifier =
        std::make_unique<rrc_ue_pdcp_rx_security_adapter>(srb.pdcp_context->entity->get_rx_upper_control_interface());

    // update notifier in RRC
    rrc_ue_interface* rrc_ue = rrc->find_ue(msg.ue_index);
    rrc_ue->connect_srb_notifier(msg.srb_id,
                                 *ue->get_srbs().at(msg.srb_id).rrc_tx_notifier,
                                 ue->get_srbs().at(msg.srb_id).pdcp_context->rrc_tx_sec_notifier.get(),
                                 ue->get_srbs().at(msg.srb_id).pdcp_context->rrc_rx_sec_notifier.get());

    // update notifier in F1AP
    f1ap->connect_srb_notifier(msg.ue_index, msg.srb_id, *ue->get_srbs().at(msg.srb_id).rx_notifier);
    if (msg.srb_id == srb_id_t::srb2) {
      security::sec_as_config sec_cfg = rrc_ue->get_rrc_ue_security_context().get_as_config(security::sec_domain::rrc);
      srb.pdcp_context->rrc_tx_sec_notifier->enable_security(security::truncate_config(sec_cfg));
      srb.pdcp_context->rrc_rx_sec_notifier->enable_security(security::truncate_config(sec_cfg));
    }
  } else {
    logger.error("Couldn't create SRB{}.", msg.srb_id);
  }
}

void du_processor_impl::handle_ue_context_release_command(const rrc_ue_context_release_command& cmd)
{
  du_ue* ue = ue_manager.find_du_ue(cmd.ue_index);
  srsran_assert(ue != nullptr, "Could not find DU UE");

  task_sched.schedule_async_task(cmd.ue_index,
                                 routine_mng->start_ue_context_release_routine(cmd, ue->get_up_resource_manager()));
}

async_task<bool> du_processor_impl::handle_rrc_reestablishment_context_modification_required(ue_index_t ue_index)
{
  du_ue* ue = ue_manager.find_du_ue(ue_index);
  srsran_assert(ue != nullptr, "ue={} Could not find DU UE", ue_index);

  return routine_mng->start_reestablishment_context_modification_routine(
      ue_index, ue->get_rrc_ue_notifier(), ue->get_up_resource_manager());
}

async_task<cu_cp_pdu_session_resource_setup_response>
du_processor_impl::handle_new_pdu_session_resource_setup_request(const cu_cp_pdu_session_resource_setup_request& msg)
{
  du_ue* ue = ue_manager.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "ue={} Could not find DU UE", msg.ue_index);

  rrc_ue_interface* rrc_ue = rrc->find_ue(msg.ue_index);
  srsran_assert(rrc_ue != nullptr, "ue={} Could not find RRC UE", msg.ue_index);

  return routine_mng->start_pdu_session_resource_setup_routine(
      msg,
      rrc_ue->get_rrc_ue_security_context().get_as_config(security::sec_domain::rrc),
      ue->get_rrc_ue_notifier(),
      ue->get_up_resource_manager());
}

async_task<cu_cp_pdu_session_resource_modify_response>
du_processor_impl::handle_new_pdu_session_resource_modify_request(const cu_cp_pdu_session_resource_modify_request& msg)
{
  du_ue* ue = ue_manager.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "ue={} Could not find DU UE", msg.ue_index);

  return routine_mng->start_pdu_session_resource_modification_routine(
      msg, ue->get_rrc_ue_notifier(), ue->get_up_resource_manager());
}

async_task<cu_cp_pdu_session_resource_release_response>
du_processor_impl::handle_new_pdu_session_resource_release_command(
    const cu_cp_pdu_session_resource_release_command& msg)
{
  du_ue* ue = ue_manager.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "Could not find DU UE");

  return routine_mng->start_pdu_session_resource_release_routine(
      msg, ngap_ctrl_notifier, task_sched, ue->get_up_resource_manager());
}

cu_cp_ue_context_release_complete
du_processor_impl::handle_new_ue_context_release_command(const cu_cp_ngap_ue_context_release_command& cmd)
{
  du_ue* ue = ue_manager.find_du_ue(cmd.ue_index);
  srsran_assert(ue != nullptr, "Could not find DU UE");

  cu_cp_ue_context_release_complete release_complete;
  release_complete.pdu_session_res_list_cxt_rel_cpl = ue->get_up_resource_manager().get_pdu_sessions();

  // Call RRC UE notifier to get the release context of the UE and add the location info to the UE context release
  // complete message
  rrc_ue_release_context release_context = ue->get_rrc_ue_notifier().get_rrc_ue_release_context();
  release_complete.user_location_info    = release_context.user_location_info;

  // Create release command from NGAP UE context release command
  rrc_ue_context_release_command release_command;
  release_command.ue_index        = cmd.ue_index;
  release_command.cause           = cmd.cause;
  release_command.rrc_release_pdu = release_context.rrc_release_pdu.copy();
  release_command.srb_id          = release_context.srb_id;

  handle_ue_context_release_command(release_command);

  return release_complete;
}

void du_processor_impl::handle_paging_message(cu_cp_paging_message& msg)
{
  // Add assist data for paging
  // This will go through all tai items in the paging message and add the related NR CGI to the assist data for paging
  // if it doesn't exist yet.
  // This way the F1AP will always receive messages with the assist data for paging set.

  bool nr_cgi_for_tac_found = false;

  for (const auto& tai_list_item : msg.tai_list_for_paging) {
    if (tac_to_nr_cgi.find(tai_list_item.tai.tac) == tac_to_nr_cgi.end()) {
      logger.debug("Could not find nr cgi for tac={}", tai_list_item.tai.tac);
      continue;
    }

    nr_cgi_for_tac_found = true;

    // Setup recommended cell item to add in case it doesn't exist
    cu_cp_recommended_cell_item cell_item;
    cell_item.ngran_cgi = tac_to_nr_cgi.at(tai_list_item.tai.tac);

    // Check if assist data for paging is already present
    if (msg.assist_data_for_paging.has_value()) {
      // Check if assist data for recommended cells is already present
      if (msg.assist_data_for_paging.value().assist_data_for_recommended_cells.has_value()) {
        // Check if recommended cell list already contains values
        if (!msg.assist_data_for_paging.value()
                 .assist_data_for_recommended_cells.value()
                 .recommended_cells_for_paging.recommended_cell_list.empty()) {
          // Check if NR CGI already present
          bool is_present = false;
          for (const auto& present_cell_item : msg.assist_data_for_paging.value()
                                                   .assist_data_for_recommended_cells.value()
                                                   .recommended_cells_for_paging.recommended_cell_list) {
            if (present_cell_item.ngran_cgi.nci == tac_to_nr_cgi.at(tai_list_item.tai.tac).nci) {
              is_present = true;
              continue;
            }
          }
          if (is_present) {
            // NR CGI for TAC is already present
            continue;
          }
        }

        // NR CGI for TAC is not present so we add it
        msg.assist_data_for_paging.value()
            .assist_data_for_recommended_cells.value()
            .recommended_cells_for_paging.recommended_cell_list.push_back(cell_item);
      } else {
        // Assist data for recommended cells is not present, we need to add it
        cu_cp_assist_data_for_recommended_cells assist_data_for_recommended_cells;
        assist_data_for_recommended_cells.recommended_cells_for_paging.recommended_cell_list.push_back(cell_item);

        msg.assist_data_for_paging.value().assist_data_for_recommended_cells = assist_data_for_recommended_cells;
      }
    } else {
      // Assist data for paging is not present, we need to add it
      cu_cp_assist_data_for_paging assist_data_for_paging;

      // Add assist data for recommended cells
      cu_cp_assist_data_for_recommended_cells assist_data_for_recommended_cells;
      // Add cell item
      assist_data_for_recommended_cells.recommended_cells_for_paging.recommended_cell_list.push_back(cell_item);

      assist_data_for_paging.assist_data_for_recommended_cells = assist_data_for_recommended_cells;

      msg.assist_data_for_paging = assist_data_for_paging;
    }
  }

  // If not nr cgi for a tac from the paging message is found paging message is not forwarded to DU
  if (!nr_cgi_for_tac_found) {
    logger.info("No NR CGI for paging TACs available at this DU du_index={}", context.du_index);
    return;
  }

  f1ap_paging_notifier.on_paging_message(msg);
}

void du_processor_impl::handle_inactivity_notification(const cu_cp_inactivity_notification& msg)
{
  du_ue* ue = ue_manager.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "Could not find DU UE");

  if (msg.ue_inactive) {
    cu_cp_ue_context_release_request req;
    req.ue_index = msg.ue_index;
    req.cause    = cause_t::radio_network;

    // Add PDU Session IDs
    auto& up_resource_manager            = ue->get_up_resource_manager();
    req.pdu_session_res_list_cxt_rel_req = up_resource_manager.get_pdu_sessions();

    ngap_ctrl_notifier.on_ue_context_release_request(req);
  } else {
    logger.debug("Inactivity notification level not supported");
  }
}

bool du_processor_impl::has_cell(pci_t pci)
{
  for (const auto& cell : cell_db) {
    if (cell.second.pci == pci) {
      return true;
    }
  }
  return false;
}

bool du_processor_impl::has_cell(nr_cell_global_id_t cgi)
{
  for (const auto& cell : cell_db) {
    if (cell.second.cgi == cgi) {
      return true;
    }
  }
  return false;
}

void du_processor_impl::remove_ue(ue_index_t ue_index)
{
  // Remove UE from RRC
  rrc_du_adapter.on_ue_context_release_command(ue_index);

  // Remove UE from UE database
  logger.info("Removing DU UE (id={})", ue_index);
  ue_manager.remove_du_ue(ue_index);
}

optional<nr_cell_global_id_t> du_processor_impl::get_cgi(pci_t pci)
{
  optional<nr_cell_global_id_t> cgi;
  for (const auto& cell : cell_db) {
    if (cell.second.pci == pci) {
      return cell.second.cgi;
    }
  }
  return cgi;
}

async_task<cu_cp_inter_du_handover_response> du_processor_impl::handle_inter_du_handover_request(
    const cu_cp_inter_du_handover_request& msg,
    du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notif_)
{
  du_ue* ue = ue_manager.find_du_ue(msg.source_ue_index);
  srsran_assert(ue != nullptr, "ue={} Could not find DU UE", msg.source_ue_index);

  return routine_mng->start_inter_du_handover_routine(
      msg, target_du_f1ap_ue_ctxt_notif_, ue->get_rrc_ue_notifier(), ue->get_up_resource_manager());
}

async_task<cu_cp_inter_ngran_node_n2_handover_response>
du_processor_impl::handle_inter_ngran_node_n2_handover_request(const cu_cp_inter_ngran_node_n2_handover_request& msg)
{
  return routine_mng->start_inter_ngran_node_n2_handover_routine(msg, ngap_ctrl_notifier);
}

async_task<ngap_handover_resource_allocation_response>
du_processor_impl::handle_ngap_handover_request(const ngap_handover_request& request)
{
  return routine_mng->start_inter_cu_handover_target_routine(request, ngap_ctrl_notifier);
}
