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

#include "ngap_impl.h"
#include "log_helpers.h"
#include "ngap_asn1_helpers.h"
#include "ngap_asn1_utils.h"
#include "ngap_error_indication_helper.h"
#include "procedures/ng_reset_procedure.h"
#include "procedures/ng_setup_procedure.h"
#include "procedures/ngap_dl_nas_message_transfer_procedure.h"
#include "procedures/ngap_handover_preparation_procedure.h"
#include "procedures/ngap_handover_resource_allocation_procedure.h"
#include "procedures/ngap_initial_context_setup_procedure.h"
#include "procedures/ngap_pdu_session_resource_modify_procedure.h"
#include "procedures/ngap_pdu_session_resource_release_procedure.h"
#include "procedures/ngap_pdu_session_resource_setup_procedure.h"
#include "procedures/ngap_ue_context_release_procedure.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/ngap/ngap_reset.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cause/ngap_cause.h"

using namespace srsran;
using namespace asn1::ngap;
using namespace srs_cu_cp;

ngap_impl::ngap_impl(const ngap_configuration&          ngap_cfg_,
                     ngap_cu_cp_notifier&               cu_cp_notifier_,
                     ngap_cu_cp_du_repository_notifier& cu_cp_du_repository_notifier_,
                     n2_connection_client&              n2_gateway,
                     timer_manager&                     timers_,
                     task_executor&                     ctrl_exec_) :
  logger(srslog::fetch_basic_logger("NGAP")),
  ue_ctxt_list(logger),
  cu_cp_notifier(cu_cp_notifier_),
  cu_cp_du_repository_notifier(cu_cp_du_repository_notifier_),
  timers(timers_),
  ctrl_exec(ctrl_exec_),
  ev_mng(timer_factory{timers, ctrl_exec}),
  conn_handler(n2_gateway, *this, cu_cp_notifier, ctrl_exec)
{
  context.gnb_id                    = ngap_cfg_.gnb_id;
  context.ran_node_name             = ngap_cfg_.ran_node_name;
  context.plmn                      = ngap_cfg_.plmn;
  context.tac                       = ngap_cfg_.tac;
  context.pdu_session_setup_timeout = ngap_cfg_.pdu_session_setup_timeout;
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
ngap_impl::~ngap_impl() {}

bool ngap_impl::update_ue_index(ue_index_t              new_ue_index,
                                ue_index_t              old_ue_index,
                                ngap_cu_cp_ue_notifier& new_ue_notifier)
{
  if (!ue_ctxt_list.contains(old_ue_index)) {
    logger.warning("Failed to transfer NGAP UE context from ue={} to ue={}. Old UE context does not exist",
                   old_ue_index,
                   new_ue_index);
    return false;
  }

  ue_ctxt_list[old_ue_index].logger.log_debug("Transferring NGAP UE context to ue={}", new_ue_index);

  // Notify CU-CP about creation of NGAP UE
  ngap_cu_cp_ue_notifier* ue = cu_cp_notifier.on_new_ngap_ue(new_ue_index);
  if (ue == nullptr) {
    logger.error("ue={}: Failed to transfer UE context", new_ue_index);
    return false;
  }

  ue_ctxt_list.update_ue_index(new_ue_index, old_ue_index, new_ue_notifier);

  return true;
}

bool ngap_impl::handle_amf_tnl_connection_request()
{
  std::unique_ptr<ngap_message_notifier> pdu_notifier = conn_handler.connect_to_amf();
  if (pdu_notifier == nullptr) {
    return false;
  }
  tx_pdu_notifier = std::make_unique<tx_pdu_notifier_with_logging>(*this, std::move(pdu_notifier));

  return true;
}

async_task<void> ngap_impl::handle_amf_disconnection_request()
{
  return conn_handler.handle_tnl_association_removal();
}

async_task<ngap_ng_setup_result> ngap_impl::handle_ng_setup_request(const ngap_ng_setup_request& request)
{
  logger.info("Sending NgSetupRequest");

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& ng_setup_request = ngap_msg.pdu.init_msg().value.ng_setup_request();
  fill_asn1_ng_setup_request(ng_setup_request, request);

  return launch_async<ng_setup_procedure>(
      context, ngap_msg, request.max_setup_retries, *tx_pdu_notifier, ev_mng, timer_factory{timers, ctrl_exec}, logger);
}

async_task<void> ngap_impl::handle_ng_reset_message(const cu_cp_ng_reset& msg)
{
  ngap_ng_reset ng_reset = {};
  ng_reset.cause         = msg.cause;

  if (msg.ng_interface_reset) {
    // Reset all UEs
    ng_reset.reset_type = ngap_reset_ng_interface{};
  } else {
    // Reset only specific UEs
    ngap_reset_type_part_of_interface ng_reset_part_of_interface = {};
    for (auto& ue : msg.ues_to_reset) {
      if (!ue_ctxt_list.contains(ue)) {
        logger.warning("ue={}: Excluding UE from NG Reset. UE context does not exist", ue);
      } else {
        auto& ue_ctxt = ue_ctxt_list[ue];

        ngap_ue_associated_lc_ng_conn_item conn_item = {};
        if (ue_ctxt.ue_ids.amf_ue_id != amf_ue_id_t::invalid) {
          conn_item.amf_ue_id = ue_ctxt.ue_ids.amf_ue_id;
        }
        if (ue_ctxt.ue_ids.ran_ue_id != ran_ue_id_t::invalid) {
          conn_item.ran_ue_id = ue_ctxt.ue_ids.ran_ue_id;
        }

        ng_reset_part_of_interface.push_back(conn_item);
      }
    }
    ng_reset.reset_type = ng_reset_part_of_interface;
  }

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_NG_RESET);

  fill_asn1_ng_reset(ngap_msg.pdu.init_msg().value.ng_reset(), ng_reset);

  // Schedule NG Reset procedure
  return launch_async<ng_reset_procedure>(ngap_msg, *tx_pdu_notifier, ev_mng, logger);
}

