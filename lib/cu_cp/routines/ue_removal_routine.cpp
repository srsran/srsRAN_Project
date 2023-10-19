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

#include "ue_removal_routine.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

ue_removal_routine::ue_removal_routine(ue_index_t                      ue_index_,
                                       cu_cp_rrc_ue_removal_notifier&  rrc_du_notifier_,
                                       cu_cp_e1ap_ue_removal_notifier& e1ap_notifier_,
                                       cu_cp_f1ap_ue_removal_notifier& f1ap_notifier_,
                                       cu_cp_ngap_control_notifier&    ngap_notifier_,
                                       ue_manager&                     ue_mng_,
                                       ue_task_scheduler&              task_scheduler_,
                                       srslog::basic_logger&           logger_) :
  ue_index(ue_index_),
  rrc_du_notifier(rrc_du_notifier_),
  e1ap_notifier(e1ap_notifier_),
  f1ap_notifier(f1ap_notifier_),
  ngap_notifier(ngap_notifier_),
  ue_mng(ue_mng_),
  task_scheduler(task_scheduler_),
  logger(logger_)
{
}

void ue_removal_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", ue_index, name());

  // Remove RRC UE
  rrc_du_notifier.remove_ue(ue_index);

  // Remove UE from UE manager
  ue_mng.remove_ue(ue_index);

  // Remove Bearer Context from E1AP
  e1ap_notifier.remove_ue(ue_index);

  // Remove UE Context from F1AP
  f1ap_notifier.remove_ue(ue_index);

  // Remove UE Context from NGAP
  ngap_notifier.remove_ue(ue_index);

  // Remove pending UE tasks
  task_scheduler.clear_pending_tasks(ue_index);

  logger.debug("ue={}: \"{}\" finalized.", ue_index, name());

  CORO_RETURN();
}
