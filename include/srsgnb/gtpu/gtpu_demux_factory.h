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

#include "gtpu_demux.h"
#include "srsgnb/support/executors/task_executor.h"
#include <memory>

namespace srsgnb {

struct gtpu_demux_creation_request {
  task_executor* cu_up_exec = nullptr;
};

/// Creates an instance of an GTP-U demux object.
std::unique_ptr<gtpu_demux> create_gtpu_demux(const gtpu_demux_creation_request& msg);

} // namespace srsgnb
