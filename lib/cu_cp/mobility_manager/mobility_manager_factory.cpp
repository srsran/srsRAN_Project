/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mobility_manager_impl.h"
#include "srsran/cu_cp/mobility_manager.h"

using namespace srsran;

std::unique_ptr<srs_cu_cp::mobility_manager>
srsran::srs_cu_cp::create_mobility_manager(const srs_cu_cp::mobility_manager_cfg& cfg_,
                                           mobility_manager_cu_cp_notifier&       cu_cp_notifier_)
{
  return std::make_unique<srs_cu_cp::mobility_manager_impl>(cfg_, cu_cp_notifier_);
}
