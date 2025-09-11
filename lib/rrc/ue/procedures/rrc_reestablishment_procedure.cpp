/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "rrc_reestablishment_procedure.h"
#include "rrc_setup_procedure.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg.h"
#include "srsran/asn1/rrc_nr/nr_ue_variables.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/security/integrity.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

rrc_reestablishment_procedure::rrc_reestablishment_procedure(
    const asn1::rrc_nr::rrc_reest_request_s& request_,
    rrc_ue_context_t&                        context_,
    const byte_buffer&                       du_to_cu_container_,
    rrc_ue_setup_proc_notifier&              rrc_ue_setup_notifier_,
    rrc_ue_reestablishment_proc_notifier&    rrc_ue_reest_notifier_,
    rrc_ue_control_message_handler&          srb_notifier_,
    rrc_ue_context_update_notifier&          cu_cp_notifier_,
    rrc_ue_cu_cp_ue_notifier&                cu_cp_ue_notifier_,
    rrc_ue_event_notifier&                   metrics_notifier_,
    rrc_ue_ngap_notifier&                    ngap_notifier_,
    rrc_ue_event_manager&                    event_mng_,
    rrc_ue_logger&                           logger_) :
  reestablishment_request(request_),
  context(context_),
  du_to_cu_container(du_to_cu_container_),
  rrc_ue_setup_notifier(rrc_ue_setup_notifier_),
  rrc_ue_reest_notifier(rrc_ue_reest_notifier_),
  srb_notifier(srb_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  cu_cp_ue_notifier(cu_cp_ue_notifier_),
  metrics_notifier(metrics_notifier_),
  ngap_notifier(ngap_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
  procedure_timeout = context.cell.timers.t311 + context.cfg.rrc_procedure_guard_time_ms;
}

void rrc_reestablishment_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Notify metrics about attempted RRC connection reestablishment.
  metrics_notifier.on_attempted_rrc_connection_reestablishment();

  logger.log_debug("\"{}\" for old c-rnti={}, pci={} initialized",
                   name(),
                   to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti),
                   reestablishment_request.rrc_reest_request.ue_id.pci);

  // Verify if we are in conditions for a Reestablishment, or should opt for RRC Setup as fallback.
  if (not is_reestablishment_accepted()) {
    CORO_AWAIT(handle_rrc_reestablishment_fallback());
    logger.log_debug("\"{}\" finalized", name());
    CORO_EARLY_RETURN();
  }

  // Transfer old UE context to new UE context. If it fails, resort to fallback.
  CORO_AWAIT_VALUE(context_transfer_success, cu_cp_notifier.on_ue_transfer_required(old_ue_reest_context.ue_index));
  if (not context_transfer_success) {
    CORO_AWAIT(handle_rrc_reestablishment_fallback());
    logger.log_debug("\"{}\" for old_ue={} finalized", name(), old_ue_reest_context.ue_index);
    CORO_EARLY_RETURN();
  }

  // Accept RRC Reestablishment Request by sending RRC Reestablishment.
  // Note: From this point we should guarantee that a Reestablishment will be performed.

  // Transfer reestablishment context and update security keys.
  transfer_reestablishment_context_and_update_keys();

  // Create SRB1.
  create_srb1();

  // Create new transaction for RRC Reestablishment.
  transaction = event_mng.transactions.create_transaction(procedure_timeout);

  // Send RRC Reestablishment to UE.
  send_rrc_reestablishment();

  // Enable ciphering.
  // Note: Ciphering needs to be enabled after transmitting the RRC Reestablishment message, as the
  // ReestablishmentComplete will be ciphered.
  enable_srb1_ciphering();

  // Await UE response.
  CORO_AWAIT(transaction);

  if (transaction.has_response()) {
    context.state = rrc_state::connected;

    // Notify metrics about successful RRC connection reestablishment.
    metrics_notifier.on_successful_rrc_connection_reestablishment();
    metrics_notifier.on_new_rrc_connection();

    // Notify DU Processor to start a Reestablishment Context Modification Routine.
    CORO_AWAIT_VALUE(context_modification_success,
                     cu_cp_notifier.on_rrc_reestablishment_context_modification_required());

    // Trigger UE context release at AMF in case of failure.
    if (not context_modification_success) {
      logger.log_debug(
          "\"{}\" for old_ue={} failed. Requesting UE context release", name(), old_ue_reest_context.ue_index);
      // Release the old UE.
      ue_context_release_request.ue_index = context.ue_index;
      ue_context_release_request.cause    = ngap_cause_radio_network_t::unspecified;
      CORO_AWAIT(cu_cp_notifier.on_ue_release_required(ue_context_release_request));
    } else {
      logger.log_debug("\"{}\" for old_ue={} finalized", name(), old_ue_reest_context.ue_index);
    }

  } else {
    logger.log_warning(
        "\"{}\" for old_ue={} timed out after {}ms", name(), old_ue_reest_context.ue_index, procedure_timeout.count());
    logger.log_debug("\"{}\" for old_ue={} failed", name(), old_ue_reest_context.ue_index);
  }

  // Notify CU-CP to remove the old UE.
  cu_cp_notifier.on_rrc_reestablishment_complete(old_ue_reest_context.ue_index);

  // Note: From this point the UE is removed and only the stored context can be accessed.

  CORO_RETURN();
}

