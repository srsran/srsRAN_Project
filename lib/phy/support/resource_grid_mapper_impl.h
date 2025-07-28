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

#pragma once

#include "srsran/phy/generic_functions/precoding/channel_precoder.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_writer.h"

namespace srsran {

/// Implements a resource grid mapper.
class resource_grid_mapper_impl : public resource_grid_mapper
{
public:
  resource_grid_mapper_impl(std::unique_ptr<channel_precoder> precoder_);

  ~resource_grid_mapper_impl() = default;

  // See interface for documentation.
  void map(resource_grid_writer&          writer,
           const re_buffer_reader<>&      input,
           const re_pattern&              pattern,
           const precoding_configuration& precoding) override;

  // See interface for documentation.
  void map(resource_grid_writer&           grid,
           symbol_buffer&                  buffer,
           const allocation_configuration& allocation,
           const re_pattern_list&          reserved,
           const precoding_configuration&  precoding,
           unsigned                        re_skip) override;

private:
  /// Maximum number of subcarriers that can be accomodated in an OFDM symbol.
  static constexpr unsigned max_nof_subcarriers = MAX_RB * NRE;
  /// Maximum number of ports to map in a mapping call.
  static constexpr unsigned max_nof_ports = precoding_constants::MAX_NOF_PORTS;

  /// \brief Maps a Resource Element block in an OFDM symbol.
  ///
  /// \param[out] writer       Destination resource grid.
  /// \param[in]  block_mask   Allocation mask.
  /// \param[in]  prg_weights  Precoding weights for the block.
  /// \param[in]  block        Complex symbols to map.
  /// \param[in]  i_symbol     Destination OFDM symbol index within the slot.
  /// \param[in]  i_subc       Initial subcarrier index at which start mapping.
  void map_re_block(resource_grid_writer&                      writer,
                    const bounded_bitset<max_nof_subcarriers>& block_mask,
                    const precoding_weight_matrix&             prg_weights,
                    span<const ci8_t>                          block,
                    unsigned                                   i_symbol,
                    unsigned                                   i_subc);

  /// Channel precoder.
  std::unique_ptr<channel_precoder> precoder;
};

} // namespace srsran
