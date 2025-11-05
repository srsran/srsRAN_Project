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

#include "flexible_o_du_ntn_configuration_manager_factory.h"
#include "srsran/ntn/ntn_configuration_manager.h"

using namespace srsran;

#ifndef SRSRAN_HAS_ENTERPRISE_NTN

std::unique_ptr<srs_ntn::ntn_configuration_manager>
srsran::create_ntn_configuration_manager(const srs_ntn::ntn_configuration_manager_config& ntn_config,
                                         srs_du::du_configurator&                         du_cfgtr,
                                         srs_du::du_manager_time_mapper_accessor&         du_time_mapper_accessor,
                                         ru_controller&                                   ru_ctrl,
                                         timer_manager&                                   timers,
                                         task_executor&                                   executor)
{
  return nullptr;
}

#endif // SRSRAN_HAS_ENTERPRISE_NTN
