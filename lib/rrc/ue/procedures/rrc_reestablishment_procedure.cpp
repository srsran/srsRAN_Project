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

#include "rrc_reestablishment_procedure.h"
#include "rrc_setup_procedure.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg.h"
#include "srsran/asn1/rrc_nr/nr_ue_variables.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/security/integrity.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

rrc_reestablishment_procedure::rrc_reestablishment_procedure(
    const asn1::rrc_nr::rrc_reest_request_s& request_,
    rrc_ue_context_t&                        context_,
    const byte_buffer&                       du_to_cu_container_,
    up_resource_manager&                     up_resource_mng_,
    rrc_ue_setup_proc_notifier&              rrc_ue_setup_notifier_,
    rrc_ue_reestablishment_proc_notifier&    rrc_ue_reest_notifier_,
    rrc_ue_srb_handler&                      srb_notifier_,
    rrc_ue_du_processor_notifier&            du_processor_notifier_,
    rrc_ue_context_update_notifier&          cu_cp_notifier_,
    rrc_ue_control_notifier&                 ngap_ctrl_notifier_,
    rrc_ue_nas_notifier&                     nas_notifier_,
    rrc_ue_event_manager&                    event_mng_,
    rrc_ue_logger&                           logger_) :
  reestablishment_request(request_),
  context(context_),
  du_to_cu_container(du_to_cu_container_),
  up_resource_mng(up_resource_mng_),
  rrc_ue_setup_notifier(rrc_ue_setup_notifier_),
  rrc_ue_reest_notifier(rrc_ue_reest_notifier_),
  srb_notifier(srb_notifier_),
  du_processor_notifier(du_processor_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  ngap_ctrl_notifier(ngap_ctrl_notifier_),
  nas_notifier(nas_notifier_),
  event_mng(event_mng_),
  logger(logger_)
{
}

void rrc_reestablishment_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" for old_ue={} initialized", name(), reestablishment_context.ue_index);

  // Verify if we are in conditions for a Reestablishment, or should opt for a RRC Setup.
  if (is_reestablishment_rejected()) {
    CORO_AWAIT(handle_rrc_reestablishment_fallback());
    logger.log_debug("\"{}\" for old_ue={} finalized", name(), reestablishment_context.ue_index);
    CORO_EARLY_RETURN();
  }

  // Transfer old UE context to new UE context. If it fails, resort to fallback.
  CORO_AWAIT_VALUE(context_transfer_success,
                   cu_cp_notifier.on_ue_transfer_required(context.ue_index, reestablishment_context.ue_index));
  if (not context_transfer_success) {
    CORO_AWAIT(handle_rrc_reestablishment_fallback());
    logger.log_debug("\"{}\" for old_ue={} finalized", name(), reestablishment_context.ue_index);
    CORO_EARLY_RETURN();
  }

  // Accept RRC Reestablishment Request by sending RRC Reestablishment
  // Note: From this point we should guarantee that a Reestablishment will be performed.

  // transfer reestablishment context and update security keys
  transfer_reestablishment_context_and_update_keys();

  // create SRB1
  create_srb1();

  // create new transaction for RRC Reestablishment
  transaction =
      event_mng.transactions.create_transaction(std::chrono::milliseconds(context.cfg.rrc_procedure_timeout_ms));

  // send RRC Reestablishment to UE
  send_rrc_reestablishment();

  // Await UE response
  CORO_AWAIT(transaction);

  if (transaction.has_response()) {
    context.state = rrc_state::connected;

    // Notify DU Processor to start a Reestablishment Context Modification Routine
    CORO_AWAIT_VALUE(context_modification_success,
                     du_processor_notifier.on_rrc_reestablishment_context_modification_required(context.ue_index));

    // trigger UE context release at AMF in case of failure
    if (not context_modification_success) {
      logger.log_debug(
          "\"{}\" for old_ue={} failed. Requesting UE context release", name(), reestablishment_context.ue_index);
      // Release the old UE
      ue_context_release_request.ue_index = context.ue_index;
      ue_context_release_request.cause    = ngap_cause_radio_network_t::unspecified;
      CORO_AWAIT(ngap_ctrl_notifier.on_ue_context_release_request(ue_context_release_request));
    } else {
      logger.log_debug("\"{}\" for old_ue={} finalized", name(), reestablishment_context.ue_index);
    }

  } else {
    logger.log_warning("\"{}\" for old_ue={} timed out after {}ms",
                       name(),
                       reestablishment_context.ue_index,
                       context.cfg.rrc_procedure_timeout_ms);
    logger.log_debug("\"{}\" for old_ue={} failed", name(), reestablishment_context.ue_index);
  }

  // Notify CU-CP to remove the old UE
  cu_cp_notifier.on_ue_removal_required(reestablishment_context.ue_index);

  CORO_RETURN();
}

