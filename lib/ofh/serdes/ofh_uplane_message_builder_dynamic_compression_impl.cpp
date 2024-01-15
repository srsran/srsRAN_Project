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

#include "ofh_uplane_message_builder_dynamic_compression_impl.h"
#include "../support/network_order_binary_serializer.h"

using namespace srsran;
using namespace ofh;

void ofh_uplane_message_builder_dynamic_compression_impl::serialize_compression_header(
    network_order_binary_serializer& serializer,
    const ru_compression_params&     params)
{
  // Serialize compression header.
  uint8_t value = 0U;
  value |= uint8_t(params.data_width) << 4U;
  value |= uint8_t(to_value(params.type));
  serializer.write(value);

  // Serialize reserved byte.
  serializer.write(uint8_t(0));
}

units::bytes
ofh_uplane_message_builder_dynamic_compression_impl::get_header_size(const ru_compression_params& params) const
{
  units::bytes hdr_size(10);
  if (params.type == compression_type::bfp_selective || params.type == compression_type::mod_selective) {
    hdr_size += units::bytes(2);
  }

  return hdr_size;
}
