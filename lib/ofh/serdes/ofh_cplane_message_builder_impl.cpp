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

#include "ofh_cplane_message_builder_impl.h"
#include "../support/network_order_binary_serializer.h"
#include "ofh_cuplane_constants.h"

using namespace srsran;
using namespace ofh;

/// Encodes and returns the frame structure field. It is comprised by 4 LSB bits storing subcarrier spacing and 4 MSB
/// bits storing the FFT/iFFT size.
static uint8_t encode_frame_structure(cplane_scs scs, cplane_fft_size fft_size)
{
  uint8_t value = to_value(scs);
  return value | (uint8_t(to_value(fft_size)) << 4u);
}

/// Encodes and returns the data direction Byte.
static uint8_t encode_data_direction(const cplane_radio_application_header& params)
{
  uint8_t value = 0;
  value |= uint8_t(to_value(params.direction)) << 7;
  value |= uint8_t(OFH_PAYLOAD_VERSION & 0x7) << 4;
  value |= uint8_t(to_value(params.filter_index));

  return value;
}

/// Encodes and returns the subframe and the 4 MSB bits of the slot index.
static uint8_t encode_subframe_and_slot(slot_point slot)
{
  uint8_t value = 0;
  value |= uint8_t(slot.subframe_index() & 0xf) << 4;
  value |= uint8_t(slot.subframe_slot_index() >> 2) & 0xf;

  return value;
}

/// Encodes and returns the 2 LSB bits of the slot index and the start symbol.
static uint8_t encode_slot_and_start_symbol(slot_point slot, uint8_t start_symbol)
{
  uint8_t value = 0;
  value |= uint8_t(slot.subframe_slot_index() & 0x3) << 6;
  value |= start_symbol & 0x3f;

  return value;
}

/// Encodes and returns the 4 LSB bits of section id, the rb bit, number of symbols and the 2 MSB bits of start PRB.
static uint8_t encode_sect_id_rb_symbols(const cplane_common_section_0_1_3_5_fields& section)
{
  uint8_t value = 0;
  value |= uint8_t(section.section_id & 0xf) << 4;
  // Resource block set to all PRB.
  value |= uint8_t(to_value(rb_id_type::every_rb_used)) << 3;
  // Symbol increment set to current symbol number.
  value |= uint8_t(to_value(symbol_incr_type::current_symbol_number)) << 2;
  value |= uint8_t(section.prb_start >> 8) & 0x3;

  return value;
}

/// Encodes and returns the 4 LSB bits of the RE mask and the number of symbols.
static uint8_t encode_re_mask_and_nof_symbols(const cplane_common_section_0_1_3_5_fields& section)
{
  uint8_t value = 0;
  value |= uint8_t(section.re_mask & 0xf) << 4;
  value |= uint8_t(section.nof_symbols & 0xf);

  return value;
}

/// Serializes the fields common to section types 0, 1, 3 and 5 using the serializer.
static void serialize_section_0_1_3_5_fields(ofh::network_order_binary_serializer&       serializer,
                                             const cplane_common_section_0_1_3_5_fields& section)
{
  // 8 MSB section identifier (1 Byte).
  serializer.write(uint8_t(section.section_id >> 4));

  // Section identifier, RB, Symbol increment and PRB start (1 Byte).
  serializer.write(encode_sect_id_rb_symbols(section));

  // PRB start (1 Byte).
  serializer.write(uint8_t(section.prb_start));

  // Number of PRBs (1 Byte).
  serializer.write(uint8_t((section.nof_prb > std::numeric_limits<uint8_t>::max()) ? 0U : section.nof_prb));

  // Most significant bits of RE mask (1 Byte).
  serializer.write(uint8_t(section.re_mask >> 4));

  // Less significant bits of RE mask and number of symbols (1 Byte).
  serializer.write(encode_re_mask_and_nof_symbols(section));
}