async_task<void> rrc_reestablishment_procedure::handle_rrc_reestablishment_fallback()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    logger.log_debug("RRCReestablishmentRequest for old_ue={} rejected. Starting RRC Setup Procedure",
                     reestablishment_context.ue_index);

    // Reject RRC Reestablishment Request by sending RRC Setup
    CORO_AWAIT(launch_async<rrc_setup_procedure>(context,
                                                 asn1::rrc_nr::establishment_cause_e::mt_access,
                                                 du_to_cu_container,
                                                 rrc_ue_setup_notifier,
                                                 srb_notifier,
                                                 nas_notifier,
                                                 event_mng,
                                                 logger));

    if (reestablishment_context.ue_index != ue_index_t::invalid and !reestablishment_context.old_ue_fully_attached) {
      // The UE exists but still has not established an SRB2 and DRB. Request the release of the old UE.
      logger.log_debug("old_ue={} was not fully attached yet. Requesting UE context release",
                       reestablishment_context.ue_index);
      ue_context_release_request.ue_index = reestablishment_context.ue_index;
      ue_context_release_request.cause    = ngap_cause_radio_network_t::unspecified;
      CORO_AWAIT(ngap_ctrl_notifier.on_ue_context_release_request(ue_context_release_request));
    }

    CORO_RETURN();
  });
}

bool rrc_reestablishment_procedure::get_and_verify_reestablishment_context()
{
  bool security_context_valid = false;

  // Notify CU-CP about the RRC Reestablishment Request to get old RRC UE context
  reestablishment_context =
      cu_cp_notifier.on_rrc_reestablishment_request(reestablishment_request.rrc_reest_request.ue_id.pci,
                                                    to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti),
                                                    context.ue_index);

  // check if reestablishment context exists
  if (reestablishment_context.ue_index == ue_index_t::invalid) {
    logger.log_debug("Reestablishment context for UE with pci={} and rnti={} not found",
                     reestablishment_request.rrc_reest_request.ue_id.pci,
                     to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti));
  } else {
    // verify security context
    security_context_valid = verify_security_context();
  }

  return security_context_valid;
}

bool rrc_reestablishment_procedure::is_reestablishment_rejected()
{
  return context.cfg.force_reestablishment_fallback or /* reject because of configuration */
         reestablishment_request.rrc_reest_request.reest_cause.value ==
             asn1::rrc_nr::reest_cause_opts::recfg_fail or /* reject because we can't recover if a reconfiguration
                                                              failed */
         !get_and_verify_reestablishment_context(); /* reject when reestablishment context is missing or verification
                                                       fails */
}

bool rrc_reestablishment_procedure::verify_security_context()
{
  bool valid = false;

  // Get RX short MAC
  security::sec_short_mac_i short_mac = {};
  uint16_t short_mac_int              = htons(reestablishment_request.rrc_reest_request.ue_id.short_mac_i.to_number());
  memcpy(short_mac.data(), &short_mac_int, 2);

  // Get packed varShortMAC-Input
  asn1::rrc_nr::var_short_mac_input_s var_short_mac_input = {};
  var_short_mac_input.source_pci                          = reestablishment_request.rrc_reest_request.ue_id.pci;
  var_short_mac_input.target_cell_id.from_number(context.cell.cgi.nci);
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

  // Verify ShortMAC-I
  if (reestablishment_context.sec_context.sel_algos.algos_selected) {
    security::sec_as_config source_as_config =
        reestablishment_context.sec_context.get_as_config(security::sec_domain::rrc);
    valid = security::verify_short_mac(short_mac, var_short_mac_input_packed, source_as_config);
    logger.log_debug("Received RRC re-establishment request. short_mac_valid={}", valid);
  } else {
    logger.log_warning("Received RRC re-establishment request, but old UE does not have valid security context");
  }

  return valid;
}

void rrc_reestablishment_procedure::transfer_reestablishment_context_and_update_keys()
{
  // store capabilities if available
  if (reestablishment_context.capabilities.has_value()) {
    context.capabilities = reestablishment_context.capabilities.value();
  }

  // Transfer UP context from old UE
  up_resource_mng.set_up_context(reestablishment_context.up_ctx);

  // Update security keys
  // freq_and_timing must be present, otherwise the RRC UE would've never been created
  uint32_t ssb_arfcn  = context.cfg.meas_timings.begin()->freq_and_timing.value().carrier_freq;
  context.sec_context = reestablishment_context.sec_context;
  context.sec_context.horizontal_key_derivation(context.cell.pci, ssb_arfcn);
  logger.log_debug("Refreshed keys horizontally. pci={} ssb-arfcn={}", context.cell.pci, ssb_arfcn);
}

void rrc_reestablishment_procedure::create_srb1()
{
  // create SRB1
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index     = context.ue_index;
  srb1_msg.old_ue_index = reestablishment_context.ue_index;
  srb1_msg.srb_id       = srb_id_t::srb1;
  srb1_msg.pdcp_cfg     = {}; // TODO: Get SRB1 PDCP config of the old UE context.
  srb_notifier.create_srb(srb1_msg);

  // activate SRB1 PDCP security
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