void ngap_impl::handle_initial_ue_message(const cu_cp_initial_ue_message& msg)
{
  if (ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping InitialUEMessage. UE context already exist", msg.ue_index);
    return;
  }

  // Create NGAP UE
  // Allocate RAN-UE-ID
  ran_ue_id_t ran_ue_id = ue_ctxt_list.allocate_ran_ue_id();
  if (ran_ue_id == ran_ue_id_t::invalid) {
    logger.error("ue={}: No RAN-UE-ID available", msg.ue_index);
    return;
  }

  // Create UE context and store it
  ngap_cu_cp_ue_notifier* ue_notifier = cu_cp_notifier.on_new_ngap_ue(msg.ue_index);
  ue_ctxt_list.add_ue(msg.ue_index, ran_ue_id, *ue_notifier, timers, ctrl_exec);

  ngap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_INIT_UE_MSG);

  auto& init_ue_msg           = ngap_msg.pdu.init_msg().value.init_ue_msg();
  init_ue_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);

  fill_asn1_initial_ue_message(init_ue_msg, msg, context);

  // Start PDU session setup timer
  ue_ctxt.pdu_session_setup_timer.set(context.pdu_session_setup_timeout, [this, msg](timer_id_t /*tid*/) {
    on_pdu_session_setup_timer_expired(msg.ue_index);
  });
  ue_ctxt.pdu_session_setup_timer.run();

  ue_ctxt.logger.log_debug("Starting PDU session creation timer (timeout={}ms)...",
                           ue_ctxt.pdu_session_setup_timer.duration().count());

  // Forward message to AMF
  tx_pdu_notifier->on_new_message(ngap_msg);
}

void ngap_impl::handle_ul_nas_transport_message(const cu_cp_ul_nas_transport& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping UlNasTransportMessage. UE context does not exist", msg.ue_index);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  if (ue_ctxt.release_scheduled) {
    ue_ctxt.logger.log_info("Dropping UlNasTransportMessage. UE is already scheduled for release");
    return;
  }

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UL_NAS_TRANSPORT);

  auto& ul_nas_transport_msg = ngap_msg.pdu.init_msg().value.ul_nas_transport();

  ul_nas_transport_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);

  amf_ue_id_t amf_ue_id = ue_ctxt.ue_ids.amf_ue_id;
  if (amf_ue_id == amf_ue_id_t::invalid) {
    logger.warning("ue={}: Dropping UL NAS transport. UE AMF ID not found", msg.ue_index);
    return;
  }
  ul_nas_transport_msg->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);

  fill_asn1_ul_nas_transport(ul_nas_transport_msg, msg);

  // Schedule transmission of UL NAS transport message to AMF
  ue->schedule_async_task(launch_async([this, ngap_msg](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    tx_pdu_notifier->on_new_message(ngap_msg);
    CORO_RETURN();
  }));
}

void ngap_impl::handle_tx_ue_radio_capability_info_indication_required(
    const ngap_ue_radio_capability_info_indication& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping UE Radio Capability Info Indication. UE context does not exist", msg.ue_index);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UE_RADIO_CAP_INFO_IND);
  auto& ue_radio_cap_info_ind_msg = ngap_msg.pdu.init_msg().value.ue_radio_cap_info_ind();

  ue_radio_cap_info_ind_msg->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);

  amf_ue_id_t amf_ue_id = ue_ctxt.ue_ids.amf_ue_id;
  if (amf_ue_id == amf_ue_id_t::invalid) {
    logger.warning("ue={}: Dropping UE Radio Capability Info message. UE does not have an AMF UE ID", msg.ue_index);
    return;
  }
  ue_radio_cap_info_ind_msg->amf_ue_ngap_id = amf_ue_id_to_uint(amf_ue_id);
  ue_radio_cap_info_ind_msg->ue_radio_cap   = msg.ue_cap_rat_container_list.copy();

  ue_ctxt.logger.log_info("Scheduling UE Radio Capability Info Indication");

  // Schedule transmission of UE Radio Capability Info Indication to AMF
  ue->schedule_async_task(launch_async([this, ngap_msg](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    tx_pdu_notifier->on_new_message(ngap_msg);
    CORO_RETURN();
  }));
}

void ngap_impl::handle_message(const ngap_message& msg)
{
  // Run NGAP protocols in Control executor.
  if (not ctrl_exec.execute([this, msg]() {
        log_rx_pdu(msg);

        switch (msg.pdu.type().value) {
          case ngap_pdu_c::types_opts::init_msg:
            handle_initiating_message(msg.pdu.init_msg());
            break;
          case ngap_pdu_c::types_opts::successful_outcome:
            handle_successful_outcome(msg.pdu.successful_outcome());
            break;
          case ngap_pdu_c::types_opts::unsuccessful_outcome:
            handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
            break;
          default:
            logger.error("Invalid PDU type");
            break;
        }
      })) {
    logger.error("Discarding Rx NGAP PDU. Cause: task queue is full");
  }
}

