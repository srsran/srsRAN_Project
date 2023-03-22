/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

std::unique_ptr<prach_buffer> srsran::create_prach_buffer_long(unsigned max_nof_fd_occasions)
{
  return std::make_unique<prach_buffer_impl>(1,
                                             1,
                                             max_nof_fd_occasions,
                                             prach_constants::LONG_SEQUENCE_MAX_NOF_SYMBOLS,
                                             prach_constants::LONG_SEQUENCE_LENGTH);
}

std::unique_ptr<prach_buffer> srsran::create_prach_buffer_short(unsigned max_nof_td_occasions,
                                                                unsigned max_nof_fd_occasions)
{
  return std::make_unique<prach_buffer_impl>(1,
                                             max_nof_td_occasions,
                                             max_nof_fd_occasions,
                                             prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS,
                                             prach_constants::SHORT_SEQUENCE_LENGTH);
}

std::unique_ptr<srsran::interpolator> srsran::create_interpolator()
{
  return std::make_unique<interpolator_linear_impl>();
}
