/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_uplane_message_decoder_dynamic_compression_impl.h"
#include "../support/network_order_binary_deserializer.h"

using namespace srsran;
using namespace ofh;

uplane_message_decoder_impl::decoded_section_status
uplane_message_decoder_dynamic_compression_impl::decode_compression_header(
    decoder_uplane_section_params&     results,
    network_order_binary_deserializer& deserializer)
{
  if (deserializer.remaining_bytes() < 2 * sizeof(uint8_t)) {
    logger.info("Sector#{}: received an Open Fronthaul packet with size of '{}' bytes that is smaller than the user "
                "data compression header length",
                sector_id,
                deserializer.remaining_bytes());

    return uplane_message_decoder_impl::decoded_section_status::incomplete;
  }

  uint8_t value            = deserializer.read<uint8_t>();
  results.ud_comp_hdr.type = to_compression_type(value & 0x0f);

  // Consider a reserved value as malformed message.
  if (results.ud_comp_hdr.type == compression_type::reserved) {
    logger.info("Sector#{}: detected malformed Open Fronthaul message as the decoded compression type '{}' is invalid",
                sector_id,
                value & 0x0f);

    return uplane_message_decoder_impl::decoded_section_status::malformed;
  }

  unsigned data_width            = value >> 4U;
  results.ud_comp_hdr.data_width = (data_width == 0) ? MAX_IQ_WIDTH : data_width;

  // Advance the reserved byte.
  deserializer.advance(1U);

  return uplane_message_decoder_impl::decoded_section_status::ok;
}
