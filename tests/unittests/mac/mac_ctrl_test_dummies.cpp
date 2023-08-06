/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "mac_ctrl_test_dummies.h"

using namespace srsran;

async_task<bool> mac_ul_dummy_configurer::add_ue(const mac_ue_create_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    last_ue_create_request = msg;
    CORO_AWAIT(ue_created_ev);
    CORO_RETURN(expected_result);
  });
}

async_task<bool>
mac_ul_dummy_configurer::addmod_bearers(du_ue_index_t                                  ue_index,
                                        const std::vector<mac_logical_channel_config>& ul_logical_channels)
{
  return launch_async([this, ue_index, ul_logical_channels](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    last_ue_bearers_added = std::make_pair(ue_index, ul_logical_channels);
    CORO_RETURN(true);
  });
}

async_task<bool> mac_ul_dummy_configurer::remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids_to_rem)
{
  std::vector<lcid_t> lcids(lcids_to_rem.begin(), lcids_to_rem.end());
  return launch_async([this, ue_index, lcids](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    last_ue_bearers_rem = std::make_pair(ue_index, lcids);
    CORO_RETURN(true);
  });
}

async_task<void> mac_ul_dummy_configurer::remove_ue(const mac_ue_delete_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    last_ue_delete_request = msg;
    CORO_RETURN();
  });
}

async_task<void> mac_cell_dummy_controller::start()
{
  return launch_async([](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN();
  });
}

async_task<bool> mac_dl_dummy_configurer::add_ue(const mac_ue_create_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    last_ue_create_request = msg;
    CORO_AWAIT(ue_created_ev);
    CORO_RETURN(expected_result);
  });
}
async_task<void> mac_dl_dummy_configurer::remove_ue(const mac_ue_delete_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    last_ue_delete_request = msg;
    CORO_RETURN();
  });
}

async_task<bool>
mac_dl_dummy_configurer::addmod_bearers(du_ue_index_t                                  ue_index,
                                        du_cell_index_t                                pcell_index,
                                        const std::vector<mac_logical_channel_config>& logical_channels)
{
  return launch_async([this, ue_index, logical_channels](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    last_ue_bearers_added = std::make_pair(ue_index, logical_channels);
    CORO_RETURN(true);
  });
}
async_task<bool> mac_dl_dummy_configurer::remove_bearers(du_ue_index_t      ue_index,
                                                         du_cell_index_t    pcell_index,
                                                         span<const lcid_t> lcids_to_rem)
{
  std::vector<lcid_t> lcids(lcids_to_rem.begin(), lcids_to_rem.end());
  return launch_async([this, ue_index, lcids](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    last_ue_bearers_rem = std::make_pair(ue_index, lcids);
    CORO_RETURN(true);
  });
}

async_task<bool> mac_scheduler_dummy_adapter::handle_ue_creation_request(const mac_ue_create_request& msg)
{
  return launch_async([this, msg](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    last_ue_create_request = msg;
    CORO_AWAIT_VALUE(bool result, ue_created_ev);
    CORO_RETURN(result);
  });
}

async_task<bool>
mac_scheduler_dummy_adapter::handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& msg)
{
  return launch_async([](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(true);
  });
}

async_task<bool> mac_scheduler_dummy_adapter::handle_ue_removal_request(const mac_ue_delete_request& msg)
{
  return launch_async([](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(true);
  });
}
