/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_processor_impl.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_factory.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_up_processor_impl::cu_up_processor_impl(const cu_up_processor_config_t cu_up_processor_config_,
                                           e1ap_message_notifier&         e1ap_notifier_,
                                           e1ap_cu_cp_notifier&           cu_cp_notifier_,
                                           common_ue_manager&             ue_mng_,
                                           cu_up_task_scheduler&          task_sched_,
                                           task_executor&                 ctrl_exec_) :
  cfg(cu_up_processor_config_),
  e1ap_notifier(e1ap_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  ue_mng(ue_mng_),
  task_sched(task_sched_),
  ctrl_exec(ctrl_exec_)
{
  context.cu_cp_name  = cfg.name;
  context.cu_up_index = cfg.cu_up_index;

  // create e1
  e1ap = create_e1ap(e1ap_notifier,
                     e1ap_ev_notifier,
                     cu_cp_notifier,
                     ue_mng,
                     task_sched.get_timer_manager(),
                     ctrl_exec,
                     cfg.max_nof_supported_ues);
  e1ap_ev_notifier.connect_cu_up_processor(*this);
}

void cu_up_processor_impl::stop(ue_index_t ue_idx)
{
  // Cancel E1AP running tasks.
  e1ap->cancel_ue_tasks(ue_idx);
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