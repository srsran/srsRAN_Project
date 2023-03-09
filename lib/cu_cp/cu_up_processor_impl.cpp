/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
                                           cu_up_processor_task_scheduler&            task_sched_,
                                           task_executor&                             ctrl_exec_) :
  cfg(cu_up_processor_config_), cu_cp_notifier(cu_cp_notifier_), e1ap_notifier(e1ap_notifier_), task_sched(task_sched_)
{
  context.cu_cp_name  = cfg.name;
  context.cu_up_index = cfg.index;

  // create e1
  e1ap = create_e1ap(
      timer_factory{task_sched.get_timer_manager(), ctrl_exec_}, e1ap_notifier, e1ap_ev_notifier, ctrl_exec_);
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
