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

#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_du {

class du_cell_manager;
struct du_manager_config_t;

class initial_du_setup_procedure
{
public:
  initial_du_setup_procedure(const du_manager_config_t& cfg_, du_cell_manager& cell_mng_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<f1_setup_response_message> start_f1_setup_request();
  void                                  handle_f1_setup_response(const asn1::f1ap::f1_setup_resp_s& resp);

  const du_manager_config_t& cfg;
  du_cell_manager&           cell_mng;

  f1_setup_response_message response_msg = {};
};

} // namespace srs_du
} // namespace srsgnb