void ngap_impl::handle_initiating_message(const init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case ngap_elem_procs_o::init_msg_c::types_opts::dl_nas_transport:
      handle_dl_nas_transport_message(msg.value.dl_nas_transport());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::init_context_setup_request:
      handle_initial_context_setup_request(msg.value.init_context_setup_request());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::pdu_session_res_setup_request:
      handle_pdu_session_resource_setup_request(msg.value.pdu_session_res_setup_request());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::pdu_session_res_modify_request:
      handle_pdu_session_resource_modify_request(msg.value.pdu_session_res_modify_request());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::pdu_session_res_release_cmd:
      handle_pdu_session_resource_release_command(msg.value.pdu_session_res_release_cmd());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd:
      handle_ue_context_release_command(msg.value.ue_context_release_cmd());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::paging:
      handle_paging(msg.value.paging());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::ho_request:
      handle_handover_request(msg.value.ho_request());
      break;
    case ngap_elem_procs_o::init_msg_c::types_opts::error_ind:
      handle_error_indication(msg.value.error_ind());
      break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void ngap_impl::handle_dl_nas_transport_message(const asn1::ngap::dl_nas_transport_s& msg)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(msg->ran_ue_ngap_id))) {
    logger.warning("ran_ue={} amf_ue={}: Dropping DlNasTransportMessage. UE context does not exist",
                   msg->ran_ue_ngap_id,
                   msg->amf_ue_ngap_id);
    send_error_indication(*tx_pdu_notifier,
                          logger,
                          {},
                          uint_to_amf_ue_id(msg->amf_ue_ngap_id),
                          ngap_cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(msg->ran_ue_ngap_id)];

  if (ue_ctxt.release_scheduled) {
    ue_ctxt.logger.log_info("Dropping DlNasTransportMessage. UE is already scheduled for release");
    stored_error_indications.emplace(ue_ctxt.ue_ids.ue_index,
                                     error_indication_request_t{ngap_cause_radio_network_t::interaction_with_other_proc,
                                                                ue_ctxt.ue_ids.ran_ue_id,
                                                                uint_to_amf_ue_id(msg->amf_ue_ngap_id)});
    return;
  }

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  // Add AMF UE ID to ue ngap context if it is not set (this is the first DL NAS Transport message)
  if (ue_ctxt.ue_ids.amf_ue_id == amf_ue_id_t::invalid) {
    // Set AMF UE ID in the UE context and also in the lookup
    ue_ctxt_list.update_amf_ue_id(ue_ctxt.ue_ids.ran_ue_id, uint_to_amf_ue_id(msg->amf_ue_ngap_id));
  }

  ngap_dl_nas_transport_message dl_nas_msg;
  fill_ngap_dl_nas_transport_message(dl_nas_msg, ue->get_ue_index(), msg);

  // start routine
  ue->schedule_async_task(
      launch_async<ngap_dl_nas_message_transfer_procedure>(dl_nas_msg,
                                                           ue->get_rrc_ue_pdu_notifier(),
                                                           ue->get_rrc_ue_control_notifier(),
                                                           get_ngap_ue_radio_cap_management_handler(),
                                                           ue_ctxt.logger));
}

void ngap_impl::handle_initial_context_setup_request(const asn1::ngap::init_context_setup_request_s& request)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(request->ran_ue_ngap_id))) {
    logger.warning("ran_ue={} amf_ue={}: Dropping InitialContextSetupRequest. UE context does not exist",
                   request->ran_ue_ngap_id,
                   request->amf_ue_ngap_id);
    send_error_indication(*tx_pdu_notifier, logger, {}, {}, ngap_cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(request->ran_ue_ngap_id)];

  if (ue_ctxt.release_scheduled) {
    ue_ctxt.logger.log_info("Dropping InitialContextSetup. UE is already scheduled for release");
    stored_error_indications.emplace(ue_ctxt.ue_ids.ue_index,
                                     error_indication_request_t{ngap_cause_radio_network_t::interaction_with_other_proc,
                                                                ue_ctxt.ue_ids.ran_ue_id,
                                                                uint_to_amf_ue_id(request->amf_ue_ngap_id)});
    return;
  }

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  // If InitialContextSetupRequest contains PDU Session Setup list, stop pdu session setup timer
  if (request->pdu_session_res_setup_list_cxt_req_present) {
    ue_ctxt.pdu_session_setup_timer.stop();
  }

  // Update AMF ID and use the one from this Context Setup as per TS 38.413 v16.2 page 38
  if (ue_ctxt.ue_ids.amf_ue_id != uint_to_amf_ue_id(request->amf_ue_ngap_id)) {
    ue_ctxt_list.update_amf_ue_id(ue_ctxt.ue_ids.ran_ue_id, uint_to_amf_ue_id(request->amf_ue_ngap_id));
  }

  // Convert to common type
  ngap_init_context_setup_request init_ctxt_setup_req;
  init_ctxt_setup_req.ue_index = ue_ctxt.ue_ids.ue_index;
  if (!fill_ngap_initial_context_setup_request(init_ctxt_setup_req, request)) {
    ue_ctxt.logger.log_warning("Conversion of PduSessionResourceSetupRequest failed");
    send_error_indication(*tx_pdu_notifier, logger, ue_ctxt.ue_ids.ran_ue_id, ue_ctxt.ue_ids.amf_ue_id);
    return;
  }

  // Store UE Aggregate Maximum Bitrate
  if (init_ctxt_setup_req.ue_aggr_max_bit_rate.has_value()) {
    ue_ctxt.aggregate_maximum_bit_rate_dl = init_ctxt_setup_req.ue_aggr_max_bit_rate.value().ue_aggr_max_bit_rate_dl;
  }

  // Log security context
  ue_ctxt.logger.log_debug(request->security_key.data(), 32, "K_gnb");
  ue_ctxt.logger.log_debug("Supported integrity algorithms: {}",
                           init_ctxt_setup_req.security_context.supported_int_algos);
  ue_ctxt.logger.log_debug("Supported ciphering algorithms: {}",
                           init_ctxt_setup_req.security_context.supported_enc_algos);

  // start routine
  ue->schedule_async_task(launch_async<ngap_initial_context_setup_procedure>(
      init_ctxt_setup_req, ue_ctxt.ue_ids, cu_cp_notifier, *tx_pdu_notifier, ue_ctxt.logger));
}

