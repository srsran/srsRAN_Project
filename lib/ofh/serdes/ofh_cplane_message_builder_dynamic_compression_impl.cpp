/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_cplane_message_builder_dynamic_compression_impl.h"
#include "../support/network_order_binary_serializer.h"

using namespace srsran;
using namespace ofh;

void cplane_message_builder_dynamic_compression_impl::serialize_compression_header(
    network_order_binary_serializer& serializer,
    const ru_compression_params&     compr,
    data_direction                   direction) const
{
  // Downlink always encodes a 0.
  if (direction == data_direction::downlink) {
    static constexpr uint8_t reserved = 0;
    serializer.write(reserved);
    return;
  }

  uint8_t value = 0;
  // Note that an IQ bitwidth of 16 bits translates to a value of 0.
  value |= uint8_t(compr.data_width == 16 ? 0 : compr.data_width) << 4;
  value |= uint8_t(to_value(compr.type));

  serializer.write(value);
}
