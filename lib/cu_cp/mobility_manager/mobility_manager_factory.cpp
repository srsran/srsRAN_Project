/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mobility_manager.h"
#include "mobility_manager_impl.h"

using namespace srsran;

std::unique_ptr<srs_cu_cp::mobility_manager>
srsran::srs_cu_cp::create_mobility_manager(const srs_cu_cp::mobility_manager_cfg& cfg_,
                                           du_repository&                         du_db_,
                                           du_processor_ue_manager&               ue_mng_)
{
  return std::make_unique<srs_cu_cp::mobility_manager_impl>(cfg_, du_db_, ue_mng_);
}
