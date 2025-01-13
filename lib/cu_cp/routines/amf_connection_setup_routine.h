/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../ngap_repository.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

async_task<bool> start_amf_connection_setup(ngap_repository&                                    ngap_db,
                                            std::unordered_map<amf_index_t, std::atomic<bool>>& amfs_connected);

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
