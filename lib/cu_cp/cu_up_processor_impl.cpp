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

#include "cu_up_processor_impl.h"
#include "adapters/e1ap_adapters.h"
#include "adapters/ngap_adapters.h"
#include "routines/initial_cu_up_processor_setup_routine.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_factory.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_up_processor_impl::cu_up_processor_impl(const cu_up_processor_config_t             cu_up_processor_config_,
                                           cu_up_processor_cu_up_management_notifier& cu_cp_notifier_,
                                           e1ap_message_notifier&                     e1ap_notifier_,
                                           e1ap_cu_cp_notifier&                       e1ap_cu_cp_notif_,
                                           cu_up_processor_task_scheduler&            task_sched_,
                                           task_executor&                             ctrl_exec_) :
  cfg(cu_up_processor_config_),
  cu_cp_notifier(cu_cp_notifier_),
  e1ap_notifier(e1ap_notifier_),
  e1ap_cu_cp_notif(e1ap_cu_cp_notif_),
  task_sched(task_sched_)
{
  context.cu_cp_name  = cfg.name;
  context.cu_up_index = cfg.index;

  // create e1
  e1ap = create_e1ap(timer_factory{task_sched.get_timer_manager(), ctrl_exec_},
                     e1ap_notifier,
                     e1ap_ev_notifier,
                     e1ap_cu_cp_notif,
                     ctrl_exec_);
  e1ap_ev_notifier.connect_cu_up_processor(*this);
  e1ap_adapter.connect_e1ap(*e1ap);

  routine_mng = std::make_unique<cu_up_processor_routine_manager>(context, e1ap_adapter, cu_cp_notifier, task_sched);
}

void cu_up_processor_impl::start()
{
  // Start E1 setup procedure
  routine_mng->start_initial_cu_up_processor_setup_routine();
}

void cu_up_processor_impl::stop() {}

void cu_up_processor_impl::handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& msg)
{
  // TODO: Handle setup request

  // send setup response
  send_cu_up_e1_setup_response();
}

/// Sender for F1AP messages
void cu_up_processor_impl::send_cu_up_e1_setup_response()
{
  cu_up_e1_setup_response response;
  // TODO: fill message

  e1ap->handle_cu_up_e1_setup_response(response);
}

void cu_up_processor_impl::send_cu_up_e1_setup_failure(cause_t cause)
{
  cu_up_e1_setup_response response;
  response.success = false;
  response.cause   = cause;
  e1ap->handle_cu_up_e1_setup_response(response);
}

void cu_up_processor_impl::update_ue_index(ue_index_t ue_index, ue_index_t old_ue_index)
{
  e1ap->update_ue_context(ue_index, old_ue_index);
}