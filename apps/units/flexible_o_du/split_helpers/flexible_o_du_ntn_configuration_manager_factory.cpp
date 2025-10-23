/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
