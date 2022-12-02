/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../f1ap_du_ue.h"
#include "srsgnb/asn1/f1ap/f1ap.h"

namespace srsgnb {
namespace srs_du {

class f1ap_du_ue_context_setup_procedure
{
public:
  f1ap_du_ue_context_setup_procedure(const asn1::f1ap::ue_context_setup_request_s& msg, f1ap_du_ue& ue_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  void send_ue_context_setup_response();
  void send_ue_context_setup_failure();

  f1ap_du_ue&                   ue;
  f1ap_ue_config_update_request du_request;

  f1ap_ue_config_update_response du_response;
};
} // namespace srs_du
} // namespace srsgnb
