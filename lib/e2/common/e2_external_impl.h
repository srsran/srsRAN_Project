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

#include "procedures/e2_setup_procedure.h"
#include "procedures/e2_subscription_setup_procedure.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm_factory.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/support/async/async_task_loop.h"
#include <map>
#include <memory>

namespace srsran {

class e2_external_impl final : public e2_interface
{
public:
  e2_external_impl(std::unique_ptr<e2_interface> decorated_e2_iface_, task_executor& task_exec_);

  void start(e2_setup_request_message& request) override;
  void stop() override;

  /// E2 connection manager functions.
  async_task<e2_setup_response_message> handle_e2_setup_request(e2_setup_request_message& request) override;

  /// E2_event_ handler functions.
  void handle_connection_loss() override;

  /// E2 message handler functions.
  void handle_message(const e2_message& msg) override;

private:
  srslog::basic_logger&         logger;
  std::unique_ptr<e2_interface> decorated_e2_iface;
  task_executor&                task_exec;

  // Handler for E2AP tasks.
  async_task_sequencer main_ctrl_loop;
};

} // namespace srsran