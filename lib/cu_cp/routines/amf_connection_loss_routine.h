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
                              cu_cp_ue_context_release_handler& ue_release_handler_,
                              ue_manager&                       ue_mng_,
                              cu_cp_controller&                 controller_,
                              srslog::basic_logger&             logger_);

  static std::string name() { return "AMF Connection Loss Routine"; }

  void operator()(coro_context<async_task<void>>& ctx);

  void release_ues();

private:
  const amf_index_t                 amf_index;
  const cu_cp_configuration&        cu_cp_cfg;
  std::vector<plmn_identity>&       plmns;
  cu_cp_ue_context_release_handler& ue_release_handler;
  ue_manager&                       ue_mng;
  cu_cp_controller&                 controller;
  srslog::basic_logger&             logger;
};

} // namespace srs_cu_cp
} // namespace srsran
