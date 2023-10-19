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

#pragma once

#include "interpolator.h"
#include "srsran/phy/generic_functions/precoding/precoding_factories.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_pool.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include <memory>
#include <vector>

namespace srsran {

/// Factory that builds resource grids.
class resource_grid_factory
{
public:
  /// Default destructor.
  virtual ~resource_grid_factory() = default;

  /// Creates and returns an instance of a resource grid.
  virtual std::unique_ptr<resource_grid> create(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc) = 0;
};

/// \brief Creates and returns a resource grid factory that instantiates resource grids.
///
/// \param[in] precoder_factory Channel precoder factory.
/// \return A pointer to a resource grid factory.
std::shared_ptr<resource_grid_factory>
create_resource_grid_factory(std::shared_ptr<channel_precoder_factory> precoder_factory);

/// Creates a resource grid mapper with an ideal precoding.
std::unique_ptr<resource_grid_mapper>
create_resource_grid_mapper(unsigned nof_ports, unsigned nof_subc, resource_grid_writer& writer);

/// \brief Creates a generic resource grid pool.
/// \param[in] nof_sectors Number of radio sectors.
/// \param[in] nof_slots umber of slots to be buffered, per sector.
/// \param[in] grids Resource grids, ownerships are transferred to the pool.
/// \return A generic resource grid pool.
std::unique_ptr<resource_grid_pool> create_resource_grid_pool(unsigned                                      nof_sectors,
                                                              unsigned                                      nof_slots,
                                                              std::vector<std::unique_ptr<resource_grid>>&& grids);

/// \brief Creates a long PRACH sequence buffer.
///
/// Long buffers contain 839-element PRACH sequences for up to 4 OFDM symbols and a given maximum number of
/// frequency-domain occasions.
///
/// \param[in] max_nof_antennas     Maximum number of antennas.
/// \param[in] max_nof_fd_occasions Maximum number of frequency-domain occasions.
/// \return A long preamble sequence buffer.
std::unique_ptr<prach_buffer> create_prach_buffer_long(unsigned max_nof_antennas, unsigned max_nof_fd_occasions);

/// \brief Creates a short PRACH sequence buffer.
///
/// Short buffers contain 139-element PRACH sequences for up to \ref prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS
/// symbols per occasion.
///
/// \param[in] max_nof_antennas     Maximum number of antennas.
/// \param[in] max_nof_td_occasions Maximum number of time-domain occasions.
/// \param[in] max_nof_fd_occasions Maximum number of frequency-domain occasions.
/// \return A short preamble sequence buffer containing PRACH sequence buffers for the number of selected occasions.
std::unique_ptr<prach_buffer>
create_prach_buffer_short(unsigned max_nof_antennas, unsigned max_nof_td_occasions, unsigned max_nof_fd_occasions);

/// \brief Returns a pool for the given PRACH buffers.
std::unique_ptr<prach_buffer_pool> create_prach_buffer_pool(std::vector<std::unique_ptr<prach_buffer>>&& elements);

/// \brief Returns an interpolator.
std::unique_ptr<srsran::interpolator> create_interpolator();

} // namespace srsran
