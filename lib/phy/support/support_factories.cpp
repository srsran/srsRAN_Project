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

#include "srsran/phy/support/support_factories.h"
#include "interpolator/interpolator_linear_impl.h"
#include "prach_buffer_impl.h"
#include "prach_buffer_pool_impl.h"
#include "resource_grid_impl.h"
#include "resource_grid_pool_impl.h"
#include "srsran/phy/generic_functions/precoding/precoding_factories.h"
#include "srsran/ran/prach/prach_constants.h"
#include <utility>

using namespace srsran;

namespace {

/// Factory that builds resource grid objects.
class resource_grid_factory_impl : public resource_grid_factory
{
public:
  /// \brief Creates a resource grid instance for a number of ports, symbols and subcarriers.
  /// \param[in] nof_ports Number of ports.
  /// \param[in] nof_symb  Number of OFDM symbols.
  /// \param[in] nof_subc  Number of subcarriers.
  /// \return A resource grid object.
  std::unique_ptr<resource_grid> create(unsigned nof_ports, unsigned nof_symb, unsigned nof_subc) override
  {
    return std::make_unique<resource_grid_impl>(nof_ports, nof_symb, nof_subc);
  }
};

} // namespace

std::unique_ptr<resource_grid_pool>
srsran::create_generic_resource_grid_pool(std::vector<std::unique_ptr<resource_grid>> grids)
{
  std::vector<resource_grid_pool_wrapper> rg_wrappers;
  std::for_each(grids.begin(), grids.end(), [&rg_wrappers](std::unique_ptr<resource_grid>& grid) {
    rg_wrappers.emplace_back(resource_grid_pool_wrapper(std::move(grid), nullptr));
  });

  return std::make_unique<resource_grid_pool_impl>(std::move(rg_wrappers));
}

std::unique_ptr<resource_grid_pool>
srsran::create_asynchronous_resource_grid_pool(task_executor&                              async_executor,
                                               std::vector<std::unique_ptr<resource_grid>> grids)
{
  std::vector<resource_grid_pool_wrapper> rg_wrappers;
  std::for_each(grids.begin(),
                grids.end(),
                [&rg_wrappers, async_executor_ptr = &async_executor](std::unique_ptr<resource_grid>& grid) {
                  rg_wrappers.emplace_back(resource_grid_pool_wrapper(std::move(grid), async_executor_ptr));
                });

  return std::make_unique<resource_grid_pool_impl>(std::move(rg_wrappers));
}

namespace {

/// Factory that builds resource grid mapper objects.
class resource_grid_mapper_factory_impl : public resource_grid_mapper_factory
{
public:
  resource_grid_mapper_factory_impl(std::shared_ptr<channel_precoder_factory> precoder_factory_) :
    precoder_factory(std::move(precoder_factory_))
  {
    srsran_assert(precoder_factory, "Invalid precoder factory.");
  }

  /// \brief Creates a resource grid mapper instance.
  /// \return A resource grid mapper object.
  std::unique_ptr<resource_grid_mapper> create() override
  {
    return std::make_unique<resource_grid_mapper_impl>(precoder_factory->create());
  }

private:
  std::shared_ptr<channel_precoder_factory> precoder_factory;
};

} // namespace

std::shared_ptr<resource_grid_mapper_factory>
srsran::create_resource_grid_mapper_factory(std::shared_ptr<channel_precoder_factory> precoder_factory)
{
  return std::make_shared<resource_grid_mapper_factory_impl>(precoder_factory);
}

std::unique_ptr<prach_buffer_pool>
srsran::create_prach_buffer_pool(std::vector<std::unique_ptr<prach_buffer>>&& elements)
{
  return std::make_unique<prach_buffer_pool_impl>(std::move(elements));
}

std::unique_ptr<prach_buffer> srsran::create_prach_buffer_long(unsigned max_nof_antennas, unsigned max_nof_fd_occasions)
{
  static constexpr interval<unsigned, true> nof_rx_ports_range(1, MAX_PORTS);
  static constexpr interval<unsigned, true> max_nof_fd_prach_occasions_range(
      1, prach_constants::MAX_NOF_PRACH_FD_OCCASIONS);
  srsran_assert(nof_rx_ports_range.contains(max_nof_antennas),
                "The maximum number of antennas (i.e., {}) is out of range {}·",
                max_nof_antennas,
                nof_rx_ports_range);
  srsran_assert(max_nof_fd_prach_occasions_range.contains(max_nof_fd_occasions),
                "The maximum number of frequency domain occasions (i.e., {}) is out of range {}·",
                max_nof_fd_occasions,
                max_nof_fd_prach_occasions_range);
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
  static constexpr interval<unsigned, true> nof_rx_ports_range(1, MAX_PORTS);
  static constexpr interval<unsigned, true> max_nof_td_prach_occasions_range(
      1, prach_constants::MAX_NOF_PRACH_TD_OCCASIONS);
  static constexpr interval<unsigned, true> max_nof_fd_prach_occasions_range(
      1, prach_constants::MAX_NOF_PRACH_FD_OCCASIONS);
  srsran_assert(nof_rx_ports_range.contains(max_nof_antennas),
                "The maximum number of antennas (i.e., {}) is out of range {}·",
                max_nof_antennas,
                nof_rx_ports_range);
  srsran_assert(max_nof_td_prach_occasions_range.contains(max_nof_td_occasions),
                "The maximum number of time domain occasions (i.e., {}) is out of range {}·",
                max_nof_td_occasions,
                max_nof_td_prach_occasions_range);
  srsran_assert(max_nof_fd_prach_occasions_range.contains(max_nof_fd_occasions),
                "The maximum number of frequency domain occasions (i.e., {}) is out of range {}·",
                max_nof_fd_occasions,
                max_nof_fd_prach_occasions_range);
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

std::shared_ptr<resource_grid_factory> srsran::create_resource_grid_factory()
{
  return std::make_shared<resource_grid_factory_impl>();
}
