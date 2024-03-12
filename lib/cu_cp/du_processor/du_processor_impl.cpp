/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_factory.h"
#include "srsran/ran/cause/f1ap_cause_converters.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/ran/nr_cgi_helpers.h"
#include "srsran/rrc/rrc_du_factory.h"

using namespace srsran;
using namespace srs_cu_cp;

du_processor_impl::du_processor_impl(const du_processor_config_t&        du_processor_config_,
                                     du_processor_cu_cp_notifier&        cu_cp_notifier_,
                                     f1ap_du_management_notifier&        f1ap_du_mgmt_notifier_,
                                     f1ap_message_notifier&              f1ap_notifier_,
                                     du_processor_e1ap_control_notifier& e1ap_ctrl_notifier_,
                                     du_processor_ngap_control_notifier& ngap_ctrl_notifier_,
                                     f1ap_ue_removal_notifier&           f1ap_cu_cp_notifier_,
                                     rrc_ue_nas_notifier&                rrc_ue_nas_pdu_notifier_,
                                     rrc_ue_control_notifier&            rrc_ue_ngap_ctrl_notifier_,
                                     rrc_du_measurement_config_notifier& rrc_du_cu_cp_notifier,
                                     du_processor_ue_task_scheduler&     task_sched_,
                                     du_processor_ue_manager&            ue_manager_,
                                     task_executor&                      ctrl_exec_) :
  cfg(du_processor_config_),
  cu_cp_notifier(cu_cp_notifier_),
  f1ap_du_mgmt_notifier(f1ap_du_mgmt_notifier_),
  f1ap_notifier(f1ap_notifier_),
  e1ap_ctrl_notifier(e1ap_ctrl_notifier_),
  ngap_ctrl_notifier(ngap_ctrl_notifier_),
  f1ap_cu_cp_notifier(f1ap_cu_cp_notifier_),
  rrc_ue_nas_pdu_notifier(rrc_ue_nas_pdu_notifier_),
  rrc_ue_ngap_ctrl_notifier(rrc_ue_ngap_ctrl_notifier_),
  task_sched(task_sched_),
  ue_manager(ue_manager_),
  f1ap_ev_notifier(*this)
{
  context.du_index = cfg.du_index;

  // create f1ap
  f1ap = create_f1ap(f1ap_notifier,
                     f1ap_ev_notifier,
                     f1ap_du_mgmt_notifier,
                     f1ap_cu_cp_notifier,
                     task_sched.get_timer_manager(),
                     ctrl_exec_);

  f1ap_ev_notifier.connect_du_processor(get_du_processor_f1ap_interface());
  f1ap_ue_context_notifier.connect_f1(f1ap->get_f1ap_ue_context_manager());

  // create RRC
  rrc_du_creation_message rrc_creation_msg(
      cfg.rrc_cfg, rrc_ue_ev_notifier, rrc_ue_nas_pdu_notifier, rrc_ue_ngap_ctrl_notifier, rrc_du_cu_cp_notifier);
  rrc = create_rrc_du(rrc_creation_msg);
  rrc_du_adapter.connect_rrc_du(rrc->get_rrc_du_cell_manager(), rrc->get_rrc_du_ue_repository());

  rrc_ue_ev_notifier.connect_du_processor(get_du_processor_rrc_ue_interface());

  routine_mng = std::make_unique<du_processor_routine_manager>(
      e1ap_ctrl_notifier, f1ap_ue_context_notifier, ue_manager, cfg.default_security_indication, logger);

  cu_cp_notifier.on_du_processor_created(context.du_index,
                                         f1ap->get_f1ap_ue_context_removal_handler(),
                                         f1ap->get_f1ap_statistics_handler(),
                                         rrc->get_rrc_ue_removal_handler(),
                                         rrc->get_rrc_du_statistics_handler());
}

