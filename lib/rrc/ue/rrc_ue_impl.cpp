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

#include "rrc_ue_impl.h"
#include "rrc_ue_helpers.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg.h"
#include "srsran/asn1/rrc_nr/ho_prep_info.h"
#include "srsran/support/cpu_architecture_info.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_ue_impl::rrc_ue_impl(rrc_pdu_f1ap_notifier&                 f1ap_pdu_notifier_,
                         rrc_ue_ngap_notifier&                  ngap_notifier_,
                         rrc_ue_context_update_notifier&        cu_cp_notifier_,
                         rrc_ue_measurement_notifier&           measurement_notifier_,
                         rrc_ue_cu_cp_ue_notifier&              cu_cp_ue_notifier_,
                         rrc_ue_event_notifier&                 metrics_notifier_,
                         const ue_index_t                       ue_index_,
                         const rnti_t                           c_rnti_,
                         rrc_cell_context                       cell_,
                         const rrc_ue_cfg_t&                    cfg_,
                         const byte_buffer                      du_to_cu_container_,
                         std::optional<rrc_ue_transfer_context> rrc_context) :
  context(ue_index_, c_rnti_, cell_, cfg_, rrc_context),
  f1ap_pdu_notifier(f1ap_pdu_notifier_),
  ngap_notifier(ngap_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  measurement_notifier(measurement_notifier_),
  cu_cp_ue_notifier(cu_cp_ue_notifier_),
  metrics_notifier(metrics_notifier_),
  du_to_cu_container(du_to_cu_container_),
  logger("RRC", {ue_index_, c_rnti_}),
  event_mng(std::make_unique<rrc_ue_event_manager>(cu_cp_ue_notifier.get_timer_factory()))
{
  srsran_assert(context.cell.bands.empty() == false, "Band must be present in RRC cell configuration.");

  // Update security context and keys
  if (rrc_context.has_value()) {
    if (!rrc_context.value().is_inter_cu_handover) {
      cu_cp_ue_notifier.update_security_context(rrc_context.value().sec_context);
      cu_cp_ue_notifier.perform_horizontal_key_derivation(cell_.pci, cell_.ssb_arfcn);
    }

    // Create SRBs.
    for (const auto& srb : rrc_context.value().srbs) {
      srb_creation_message srb_msg{};
      srb_msg.ue_index        = ue_index_;
      srb_msg.srb_id          = srb;
      srb_msg.enable_security = true;
      srb_msg.pdcp_cfg        = {}; // TODO: add support for non-default config.
      create_srb(srb_msg);
    }
  }
}

rrc_ue_impl::~rrc_ue_impl() {}

void rrc_ue_impl::create_srb(const srb_creation_message& msg)
{
  logger.log_debug("Creating {}", msg.srb_id);

  // create adapter objects and PDCP bearer as needed
  if (msg.srb_id == srb_id_t::srb0) {
    // SRB0 is already set up
    return;
  } else if (msg.srb_id <= srb_id_t::srb2) {
    auto&    cpu_desc  = cpu_architecture_info::get();
    uint32_t nof_cores = cpu_desc.get_host_nof_available_cpus();

    // create PDCP entity for this SRB
    context.srbs.emplace(std::piecewise_construct,
                         std::forward_as_tuple(msg.srb_id),
                         std::forward_as_tuple(msg.ue_index,
                                               msg.srb_id,
                                               cu_cp_ue_notifier.get_timer_factory(),
                                               cu_cp_ue_notifier.get_executor(),
                                               nof_cores));
    auto& srb_context = context.srbs.at(msg.srb_id);

    if (msg.srb_id == srb_id_t::srb2 || msg.enable_security) {
      security::sec_as_config sec_cfg = cu_cp_ue_notifier.get_rrc_as_config();
      srb_context.enable_full_security(security::truncate_config(sec_cfg));
    }

  } else {
    logger.log_error("Couldn't create {}", msg.srb_id);
  }
}

static_vector<srb_id_t, MAX_NOF_SRBS> rrc_ue_impl::get_srbs()
{
  static_vector<srb_id_t, MAX_NOF_SRBS> srb_ids;
  for (const auto& srb_ctxt : context.srbs) {
    srb_ids.push_back(srb_ctxt.first);
  }

  return srb_ids;
}

rrc_state rrc_ue_impl::get_rrc_state() const
{
  return context.state;
}

void rrc_ue_impl::cancel_handover_reconfiguration_transaction(uint8_t transaction_id)
{
  // If the UE is awaiting a RRC Reconfiguration Complete for an ongoing handover, cancel the transaction.
  logger.log_debug(
      "Received a RRC Reestablishment Request during an ongoing handover. Cancelling the handover transaction");
  if (not event_mng->transactions.cancel_transaction(transaction_id)) {
    logger.log_warning("Unexpected RRC transaction id={}", transaction_id);
  }
}

void rrc_ue_impl::on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg)
{
  send_dl_ccch(dl_ccch_msg);
}

void rrc_ue_impl::on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg)
{
  send_dl_dcch(srb_id, dl_dcch_msg);
}

void rrc_ue_impl::on_new_as_security_context()
{
  srsran_sanity_check(context.srbs.find(srb_id_t::srb1) != context.srbs.end(),
                      "Attempted to configure security, but there is no interface to PDCP");

  context.srbs.at(srb_id_t::srb1)
      .enable_rx_security(
          security::integrity_enabled::on, security::ciphering_enabled::off, cu_cp_ue_notifier.get_rrc_128_as_config());
  context.srbs.at(srb_id_t::srb1)
      .enable_tx_security(
          security::integrity_enabled::on, security::ciphering_enabled::off, cu_cp_ue_notifier.get_rrc_128_as_config());

  cu_cp_ue_notifier.enable_security();
}

byte_buffer rrc_ue_impl::get_packed_handover_preparation_message()
{
  struct ho_prep_info_s ho_prep;
  ho_prep_info_ies_s&   ies = ho_prep.crit_exts.set_c1().set_ho_prep_info();

  if (not context.capabilities_list.has_value()) {
    logger.log_error("No UE capabilities stored. Handover preparation message can't be generated.");
    return {}; // no capabilities present, return empty buffer.
  }
  ies.ue_cap_rat_list = *context.capabilities_list;

  // TODO fill source and as configs.
  return pack_into_pdu(ho_prep, "handover preparation info");
}

void rrc_ue_impl::on_ue_release_required(const ngap_cause_t& cause)
{
  cu_cp_ue_notifier.schedule_async_task(launch_async([this, cause](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT(cu_cp_notifier.on_ue_release_required({context.ue_index, {}, cause}));

    CORO_RETURN();
  }));
}