async_task<void> rrc_reestablishment_procedure::handle_rrc_reestablishment_fallback()
{
  context.connection_cause = establishment_cause_t::mt_access;

  return launch_async([this](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Reject RRC Reestablishment Request by sending RRC Setup.
    CORO_AWAIT(launch_async<rrc_setup_procedure>(context,
                                                 du_to_cu_container,
                                                 rrc_ue_setup_notifier,
                                                 srb_notifier,
                                                 metrics_notifier,
                                                 ngap_notifier,
                                                 event_mng,
                                                 logger,
                                                 true));

    if (old_ue_reest_context.ue_index != ue_index_t::invalid and !old_ue_reest_context.old_ue_fully_attached) {
      // The UE exists but still has not established an SRB2 and DRB. Request the release of the old UE.
      logger.log_debug("old_ue={} was not fully attached yet. Requesting UE context release",
                       old_ue_reest_context.ue_index);
      ue_context_release_request.ue_index = old_ue_reest_context.ue_index;
      ue_context_release_request.cause    = ngap_cause_radio_network_t::unspecified;
      cu_cp_notifier.on_rrc_reestablishment_failure(ue_context_release_request);
    }

    CORO_RETURN();
  });
}

bool rrc_reestablishment_procedure::is_reestablishment_accepted()
{
  // Notify the CU-CP about the reestablishment. This will return the old RRC UE context if it exists.
  // Note that this needs to be run before any other sanity check, as it will also cancel an possibly ongoing handover
  // transaction for the old UE.
  old_ue_reest_context =
      cu_cp_notifier.on_rrc_reestablishment_request(reestablishment_request.rrc_reest_request.ue_id.pci,
                                                    to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti));

  if (context.cfg.force_reestablishment_fallback) {
    log_rejected_reestablishment("RRC Reestablishments were disabled by the app configuration");
    return false;
  }

  if (reestablishment_request.rrc_reest_request.reest_cause.value == asn1::rrc_nr::reest_cause_opts::recfg_fail) {
    log_rejected_reestablishment("Cannot recover from failed RRC Reconfiguration for old UE");
    return false;
  }

  // check if an old UE context with matching C-RNTI, PCI exists.
  if (old_ue_reest_context.ue_index == ue_index_t::invalid) {
    log_rejected_reestablishment("Old UE context not found");
    return false;
  }

  if (old_ue_reest_context.reestablishment_ongoing) {
    log_rejected_reestablishment("Old UE is already in reestablishment procedure");
    return false;
  }

  // Check if the old UE completed the SRB2 and DRB establishment.
  if (not old_ue_reest_context.old_ue_fully_attached) {
    log_rejected_reestablishment("Old UE bearers were not fully established");
    return false;
  }

  // Verify security context.
  return verify_security_context();
}

