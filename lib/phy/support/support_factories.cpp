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
#include "srsran/phy/generic_functions/precoding/precoding_factories.h"
#include "srsran/ran/prach/prach_constants.h"

using namespace srsran;

namespace {

/// Factory that builds resource grid objects.
class resource_grid_factory_impl : public resource_grid_factory
{
public:
  explicit resource_grid_factory_impl(std::shared_ptr<channel_precoder_factory> precoder_factory_) :
    precoder_factory(std::move(precoder_factory_))
  {
    srsran_assert(precoder_factory, "Invalid channel precoder factory.");
  }

  /// \brief Creates a resource grid instance for a number of ports, symbols and subcarriers.
  /// \param[in] nof_ports   Number of ports.
  /// \param[in] nof_symbols Number of OFDM symbols.
  /// \param[in] nof_subc    Number of subcarriers.
  /// \return A resource grid object.
  std::unique_ptr<resource_grid> create(unsigned nof_ports, unsigned nof_symb, unsigned nof_subc) override
  {
    return std::make_unique<resource_grid_impl>(nof_ports, nof_symb, nof_subc, precoder_factory->create());
  }

private:
  // Channel precoder factory.
  std::shared_ptr<channel_precoder_factory> precoder_factory;
};

} // namespace

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

std::unique_ptr<prach_buffer> srsran::create_prach_buffer_long(unsigned max_nof_antennas, unsigned max_nof_fd_occasions)
{
  return std::make_unique<prach_buffer_impl>(max_nof_antennas,
                                             1,
                                             max_nof_fd_occasions,
                                             prach_constants::LONG_SEQUENCE_MAX_NOF_SYMBOLS,
                                             prach_constants::LONG_SEQUENCE_LENGTH);
}

std::unique_ptr<prach_buffer> srsran::create_prach_buffer_short(unsigned max_nof_antennas,
                                                                unsigned max_nof_td_occasions,
                                                                unsigned max_nof_fd_occasions)
{
  return std::make_unique<prach_buffer_impl>(max_nof_antennas,
                                             max_nof_td_occasions,
                                             max_nof_fd_occasions,
                                             prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS,
                                             prach_constants::SHORT_SEQUENCE_LENGTH);
}

std::unique_ptr<srsran::interpolator> srsran::create_interpolator()
{
  return std::make_unique<interpolator_linear_impl>();
}

std::shared_ptr<resource_grid_factory>
srsran::create_resource_grid_factory(std::shared_ptr<channel_precoder_factory> precoder_factory)
{
  return std::make_shared<resource_grid_factory_impl>(std::move(precoder_factory));
}
