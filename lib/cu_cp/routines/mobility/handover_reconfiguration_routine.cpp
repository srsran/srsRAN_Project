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

#include "handover_reconfiguration_routine.h"
#include "../../du_processor/du_processor.h"
#include "../../up_resource_manager/up_resource_manager_impl.h"
#include "srsran/asn1/rrc_nr/cell_group_config.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

handover_reconfiguration_routine::handover_reconfiguration_routine(
    const rrc_reconfiguration_procedure_request&    request_,
    const e1ap_bearer_context_modification_request& target_bearer_context_modification_request_,
    const ue_index_t&                               target_ue_index_,
    cu_cp_ue&                                       source_ue_,
    f1ap_ue_context_manager&                        source_f1ap_ue_ctxt_mng_,
    cu_cp_ue_context_manipulation_handler&          cu_cp_handler_,
    srslog::basic_logger&                           logger_) :
  request(request_),
  target_bearer_context_modification_request(target_bearer_context_modification_request_),
  target_ue_index(target_ue_index_),
  source_ue(source_ue_),
  source_f1ap_ue_ctxt_mng(source_f1ap_ue_ctxt_mng_),
  cu_cp_handler(cu_cp_handler_),
  logger(logger_)
{
  srsran_assert(
      source_ue.get_ue_index() != ue_index_t::invalid, "Invalid source UE index {}", source_ue.get_ue_index());
}

void handover_reconfiguration_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("source_ue={} target_ue={}: \"{}\" started...", source_ue.get_ue_index(), target_ue_index, name());

  // Get RRC handover reconfiguration context.
  ho_reconf_ctxt = source_ue.get_rrc_ue()->get_rrc_ue_handover_reconfiguration_context(request);

  generate_ue_context_modification_request();

  // Call F1AP procedure to send RRC reconfiguration to source UE via UE context modification request.
  CORO_AWAIT_VALUE(ue_context_mod_response,
                   source_f1ap_ue_ctxt_mng.handle_ue_context_modification_request(ue_context_mod_request));

  if (!ue_context_mod_response.success) {
    logger.debug(
        "source_ue={} target_ue={}: UE context modification failed", source_ue.get_ue_index(), target_ue_index);
    logger.debug("source_ue={} target_ue={}: \"{}\" failed", source_ue.get_ue_index(), target_ue_index, name());
    CORO_EARLY_RETURN(false);
  }

  // Initialize UE release timer for source UE.
  initialize_handover_ue_release_timer(source_ue.get_ue_index());

  // Notify CU-CP that RRC reconfiguration was sent.
  cu_cp_handler.handle_handover_reconfiguration_sent({target_ue_index,
                                                      source_ue.get_ue_index(),
                                                      (uint8_t)ho_reconf_ctxt.transaction_id,
                                                      target_ue_release_timeout,
                                                      target_bearer_context_modification_request});

  // Store handover context in case of for possible re-establishment.
  logger.debug("ue={}: Storing handover context", source_ue.get_ue_index());
  cu_cp_ue_handover_context ue_ho_ctxt;
  ue_ho_ctxt.target_ue_index             = target_ue_index;
  ue_ho_ctxt.rrc_reconfig_transaction_id = (uint8_t)ho_reconf_ctxt.transaction_id;
  source_ue.get_ho_context()             = ue_ho_ctxt;

  logger.debug(
      "source_ue={} target_ue={}: \"{}\" finished successfully", source_ue.get_ue_index(), target_ue_index, name());

  CORO_RETURN(true);
}

void handover_reconfiguration_routine::generate_ue_context_modification_request()
{
  ue_context_mod_request.ue_index                 = source_ue.get_ue_index();
  ue_context_mod_request.drbs_to_be_released_list = source_ue.get_up_resource_manager().get_drbs();
  ue_context_mod_request.rrc_container            = ho_reconf_ctxt.rrc_ue_handover_reconfiguration_pdu.copy();

  // Stop data transmission for the UE on the source DU (see TS 38.473 section 8.3.4.2).
  ue_context_mod_request.tx_action_ind = f1ap_tx_action_ind::stop;
}

void handover_reconfiguration_routine::initialize_handover_ue_release_timer(ue_index_t ue_index)
{
  // Unpack MasterCellGroup to extract T304.
  asn1::rrc_nr::cell_group_cfg_s cell_group_cfg;
  asn1::cbit_ref                 bref(request.non_crit_ext->master_cell_group);
  if (cell_group_cfg.unpack(bref) != asn1::SRSASN_SUCCESS) {
    report_fatal_error("Failed to unpack MasterCellGroupCfg");
  }
  unsigned t304_ms = cell_group_cfg.sp_cell_cfg.recfg_with_sync.t304.to_number();
  target_ue_release_timeout =
      std::chrono::milliseconds{t304_ms + /*We add 1s of extra time for the UE to reestablish*/ 1000};

  // Unpack SIB1 to extract T311.
  asn1::rrc_nr::sib1_s sib1_msg;
  asn1::cbit_ref       bref2(request.non_crit_ext->ded_sib1_delivery);
  if (sib1_msg.unpack(bref2) != asn1::SRSASN_SUCCESS) {
    report_fatal_error("Failed to unpack SIB1");
  }
  unsigned t301_ms = sib1_msg.ue_timers_and_consts.t301.to_number();
  unsigned t311_ms = sib1_msg.ue_timers_and_consts.t311.to_number();

  cu_cp_handler.initialize_handover_ue_release_timer(
      ue_index,
      std::chrono::milliseconds{t301_ms + t304_ms + t311_ms +
                                /*We add 1s of extra time for the UE to reestablish*/ 1000},
      cu_cp_ue_context_release_request{ue_index,
                                       source_ue.get_up_resource_manager().get_pdu_sessions(),
                                       ngap_cause_radio_network_t::ho_fail_in_target_5_gc_ngran_node_or_target_sys});
}
