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
    logger.info("Received an Open Fronthaul packet with size of '{}' bytes that is smaller than the user data "
                "compression header length",
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
