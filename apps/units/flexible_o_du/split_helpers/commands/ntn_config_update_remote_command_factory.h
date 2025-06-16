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

namespace srsran {

class task_executor;
class timer_manager;
class ru_controller;
struct application_unit_commands;
struct ntn_config;

namespace srs_du {

class du_configurator;
class du_manager_time_mapper_accessor;

/// \brief Adds NTN config update remote command.
/// \param commands Application unit commands.
/// \param ntn_cfg  NTN config.
/// \param du_cfgtr DU configurator interface.
/// \param du_time_mapper_accessor entity to access MAC slot-time mapper.
/// \param ru_ctrl  RU controller interface.
/// \param timers   Timers for the update task.
/// \param timer_exec Task executor for the periodic SIB19 update task.
/// \return True on success.
void add_ntn_config_update_remote_command(application_unit_commands&       commands,
                                          const ntn_config&                ntn_cfg,
                                          du_configurator&                 du_cfgtr,
                                          du_manager_time_mapper_accessor& du_time_mapper_accessor,
                                          ru_controller&                   ru_ctrl,
                                          timer_manager&                   timers,
                                          task_executor&                   timer_exec);
} // namespace srs_du
} // namespace srsran
