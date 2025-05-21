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

#include "cu_up_test_mode_routines.h"
#include "../cu_up_manager_helpers.h"

using namespace srsran;
using namespace srs_cu_up;

/// Enable test mode routine.
cu_up_enable_test_mode_routine::cu_up_enable_test_mode_routine(cu_up_test_mode_config test_mode_cfg_,
                                                               cu_up_manager_impl&    cu_up_mngr_,
                                                               gtpu_demux_ctrl&       ngu_demux_) :
  test_mode_cfg(test_mode_cfg_),
  cu_up_mngr(cu_up_mngr_),
  ngu_demux(ngu_demux_),
  logger(srslog::fetch_basic_logger("CU-UP"))
{
}

void cu_up_enable_test_mode_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  bearer_context_setup = fill_test_mode_bearer_context_setup_request(test_mode_cfg);

  // Setup bearer context and PDU session.
  setup_resp = cu_up_mngr.handle_bearer_context_setup_request(bearer_context_setup);

  // Apply test TEID to demux.
  teid = setup_resp.pdu_session_resource_setup_list.begin()->ng_dl_up_tnl_info.gtp_teid;
  ngu_demux.apply_test_teid(teid);

  //  Modify bearer
  bearer_modify = fill_test_mode_bearer_context_modification_request(setup_resp);

  CORO_AWAIT(cu_up_mngr.handle_bearer_context_modification_request(bearer_modify));

  cu_up_mngr.trigger_disable_test_mode();

  CORO_RETURN();
}

/// Disable test mode routine.
cu_up_disable_test_mode_routine::cu_up_disable_test_mode_routine(cu_up_manager_impl& cu_up_mngr_,
                                                                 ue_manager&         ue_mngr_) :
  cu_up_mngr(cu_up_mngr_), ue_mngr(ue_mngr_), logger(srslog::fetch_basic_logger("CU-UP"))
{
  for (const auto& ue : ue_mngr.get_ues()) {
    release_command.ue_index = ue.first;
  }
}

void cu_up_disable_test_mode_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);
  CORO_AWAIT(cu_up_mngr.handle_bearer_context_release_command(release_command));
  cu_up_mngr.trigger_enable_test_mode();
  CORO_RETURN();
}