void ngap_impl::handle_pdu_session_resource_setup_request(const asn1::ngap::pdu_session_res_setup_request_s& request)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(request->ran_ue_ngap_id))) {
    logger.warning("ran_ue={} amf_ue={}: Dropping PduSessionResourceSetupRequest. UE context does not exist",
                   request->ran_ue_ngap_id,
                   request->amf_ue_ngap_id);
    send_error_indication(*tx_pdu_notifier, logger, {}, {}, ngap_cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(request->ran_ue_ngap_id)];

  if (ue_ctxt.release_scheduled) {
    ue_ctxt.logger.log_info("Dropping PduSessionResourceSetupRequest. UE is already scheduled for release");
    stored_error_indications.emplace(ue_ctxt.ue_ids.ue_index,
                                     error_indication_request_t{ngap_cause_radio_network_t::interaction_with_other_proc,
                                                                ue_ctxt.ue_ids.ran_ue_id,
                                                                uint_to_amf_ue_id(request->amf_ue_ngap_id)});
    return;
  }

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  // Stop PDU session setup timer
  ue_ctxt.pdu_session_setup_timer.stop();

  if (!ue->is_security_enabled()) {
    ue_ctxt.logger.log_warning("Dropping PduSessionResourceSetupRequest. Security context does not exist");
    send_error_indication(*tx_pdu_notifier, logger, ue_ctxt.ue_ids.ran_ue_id, ue_ctxt.ue_ids.amf_ue_id, {});
    return;
  }

  // Store information in UE context
  if (request->ue_aggr_max_bit_rate_present) {
    ue_ctxt.aggregate_maximum_bit_rate_dl = request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl;
  }

  // Convert to common type
  cu_cp_pdu_session_resource_setup_request msg;
  msg.ue_index     = ue_ctxt.ue_ids.ue_index;
  msg.serving_plmn = context.plmn;
  if (!fill_cu_cp_pdu_session_resource_setup_request(msg, request->pdu_session_res_setup_list_su_req)) {
    ue_ctxt.logger.log_warning("Conversion of PduSessionResourceSetupRequest failed");
    send_error_indication(*tx_pdu_notifier, logger, ue_ctxt.ue_ids.ran_ue_id, ue_ctxt.ue_ids.amf_ue_id, {});
    return;
  }
  msg.ue_aggregate_maximum_bit_rate_dl = ue_ctxt.aggregate_maximum_bit_rate_dl;

  // start routine
  ue->schedule_async_task(launch_async<ngap_pdu_session_resource_setup_procedure>(
      msg, request, ue_ctxt.ue_ids, cu_cp_notifier, *tx_pdu_notifier, ue_ctxt.logger));
}

void ngap_impl::handle_pdu_session_resource_modify_request(const asn1::ngap::pdu_session_res_modify_request_s& request)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(request->ran_ue_ngap_id))) {
    logger.warning("ran_ue={} amf_ue={}: Dropping PduSessionResourceModifyRequest. UE context does not exist",
                   request->ran_ue_ngap_id,
                   request->amf_ue_ngap_id);
    send_error_indication(*tx_pdu_notifier, logger, {}, {}, ngap_cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(request->ran_ue_ngap_id)];
  if (ue_ctxt.release_scheduled) {
    ue_ctxt.logger.log_info("Dropping PduSessionResourceModifyRequest. UE is already scheduled for release");
    stored_error_indications.emplace(ue_ctxt.ue_ids.ue_index,
                                     error_indication_request_t{ngap_cause_radio_network_t::interaction_with_other_proc,
                                                                ue_ctxt.ue_ids.ran_ue_id,
                                                                uint_to_amf_ue_id(request->amf_ue_ngap_id)});
    return;
  }

  // Check for duplicate messages
  byte_buffer asn1_request_pdu = pack_into_pdu(request);
  if (asn1_request_pdu == ue_ctxt.last_pdu_session_resource_modify_request) {
    ue_ctxt.logger.log_warning("Received duplicate PduSessionResourceModifyRequest");
    schedule_error_indication(ue_ctxt.ue_ids.ue_index, ngap_cause_radio_network_t::unspecified);
    return;
  }

  // Store last PDU session resource modify request
  ue_ctxt.last_pdu_session_resource_modify_request = asn1_request_pdu.copy();

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  if (request->ran_paging_prio_present) {
    ue_ctxt.logger.log_debug("Not handling RAN paging prio");
  }

  // Convert to common type
  cu_cp_pdu_session_resource_modify_request msg;
  msg.ue_index = ue_ctxt.ue_ids.ue_index;
  if (!fill_cu_cp_pdu_session_resource_modify_request(msg, request->pdu_session_res_modify_list_mod_req)) {
    ue_ctxt.logger.log_warning("Unable to fill ASN1 contents for PduSessionResourceModifyRequest");
    schedule_error_indication(ue_ctxt.ue_ids.ue_index, ngap_cause_radio_network_t::unspecified);
    return;
  }

  // start routine
  ue->schedule_async_task(launch_async<ngap_pdu_session_resource_modify_procedure>(msg,
                                                                                   request,
                                                                                   ue_ctxt.ue_ids,
                                                                                   cu_cp_notifier,
                                                                                   *tx_pdu_notifier,
                                                                                   get_ngap_control_message_handler(),
                                                                                   ue_ctxt.logger));
}

