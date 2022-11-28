/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/scheduler/scheduler_slot_handler.h"
#include "srsgnb/support/format_utils.h"

namespace srsgnb {

class scheduler_result_logger
{
public:
  scheduler_result_logger() : logger(srslog::fetch_basic_logger("MAC")) {}

  void log(const sched_result& result);

private:
  void log_debug(const sched_result& result);

  void log_info(const sched_result& result);

  srslog::basic_logger& logger;
  fmt::memory_buffer    fmtbuf;
};

} // namespace srsgnb
