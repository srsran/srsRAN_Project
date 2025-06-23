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

#include "ntn_config_update_remote_command_factory.h"

using namespace srsran;
using namespace srs_du;

#ifndef SRSRAN_HAS_ENTERPRISE_NTN

void srs_du::add_ntn_config_update_remote_command(application_unit_commands&       commands,
                                                  const ntn_config&                ntn_cfg,
                                                  du_configurator&                 du_cfgtr,
                                                  du_manager_time_mapper_accessor& du_time_mapper_accessor,
                                                  ru_controller&                   ru_ctrl,
                                                  timer_manager&                   timers_,
                                                  task_executor&                   timer_exec_)
{
}

#endif // SRSRAN_HAS_ENTERPRISE_NTN
