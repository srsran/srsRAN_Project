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

#include "../cu_up_manager_impl.h"
#include "../ue_manager.h"
#include "srsran/cu_up/cu_up_config.h"
#include "srsran/gtpu/gtpu_demux.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_up {

class cu_up_enable_test_mode_routine
{
public:
  cu_up_enable_test_mode_routine(cu_up_test_mode_config test_mode_cfg_,
                                 cu_up_manager_impl&    cu_up_mngr_,
                                 gtpu_demux_ctrl&       ngu_demux_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-UP enable test mode routine"; }

private:
  const cu_up_test_mode_config test_mode_cfg;
  cu_up_manager_impl&          cu_up_mngr;
  gtpu_demux_ctrl&             ngu_demux;

  unique_timer test_mode_ue_timer;

  srslog::basic_logger& logger;

  e1ap_bearer_context_setup_request        bearer_context_setup;
  e1ap_bearer_context_setup_response       setup_resp;
  gtpu_teid_t                              teid;
  e1ap_bearer_context_modification_request bearer_modify;
};

class cu_up_disable_test_mode_routine
{
public:
  cu_up_disable_test_mode_routine(cu_up_manager_impl& cu_up_mngr_, ue_manager& ue_mngr_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "CU-UP disable test mode routine"; }

private:
  cu_up_manager_impl&   cu_up_mngr;
  ue_manager&           ue_mngr;
  srslog::basic_logger& logger;

  e1ap_bearer_context_release_command release_command;
};

} // namespace srs_cu_up
} // namespace srsran
