/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

/// Adapter between E1AP and CU-UP processor
class cu_up_processor_impl::e1ap_cu_up_processor_adapter : public e1ap_cu_up_processor_notifier
{
public:
  e1ap_cu_up_processor_adapter(cu_up_processor_impl& parent_) : parent(parent_) {}

  void on_cu_up_e1_setup_request_received(const cu_up_e1_setup_request& msg) override
  {
    parent.handle_cu_up_e1_setup_request(msg);
  }

private:
  cu_up_processor_impl& parent;
};

cu_up_processor_impl::cu_up_processor_impl(const cu_up_processor_config_t cu_up_processor_config_,
                                           e1ap_message_notifier&         e1ap_notifier_,
                                           e1ap_cu_cp_notifier&           cu_cp_notifier_) :
  cfg(cu_up_processor_config_),
  e1ap_notifier(e1ap_notifier_),
  cu_cp_notifier(cu_cp_notifier_),
  e1ap_ev_notifier(std::make_unique<e1ap_cu_up_processor_adapter>(*this))
{
  context.cu_cp_name  = cfg.name;
  context.cu_up_index = cfg.cu_up_index;

  // create e1
  e1ap = create_e1ap(cfg.cu_cp_cfg.e1ap,
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

void cu_up_processor_impl::update_ue_index(ue_index_t ue_index, ue_index_t old_ue_index)
{
  e1ap->update_ue_context(ue_index, old_ue_index);
}
