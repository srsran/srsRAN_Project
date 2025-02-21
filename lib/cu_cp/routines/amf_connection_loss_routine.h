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

#include "../ngap_repository.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

async_task<bool> start_amf_reconnection(ngap_interface& ngap, timer_factory timers);

/// \brief Handles the reconnection between the CU-CP and AMF.
class amf_connection_loss_routine
{
public:
  amf_connection_loss_routine(ngap_interface& ngap_, timer_factory timers);

  void operator()(coro_context<async_task<bool>>& ctx);

private:
  ngap_interface&       ngap;
  srslog::basic_logger& logger;

  unique_timer amf_tnl_connection_retry_timer;

  ngap_ng_setup_result result_msg = {};
  bool                 success    = false;
};

} // namespace srs_cu_cp
} // namespace srsran
