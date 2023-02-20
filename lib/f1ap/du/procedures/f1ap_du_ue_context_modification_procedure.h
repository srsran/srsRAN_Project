/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ue_context/f1ap_du_ue.h"
#include "srsran/asn1/f1ap/f1ap.h"

namespace srsran {
namespace srs_du {

class f1ap_du_ue_context_modification_procedure
{
public:
  f1ap_du_ue_context_modification_procedure(const asn1::f1ap::ue_context_mod_request_s& msg, f1ap_du_ue& ue_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  void create_du_request(const asn1::f1ap::ue_context_mod_request_s& msg);
  void send_ue_context_modification_response();
  void send_ue_context_modification_failure();

  f1ap_du_ue&                    ue;
  f1ap_ue_context_update_request du_request;

  f1ap_ue_context_update_response du_response;
};

} // namespace srs_du
} // namespace srsran