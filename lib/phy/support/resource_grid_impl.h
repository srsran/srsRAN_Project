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

#pragma once

#include "resource_grid_dimensions.h"
#include "resource_grid_mapper_impl.h"
#include "resource_grid_reader_impl.h"
#include "resource_grid_writer_impl.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/generic_functions/precoding/channel_precoder.h"
#include "srsran/phy/support/resource_grid.h"

namespace srsran {

class resource_grid_mapper;

/// Implements a resource grid and mapper.
class resource_grid_impl : public resource_grid
{
private:
  std::atomic<unsigned> empty = {};
  unsigned              nof_ports;
  unsigned              nof_symb;
  unsigned              nof_subc;

  /// \brief Stores the resource grid data.
  ///
  /// The resource grid buffer is a three-dimensional array with the dimensions representing, in order, subcarriers,
  /// OFDM symbols and antenna ports.
  dynamic_tensor<static_cast<unsigned>(resource_grid_dimensions::all), cf_t, resource_grid_dimensions> rg_buffer;

  /// Resource grid writer implementation.
  resource_grid_writer_impl writer;

  /// Resource grid reader implementation.
  resource_grid_reader_impl reader;

  /// Resource grid mapper implementation.
  resource_grid_mapper_impl mapper;

public:
  /// \brief Creates a resource grid.
  ///
  /// \param[in] nof_ports Number of ports.
  /// \param[in] nof_symb  Number of OFDM symbols.
  /// \param[in] nof_subc  Number of subcarriers.
  /// \param[in] precoder_ Resource grid precoder instance.
  resource_grid_impl(unsigned                          nof_ports,
                     unsigned                          nof_symb,
                     unsigned                          nof_subc,
                     std::unique_ptr<channel_precoder> precoder_);

  // See interface for documentation.
  void set_all_zero() override;

  // See interface for documentation.
  resource_grid_writer& get_writer() override;

  // See interface for documentation.
  const resource_grid_reader& get_reader() const override;

  // See interface for documentation.
  resource_grid_mapper& get_mapper() override { return mapper; }
};

} // namespace srsran