du_setup_result du_processor_impl::handle_du_setup_request(const du_setup_request& request)
{
  du_setup_result res;

  // Set DU context
  context.id   = request.gnb_du_id;
  context.name = request.gnb_du_name;

  // Check if CU-CP is in a state to accept a new DU connection.
  if (not cfg.du_setup_notif->on_du_setup_request(request)) {
    res.result =
        du_setup_result::rejected{cause_misc_t::unspecified, "CU-CP is not in a state to accept a new DU connection"};
    return res;
  }

  // Forward serving cell list to RRC DU
  // TODO: How to handle missing optional freq and timing in meas timing config?
  if (!rrc_du_adapter.on_new_served_cell_list(request.gnb_du_served_cells_list)) {
    res.result =
        du_setup_result::rejected{f1ap_cause_transport_t::unspecified, "Could not establish served cell list in RRC"};
    return res;
  }

  for (const auto& served_cell : request.gnb_du_served_cells_list) {
    du_cell_context du_cell;
    du_cell.du_index   = context.du_index;
    du_cell.cell_index = get_next_du_cell_index();

    if (du_cell.cell_index == du_cell_index_t::invalid) {
      res.result = du_setup_result::rejected{cause_misc_t::ctrl_processing_overload,
                                             "Maximum number of DU cells supported in the CU-CP reached"};
      return res;
    }

    du_cell.cgi = served_cell.served_cell_info.nr_cgi;
    if (not config_helpers::is_valid(du_cell.cgi)) {
      res.result = du_setup_result::rejected{cause_protocol_t::semantic_error,
                                             fmt::format("Invalid CGI for cell {}", du_cell.cell_index)};
      return res;
    }

    du_cell.pci = served_cell.served_cell_info.nr_pci;

    if (not served_cell.served_cell_info.five_gs_tac.has_value()) {
      res.result = du_setup_result::rejected{cause_protocol_t::msg_not_compatible_with_receiver_state,
                                             fmt::format("Missing TAC for cell {}", du_cell.cell_index)};
      return res;
    }
    du_cell.tac = served_cell.served_cell_info.five_gs_tac.value();

    if (not served_cell.gnb_du_sys_info.has_value()) {
      res.result = du_setup_result::rejected{cause_protocol_t::semantic_error,
                                             fmt::format("Missing system information for cell {}", du_cell.cell_index)};
      return res;
    }
    // Store and unpack system information
    du_cell.sys_info.packed_mib  = served_cell.gnb_du_sys_info.value().mib_msg.copy();
    du_cell.sys_info.packed_sib1 = served_cell.gnb_du_sys_info.value().sib1_msg.copy();

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
    auto& cell = cell_db.at(cell_index);
    if (tac_to_nr_cgi.find(cell.tac) == tac_to_nr_cgi.end()) {
      tac_to_nr_cgi.emplace(cell.tac, std::vector<nr_cell_global_id_t>{cell.cgi});
    } else {
      tac_to_nr_cgi.at(cell.tac).push_back(cell.cgi);
    }
  }

  // connect paging f1ap paging adapter
  f1ap_paging_notifier.connect_f1(f1ap->get_f1ap_paging_manager());

  // Prepare DU response with accepted setup.
  auto& accepted              = res.result.emplace<du_setup_result::accepted>();
  accepted.gnb_cu_name        = cfg.name;
  accepted.gnb_cu_rrc_version = cfg.rrc_version;

  // Accept all cells
  accepted.cells_to_be_activ_list.resize(request.gnb_du_served_cells_list.size());
  for (unsigned i = 0; i != accepted.cells_to_be_activ_list.size(); ++i) {
    accepted.cells_to_be_activ_list[i].nr_cgi = request.gnb_du_served_cells_list[i].served_cell_info.nr_cgi;
    accepted.cells_to_be_activ_list[i].nr_pci = request.gnb_du_served_cells_list[i].served_cell_info.nr_pci;
  }

  return res;
}

