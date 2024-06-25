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

#include "handover_reconfiguration_routine.h"
#include "../../du_processor/du_processor.h"
#include "../../up_resource_manager/up_resource_manager_impl.h"

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

  // Get RRC handover reconfiguration context
  ho_reconf_ctxt = source_ue.get_rrc_ue_notifier().get_rrc_ue_handover_reconfiguration_context(request);

  generate_ue_context_modification_request();

  // Call F1AP procedure to send RRC reconfiguration to source UE via UE context modification request
  CORO_AWAIT_VALUE(ue_context_mod_response,
                   source_f1ap_ue_ctxt_mng.handle_ue_context_modification_request(ue_context_mod_request));

  if (ue_context_mod_response.success) {
    CORO_AWAIT_VALUE(
        procedure_result,
        cu_cp_handler.handle_handover_reconfiguration_sent(target_ue_index, ho_reconf_ctxt.transaction_id));
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
}
