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
