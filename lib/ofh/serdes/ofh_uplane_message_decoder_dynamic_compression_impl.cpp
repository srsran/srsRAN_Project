/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

bool uplane_message_decoder_dynamic_compression_impl::decode_compression_header(
    uplane_section_params&             results,
    network_order_binary_deserializer& deserializer,
    bool                               is_a_prach_msg)
{
  if (deserializer.remaining_bytes() < 2 * sizeof(uint8_t)) {
    logger.debug(
        "Incoming Open Fronthaul message size is {} and it is smaller than the user data compression header length",
        deserializer.remaining_bytes());

    return false;
  }

  uint8_t value                  = deserializer.read<uint8_t>();
  results.ud_comp_hdr.type       = to_compression_type(value & 0x0f);
  unsigned data_width            = value >> 4U;
  results.ud_comp_hdr.data_width = (data_width == 0) ? MAX_IQ_WIDTH : data_width;

  // Advance the reserved byte.
  deserializer.advance(1U);

  return true;
}
