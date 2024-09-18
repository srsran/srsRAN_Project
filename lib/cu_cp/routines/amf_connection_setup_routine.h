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

#include "../ngap_repository.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the setup of the connection between the CU-CP and AMF, handling in particular the NG Setup procedure.
class amf_connection_setup_routine
{
public:
  amf_connection_setup_routine(ngap_repository& ngap_db_, std::atomic<bool>& amf_connected_);

  void operator()(coro_context<async_task<bool>>& ctx);

private:
  void handle_connection_setup_result();

  ngap_repository&      ngap_db;
  std::atomic<bool>&    amf_connected;
  amf_index_t           amf_index = amf_index_t::invalid;
  ngap_interface*       ngap      = nullptr;
  srslog::basic_logger& logger;

  ngap_ng_setup_result result_msg = {};
  bool                 success    = false;
};

} // namespace srs_cu_cp
} // namespace srsran
