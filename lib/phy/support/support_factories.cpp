/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/phy/support/support_factories.h"
#include "interpolator/interpolator_linear_impl.h"
#include "prach_buffer_impl.h"
#include "prach_buffer_pool_impl.h"
#include "resource_grid_impl.h"
#include "resource_grid_pool_impl.h"
#include "srsran/ran/prach/prach_constants.h"

using namespace srsran;

std::unique_ptr<resource_grid> srsran::create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  return std::make_unique<resource_grid_impl>(nof_ports, nof_symbols, nof_subc);
}

std::unique_ptr<resource_grid_pool>
srsran::create_resource_grid_pool(unsigned                                      nof_sectors,
                                  unsigned                                      nof_slots,
                                  std::vector<std::unique_ptr<resource_grid>>&& grids)
{
  return std::make_unique<resource_grid_pool_impl>(nof_sectors, nof_slots, std::move(grids));
}

std::unique_ptr<prach_buffer_pool>
srsran::create_prach_buffer_pool(std::vector<std::unique_ptr<prach_buffer>>&& elements)
{
  return std::make_unique<prach_buffer_pool_impl>(std::move(elements));
}

std::unique_ptr<prach_buffer> srsran::create_prach_buffer_long()
{
  return std::make_unique<
      prach_buffer_static<prach_constants::LONG_SEQUENCE_LENGTH, prach_constants::LONG_SEQUENCE_MAX_NOF_SYMBOLS>>();
}

std::unique_ptr<prach_buffer> srsran::create_prach_buffer_short()
{
  return std::make_unique<
      prach_buffer_static<prach_constants::SHORT_SEQUENCE_LENGTH, prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS>>();
}

std::unique_ptr<srsran::interpolator> srsran::create_interpolator()
{
  return std::make_unique<interpolator_linear_impl>();
}
