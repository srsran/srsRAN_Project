/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "inter_ngran_node_n2_handover_target_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

inter_ngran_node_n2_handover_target_routine::inter_ngran_node_n2_handover_target_routine(
    const cu_cp_inter_ngran_node_n2_handover_target_request& request_) :
  request(request_)
{
}

void inter_ngran_node_n2_handover_target_routine::operator()(
    coro_context<async_task<cu_cp_inter_ngran_node_n2_handover_target_response>>& ctx)
{
  CORO_BEGIN(ctx);
  CORO_RETURN(response);
}
