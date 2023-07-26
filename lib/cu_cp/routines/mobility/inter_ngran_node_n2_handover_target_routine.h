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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class inter_ngran_node_n2_handover_target_routine
{
public:
  inter_ngran_node_n2_handover_target_routine(const cu_cp_inter_ngran_node_n2_handover_target_request& request_);

  void operator()(coro_context<async_task<cu_cp_inter_ngran_node_n2_handover_target_response>>& ctx);

private:
  const cu_cp_inter_ngran_node_n2_handover_target_request request;
  cu_cp_inter_ngran_node_n2_handover_target_response      response = {false};
};

} // namespace srs_cu_cp
} // namespace srsran
