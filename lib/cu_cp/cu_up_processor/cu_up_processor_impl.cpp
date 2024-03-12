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

#include "cu_up_processor_impl.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_factory.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_up_processor_impl::cu_up_processor_impl(const cu_up_processor_config_t cu_up_processor_config_,
                                           e1ap_message_notifier&         e1ap_notifier_,
                                           e1ap_cu_cp_notifier&           e1ap_cu_cp_notif_,
                                           cu_up_task_scheduler&          task_sched_,
                                           task_executor&                 ctrl_exec_) :
  cfg(cu_up_processor_config_),
  e1ap_notifier(e1ap_notifier_),
  e1ap_cu_cp_notif(e1ap_cu_cp_notif_),
  task_sched(task_sched_),
  ctrl_exec(ctrl_exec_)
{
  context.cu_cp_name  = cfg.name;
  context.cu_up_index = cfg.cu_up_index;

  // create e1
  e1ap = create_e1ap(e1ap_notifier,
                     e1ap_ev_notifier,
                     e1ap_cu_cp_notif,
                     task_sched.get_timer_manager(),
                     ctrl_exec,
                     cfg.max_nof_supported_ues);
  e1ap_ev_notifier.connect_cu_up_processor(*this);
}

void cu_up_processor_impl::handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& msg)
{
  if (msg.gnb_cu_up_name.has_value()) {
    context.cu_up_name = msg.gnb_cu_up_name.value();
  }
  context.id = msg.gnb_cu_up_id;

  // TODO: handle response

  // send setup response
  send_cu_up_e1_setup_response();
}

/// Sender for F1AP messages
void cu_up_processor_impl::send_cu_up_e1_setup_response()
{
  cu_up_e1_setup_response response;
  response.success        = true;
  response.gnb_cu_cp_name = context.cu_cp_name;

  e1ap->handle_cu_up_e1_setup_response(response);
}

void cu_up_processor_impl::send_cu_up_e1_setup_failure(e1ap_cause_t cause)
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