ue_index_t du_processor_impl::allocate_new_ue_index()
{
  return ue_manager.add_ue(context.du_index);
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

du_cell_index_t du_processor_impl::get_next_du_cell_index()
{
  for (int du_cell_idx_int = du_cell_index_to_uint(du_cell_index_t::min); du_cell_idx_int < MAX_NOF_DU_CELLS;
       du_cell_idx_int++) {
    du_cell_index_t cell_idx = uint_to_du_cell_index(du_cell_idx_int);
    if (cell_db.find(cell_idx) == cell_db.end()) {
      return cell_idx;
    }
  }
  logger.warning("No DU cell index available");
  return du_cell_index_t::invalid;
}

bool du_processor_impl::create_rrc_ue(du_ue&                            ue,
                                      rnti_t                            c_rnti,
                                      const nr_cell_global_id_t&        cgi,
                                      byte_buffer                       du_to_cu_rrc_container,
                                      optional<rrc_ue_transfer_context> rrc_context)
{
  // Create RRC UE to F1AP adapter
  rrc_ue_f1ap_adapters.emplace(std::piecewise_construct,
                               std::forward_as_tuple(ue.get_ue_index()),
                               std::forward_as_tuple(f1ap->get_f1ap_rrc_message_handler(), ue.get_ue_index()));

  // Create new RRC UE entity
  rrc_ue_creation_message rrc_ue_create_msg{};
  rrc_ue_create_msg.ue_index              = ue.get_ue_index();
  rrc_ue_create_msg.c_rnti                = c_rnti;
  rrc_ue_create_msg.cell.cgi              = cgi;
  rrc_ue_create_msg.cell.tac              = cell_db.at(ue.get_pcell_index()).tac;
  rrc_ue_create_msg.cell.pci              = cell_db.at(ue.get_pcell_index()).pci;
  rrc_ue_create_msg.cell.bands            = cell_db.at(ue.get_pcell_index()).bands;
  rrc_ue_create_msg.f1ap_pdu_notifier     = &rrc_ue_f1ap_adapters.at(ue.get_ue_index());
  rrc_ue_create_msg.rrc_ue_cu_cp_notifier = &ue.get_rrc_ue_context_update_notifier();
  rrc_ue_create_msg.measurement_notifier  = &ue.get_rrc_ue_measurement_notifier();
  rrc_ue_create_msg.du_to_cu_container    = std::move(du_to_cu_rrc_container);
  rrc_ue_create_msg.ue_task_sched         = &ue.get_task_sched();
  rrc_ue_create_msg.rrc_context           = std::move(rrc_context);
  auto* rrc_ue = rrc_du_adapter.on_ue_creation_request(ue.get_up_resource_manager(), std::move(rrc_ue_create_msg));
  if (rrc_ue == nullptr) {
    logger.warning("Could not create RRC UE");
    return false;
  }

  // Create and connect DU Processor to RRC UE adapter
  rrc_ue_adapters[ue.get_ue_index()] = {};
  rrc_ue_adapters.at(ue.get_ue_index())
      .connect_rrc_ue(rrc_ue->get_rrc_ue_control_message_handler(), rrc_ue->get_rrc_ue_srb_handler());
  ue.set_rrc_ue_notifier(rrc_ue_adapters.at(ue.get_ue_index()));
  ue.set_rrc_ue_srb_notifier(rrc_ue_adapters.at(ue.get_ue_index()));

  // Notify CU-CP about the creation of the RRC UE
  cu_cp_notifier.on_rrc_ue_created(ue.get_ue_index(), *rrc_ue);

  return true;
}

ue_rrc_context_creation_response
du_processor_impl::handle_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& req)
{
  srsran_assert(req.ue_index != ue_index_t::invalid, "Invalid UE index");
  srsran_assert(req.c_rnti != rnti_t::INVALID_RNTI, "ue={}: Invalid C-RNTI", req.ue_index);
  srsran_assert(config_helpers::is_valid(req.cgi), "ue={}: Invalid CGI", req.ue_index);

  // Check that creation message is valid
  du_cell_index_t pcell_index = find_cell(req.cgi.nci);
  if (pcell_index == du_cell_index_t::invalid) {
    logger.warning("ue={}: Could not find cell with NCI={}", req.ue_index, req.cgi.nci);
    cu_cp_notifier.on_ue_removal_required(req.ue_index);
    return {};
  }
  const pci_t pci = cell_db.at(pcell_index).pci;

  // Create new UE RRC context
  du_ue* ue = ue_manager.set_ue_du_context(req.ue_index, context.id, pci, req.c_rnti);
  if (ue == nullptr) {
    logger.warning("ue={}: Could not create UE context", req.ue_index);
    return {};
  }
  ue->set_pcell_index(pcell_index);

  // Create RRC UE. If the DU-to-CU-RRC-Container is empty, the UE will be rejected.
  if (not create_rrc_ue(*ue, req.c_rnti, req.cgi, req.du_to_cu_rrc_container.copy(), std::move(req.prev_context))) {
    logger.warning("ue={}: Could not create RRC UE object", req.ue_index);
    return {};
  }
  rrc_ue_interface* rrc_ue           = rrc->find_ue(req.ue_index);
  f1ap_rrc_ue_adapters[req.ue_index] = {};
  f1ap_rrc_ue_adapters.at(req.ue_index)
      .connect_rrc_ue(rrc_ue->get_ul_ccch_pdu_handler(), rrc_ue->get_ul_dcch_pdu_handler());

  // Signal back to F1AP that the UE was successfully created.
  ue_rrc_context_creation_response response;
  response.f1ap_rrc_notifier = &f1ap_rrc_ue_adapters.at(req.ue_index);

  logger.info("ue={} c-rnti={}: UE created", ue->get_ue_index(), req.c_rnti);

  return response;
}

