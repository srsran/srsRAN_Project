/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "mobility_manager_impl.h"
#include "srsran/cu_cp/mobility_manager.h"

using namespace srsran;

std::unique_ptr<srs_cu_cp::mobility_manager>
srsran::srs_cu_cp::create_mobility_manager(const srs_cu_cp::mobility_manager_cfg& cfg_,
                                           du_repository&                         du_db_,
                                           du_processor_ue_manager&               ue_mng_)
{
  return std::make_unique<srs_cu_cp::mobility_manager_impl>(cfg_, du_db_, ue_mng_);
}