void ngap_impl::handle_pdu_session_resource_release_command(const asn1::ngap::pdu_session_res_release_cmd_s& command)
{
  if (!ue_ctxt_list.contains(uint_to_ran_ue_id(command->ran_ue_ngap_id))) {
    logger.warning("ran_ue={} amf_ue={}: Dropping PduSessionResourceReleaseCommand. UE context does not exist",
                   command->ran_ue_ngap_id,
                   command->amf_ue_ngap_id);
    send_error_indication(*tx_pdu_notifier, logger, {}, {}, ngap_cause_radio_network_t::unknown_local_ue_ngap_id);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[uint_to_ran_ue_id(command->ran_ue_ngap_id)];

  if (ue_ctxt.release_scheduled) {
    ue_ctxt.logger.log_info("Dropping PduSessionResourceReleaseCommand. UE is already scheduled for release");
    stored_error_indications.emplace(ue_ctxt.ue_ids.ue_index,
                                     error_indication_request_t{ngap_cause_radio_network_t::interaction_with_other_proc,
                                                                ue_ctxt.ue_ids.ran_ue_id,
                                                                uint_to_amf_ue_id(command->amf_ue_ngap_id)});
    return;
  }

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  ue_ctxt.logger.log_info("Received PduSessionResourceReleaseCommand");

  // Convert to common type
  cu_cp_pdu_session_resource_release_command msg;
  msg.ue_index = ue_ctxt.ue_ids.ue_index;
  fill_cu_cp_pdu_session_resource_release_command(msg, command);

  // start routine
  ue->schedule_async_task(launch_async<ngap_pdu_session_resource_release_procedure>(
      msg, ue_ctxt.ue_ids, cu_cp_notifier, *tx_pdu_notifier, ue_ctxt.logger));
}

void ngap_impl::handle_ue_context_release_command(const asn1::ngap::ue_context_release_cmd_s& cmd)
{
  amf_ue_id_t amf_ue_id = amf_ue_id_t::invalid;
  ran_ue_id_t ran_ue_id = ran_ue_id_t::invalid;
  if (cmd->ue_ngap_ids.type() == asn1::ngap::ue_ngap_ids_c::types_opts::amf_ue_ngap_id) {
    amf_ue_id = uint_to_amf_ue_id(cmd->ue_ngap_ids.amf_ue_ngap_id());

    if (!ue_ctxt_list.contains(amf_ue_id)) {
      // TS 38.413 section 8.3.3 doesn't specify abnormal conditions, so we just drop the message and send an error
      // indication
      logger.warning("{}amf_ue={}: Dropping UeContextReleaseCommand. UE does not exist",
                     ran_ue_id == ran_ue_id_t::invalid ? "" : fmt::format("ran_ue={} ", ran_ue_id),
                     amf_ue_id);
      send_error_indication(
          *tx_pdu_notifier, logger, {}, amf_ue_id, ngap_cause_radio_network_t::unknown_local_ue_ngap_id);
      return;
    }
  } else if (cmd->ue_ngap_ids.type() == asn1::ngap::ue_ngap_ids_c::types_opts::ue_ngap_id_pair) {
    amf_ue_id = uint_to_amf_ue_id(cmd->ue_ngap_ids.ue_ngap_id_pair().amf_ue_ngap_id);
    ran_ue_id = uint_to_ran_ue_id(cmd->ue_ngap_ids.ue_ngap_id_pair().ran_ue_ngap_id);

    if (!ue_ctxt_list.contains(ran_ue_id)) {
      // TS 38.413 section 8.3.3 doesn't specify abnormal conditions, so we just drop the message and send an error
      // indication
      logger.warning("ran_ue={} amf_ue={}: Dropping UeContextReleaseCommand. UE does not exist", ran_ue_id, amf_ue_id);
      send_error_indication(
          *tx_pdu_notifier, logger, {}, amf_ue_id, ngap_cause_radio_network_t::unknown_local_ue_ngap_id);
      return;
    }

    // Update AMF UE ID
    if (ue_ctxt_list[ran_ue_id].ue_ids.amf_ue_id == amf_ue_id_t::invalid or
        ue_ctxt_list[ran_ue_id].ue_ids.amf_ue_id != amf_ue_id) {
      ue_ctxt_list.update_amf_ue_id(ran_ue_id, amf_ue_id);
    }
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[amf_ue_id];

  if (ue_ctxt.release_scheduled) {
    ue_ctxt.logger.log_info("Dropping UeContextReleaseCommand. UE is already scheduled for release");
    stored_error_indications.emplace(ue_ctxt.ue_ids.ue_index,
                                     error_indication_request_t{ngap_cause_radio_network_t::interaction_with_other_proc,
                                                                ue_ctxt.ue_ids.ran_ue_id,
                                                                amf_ue_id});
    return;
  }

  ue_ctxt.release_scheduled = true;

  if (ran_ue_id == ran_ue_id_t::invalid) {
    ran_ue_id = ue_ctxt.ue_ids.ran_ue_id;
  }

  // Add AMF UE ID to UE, if its not set
  if (ue_ctxt.ue_ids.amf_ue_id == amf_ue_id_t::invalid) {
    ue_ctxt_list.update_amf_ue_id(ran_ue_id, amf_ue_id);
  }

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  ue_ctxt.logger.log_info("Received UeContextReleaseCommand");

  // Convert to common type
  cu_cp_ue_context_release_command msg;
  msg.ue_index = ue_ctxt.ue_ids.ue_index;
  fill_cu_cp_ue_context_release_command(msg, cmd);

  // start routine
  ue->schedule_async_task(launch_async<ngap_ue_context_release_procedure>(
      msg, ue_ctxt.ue_ids, stored_error_indications, cu_cp_notifier, *tx_pdu_notifier, ue_ctxt.logger));
}

void ngap_impl::handle_paging(const asn1::ngap::paging_s& msg)
{
  if (msg->ue_paging_id.type() != asn1::ngap::ue_paging_id_c::types::five_g_s_tmsi) {
    logger.warning("Dropping PDU. Unsupported UE Paging ID");
    send_error_indication(*tx_pdu_notifier, logger);
    return;
  }

  // Convert to common type
  cu_cp_paging_message cu_cp_paging_msg;
  fill_cu_cp_paging_message(cu_cp_paging_msg, msg);

  cu_cp_du_repository_notifier.on_paging_message(cu_cp_paging_msg);
}

// free function to generate a handover failure message
ngap_message generate_handover_failure(uint64_t amf_ue_id)
{
  ngap_message ngap_msg;
  ngap_msg.pdu.set_unsuccessful_outcome();
  ngap_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_NGAP_ID_HO_RES_ALLOC);
  auto& ho_fail           = ngap_msg.pdu.unsuccessful_outcome().value.ho_fail();
  ho_fail->amf_ue_ngap_id = amf_ue_id;
  ho_fail->cause.set_protocol();

  return ngap_msg;
}

void ngap_impl::handle_handover_request(const asn1::ngap::ho_request_s& msg)
{
  logger.debug("Received HandoverRequest");

  // Convert Handover Request to common type
  ngap_handover_request ho_request;
  if (!fill_ngap_handover_request(ho_request, msg)) {
    logger.warning("Sending HandoverFailure. Received invalid HandoverRequest");
    tx_pdu_notifier->on_new_message(generate_handover_failure(msg->amf_ue_ngap_id));
    return;
  }

  logger.info("HandoverRequest - extracted target cell. plmn={}, target cell_id={}",
              ho_request.source_to_target_transparent_container.target_cell_id.plmn_id,
              ho_request.source_to_target_transparent_container.target_cell_id.nci);

  // Create UE in target cell
  ho_request.ue_index = cu_cp_du_repository_notifier.request_new_ue_index_allocation(
      ho_request.source_to_target_transparent_container.target_cell_id);
  if (ho_request.ue_index == ue_index_t::invalid) {
    logger.warning("Sending HandoverFailure. Couldn't allocate UE index");
    tx_pdu_notifier->on_new_message(generate_handover_failure(msg->amf_ue_ngap_id));
    return;
  }

  // Inititialize security context of target UE
  if (!cu_cp_notifier.on_handover_request_received(ho_request.ue_index, ho_request.security_context)) {
    logger.warning("Sending HandoverFailure. Couldn't initialize security context");
    tx_pdu_notifier->on_new_message(generate_handover_failure(msg->amf_ue_ngap_id));
    return;
  }

  if (!cu_cp_notifier.schedule_async_task(
          ho_request.ue_index,
          launch_async<ngap_handover_resource_allocation_procedure>(ho_request,
                                                                    uint_to_amf_ue_id(msg->amf_ue_ngap_id),
                                                                    ue_ctxt_list,
                                                                    cu_cp_notifier,
                                                                    cu_cp_du_repository_notifier,
                                                                    *tx_pdu_notifier,
                                                                    timers,
                                                                    ctrl_exec,
                                                                    logger))) {
    logger.warning("Sending HandoverFailure. Couldn't schedule handover resource allocation procedure");
    tx_pdu_notifier->on_new_message(generate_handover_failure(msg->amf_ue_ngap_id));
    return;
  }
}

void ngap_impl::handle_error_indication(const asn1::ngap::error_ind_s& msg)
{
  amf_ue_id_t amf_ue_id = amf_ue_id_t::invalid;
  ran_ue_id_t ran_ue_id = ran_ue_id_t::invalid;
  ue_index_t  ue_index  = ue_index_t::invalid;
  std::string msg_cause;

  if (msg->cause_present) {
    msg_cause = asn1_cause_to_string(msg->cause);
  }

  if (msg->amf_ue_ngap_id_present) {
    amf_ue_id = uint_to_amf_ue_id(msg->amf_ue_ngap_id);
    if (!ue_ctxt_list.contains(uint_to_amf_ue_id(msg->amf_ue_ngap_id))) {
      logger.warning("amf_ue={}: Dropping ErrorIndication. UE context does not exist", msg->amf_ue_ngap_id);
      send_error_indication(
          *tx_pdu_notifier, logger, {}, {}, ngap_cause_radio_network_t::inconsistent_remote_ue_ngap_id);
      return;
    }
    ue_index = ue_ctxt_list[amf_ue_id].ue_ids.ue_index;
  } else if (msg->ran_ue_ngap_id_present) {
    ran_ue_id = uint_to_ran_ue_id(msg->ran_ue_ngap_id);
    if (!ue_ctxt_list.contains(uint_to_ran_ue_id(msg->ran_ue_ngap_id))) {
      logger.warning("ran_ue={}: Dropping ErrorIndication. UE context does not exist", msg->ran_ue_ngap_id);
      send_error_indication(*tx_pdu_notifier, logger, {}, {}, ngap_cause_radio_network_t::unknown_local_ue_ngap_id);
      return;
    }
    ue_index = ue_ctxt_list[ran_ue_id].ue_ids.ue_index;
  } else {
    logger.info("Received ErrorIndication{}", msg_cause.empty() ? "" : ". Cause: " + msg_cause);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  ue_ctxt.logger.log_info("Received ErrorIndication{}", msg_cause.empty() ? "" : ". Cause: " + msg_cause);

  // Request UE release
  ue->schedule_async_task(launch_async([this, ue_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(handle_ue_context_release_request(
        cu_cp_ue_context_release_request{ue_index, {}, ngap_cause_radio_network_t::unspecified}));
    CORO_RETURN();
  }));

  // TODO: handle error indication
}

void ngap_impl::handle_successful_outcome(const successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ng_setup_resp: {
      ev_mng.ng_setup_outcome.set(outcome.value.ng_setup_resp());
    } break;
    case ngap_elem_procs_o::successful_outcome_c::types_opts::ho_cmd: {
      ev_mng.handover_preparation_outcome.set(outcome.value.ho_cmd());
    } break;
    default:
      logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void ngap_impl::handle_unsuccessful_outcome(const unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::ng_setup_fail: {
      ev_mng.ng_setup_outcome.set(outcome.value.ng_setup_fail());
    } break;
    case ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::ho_prep_fail: {
      ev_mng.handover_preparation_outcome.set(outcome.value.ho_prep_fail());
    } break;
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

async_task<bool> ngap_impl::handle_ue_context_release_request(const cu_cp_ue_context_release_request& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.info("ue={}: Ignoring UEContextReleaseRequest. Cause: UE has no NGAP context", msg.ue_index);
    return launch_async([](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(false);
    });
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  // Stop PDU session setup timer
  ue_ctxt.pdu_session_setup_timer.stop();

  if (ue_ctxt.ue_ids.amf_ue_id == amf_ue_id_t::invalid) {
    ue_ctxt.logger.log_debug("Ignoring UeContextReleaseRequest. UE does not have an AMF UE ID");
    return launch_async([](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(false);
    });
  }

  if (ue_ctxt.release_requested or ue_ctxt.release_scheduled) {
    ue_ctxt.logger.log_debug("Ignoring UeContextReleaseRequest. Cause: Release {} already pending",
                             ue_ctxt.release_scheduled ? "command" : "request");
    return launch_async([](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  ngap_message ngap_msg;
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE_REQUEST);

  auto& ue_context_release_request = ngap_msg.pdu.init_msg().value.ue_context_release_request();

  ue_context_release_request->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);
  ue_context_release_request->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ctxt.ue_ids.amf_ue_id);

  fill_asn1_ue_context_release_request(ue_context_release_request, msg);

  // Forward message to AMF
  ue_ctxt.release_requested = true; // Mark UE so retx of request are avoided.

  // Schedule transmission of UE Context Release Request
  return launch_async([this, msg, ngap_msg](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);

    if (!ue_ctxt_list.contains(msg.ue_index)) {
      logger.warning("ue={}: Dropping scheduled UeContextReleaseRequest. UE context does not exist anymore",
                     msg.ue_index);
    } else {
      tx_pdu_notifier->on_new_message(ngap_msg);
    }
    CORO_RETURN(true);
  });
}

// TODO make preparation result an async task
async_task<ngap_handover_preparation_response>
ngap_impl::handle_handover_preparation_request(const ngap_handover_preparation_request& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping HandoverPreparationRequest. UE context does not exist", msg.ue_index);
    return launch_async([](coro_context<async_task<ngap_handover_preparation_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(ngap_handover_preparation_response{false});
    });
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  auto* ue = ue_ctxt.get_cu_cp_ue();
  srsran_assert(ue != nullptr,
                "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_ctxt.ue_ids.ue_index,
                ue_ctxt.ue_ids.ran_ue_id,
                ue_ctxt.ue_ids.amf_ue_id);

  ue_ctxt.logger.log_info("Starting HO preparation");

  return launch_async<ngap_handover_preparation_procedure>(msg,
                                                           context,
                                                           ue_ctxt.ue_ids,
                                                           *tx_pdu_notifier,
                                                           ue->get_rrc_ue_control_notifier(),
                                                           cu_cp_notifier,
                                                           ev_mng,
                                                           timer_factory{timers, ctrl_exec},
                                                           ue_ctxt.logger);
}

void ngap_impl::handle_inter_cu_ho_rrc_recfg_complete(const ue_index_t           ue_index,
                                                      const nr_cell_global_id_t& cgi,
                                                      const unsigned             tac)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.warning("ue={}: Dropping RrcReconfigurationComplete. UE context does not exist", ue_index);
    return;
  }

  ngap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_init_msg();
  ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_HO_NOTIF);

  auto& ho_notify           = ngap_msg.pdu.init_msg().value.ho_notify();
  ho_notify->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ctxt.ue_ids.ran_ue_id);
  ho_notify->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ctxt.ue_ids.amf_ue_id);

  fill_asn1_handover_notify(ho_notify, cgi, tac);

  // Forward message to AMF
  ue_ctxt.logger.log_info("Sending HandoverNotify");
  tx_pdu_notifier->on_new_message(ngap_msg);
}

