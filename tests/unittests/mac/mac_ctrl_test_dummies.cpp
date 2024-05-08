/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

void mac_scheduler_dummy_adapter::handle_ue_config_applied(du_ue_index_t ue_idx) {}
