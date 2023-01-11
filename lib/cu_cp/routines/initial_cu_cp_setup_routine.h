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

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class initial_cu_cp_setup_routine
{
public:
  initial_cu_cp_setup_routine(const ngc_configuration&       ngc_cfg_,
                              ngc_connection_manager&        ngc_conn_mng_,
                              cu_cp_ngc_connection_notifier& cu_cp_ngc_ev_notifier_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<ng_setup_response_message> start_ng_setup_request();
  void                                  handle_ng_setup_response(const asn1::ngap::ng_setup_resp_s& resp);

  const ngc_configuration&       ngc_cfg;
  ngc_connection_manager&        ngc_conn_mng;
  cu_cp_ngc_connection_notifier& cu_cp_ngc_ev_notifier;

  ng_setup_response_message response_msg = {};
};

} // namespace srs_cu_cp
} // namespace srsgnb
