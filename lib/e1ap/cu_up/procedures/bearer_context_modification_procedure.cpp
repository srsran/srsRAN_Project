/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "bearer_context_modification_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_up;

bearer_context_modification_procedure::bearer_context_modification_procedure() {}

void bearer_context_modification_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  fmt::print("CU-UP bearer context modification\n");
  CORO_RETURN();
}
