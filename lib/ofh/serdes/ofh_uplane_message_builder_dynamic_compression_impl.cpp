/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  // Note that an IQ bitwidth of 16 bits translates to a value of 0.
  value |= uint8_t(params.data_width == 16 ? 0 : params.data_width) << 4U;
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