void du_processor_impl::handle_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& request)
{
  srsran_assert(request.ue_index != ue_index_t::invalid, "Invalid UE index", request.ue_index);

  du_ue* ue = ue_manager.find_du_ue(request.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: Dropping DU initiated UE context release request. UE does not exist", request.ue_index);
    return;
  }

  logger.debug("ue={}: Handling DU initiated UE context release request", request.ue_index);

  bool ngap_release_successful = false;

  // Schedule on UE task scheduler
  task_sched.schedule_async_task(
      request.ue_index,
      launch_async([this, request, ue, ngap_release_successful](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Notify NGAP to request a release from the AMF
        CORO_AWAIT_VALUE(ngap_release_successful,
                         ngap_ctrl_notifier.on_ue_context_release_request(
                             cu_cp_ue_context_release_request{request.ue_index,
                                                              ue->get_up_resource_manager().get_pdu_sessions(),
                                                              f1ap_to_ngap_cause(request.cause)}));
        if (!ngap_release_successful) {
          // Release UE from DU, if it doesn't exist in the NGAP
          logger.debug("ue={}: Releasing UE from DU. ReleaseRequest not sent to AMF", request.ue_index);
          CORO_AWAIT(handle_ue_context_release_command(
              cu_cp_ue_context_release_command{request.ue_index, ngap_cause_radio_network_t::user_inactivity}));
        }
        CORO_RETURN();
      }));
}

async_task<cu_cp_ue_context_release_complete>
du_processor_impl::handle_ue_context_release_command(const cu_cp_ue_context_release_command& cmd)
{
  du_ue* ue = ue_manager.find_du_ue(cmd.ue_index);
  if (ue == nullptr) {
    logger.warning("ue={}: Dropping UE context release command. UE does not exist", cmd.ue_index);
    return launch_async([](coro_context<async_task<cu_cp_ue_context_release_complete>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(cu_cp_ue_context_release_complete{});
    });
  }

  return routine_mng->start_ue_context_release_routine(cmd, cu_cp_notifier);
}