bool rrc_reestablishment_procedure::verify_security_context()
{
  bool valid = false;

  // Get RX short MAC.
  security::sec_short_mac_i short_mac = {};
  uint16_t short_mac_int              = htons(reestablishment_request.rrc_reest_request.ue_id.short_mac_i.to_number());
  std::memcpy(short_mac.data(), &short_mac_int, 2);

  // Get packed varShortMAC-Input.
  asn1::rrc_nr::var_short_mac_input_s var_short_mac_input = {};
  var_short_mac_input.source_pci                          = reestablishment_request.rrc_reest_request.ue_id.pci;
  var_short_mac_input.target_cell_id.from_number(context.cell.cgi.nci.value());
  var_short_mac_input.source_c_rnti        = reestablishment_request.rrc_reest_request.ue_id.c_rnti;
  byte_buffer   var_short_mac_input_packed = {};
  asn1::bit_ref bref(var_short_mac_input_packed);
  var_short_mac_input.pack(bref);

  logger.log_debug(var_short_mac_input_packed.begin(),
                   var_short_mac_input_packed.end(),
                   "Packed varShortMAC-Input. Source PCI={}, Target Cell-Id={}, Source C-RNTI={}",
                   var_short_mac_input.source_pci,
                   var_short_mac_input.target_cell_id.to_number(),
                   to_rnti(var_short_mac_input.source_c_rnti));

  // Verify ShortMAC-I.
  if (old_ue_reest_context.sec_context.sel_algos.algos_selected) {
    security::sec_as_config source_as_config =
        old_ue_reest_context.sec_context.get_as_config(security::sec_domain::rrc);
    valid = security::verify_short_mac(short_mac, var_short_mac_input_packed, source_as_config);
    logger.log_debug("Received RRC re-establishment request. short_mac_valid={}", valid);
  } else {
    log_rejected_reestablishment("Old UE does not have valid security context");
  }

  return valid;
}

void rrc_reestablishment_procedure::transfer_reestablishment_context_and_update_keys()
{
  // Store capabilities if available.
  if (old_ue_reest_context.capabilities_list.has_value()) {
    context.capabilities_list = old_ue_reest_context.capabilities_list.value();
  }

  // Transfer UP context from old UE.
  cu_cp_notifier.on_up_context_setup_required(old_ue_reest_context.up_ctx);

  // Update security keys.
  // freq_and_timing must be present, otherwise the RRC UE would've never been created.
  uint32_t ssb_arfcn = context.cfg.meas_timings.begin()->freq_and_timing.value().carrier_freq;
  cu_cp_ue_notifier.update_security_context(old_ue_reest_context.sec_context);
  cu_cp_ue_notifier.perform_horizontal_key_derivation(context.cell.pci, ssb_arfcn);
  logger.log_debug("Refreshed keys horizontally. pci={} ssb-arfcn_f_ref={}", context.cell.pci, ssb_arfcn);
}

void rrc_reestablishment_procedure::create_srb1()
{
  // Create SRB1.
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index     = context.ue_index;
  srb1_msg.old_ue_index = old_ue_reest_context.ue_index;
  srb1_msg.srb_id       = srb_id_t::srb1;
  srb1_msg.pdcp_cfg     = {}; // TODO: Get SRB1 PDCP config of the old UE context.
  srb_notifier.create_srb(srb1_msg);

  // Activate SRB1 PDCP security.
  rrc_ue_reest_notifier.on_new_as_security_context();
}

void rrc_reestablishment_procedure::send_rrc_reestablishment()
{
  asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
  dl_dcch_msg.msg.set_c1().set_rrc_reest();
  asn1::rrc_nr::rrc_reest_s& rrc_reest = dl_dcch_msg.msg.c1().rrc_reest();
  rrc_reest.rrc_transaction_id         = transaction.id();
  rrc_reest.crit_exts.set_rrc_reest();

  rrc_ue_reest_notifier.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
}

void rrc_reestablishment_procedure::enable_srb1_ciphering()
{
  auto srb_it = context.srbs.find(srb_id_t::srb1);
  if (srb_it == context.srbs.end()) {
    logger.log_error("Could not enable ciphering for SRB1. Cause: SRB1 not found in the UE context");
    return;
  }

  srb_it->second.enable_rx_security(
      security::integrity_enabled::on, security::ciphering_enabled::on, cu_cp_ue_notifier.get_rrc_128_as_config());
  srb_it->second.enable_tx_security(
      security::integrity_enabled::on, security::ciphering_enabled::on, cu_cp_ue_notifier.get_rrc_128_as_config());
}

void rrc_reestablishment_procedure::log_rejected_reestablishment(const char* cause_str)
{
  logger.log_info(
      "Rejecting RRC Reestablishment to old UE c-rnti={}, pci={}. Cause: {}. Fallback to RRC Setup Procedure...",
      to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti),
      reestablishment_request.rrc_reest_request.ue_id.pci,
      cause_str);
}
