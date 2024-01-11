/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "up_resource_manager_impl.h"
#include "srsran/cu_cp/up_resource_manager.h"

using namespace srsran;

std::unique_ptr<srs_cu_cp::up_resource_manager>
srsran::srs_cu_cp::create_up_resource_manager(const srs_cu_cp::up_resource_manager_cfg& cfg_)
{
  return std::make_unique<srs_cu_cp::up_resource_manager_impl>(cfg_);
}