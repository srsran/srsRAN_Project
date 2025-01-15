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
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

async_task<void> start_amf_connection_removal(ngap_repository&                                    ngap_db,
                                              std::unordered_map<amf_index_t, std::atomic<bool>>& amfs_connected);

/// \brief Handles the setup of the connection between the CU-CP and AMF, handling in particular the NG Setup procedure.
class amf_connection_removal_routine
{
public:
  amf_connection_removal_routine(ngap_interface* ngap_, std::atomic<bool>& amf_connected_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  ngap_interface*       ngap = nullptr;
  std::atomic<bool>&    amf_connected;
  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
