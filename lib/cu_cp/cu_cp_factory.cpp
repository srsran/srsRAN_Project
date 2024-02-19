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

#include "srsran/cu_cp/cu_cp_factory.h"
#include "cu_cp_impl.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

std::unique_ptr<srs_cu_cp::cu_cp> srsran::create_cu_cp(const srs_cu_cp::cu_cp_configuration& cfg_)
{
  return std::make_unique<srs_cu_cp::cu_cp_impl>(cfg_);
}