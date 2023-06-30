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

#include "ofh_uplane_message_builder_impl.h"

namespace srsran {
namespace ofh {

class iq_compressor;
class network_order_binary_serializer;

/// Open Fronthaul User-Plane message builder implementation with dynamic IQ compression.
class ofh_uplane_message_builder_dynamic_compression_impl : public uplane_message_builder_impl
{
public:
  ofh_uplane_message_builder_dynamic_compression_impl(srslog::basic_logger& logger_, iq_compressor& compressor_) :
    uplane_message_builder_impl(logger_, compressor_)
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
