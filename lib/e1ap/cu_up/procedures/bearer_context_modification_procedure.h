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

#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_up {

/// E1 Setup Procedure for the CU-UP as per TS 38.463, section TODO.
class bearer_context_modification_procedure
{
public:
  bearer_context_modification_procedure();

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "E1AP CU-UP Bearer Context Modification Procedure"; }

private:
};

} // namespace srs_cu_up
} // namespace srsran
