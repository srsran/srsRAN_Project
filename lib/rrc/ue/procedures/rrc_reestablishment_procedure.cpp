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

#include "rrc_reestablishment_procedure.h"
#include "rrc_setup_procedure.h"
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
    rrc_ue_du_processor_notifier&            du_processor_notifier_,
    rrc_ue_reestablishment_notifier&         cu_cp_notifier_,
    rrc_ue_control_notifier&                 ngap_ctrl_notifier_,
    rrc_ue_nas_notifier&                     nas_notifier_,
    rrc_ue_event_manager&                    event_mng_,
    srslog::basic_logger&                    logger_) :
  reestablishment_request(request_),
  context(context_),
  du_to_cu_container(du_to_cu_container_),
  up_resource_mng(up_resource_mng_),
  rrc_ue_setup_notifier(rrc_ue_setup_notifier_),
  rrc_ue_reest_notifier(rrc_ue_reest_notifier_),
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

  logger.debug("ue={} old_ue={}: \"{}\" initialized.", context.ue_index, reestablishment_context.ue_index, name());

  // Get the reestablishment context of the UE and verify the security context
  if (!get_and_verify_reestablishment_context() or context.cfg.force_reestablishment_fallback) {
    logger.debug("ue={} old_ue={} RRCReestablishmentRequest rejected, starting RRC Setup Procedure.",
                 context.ue_index,
                 reestablishment_context.ue_index);

    // Reject RRC Reestablishment Request by sending RRC Setup
    CORO_AWAIT(launch_async<rrc_setup_procedure>(context,
                                                 asn1::rrc_nr::establishment_cause_e::mt_access,
                                                 du_to_cu_container,
                                                 rrc_ue_setup_notifier,
                                                 du_processor_notifier,
                                                 nas_notifier,
                                                 event_mng,
                                                 logger));

    if (reestablishment_context.ue_index != ue_index_t::invalid) {
      // Release the old UE
      send_ue_context_release_request(reestablishment_context.ue_index);
    }
  } else {
    // Accept RRC Reestablishment Request by sending RRC Reestablishment

    // transfer reestablishment context and update security keys
    transfer_reestablishment_context_and_update_keys();

    // create SRB1
    create_srb1();

    // create new transaction for RRC Reestablishment
    transaction = event_mng.transactions.create_transaction(rrc_reest_timeout_ms);

    // send RRC Reestablishment to UE
    send_rrc_reestablishment();

    // Notify CU-CP to transfer and remove UE contexts
    cu_cp_notifier.on_ue_transfer_required(context.ue_index, reestablishment_context.ue_index);

    // Await UE response
    CORO_AWAIT(transaction);

    if (transaction.result().has_value()) {
      context.state = rrc_state::connected;

      // Notify DU Processor to start a Reestablishment Context Modification Routine
      CORO_AWAIT_VALUE(context_modification_success,
                       du_processor_notifier.on_rrc_reestablishment_context_modification_required(context.ue_index));

      // trigger UE context release at AMF in case of failure
      if (not context_modification_success) {
        // Release the old UE
        send_ue_context_release_request(context.ue_index);
        logger.debug("ue={} old_ue={}: \"{}\" failed.", context.ue_index, reestablishment_context.ue_index, name());
      } else {
        logger.debug("ue={} old_ue={}: \"{}\" finalized.", context.ue_index, reestablishment_context.ue_index, name());
      }

    } else {
      logger.debug("ue={} \"{}\" timed out", context.ue_index, name());
      logger.debug("ue={} old_ue={}: \"{}\" failed.", context.ue_index, reestablishment_context.ue_index, name());
    }
  }

  CORO_RETURN();
}

bool rrc_reestablishment_procedure::get_and_verify_reestablishment_context()
{
  bool security_context_valid = false;

  // Notifiy CU-CP about the RRC Reestablishment Request to get old RRC UE context
  reestablishment_context =
      cu_cp_notifier.on_rrc_reestablishment_request(reestablishment_request.rrc_reest_request.ue_id.pci,
                                                    to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti),
                                                    context.ue_index);

  // check if reestablishment context exists
  if (reestablishment_context.ue_index == ue_index_t::invalid) {
    logger.debug("Reestablishment context for UE with pci={} and rnti={:#04x} not found.",
                 reestablishment_request.rrc_reest_request.ue_id.pci,
                 to_rnti(reestablishment_request.rrc_reest_request.ue_id.c_rnti));
  } else {
    // verify security context
    security_context_valid = verify_security_context();
  }

  return security_context_valid;
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

  logger.debug(var_short_mac_input_packed.begin(),
               var_short_mac_input_packed.end(),
               "Packed varShortMAC-Input. Source PCI={}, Target Cell-Id={}, Source C-RNTI={:#04x}.",
               var_short_mac_input.source_pci,
               var_short_mac_input.target_cell_id.to_number(),
               var_short_mac_input.source_c_rnti);

  // Verify ShortMAC-I
  if (reestablishment_context.sec_context.sel_algos.algos_selected) {
    security::sec_as_config source_as_config =
        reestablishment_context.sec_context.get_as_config(security::sec_domain::rrc);
    valid = security::verify_short_mac(short_mac, var_short_mac_input_packed, source_as_config);
    logger.debug("Received RRC re-establishment. short_mac_valid={}.", valid);

  } else {
    logger.warning("Received RRC re-establishment, but old UE does not have valid security context.");
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
  logger.debug("ue={} refreshed keys horizontally. pci={} ssb-arfcn={}", context.ue_index, context.cell.pci, ssb_arfcn);
}

void rrc_reestablishment_procedure::create_srb1()
{
  // create SRB1
  srb_creation_message srb1_msg{};
  srb1_msg.ue_index     = context.ue_index;
  srb1_msg.old_ue_index = reestablishment_context.ue_index;
  srb1_msg.srb_id       = srb_id_t::srb1;
  srb1_msg.pdcp_cfg     = srb1_pdcp_cfg;
  du_processor_notifier.on_create_srb(srb1_msg);

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

  // if the UE requests to reestablish RRC connection in the last serving gNB-DU, the DL RRC MESSAGE TRANSFER message
  // shall include old gNB-DU UE F1AP ID, see TS 38.401 section 8.7
  if (get_du_index_from_ue_index(reestablishment_context.ue_index) == get_du_index_from_ue_index(context.ue_index)) {
    rrc_ue_reest_notifier.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg, reestablishment_context.ue_index);
  } else {
    rrc_ue_reest_notifier.on_new_dl_dcch(srb_id_t::srb1, dl_dcch_msg);
  }
}

void rrc_reestablishment_procedure::send_ue_context_release_request(ue_index_t ue_index)
{
  ue_context_release_request.ue_index = ue_index;
  ue_context_release_request.cause    = cause_t::radio_network;
  ngap_ctrl_notifier.on_ue_context_release_request(ue_context_release_request);
}
