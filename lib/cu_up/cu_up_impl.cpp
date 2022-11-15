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

using namespace srsgnb;
using namespace srs_cu_up;

void assert_cu_up_configuration_valid(const cu_up_configuration& cfg)
{
  srsgnb_assert(cfg.cu_up_executor != nullptr, "Invalid CU-UP executor");
  srsgnb_assert(cfg.e1_notifier != nullptr, "Invalid E1 notifier");
}

cu_up::cu_up(const cu_up_configuration& config_) : cfg(config_), main_ctrl_loop(128), ue_mng(logger)
{
  assert_cu_up_configuration_valid(cfg);

  // Create layers
  // .. todo ..

  // connect event notifier to layers
  // f1c_ev_notifier.connect_cu_cp(*this);
}

e1_message_handler& cu_up::get_e1_message_handler()
{
  return *e1;
}

bearer_context_setup_response_message
cu_up::handle_bearer_context_setup_request(const bearer_context_setup_request_message& msg)
{
  bearer_context_setup_response_message response = {};
  response.ue_index                              = INVALID_UE_INDEX;
  response.success                               = false;

  // 1. Create new UE context
  ue_context* ue_ctxt = ue_mng.add_ue();
  if (ue_ctxt == nullptr) {
    logger.error("Could not create UE context");
    return response;
  }
  logger.info("UE Created (ue_index={})", ue_ctxt->get_index());

  // 2. Handle bearer context setup request
  if (msg.request.is_id_valid(0)) {
    const auto& pdu_session_list = msg.request.get_value(0).pdu_session_res_to_setup_list();
    for (size_t i = 0; i < pdu_session_list.size(); ++i) {
      // TODO: call handle_pdu_session_setup_request()
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

// private