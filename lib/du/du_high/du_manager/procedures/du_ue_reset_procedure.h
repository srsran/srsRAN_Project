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

#include "srsran/support/async/async_task.h"
#include "srsran/support/async/manual_event.h"
#include <srsran/ran/du_types.h>
#include <vector>

namespace srsran {
namespace srs_du {

class du_ue_manager;
struct du_manager_params;

/// Task that handles the removal of several UEs in the DU.
class du_ue_reset_procedure
{
public:
  du_ue_reset_procedure(const std::vector<du_ue_index_t>& ues_to_reset_,
                        du_ue_manager&                    ue_mng_,
                        const du_manager_params&          du_params);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  void launch_rem_ues_tasks();

  async_task<void> launch_ue_rem_task(du_ue_index_t ue_index);

  std::vector<du_ue_index_t> ues_to_reset;
  du_ue_manager&             ue_mng;
  const du_manager_params&   du_params;

  unsigned          ue_rem = 0;
  manual_event_flag complete_flag;
};
} // namespace srs_du
} // namespace srsran
