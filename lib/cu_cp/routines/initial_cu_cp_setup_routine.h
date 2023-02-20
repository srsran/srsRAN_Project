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

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include "srsgnb/ngap/ngap.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class initial_cu_cp_setup_routine
{
public:
  initial_cu_cp_setup_routine(const ngap_configuration&       ngap_cfg_,
                              cu_cp_ngap_control_notifier&    ngap_ctrl_notifier_,
                              ngap_cu_cp_connection_notifier& cu_cp_ngap_ev_notifier_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<ng_setup_response> send_ng_setup_request();
  void                          handle_ng_setup_response(const asn1::ngap::ng_setup_resp_s& resp);

  const ngap_configuration&       ngap_cfg;
  cu_cp_ngap_control_notifier&    ngap_ctrl_notifier;
  ngap_cu_cp_connection_notifier& cu_cp_ngap_ev_notifier;

  ng_setup_response response_msg = {};
};

} // namespace srs_cu_cp
} // namespace srsgnb
