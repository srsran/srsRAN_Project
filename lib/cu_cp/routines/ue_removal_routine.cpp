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

#include "ue_removal_routine.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

ue_removal_routine::ue_removal_routine(ue_index_t                           ue_index_,
                                       rrc_ue_handler&                      rrc_du_notifier_,
                                       e1ap_bearer_context_removal_handler* e1ap_removal_handler_,
                                       f1ap_ue_context_removal_handler&     f1ap_removal_handler_,
                                       ngap_ue_context_removal_handler&     ngap_removal_handler_,
                                       nrppa_ue_context_removal_handler*    nrppa_removal_handler_,
                                       ue_manager&                          ue_mng_,
                                       srslog::basic_logger&                logger_) :
  ue_index(ue_index_),
  rrc_du_notifier(rrc_du_notifier_),
  e1ap_removal_handler(e1ap_removal_handler_),
  f1ap_removal_handler(f1ap_removal_handler_),
  ngap_removal_handler(ngap_removal_handler_),
  nrppa_removal_handler(nrppa_removal_handler_),
  ue_mng(ue_mng_),
  logger(logger_)
{
}

void ue_removal_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" started...", ue_index, name());

  // Remove RRC UE.
  rrc_du_notifier.remove_ue(ue_index);

  // Remove Bearer Context from E1AP.
  if (e1ap_removal_handler != nullptr) {
    e1ap_removal_handler->remove_bearer_context(ue_index);
  }

  // Remove UE Context from F1AP.
  f1ap_removal_handler.remove_ue_context(ue_index);

  // Remove UE Context from NGAP.
  ngap_removal_handler.remove_ue_context(ue_index);

  // Remove UE Context from NRPPa.
  if (nrppa_removal_handler != nullptr) {
    nrppa_removal_handler->remove_ue_context(ue_index);
  }

  // Remove UE from UE manager.
  ue_mng.remove_ue(ue_index);

  logger.info("ue={}: \"{}\" finished successfully", ue_index, name());

  CORO_RETURN();
}
