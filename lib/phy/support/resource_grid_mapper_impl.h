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
#include "srsran/phy/generic_functions/precoding/channel_precoder.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

/// Implements a resource grid mapper.
class resource_grid_mapper_impl : public resource_grid_mapper
{
public:
  resource_grid_mapper_impl(unsigned                          nof_ports_,
                            unsigned                          nof_subc_,
                            resource_grid_writer&             writer_,
                            std::unique_ptr<channel_precoder> precoder_);

  ~resource_grid_mapper_impl() = default;

  // See interface for documentation.
  void map(const re_buffer_reader& input, const re_pattern& pattern, const precoding_configuration& precoding) override;

  // See interface for documentation.
  void map(symbol_buffer&                 buffer,
           const re_pattern_list&         pattern,
           const re_pattern_list&         reserved,
           const precoding_configuration& precoding,
           unsigned                       re_skip) override;

private:
  /// Maximum number of subcarriers that can be accomodated in an OFDM symbol.
  static constexpr unsigned max_nof_subcarriers = MAX_RB * NRE;
  /// Maximum number of ports to map in a mapping call.
  static constexpr unsigned max_nof_ports = precoding_constants::MAX_NOF_PORTS;

  /// Resource grid dimensions.
  unsigned nof_ports;
  unsigned nof_subc;

  /// Resource grid writer.
  resource_grid_writer& writer;

  /// Channel precoder.
  std::unique_ptr<channel_precoder> precoder;
};

} // namespace srsran
