/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_cplane_message_builder_static_compression_impl.h"
#include "../support/network_order_binary_serializer.h"

using namespace srsran;
using namespace ofh;

void cplane_message_builder_static_compression_impl::serialize_compression_header(
    network_order_binary_serializer& serializer,
    const ru_compression_params&     compr,
    data_direction                   direction) const
{
  // Static compression always encodes a 0.
  static constexpr uint8_t reserved = 0;
  serializer.write(reserved);
}