/// Serializes section type 1 extensions (not supported currently).
static void serialize_section1_extensions(ofh::network_order_binary_serializer& serializer)
{
  // EF and beam identifier (1 Byte). Not supporting extensions and beams.
  static constexpr uint8_t ext_beam_byte = 0;
  serializer.write(ext_beam_byte);

  // Beam identifier (1 Byte). No beam support.
  serializer.write(ext_beam_byte);
}

/// Serializes the given section type 1 using the serializer.
static void serialize_section_1(ofh::network_order_binary_serializer&            serializer,
                                const cplane_dl_ul_radio_channel_section_fields& section)
{
  // Write the common fields.
  serialize_section_0_1_3_5_fields(serializer, section.common_fields);

  // Write the section extensions.
  serialize_section1_extensions(serializer);
}

unsigned
cplane_message_builder_impl::build_dl_ul_radio_channel_message(span<uint8_t>                          buffer,
                                                               const cplane_section_type1_parameters& msg_params)
{
  ofh::network_order_binary_serializer serializer(buffer.data());

  // Data direction + payload version + filter index (1 Byte).
  serializer.write(encode_data_direction(msg_params.radio_hdr));

  // Frame identifier (1 Byte).
  serializer.write(uint8_t(msg_params.radio_hdr.slot.sfn()));

  // Subframe and slot identifier (1 Byte).
  serializer.write(encode_subframe_and_slot(msg_params.radio_hdr.slot));

  // Slot identifier and start symbol (1 Byte).
  serializer.write(encode_slot_and_start_symbol(msg_params.radio_hdr.slot, msg_params.radio_hdr.start_symbol));

  // Only 1 section (1 Byte).
  serializer.write(uint8_t(1U));

  // Section type for DL/UL radio channel is 1 (1 Byte).
  static constexpr uint8_t section_type = 1U;
  serializer.write(section_type);

  // Compression header (1 Byte).
  serialize_compression_header(serializer, msg_params.comp_params, msg_params.radio_hdr.direction);

  // Reserved (1 Byte).
  static constexpr uint8_t reserved = 0U;
  serializer.write(reserved);

  // Write the section fields. Only one section supported.
  serialize_section_1(serializer, msg_params.section_fields);

  return serializer.get_offset();
}

/// Converts subcarrier spacing to one of the values defined in O-RAN.WG4.CUS, Table 7.5.2.13-3.
static cplane_scs to_cplane_scs_value(subcarrier_spacing scs)
{
  switch (scs) {
    case subcarrier_spacing::kHz15:
      return cplane_scs::kHz15;
    case subcarrier_spacing::kHz30:
      return cplane_scs::kHz30;
    case subcarrier_spacing::kHz60:
      return cplane_scs::kHz60;
    case subcarrier_spacing::kHz120:
      return cplane_scs::kHz120;
    default:
      return cplane_scs::reserved;
  }
}

/// Encodes and returns the cpLength field as defined in O-RAN.WG4.CUS, 7.5.2.14. Returns 0 in case CP is configured via
/// M-Plane.
static uint16_t encode_cp_length(subcarrier_spacing scs, cyclic_prefix cp)
{
  return 0;
}

/// Serializes section type 0 extensions (not supported currently).
static void serialize_section0_extensions(ofh::network_order_binary_serializer& serializer)
{
  // EF and reserved field (1 Byte). Not supporting extensions.
  static constexpr uint8_t ext_byte = 0;
  serializer.write(ext_byte);

  // Reserved (1 Byte).
  serializer.write(uint8_t(0));
}

/// Serializes the given section type 0 using the serializer.
static void serialize_section_0(ofh::network_order_binary_serializer&          serializer,
                                const cplane_idle_guard_period_section_fields& section)
{
  // Write the common fields.
  serialize_section_0_1_3_5_fields(serializer, section.common_fields);

  // Write the section extensions.
  serialize_section0_extensions(serializer);
}

