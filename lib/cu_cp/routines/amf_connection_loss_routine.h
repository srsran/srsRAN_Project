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

#include "../cu_cp_controller/cu_cp_controller.h"
#include "../ue_manager/ue_manager_impl.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the reconnection between the CU-CP and AMF.
class amf_connection_loss_routine
{
public:
  amf_connection_loss_routine(const amf_index_t                 amf_index_,
                              const cu_cp_configuration&        cu_cp_cfg_,
                              std::vector<plmn_identity>&       plmns_,
                              du_processor_repository&          du_db_,
                              cu_cp_ue_context_release_handler& ue_release_handler_,
                              ue_manager&                       ue_mng_,
                              cu_cp_controller&                 controller_,
                              srslog::basic_logger&             logger_);

  static std::string name() { return "AMF Connection Loss Routine"; }

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const amf_index_t                 amf_index;
  const cu_cp_configuration&        cu_cp_cfg;
  std::vector<plmn_identity>&       plmns;
  du_processor_repository&          du_db;
  cu_cp_ue_context_release_handler& ue_release_handler;
  ue_manager&                       ue_mng;
  cu_cp_controller&                 controller;
  srslog::basic_logger&             logger;
};

} // namespace srs_cu_cp
} // namespace srsran