async_task<bool> du_processor_impl::handle_rrc_reestablishment_context_modification_required(ue_index_t ue_index)
{
  du_ue* ue = ue_manager.find_du_ue(ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", ue_index);

  return routine_mng->start_reestablishment_context_modification_routine(
      ue_index, ue->get_rrc_ue_notifier(), ue->get_up_resource_manager());
}

async_task<cu_cp_pdu_session_resource_setup_response>
du_processor_impl::handle_new_pdu_session_resource_setup_request(const cu_cp_pdu_session_resource_setup_request& msg)
{
  du_ue* ue = ue_manager.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", msg.ue_index);

  rrc_ue_interface* rrc_ue = rrc->find_ue(msg.ue_index);
  srsran_assert(rrc_ue != nullptr, "ue={}: Could not find RRC UE", msg.ue_index);

  return routine_mng->start_pdu_session_resource_setup_routine(
      msg,
      rrc_ue->get_rrc_ue_security_context().get_as_config(security::sec_domain::up),
      ue->get_rrc_ue_notifier(),
      ue->get_up_resource_manager());
}

async_task<cu_cp_pdu_session_resource_modify_response>
du_processor_impl::handle_new_pdu_session_resource_modify_request(const cu_cp_pdu_session_resource_modify_request& msg)
{
  du_ue* ue = ue_manager.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", msg.ue_index);

  return routine_mng->start_pdu_session_resource_modification_routine(
      msg, ue->get_rrc_ue_notifier(), ue->get_up_resource_manager());
}

async_task<cu_cp_pdu_session_resource_release_response>
du_processor_impl::handle_new_pdu_session_resource_release_command(
    const cu_cp_pdu_session_resource_release_command& msg)
{
  du_ue* ue = ue_manager.find_du_ue(msg.ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", msg.ue_index);

  return routine_mng->start_pdu_session_resource_release_routine(
      msg, ngap_ctrl_notifier, task_sched, ue->get_up_resource_manager());
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

    for (const auto& cgi : tac_to_nr_cgi.at(tai_list_item.tai.tac)) {
      // Setup recommended cell item to add in case it doesn't exist
      cu_cp_recommended_cell_item cell_item;
      cell_item.ngran_cgi = cgi;

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
              if (present_cell_item.ngran_cgi.nci == cgi.nci) {
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
  }

  // If not nr cgi for a tac from the paging message is found paging message is not forwarded to DU
  if (!nr_cgi_for_tac_found) {
    logger.info("du_index={}: No NR CGI for paging TACs available at this DU", context.du_index);
    return;
  }

  f1ap_paging_notifier.on_paging_message(msg);
}

void du_processor_impl::send_ngap_ue_context_release_request(ue_index_t ue_index, ngap_cause_t cause)
{
  du_ue* ue = ue_manager.find_du_ue(ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", ue_index);

  cu_cp_ue_context_release_request req;
  req.ue_index = ue_index;
  req.cause    = cause;

  // Add PDU Session IDs
  auto& up_resource_manager            = ue->get_up_resource_manager();
  req.pdu_session_res_list_cxt_rel_req = up_resource_manager.get_pdu_sessions();

  logger.debug("ue={}: Requesting UE context release with cause={}", req.ue_index, cause);

  // Schedule on UE task scheduler
  task_sched.schedule_async_task(ue_index, launch_async([this, req](coro_context<async_task<void>>& ctx) mutable {
                                   CORO_BEGIN(ctx);
                                   // Notify NGAP to request a release from the AMF
                                   CORO_AWAIT(ngap_ctrl_notifier.on_ue_context_release_request(req));
                                   CORO_RETURN();
                                 }));
}

void du_processor_impl::handle_inactivity_notification(const cu_cp_inactivity_notification& msg)
{
  if (msg.ue_inactive) {
    send_ngap_ue_context_release_request(msg.ue_index, ngap_cause_radio_network_t::user_inactivity);
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
    du_processor_f1ap_ue_context_notifier& target_du_f1ap_ue_ctxt_notif_,
    du_processor_ue_context_notifier&      target_du_processor_notifier_)
{
  du_ue* ue = ue_manager.find_du_ue(msg.source_ue_index);
  srsran_assert(ue != nullptr, "ue={}: Could not find DU UE", msg.source_ue_index);

  return routine_mng->start_inter_du_handover_routine(msg,
                                                      cu_cp_notifier,
                                                      target_du_f1ap_ue_ctxt_notif_,
                                                      get_du_processor_ue_context_notifier(),
                                                      target_du_processor_notifier_);
}

async_task<cu_cp_inter_ngran_node_n2_handover_response>
du_processor_impl::handle_inter_ngran_node_n2_handover_request(const cu_cp_inter_ngran_node_n2_handover_request& msg)
{
  return routine_mng->start_inter_ngran_node_n2_handover_routine(msg, ngap_ctrl_notifier);
}

async_task<ngap_handover_resource_allocation_response>
du_processor_impl::handle_ngap_handover_request(const ngap_handover_request& request)
{
  return routine_mng->start_inter_cu_handover_target_routine(request, cu_cp_notifier);
}

metrics_report::du_info du_processor_impl::handle_du_metrics_report_request() const
{
  metrics_report::du_info report;
  report.id = context.id;
  for (const auto& cell : cell_db) {
    report.cells.emplace_back();
    report.cells.back().cgi = cell.second.cgi;
    report.cells.back().pci = cell.second.pci;
  }
  return report;
}
