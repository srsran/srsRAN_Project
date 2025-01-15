/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_uplane_message_builder_static_compression_impl.h"

using namespace srsran;
using namespace ofh;

void ofh_uplane_message_builder_static_compression_impl::serialize_compression_header(
    network_order_binary_serializer& serializer,
    const ru_compression_params&     params)
{
  // When the static IQ format is configured the udCompHdr and reserved fields are absent.
}

units::bytes
ofh_uplane_message_builder_static_compression_impl::get_header_size(const ru_compression_params& params) const
{
  units::bytes hdr_size(8);
  if (params.type == compression_type::bfp_selective || params.type == compression_type::mod_selective) {
    hdr_size += units::bytes(2);
  }

  return hdr_size;
}
