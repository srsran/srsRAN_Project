/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ue_context/f1ap_du_ue.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"

namespace srsran {
namespace srs_du {

class f1ap_du_ue_manager;

/// \brief This procedure handles UE CONTEXT SETUP REQUEST as per TS38.473, Section 8.3.1.
class f1ap_du_ue_context_setup_procedure
{
public:
  f1ap_du_ue_context_setup_procedure(const asn1::f1ap::ue_context_setup_request_s& msg,
                                     f1ap_du_ue_manager&                           ue_mng_,
                                     f1ap_du_configurator&                         du_mng_,
                                     du_ue_index_t                                 ue_index_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  // Initiate UE Configuration in the DU.
  async_task<f1ap_ue_context_update_response> request_du_ue_config();

  // Send UE Context Setup Response to CU.
  void send_ue_context_setup_response();

  // Send UE Context Setup Failure to CU.
  void send_ue_context_setup_failure();

  async_task<bool> handle_rrc_container();

  const char* name() const { return "UE Context Setup"; }

  const asn1::f1ap::ue_context_setup_request_s msg;
  f1ap_du_ue_manager&                          ue_mng;
  f1ap_du_configurator&                        du_mng;
  const du_ue_index_t                          ue_index;
  srslog::basic_logger&                        logger;

  f1ap_du_ue* ue = nullptr;

  std::optional<f1ap_ue_context_creation_response> du_ue_create_response;
  f1ap_ue_context_update_response                  du_ue_cfg_response;
};
} // namespace srs_du
} // namespace srsran
