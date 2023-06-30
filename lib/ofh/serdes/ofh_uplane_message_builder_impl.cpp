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

#include "ofh_uplane_message_builder_impl.h"
#include "../support/network_order_binary_serializer.h"
#include "../support/ofh_uplane_constants.h"
#include "srsran/ofh/compression/iq_compressor.h"
#include "srsran/ran/resource_block.h"

using namespace srsran;
using namespace ofh;

/// Encodes data direction, payload version and filter index.
static uint8_t encode_data_direction(const uplane_message_params& params)
{
  uint8_t octet = 0;

  // Data direction (TX=1); offset: 7, 1 bit long.
  octet |= uint8_t(0x1) << 7u;
  // Payload version; offset: 4, 3 bits long.
  octet |= uint8_t(OFH_PAYLOAD_VERSION) << 4u;
  // Filter index is fixed to 0, skip it.
  return octet;
}

/// Encodes subframe index and MSB bits of slot index.
static uint8_t encode_subframe_and_slot(const uplane_message_params& params)
{
  uint8_t octet      = 0;
  uint8_t slot_index = static_cast<uint8_t>(params.slot.subframe_slot_index());
  // Subframe index; offset: 4, 4 bits long.
  octet |= static_cast<uint8_t>(params.slot.subframe_index()) << 4u;
  // Four MSBs of the slot index within 1ms subframe; offset: 4, 6 bits long.
  octet |= (slot_index >> 2u);
  return octet;
}

/// Encodes remaining LSB bits of the slot index and then symbol index.
static uint8_t encode_slot_lsb_and_symbol(const uplane_message_params& params)
{
  uint8_t octet      = 0;
  uint8_t slot_index = static_cast<uint8_t>(params.slot.subframe_slot_index());
  octet |= (slot_index & uint8_t(0x3)) << 6u;
  octet |= static_cast<uint8_t>(params.symbol_id);
  return octet;
}

/// Encodes and returns the 4 lsb bits section id, the rb bit, number of symbols and the 2 msb bits of start PRB.
static uint8_t encode_sect_id_rb_symbols(const uplane_message_params& params)
{
  uint8_t octet = 0;
  octet |= uint8_t(0) << 3u;
  octet |= uint8_t(0) << 2u;
  octet |= static_cast<uint8_t>(params.start_prb >> 8u) & uint8_t(0x3);
  return octet;
}

/// Writes radio application header to the output buffer.
static void build_radio_app_header(network_order_binary_serializer& serializer, const uplane_message_params& params)
{
  // Data direction + payload version + filter index (1 Byte).
  serializer.write(encode_data_direction(params));

  // Write FrameId (1 Byte) - a counter for 10 ms frames (wrapping period 2.56 seconds), range [0, 256].
  serializer.write(static_cast<uint8_t>(params.slot.sfn() % 256));

  // Write subframe and slot index (1 Byte).
  serializer.write(encode_subframe_and_slot(params));

  // Write 2 LSBs of slot index and symbol index.
  serializer.write(encode_slot_lsb_and_symbol(params));
}

/// Writes section1 header to the output buffer.
static void build_section1_header(network_order_binary_serializer& serializer, const uplane_message_params& params)
{
  // Section ID is fixed to 0.
  serializer.write(uint8_t(0));

  // Write rb, symInc and 2 MSB bits of start PRB.
  serializer.write(encode_sect_id_rb_symbols(params));

  // Write remaining LSBs of start PRB.
  serializer.write(static_cast<uint8_t>(params.start_prb & uint8_t(0xff)));

  // Write number of PRBs.
  serializer.write(static_cast<uint8_t>((params.nof_prb > std::numeric_limits<uint8_t>::max()) ? 0 : params.nof_prb));
}

void uplane_message_builder_impl::serialize_iq_data(network_order_binary_serializer& serializer,
                                                    span<const cf_t>                 symbols,
                                                    unsigned                         nof_prbs,
                                                    const ru_compression_params&     compr_params)
{
  logger.debug("Writing {} PRBs to OFH User-Plane message", nof_prbs);

  static_vector<compressed_prb, MAX_NOF_PRBS> compressed_prbs(nof_prbs);
  compressor.compress(compressed_prbs, symbols, compr_params);

  // Serialize compression header.
  serialize_compression_header(serializer, compr_params);

  if (ud_comp_length_support) {
    // The udCompLen field shall only be present for the following compression methods:
    // "BFP + selective RE sending" or "Modulation compression + selective RE sending".
    if (compr_params.type == compression_type::bfp_selective || compr_params.type == compression_type::mod_selective) {
      units::bits prb_iq_data_size_bits(NOF_SUBCARRIERS_PER_RB * 2U * compr_params.data_width);
      uint16_t    udCompLen = prb_iq_data_size_bits.round_up_to_bytes().value();
      serializer.write(udCompLen);
    }
  }

  for (compressed_prb& c_prb : compressed_prbs) {
    // Serialize compression parameter.
    if (compr_params.type != compression_type::none && compr_params.type != compression_type::modulation) {
      serializer.write(c_prb.get_compression_param());
    }
    // Serialize compressed data.
    serializer.write(c_prb.get_packed_data());
  }
}

unsigned uplane_message_builder_impl::build_message(span<uint8_t>                buffer,
                                                    span<const cf_t>             iq_data,
                                                    const uplane_message_params& params)
{
  srsran_assert(params.sect_type == section_type::type_1, "Unsupported section type");
  srsran_assert(iq_data.size() == params.nof_prb * NOF_SUBCARRIERS_PER_RB,
                "Error, number of PRBs in IQ data={} and requested to write={}",
                (iq_data.size() / NOF_SUBCARRIERS_PER_RB),
                params.nof_prb);

  network_order_binary_serializer serializer(buffer.data());

  build_radio_app_header(serializer, params);
  build_section1_header(serializer, params);
  serialize_iq_data(serializer, iq_data, params.nof_prb, params.compression_params);

  return serializer.get_offset();
}
