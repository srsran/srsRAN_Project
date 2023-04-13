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

#include "../du_ue/du_ue_manager.h"
#include "srsran/du_manager/du_manager_params.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

class du_disconnect_procedure
{
public:
  du_disconnect_procedure(du_manager_params& params, du_ue_manager& ue_mng);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  du_manager_params& params;
  du_ue_manager&     ue_mng;

  unique_timer              disconnect_timer;
  std::chrono::milliseconds wait_time = std::chrono::milliseconds{5000};
};

} // namespace srs_du
} // namespace srsran