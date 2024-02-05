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
#include "../../du_processor/du_processor_impl_interface.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::rrc_nr;

handover_reconfiguration_routine::handover_reconfiguration_routine(
    const rrc_reconfiguration_procedure_request& request_,
    du_ue&                                       source_ue_,
    du_ue&                                       target_ue_,
    srslog::basic_logger&                        logger_) :
  request(request_), source_ue(source_ue_), target_ue(target_ue_), logger(logger_)
{
  srsran_assert(
      source_ue.get_ue_index() != ue_index_t::invalid, "Invalid source UE index {}", source_ue.get_ue_index());
}

void handover_reconfiguration_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug(
      "source_ue={} target_ue={}: \"{}\" initialized", source_ue.get_ue_index(), target_ue.get_ue_index(), name());

  // Send RRC Reconfiguration to source UE
  transaction_id = send_rrc_reconfiguration();

  CORO_AWAIT_VALUE(procedure_result,
                   target_ue.get_rrc_ue_notifier().on_handover_reconfiguration_complete_expected(transaction_id));

  if (procedure_result) {
    logger.debug(
        "source_ue={} target_ue={}: \"{}\" finalized", source_ue.get_ue_index(), target_ue.get_ue_index(), name());
  } else {
    logger.debug(
        "source_ue={} target_ue={}: \"{}\" failed", source_ue.get_ue_index(), target_ue.get_ue_index(), name());
  }

  CORO_RETURN(procedure_result);
}

uint8_t handover_reconfiguration_routine::send_rrc_reconfiguration()
{
  return source_ue.get_rrc_ue_notifier().on_handover_reconfiguration_request(request);
}
