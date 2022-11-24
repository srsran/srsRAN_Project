/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_impl.h"
#include "srsgnb/gtpu/gtpu_demux_factory.h"

using namespace srsgnb;
using namespace srs_cu_up;

void assert_cu_up_configuration_valid(const cu_up_configuration& cfg)
{
  srsgnb_assert(cfg.cu_up_executor != nullptr, "Invalid CU-UP executor");
  srsgnb_assert(cfg.e1_notifier != nullptr, "Invalid E1 notifier");
}

cu_up::cu_up(const cu_up_configuration& config_) : cfg(config_), main_ctrl_loop(128)
{
  assert_cu_up_configuration_valid(cfg);

  // Create layers
  ngu_demux = create_gtpu_demux();
  ue_mng    = std::make_unique<ue_manager>(logger, timers, *ngu_demux);

  // connect event notifier to layers
  // f1c_ev_notifier.connect_cu_cp(*this);
}

e1ap_bearer_context_setup_response_message
cu_up::handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request_message& msg)
{
  e1ap_bearer_context_setup_response_message response = {};
  response.ue_index                                   = INVALID_UE_INDEX;
  response.success                                    = false;

  // 1. Create new UE context
  ue_context* ue_ctxt = ue_mng->add_ue();
  if (ue_ctxt == nullptr) {
    logger.error("Could not create UE context");
    return response;
  }
  logger.info("UE Created (ue_index={})", ue_ctxt->get_index());

  // 2. Handle bearer context setup request
  for (auto it = msg.request.ng_ran_bearer_context_setup_request().begin();
       it != msg.request.ng_ran_bearer_context_setup_request().end();
       ++it) {
    const auto& pdu_session_list = it->value().pdu_session_res_to_setup_list();

    for (const auto& pdu_session : pdu_session_list) {
      pdu_session_setup_result result = ue_ctxt->setup_pdu_session(pdu_session);
    }
  }

  // 3. Create response
  response.ue_index = ue_ctxt->get_index();

  return response;
}

void cu_up::on_e1_connection_establish()
{
  e1_connected = true;
}

void cu_up::on_e1_connection_drop()
{
  e1_connected = false;
}
