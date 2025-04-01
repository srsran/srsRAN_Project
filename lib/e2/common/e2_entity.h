/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/e2/e2_agent_dependencies.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/e2sm/e2sm_manager.h"
#include "srsran/e2/subscription/e2_subscription.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"

namespace srsran {

/// Wrapper for the E2 interface that puts function calls into its own task executor
class e2_entity final : public e2_agent
{
public:
  e2_entity(e2_agent_dependencies&& dependencies);

  // E2 Agent interface.
  void          start() override;
  void          stop() override;
  e2_interface& get_e2_interface() override { return *e2ap; }

  void on_e2_disconnection() override;

private:
  srslog::basic_logger&    logger;
  const e2ap_configuration cfg;

  // Handler for E2AP tasks.
  task_executor&            task_exec;
  fifo_async_task_scheduler main_ctrl_loop;

  std::unique_ptr<e2sm_manager>              e2sm_mngr         = nullptr;
  std::unique_ptr<e2_subscription_manager>   subscription_mngr = nullptr;
  std::unique_ptr<e2_interface>              e2ap              = nullptr;
  std::vector<std::unique_ptr<e2sm_handler>> e2sm_handlers;
};

} // namespace srsran
