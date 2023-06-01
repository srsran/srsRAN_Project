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

#include "srsran/ofh/ofh_uplane_message_builder.h"

namespace srsran {
namespace ofh {

class iq_compressor;
class network_order_binary_serializer;

/// Open Fronthaul User-Plane message builder implementation.
class uplane_message_builder_impl : public uplane_message_builder
{
public:
  uplane_message_builder_impl(srslog::basic_logger& logger_,
                              iq_compressor&        compressor_,
                              unsigned              du_nof_prbs_,
                              unsigned              ru_nof_prbs_) :
    logger(logger_), compressor(compressor_), du_nof_prbs(du_nof_prbs_), ru_nof_prbs(ru_nof_prbs_), data_start_prb(0)
  {
  }

  // See interface for documentation.
  unsigned
  build_message(span<uint8_t> buffer, const resource_grid_reader& grid, const uplane_message_params& config) override;

private:
  /// Writes the IQ data from the given grid and context.
  void write_resource_grid(network_order_binary_serializer& serializer,
                           const resource_grid_reader&      grid,
                           const uplane_message_params&     params);

  /// Serializes IQ symbols from a resource grid.
  void serialize_iq_data(network_order_binary_serializer& serializer,
                         span<const cf_t>                 symbols,
                         unsigned                         nof_prbs,
                         const ru_compression_params&     params);

  /// Serializes compression header. Implementation depends on whether static or non-static IQ format is configured.
  virtual void serialize_compression_header(network_order_binary_serializer& serializer,
                                            const ru_compression_params&     params) = 0;

protected:
  srslog::basic_logger& logger;
  iq_compressor&        compressor;
  const unsigned        du_nof_prbs;
  const unsigned        ru_nof_prbs;
  const unsigned        data_start_prb;
  const bool            ud_comp_length_support = false;
};

/// Open Fronthaul User-Plane message builder implementation with static IQ compression.
class ofh_uplane_message_builder_static_compression_impl : public uplane_message_builder_impl
{
public:
  explicit ofh_uplane_message_builder_static_compression_impl(srslog::basic_logger& logger_,
                                                              iq_compressor&        compressor_,
                                                              unsigned              du_nof_prbs_,
                                                              unsigned              ru_nof_prbs_) :
    uplane_message_builder_impl(logger_, compressor_, du_nof_prbs_, ru_nof_prbs_)
  {
  }

  // See interface for documentation.
  units::bytes get_header_size(const ru_compression_params& params) const override;

private:
  // See parent for documentation.
  void serialize_compression_header(network_order_binary_serializer& serializer,
                                    const ru_compression_params&     params) override;
};

} // namespace ofh
} // namespace srsran