unsigned cplane_message_builder_impl::build_idle_guard_period_message(span<uint8_t>                          buffer,
                                                                      const cplane_section_type0_parameters& msg_params)
{
  ofh::network_order_binary_serializer serializer(buffer.data());

  // Data direction + payload version + filter index (1 Byte).
  serializer.write(encode_data_direction(msg_params.radio_hdr));

  // Frame identifier (1 Byte).
  serializer.write(uint8_t(msg_params.radio_hdr.slot.sfn()));

  // Subframe and slot identifier (1 Byte).
  serializer.write(encode_subframe_and_slot(msg_params.radio_hdr.slot));

  // Slot identifier and start symbol (1 Byte).
  serializer.write(encode_slot_and_start_symbol(msg_params.radio_hdr.slot, msg_params.radio_hdr.start_symbol));

  // Only 1 section (1 Byte).
  serializer.write(uint8_t(1U));

  // Section type for idle/guard period message is 0 (1 Byte).
  static constexpr uint8_t section_type = 0U;
  serializer.write(section_type);

  // Time offset (2 Bytes).
  serializer.write(msg_params.time_offset);

  // Frame structure (1 Byte).
  serializer.write(encode_frame_structure(to_cplane_scs_value(msg_params.scs), cplane_fft_size::fft_noop));

  // CP length (2 Bytes).
  serializer.write(encode_cp_length(msg_params.scs, msg_params.cp));

  // Reserved (1 Byte).
  serializer.write(uint8_t(0));

  // Write the section fields. Only one section supported.
  serialize_section_0(serializer, msg_params.section_fields);

  return serializer.get_offset();
}

/// Serializes the given section type 3 using the serializer.
static void serialize_section_3(ofh::network_order_binary_serializer&        serializer,
                                const cplane_prach_mixed_num_section_fields& section)
{
  // Write the common fields.
  serialize_section_0_1_3_5_fields(serializer, section.common_fields);

  // EF and BeamId field (2 Bytes). Not supporting extensions.
  static constexpr uint16_t ext_bytes = 0;
  serializer.write(ext_bytes);

  // The frequency offset (3 Bytes).
  serializer.write(uint8_t(section.frequency_offset >> 16));
  serializer.write(uint16_t(section.frequency_offset & 0xffff));

  // Reserved (1 Byte).
  serializer.write(uint8_t(0));
}

unsigned
cplane_message_builder_impl::build_prach_mixed_numerology_message(span<uint8_t>                          buffer,
                                                                  const cplane_section_type3_parameters& msg_params)
{
  ofh::network_order_binary_serializer serializer(buffer.data());

  // Data direction + payload version + filter index (1 Byte).
  serializer.write(encode_data_direction(msg_params.radio_hdr));

  // Frame identifier (1 Byte).
  serializer.write(uint8_t(msg_params.radio_hdr.slot.sfn()));

  // Subframe and slot identifier (1 Byte).
  serializer.write(encode_subframe_and_slot(msg_params.radio_hdr.slot));

  // Slot identifier and start symbol (1 Byte).
  serializer.write(encode_slot_and_start_symbol(msg_params.radio_hdr.slot, msg_params.radio_hdr.start_symbol));

  // Only 1 section (1 Byte).
  serializer.write(uint8_t(1U));

  // Section type for idle/guard period message is 3 (1 Byte).
  static constexpr uint8_t section_type = 3U;
  serializer.write(section_type);

  // Time offset (2 Bytes).
  serializer.write(msg_params.time_offset);

  // Frame structure (1 Byte).
  serializer.write(encode_frame_structure(msg_params.scs, msg_params.fft_size));

  // Cyclic prefix length (2 Bytes).
  serializer.write(uint16_t(msg_params.cpLength));

  // Compression header (1 Byte).
  serialize_compression_header(serializer, msg_params.comp_params, msg_params.radio_hdr.direction);

  // Write the section fields. Only one section supported.
  serialize_section_3(serializer, msg_params.section_fields);

  return serializer.get_offset();
}