void ngap_impl::remove_ue_context(ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.debug("ue={}: UE context not found", ue_index);
    return;
  }

  ue_ctxt_list.remove_ue_context(ue_index);
}

void ngap_impl::schedule_error_indication(ue_index_t ue_index, ngap_cause_t cause, std::optional<amf_ue_id_t> amf_ue_id)
{
  ngap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];
  auto*            ue      = ue_ctxt.get_cu_cp_ue();

  srsran_assert(ue != nullptr,
                "ue={} amf_ue={}: UE for UE context doesn't exist",
                ue_index,
                amf_ue_id.has_value() ? "" : fmt::format("amf_ue={} ", amf_ue_id.value()));

  ue->schedule_async_task(launch_async([this, ue_index, cause, amf_ue_id](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    send_error_indication(*tx_pdu_notifier, logger, ue_ctxt_list[ue_index].ue_ids.ran_ue_id, amf_ue_id, cause);
    CORO_RETURN();
  }));
}

void ngap_impl::on_pdu_session_setup_timer_expired(ue_index_t ue_index)
{
  if (ue_ctxt_list.contains(ue_index)) {
    ngap_ue_context& ue_ctxt = ue_ctxt_list[ue_index];

    auto* ue = ue_ctxt.get_cu_cp_ue();
    srsran_assert(ue != nullptr,
                  "ue={} ran_ue={} amf_ue={}: UE for UE context doesn't exist",
                  ue_ctxt.ue_ids.ue_index,
                  ue_ctxt.ue_ids.ran_ue_id,
                  ue_ctxt.ue_ids.amf_ue_id);

    if (ue_ctxt.ue_ids.amf_ue_id == amf_ue_id_t::invalid) {
      // AMF never responded to InitialUEMessage, so we only remove the UE from the DU
      ue_ctxt.logger.log_warning("PDU session setup timer expired after {}ms. Releasing UE from DU",
                                 ue_ctxt.pdu_session_setup_timer.duration().count());

      ue->schedule_async_task(launch_async([this, ue_index](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        CORO_AWAIT(
            cu_cp_notifier.on_new_ue_context_release_command({ue_index, ngap_cause_radio_network_t::unspecified}));
        CORO_RETURN();
      }));
    } else {
      ue_ctxt.logger.log_warning("PDU session setup timer expired after {}ms. Requesting UE release",
                                 ue_ctxt.pdu_session_setup_timer.duration().count());

      // Request UE release
      ue->schedule_async_task(launch_async([this, ue_index](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        CORO_AWAIT(handle_ue_context_release_request(
            cu_cp_ue_context_release_request{ue_index, {}, ngap_cause_radio_network_t::unspecified}));
        CORO_RETURN();
      }));
    }
  } else {
    logger.debug("ue={}: Ignoring expired PDU session setup timer. UE context not found", ue_index);
    return;
  }
}

static auto log_pdu_helper(srslog::basic_logger&         logger,
                           bool                          json_log,
                           bool                          is_rx,
                           const ngap_ue_context_list&   ue_ctxt_list,
                           const asn1::ngap::ngap_pdu_c& pdu)
{
  if (not logger.info.enabled()) {
    return;
  }

  std::optional<ran_ue_id_t> ran_ue_id = get_ran_ue_id(pdu);
  std::optional<ue_index_t>  ue_idx;
  if (ran_ue_id.has_value()) {
    auto* ue = ue_ctxt_list.find(ran_ue_id.value());
    if (ue != nullptr) {
      ue_idx = ue->ue_ids.ue_index;
    }
  }

  log_ngap_pdu(logger, json_log, is_rx, ue_idx, pdu);
}

void ngap_impl::log_rx_pdu(const ngap_message& msg)
{
  log_pdu_helper(logger, logger.debug.enabled(), true, ue_ctxt_list, msg.pdu);
}

void ngap_impl::tx_pdu_notifier_with_logging::on_new_message(const ngap_message& msg)
{
  log_pdu_helper(parent.logger, parent.logger.debug.enabled(), false, parent.ue_ctxt_list, msg.pdu);

  decorated->on_new_message(msg);
}
