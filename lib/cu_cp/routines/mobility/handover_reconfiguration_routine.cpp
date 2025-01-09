/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    const rrc_reconfiguration_procedure_request& request_,
    const ue_index_t&                            target_ue_index_,
    cu_cp_ue&                                    source_ue_,
    f1ap_ue_context_manager&                     source_f1ap_ue_ctxt_mng_,
    cu_cp_ue_context_manipulation_handler&       cu_cp_handler_,
    srslog::basic_logger&                        logger_) :
  request(request_),
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

  logger.debug("source_ue={} target_ue={}: \"{}\" initialized", source_ue.get_ue_index(), target_ue_index, name());

  // Get RRC handover reconfiguration context.
  ho_reconf_ctxt = source_ue.get_rrc_ue()->get_rrc_ue_handover_reconfiguration_context(request);

  generate_ue_context_modification_request();

  // Call F1AP procedure to send RRC reconfiguration to source UE via UE context modification request.
  CORO_AWAIT_VALUE(ue_context_mod_response,
                   source_f1ap_ue_ctxt_mng.handle_ue_context_modification_request(ue_context_mod_request));

  if (ue_context_mod_response.success) {
    // Initialize UE release timer.
    initialize_ue_release_timer();

    CORO_AWAIT_VALUE(procedure_result,
                     cu_cp_handler.handle_handover_reconfiguration_sent(
                         target_ue_index, ho_reconf_ctxt.transaction_id, ue_release_timeout_ms));
  } else {
    logger.debug(
        "source_ue={} target_ue={}: UE context modification failed", source_ue.get_ue_index(), target_ue_index);
  }

  if (procedure_result) {
    logger.debug("source_ue={} target_ue={}: \"{}\" finalized", source_ue.get_ue_index(), target_ue_index, name());
  } else {
    logger.debug("source_ue={} target_ue={}: \"{}\" failed", source_ue.get_ue_index(), target_ue_index, name());
  }

  CORO_RETURN(procedure_result);
}

void handover_reconfiguration_routine::generate_ue_context_modification_request()
{
  ue_context_mod_request.ue_index                 = source_ue.get_ue_index();
  ue_context_mod_request.drbs_to_be_released_list = source_ue.get_up_resource_manager().get_drbs();
  ue_context_mod_request.rrc_container            = ho_reconf_ctxt.rrc_ue_handover_reconfiguration_pdu.copy();

  // Stop data transmission for the UE on the source DU (see TS 38.473 section 8.3.4.2).
  ue_context_mod_request.tx_action_ind = f1ap_tx_action_ind::stop;
}

void handover_reconfiguration_routine::initialize_ue_release_timer()
{
  // Unpack MasterCellGroup to extract T304.
  asn1::rrc_nr::cell_group_cfg_s cell_group_cfg;
  asn1::cbit_ref                 bref(request.non_crit_ext->master_cell_group);
  if (cell_group_cfg.unpack(bref) != asn1::SRSASN_SUCCESS) {
    report_fatal_error("Failed to unpack MasterCellGroupCfg");
  }
  unsigned t304_ms = cell_group_cfg.sp_cell_cfg.recfg_with_sync.t304.to_number();

  // Unpack SIB1 to extract T311.
  asn1::rrc_nr::sib1_s sib1_msg;
  asn1::cbit_ref       bref2(request.non_crit_ext->ded_sib1_delivery);
  if (sib1_msg.unpack(bref2) != asn1::SRSASN_SUCCESS) {
    report_fatal_error("Failed to unpack SIB1");
  }
  unsigned t311_ms = sib1_msg.ue_timers_and_consts.t311.to_number();

  ue_release_timeout_ms =
      std::chrono::milliseconds{t304_ms + t311_ms + /*We add 1s of extra time for the UE to reestablish*/ 1000};

  cu_cp_handler.initialize_ue_release_timer(
      source_ue.get_ue_index(),
      ue_release_timeout_ms,
      cu_cp_ue_context_release_request{source_ue.get_ue_index(),
                                       source_ue.get_up_resource_manager().get_pdu_sessions(),
                                       ngap_cause_radio_network_t::ho_fail_in_target_5_gc_ngran_node_or_target_sys});
}
