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

#include "cu_up_processor_impl.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_factory.h"

using namespace srsran;
using namespace srs_cu_cp;

/// Adapter between E1AP and CU-UP processor
class cu_up_processor_impl::e1ap_cu_up_processor_adapter : public e1ap_cu_up_processor_notifier
{
public:
  e1ap_cu_up_processor_adapter(cu_up_processor_impl& parent_, common_task_scheduler& common_task_sched_) :
    parent(parent_), common_task_sched(&common_task_sched_)
  {
  }

  void on_cu_up_e1_setup_request_received(const cu_up_e1_setup_request& msg) override
  {
    parent.handle_cu_up_e1_setup_request(msg);
  }

  bool schedule_async_task(async_task<void> task) override
  {
    return common_task_sched->schedule_async_task(std::move(task));
  }

private:
  cu_up_processor_impl&  parent;
  common_task_scheduler* common_task_sched = nullptr;
};

cu_up_processor_impl::cu_up_processor_impl(const cu_up_processor_config_t cu_up_processor_config_,
                                           e1ap_message_notifier&         e1ap_notifier_,
                                           e1ap_cu_cp_notifier&           cu_cp_notifier_,
                                           common_task_scheduler&         common_task_sched_) :
  cfg(cu_up_processor_config_),
  e1ap_notifier(e1ap_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  e1ap_ev_notifier(std::make_unique<e1ap_cu_up_processor_adapter>(*this, common_task_sched_))
{
  context.cu_cp_name  = cfg.name;
  context.cu_up_index = cfg.cu_up_index;

  // create e1
  e1ap = create_e1ap(cfg.cu_cp_cfg.e1ap,
                     context.cu_up_index,
                     e1ap_notifier,
                     *e1ap_ev_notifier,
                     cu_cp_notifier,
                     *cfg.cu_cp_cfg.services.timers,
                     *cfg.cu_cp_cfg.services.cu_cp_executor,
                     cfg.cu_cp_cfg.admission.max_nof_ues);
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

async_task<void> cu_up_processor_impl::handle_cu_cp_e1_reset_message(const cu_cp_reset& reset)
{
  return e1ap->handle_cu_cp_e1_reset_message(reset);
}

void cu_up_processor_impl::update_ue_index(ue_index_t ue_index, ue_index_t old_ue_index)
{
  e1ap->update_ue_context(ue_index, old_ue_index);
}
