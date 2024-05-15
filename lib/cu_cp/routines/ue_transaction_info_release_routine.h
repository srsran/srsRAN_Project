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

#include "../ue_manager/ue_manager_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ue_transaction_info_release_routine
{
public:
  ue_transaction_info_release_routine(const std::vector<ue_index_t>& ues_to_release,
                                      ue_manager&                    ue_mng,
                                      cu_cp_ue_removal_handler&      ue_rem_handler);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  void launch_ue_removal(ue_index_t ue_idx);

  const std::vector<ue_index_t> ues_to_release;
  ue_manager&                   ue_mng;
  cu_cp_ue_removal_handler&     ue_rem_handler;

  unsigned          ues_remaining_count{0};
  manual_event_flag all_ues_reset;
};

} // namespace srs_cu_cp
} // namespace srsran