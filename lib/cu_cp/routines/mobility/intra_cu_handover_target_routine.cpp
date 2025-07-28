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

#include "intra_cu_handover_target_routine.h"
#include "srsran/cu_cp/cu_cp_types.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

intra_cu_handover_target_routine::intra_cu_handover_target_routine(
    const cu_cp_intra_cu_handover_target_request& request_,
    e1ap_bearer_context_manager&                  e1ap_bearer_ctxt_mng_,
    f1ap_ue_context_manager&                      target_du_f1ap_ue_ctxt_mng_,
    cu_cp_ue_context_release_handler&             ue_context_release_handler_,
    cu_cp_ue_removal_handler&                     ue_removal_handler_,
    cu_cp_ue_context_manipulation_handler&        cu_cp_handler_,
    ue_manager&                                   ue_mng_,
    mobility_manager&                             mobility_mng_,
    srslog::basic_logger&                         logger_) :
  request(request_),
  e1ap_bearer_ctxt_mng(e1ap_bearer_ctxt_mng_),
  target_du_f1ap_ue_ctxt_mng(target_du_f1ap_ue_ctxt_mng_),
  ue_context_release_handler(ue_context_release_handler_),
  ue_removal_handler(ue_removal_handler_),
  cu_cp_handler(cu_cp_handler_),
  ue_mng(ue_mng_),
  mobility_mng(mobility_mng_),
  logger(logger_)
{
  bearer_context_modification_request = request.bearer_context_modification_request;
}

void intra_cu_handover_target_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (ue_mng.find_du_ue(request.target_ue_index) == nullptr) {
    logger.warning("Target UE={} got removed", request.target_ue_index);
    CORO_EARLY_RETURN();
  }
  target_ue = ue_mng.find_du_ue(request.target_ue_index);

  logger.debug("ue={}: \"{}\" started...", request.target_ue_index, name());

  // Notify RRC UE to await ReconfigurationComplete.
  CORO_AWAIT_VALUE(reconf_result,
                   target_ue->get_rrc_ue()->handle_handover_reconfiguration_complete_expected(request.transaction_id,
                                                                                              request.timeout));

  if (!reconf_result) {
    logger.warning("ue={}: \"{}\" failed", request.target_ue_index, name());
    CORO_EARLY_RETURN();
  }
  // Notify mobility manager about successful handover execution.
  mobility_mng.get_metrics_handler().aggregate_successful_handover_execution();

  {
    // Transfer old UE context (NGAP and E1AP) to new UE context and remove old UE context.
    cu_cp_handler.handle_handover_ue_context_push(request.source_ue_index, request.target_ue_index);
  }

  // Inform CU-UP about new DL tunnels.
  {
    // Get security context of target UE.
    if (!target_ue->get_security_manager().is_security_context_initialized()) {
      logger.warning(
          "ue={}: \"{}\" failed. Cause: Security context not initialized", target_ue->get_ue_index(), name());
      CORO_EARLY_RETURN();
    }

    if (!add_security_context_to_bearer_context_modification(target_ue->get_security_manager().get_up_as_config())) {
      logger.warning("ue={}: \"{}\" failed to create UE context at target DU", request.target_ue_index, name());
      CORO_AWAIT(ue_removal_handler.handle_ue_removal_request(request.target_ue_index));
      // Note: From this point the UE is removed and only the stored context can be accessed.
      CORO_EARLY_RETURN();
    }

    // Prepare Bearer Context Modification Request and call E1AP notifier.
    bearer_context_modification_request.ue_index = request.target_ue_index;

    // Call E1AP procedure and wait for BearerContextModificationResponse.
    CORO_AWAIT_VALUE(
        bearer_context_modification_response,
        e1ap_bearer_ctxt_mng.handle_bearer_context_modification_request(bearer_context_modification_request));

    // Handle Bearer Context Modification Response.
    if (!bearer_context_modification_response.success) {
      logger.warning("ue={}: \"{}\" failed to modify bearer context at target CU-UP", request.target_ue_index, name());
      {
        // Remove target UE context if Bearer Context Modification failed.
        {
          ue_context_release_command.ue_index = request.target_ue_index;
          ue_context_release_command.cause    = ngap_cause_radio_network_t::unspecified;
          CORO_AWAIT(ue_context_release_handler.handle_ue_context_release_command(ue_context_release_command));
        }

        logger.debug("ue={}: \"{}\" removed target UE context", request.target_ue_index, name());
      }

      CORO_EARLY_RETURN();
    }
  }

  {
    // Notify DU that the handover was successful and traffic can be activated. The response doesn't contain any values,
    // so it can be ignored.
    target_ue_context_modification_request.ue_index               = target_ue->get_ue_index();
    target_ue_context_modification_request.rrc_recfg_complete_ind = f1ap_rrc_recfg_complete_ind::true_value;
    CORO_AWAIT(
        target_du_f1ap_ue_ctxt_mng.handle_ue_context_modification_request(target_ue_context_modification_request));
  }

  // Remove source UE context.
  if (ue_mng.find_du_ue(request.source_ue_index) == nullptr) {
    logger.warning("Source UE={} already got removed", request.source_ue_index);
  } else {
    source_ue = ue_mng.find_du_ue(request.source_ue_index);

    ue_context_release_command.ue_index             = source_ue->get_ue_index();
    ue_context_release_command.cause                = ngap_cause_radio_network_t::unspecified;
    ue_context_release_command.requires_rrc_release = false;
    CORO_AWAIT(ue_context_release_handler.handle_ue_context_release_command(ue_context_release_command));
    logger.debug("ue={}: \"{}\" removed source UE context", ue_context_release_command.ue_index, name());
  }

  logger.debug("ue={}: \"{}\" finished successfully", request.target_ue_index, name());

  CORO_RETURN();
}

bool intra_cu_handover_target_routine::add_security_context_to_bearer_context_modification(
    const srsran::security::sec_as_config& security_cfg)
{
  // Fill security info.
  bearer_context_modification_request.security_info.emplace();
  bearer_context_modification_request.security_info->security_algorithm.ciphering_algo = security_cfg.cipher_algo;
  bearer_context_modification_request.security_info->security_algorithm.integrity_protection_algorithm =
      security_cfg.integ_algo;
  auto k_enc_buffer = byte_buffer::create(security_cfg.k_enc);
  if (not k_enc_buffer.has_value()) {
    logger.warning("Unable to allocate byte_buffer");
    return false;
  }
  bearer_context_modification_request.security_info->up_security_key.encryption_key = std::move(k_enc_buffer.value());
  if (security_cfg.k_int.has_value()) {
    auto k_int_buffer = byte_buffer::create(security_cfg.k_int.value());
    if (not k_int_buffer.has_value()) {
      logger.warning("Unable to allocate byte_buffer");
      return false;
    }
    bearer_context_modification_request.security_info->up_security_key.integrity_protection_key =
        std::move(k_int_buffer.value());
  }

  return true;
}
