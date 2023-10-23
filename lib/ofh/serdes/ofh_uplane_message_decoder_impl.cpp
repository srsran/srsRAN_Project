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

#include "ofh_uplane_message_decoder_impl.h"
#include "../serdes/ofh_cuplane_constants.h"
#include "../support/network_order_binary_deserializer.h"
#include "srsran/ofh/compression/iq_decompressor.h"
#include "srsran/support/units.h"

using namespace srsran;
using namespace ofh;

/// Number of bytes of the User-Plane header.
static constexpr unsigned NOF_BYTES_UP_HEADER = 4U;

/// Size in bytes of a section ID header with no compression.
static constexpr unsigned SECTION_ID_HEADER_NO_COMPRESSION_SIZE = 4U;

bool uplane_message_decoder_impl::decode(uplane_message_decoder_results& results, span<const uint8_t> message)
{
  network_order_binary_deserializer deserializer(message);

  // Decode the header.
  if (!decode_header(results.params, deserializer)) {
    return false;
  }

  // Decode the sections from the message.
  if (!decode_all_sections(results, deserializer)) {
    return false;
  }

  return true;
}

/// Checks the Open Fronthaul User-Plane header and returns true on success, otherwise false.
static bool is_header_valid(const uplane_message_params& params,
                            srslog::basic_logger&        logger,
                            unsigned                     nof_symbols,
                            unsigned                     version)
{
  if (params.direction != data_direction::uplink) {
    logger.debug("Dropping incoming Open Fronthaul message as it is not an uplink message");

    return false;
  }

  if (version != OFH_PAYLOAD_VERSION) {
    logger.debug("Dropping incoming Open Fronthaul message as its payload version is {} but only {} is supported",
                 version,
                 OFH_PAYLOAD_VERSION);

    return false;
  }

  if (params.filter_index == filter_index_type::reserved) {
    logger.debug("Dropping incoming Open Fronthaul message as its filter index is a reserved value {}",
                 params.filter_index);

    return false;
  }

  if (params.symbol_id >= nof_symbols) {
    logger.debug("Dropping incoming Open Fronthaul message as its symbol index is {} and this decoder supports up to "
                 "{} symbols",
                 params.symbol_id,
                 nof_symbols);

    return false;
  }

  return true;
}

bool uplane_message_decoder_impl::decode_header(uplane_message_params&             params,
                                                network_order_binary_deserializer& deserializer)
{
  if (deserializer.remaining_bytes() < NOF_BYTES_UP_HEADER) {
    logger.debug(
        "Dropping incoming Open Fronthaul message as its size is {} and it is smaller than the message header size",
        deserializer.remaining_bytes());

    return false;
  }

  uint8_t value       = deserializer.read<uint8_t>();
  params.direction    = static_cast<data_direction>(value >> 7);
  uint8_t version     = (value >> 4) & 7;
  params.filter_index = to_filter_index_type(value & 0xf);

  uint8_t  frame             = deserializer.read<uint8_t>();
  uint8_t  subframe_and_slot = deserializer.read<uint8_t>();
  uint8_t  subframe          = subframe_and_slot >> 4;
  unsigned slot_id           = 0;
  slot_id |= (subframe_and_slot & 0x0f) << 2;

  uint8_t slot_and_symbol = deserializer.read<uint8_t>();
  params.symbol_id        = slot_and_symbol & 0x3f;
  slot_id |= slot_and_symbol >> 6;

  params.slot = slot_point(to_numerology_value(scs), frame, subframe, slot_id);

  return is_header_valid(params, logger, nof_symbols, version);
}

bool uplane_message_decoder_impl::decode_all_sections(uplane_message_decoder_results&    results,
                                                      network_order_binary_deserializer& deserializer)
{
  // Decode sections while the message has bytes remaining.
  while (deserializer.remaining_bytes()) {
    // Try to decode section.
    if (!decode_section(results, deserializer)) {
      break;
    }
  }

  bool is_result_valid = !results.sections.empty();
  if (!is_result_valid) {
    logger.debug(
        "Dropping incoming Open Fronthaul message as no section was decoded correctly. Message slot={}, symbol={}",
        results.params.slot,
        results.params.symbol_id);
  }

  return is_result_valid;
}

bool uplane_message_decoder_impl::decode_section(uplane_message_decoder_results&    results,
                                                 network_order_binary_deserializer& deserializer)
{
  // Add a section to the results.
  uplane_section_params ofh_up_section;

  if (!decode_section_header(ofh_up_section, deserializer)) {
    return false;
  }

  if (!decode_compression_header(ofh_up_section, deserializer, is_a_prach_message(results.params.filter_index))) {
    return false;
  }

  if (!decode_compression_length(ofh_up_section, deserializer, ofh_up_section.ud_comp_hdr)) {
    return false;
  }

  if (!decode_iq_data(ofh_up_section, deserializer, ofh_up_section.ud_comp_hdr)) {
    return false;
  }

  results.sections.emplace_back(ofh_up_section);

  return true;
}

bool uplane_message_decoder_impl::decode_section_header(uplane_section_params&             results,
                                                        network_order_binary_deserializer& deserializer)
{
  if (deserializer.remaining_bytes() < SECTION_ID_HEADER_NO_COMPRESSION_SIZE) {
    logger.debug("Incoming Open Fronthaul message size is {} and it is smaller than the section header size",
                 deserializer.remaining_bytes());

    return false;
  }

  results.section_id = 0;
  results.section_id |= unsigned(deserializer.read<uint8_t>()) << 4;
  uint8_t section_and_rest = deserializer.read<uint8_t>();
  results.section_id |= section_and_rest >> 4;
  results.is_every_rb_used          = ((section_and_rest >> 3) & 1U) == 0;
  results.use_current_symbol_number = ((section_and_rest >> 2) & 1U) == 0;

  unsigned& start_prb = results.start_prb;
  start_prb           = 0;
  start_prb |= unsigned(section_and_rest & 0x03) << 8;
  start_prb |= unsigned(deserializer.read<uint8_t>());

  unsigned& nof_prb = results.nof_prbs;
  nof_prb           = deserializer.read<uint8_t>();
  nof_prb           = (nof_prb == 0) ? ru_nof_prbs : nof_prb;

  return true;
}

/// \brief Decodes the compressed PRBs from the deserializer and returns true on success, otherwise false.
///
/// This function skips the udCompParam field.
///
/// \param[out] comp_prb Compressed PRBs to decode.
/// \param[in] deserializer Deserializer.
/// \param[in] prb_iq_data_size PRB size in bits.
/// \param[in] logger Logger.
/// \return True on success, false otherwise.
static bool decode_prbs_no_ud_comp_param_field(span<compressed_prb>               comp_prb,
                                               network_order_binary_deserializer& deserializer,
                                               units::bits                        prb_iq_data_size,
                                               srslog::basic_logger&              logger)
{
  if (deserializer.remaining_bytes() < prb_iq_data_size.round_up_to_bytes().value() * comp_prb.size()) {
    logger.debug("Incoming Open Fronthaul message size is {} and it is smaller than the expected IQ samples size {}",
                 deserializer.remaining_bytes(),
                 prb_iq_data_size.round_up_to_bytes().value() * comp_prb.size());

    return false;
  }

  unsigned nof_bytes = prb_iq_data_size.round_up_to_bytes().value();

  // Read the samples from the deserializer.
  for (auto& prb : comp_prb) {
    // No need to read the udCompParam field.
    prb.set_compression_param(0);

    deserializer.read(prb.get_byte_buffer().first(nof_bytes));
    prb.set_stored_size(nof_bytes);
  }

  return true;
}

/// \brief Decodes the compressed PRBs from the deserializer and returns true on success, otherwise false.
///
/// This function decodes the udCompParam field.
///
/// \param[out] comp_prb Compressed PRBs to decode.
/// \param[in] deserializer Deserializer.
/// \param[in] prb_iq_data_size PRB size in bits.
/// \param[in] logger Logger.
/// \return True on success, false otherwise.
static bool decode_prbs_with_ud_comp_param_field(span<compressed_prb>               comp_prb,
                                                 network_order_binary_deserializer& deserializer,
                                                 units::bits                        prb_iq_data_size,
                                                 srslog::basic_logger&              logger)
{
  // Add 1 byte to the PRB size as the udComParam must be decoded.
  units::bytes prb_bytes = prb_iq_data_size.round_up_to_bytes() + units::bytes(1);
  if (deserializer.remaining_bytes() < prb_bytes.value() * comp_prb.size()) {
    logger.debug("Incoming Open Fronthaul message size is {} and it is smaller than the expected IQ samples size {}",
                 deserializer.remaining_bytes(),
                 prb_bytes.value() * comp_prb.size());

    return false;
  }

  unsigned nof_bytes = prb_iq_data_size.round_up_to_bytes().value();

  // For each PRB, udCompParam must be decoded.
  for (auto& prb : comp_prb) {
    prb.set_compression_param(deserializer.read<uint8_t>());

    deserializer.read(prb.get_byte_buffer().first(nof_bytes));
    prb.set_stored_size(nof_bytes);
  }

  return true;
}

bool uplane_message_decoder_impl::decode_compression_length(uplane_section_params&             results,
                                                            network_order_binary_deserializer& deserializer,
                                                            const ru_compression_params&       compression_params)
{
  switch (compression_params.type) {
    case compression_type::none:
    case compression_type::BFP:
    case compression_type::block_scaling:
    case compression_type::mu_law:
    case compression_type::modulation:
      return true;
    default:
      break;
  }

  if (deserializer.remaining_bytes() < sizeof(uint16_t)) {
    logger.debug("Incoming Open Fronthaul message size is {} and it is smaller than the user data compression length",
                 deserializer.remaining_bytes());

    return false;
  }

  results.ud_comp_len.emplace(deserializer.read<uint16_t>());

  return true;
}

bool uplane_message_decoder_impl::decode_iq_data(uplane_section_params&             results,
                                                 network_order_binary_deserializer& deserializer,
                                                 const ru_compression_params&       compression_params)
{
  std::array<compressed_prb, MAX_NOF_PRBS> comp_prbs_buffer;
  span<compressed_prb>                     comp_prbs(comp_prbs_buffer.data(), results.nof_prbs);
  units::bits prb_iq_data_size_bits(NOF_SUBCARRIERS_PER_RB * 2 * compression_params.data_width);

  // udCompParam field is not present when compression type is none or modulation.
  if (compression_params.type == compression_type::none || compression_params.type == compression_type::modulation) {
    if (!decode_prbs_no_ud_comp_param_field(comp_prbs, deserializer, prb_iq_data_size_bits, logger)) {
      return false;
    }
  } else {
    if (!decode_prbs_with_ud_comp_param_field(comp_prbs, deserializer, prb_iq_data_size_bits, logger)) {
      return false;
    }
  }

  // Decompress the samples.
  results.iq_samples.resize(results.nof_prbs * NOF_SUBCARRIERS_PER_RB);
  decompressor->decompress(results.iq_samples, comp_prbs, compression_params);

  return true;
}

filter_index_type uplane_message_decoder_impl::peek_filter_index(span<const uint8_t> message) const
{
  if (message.empty()) {
    return filter_index_type::reserved;
  }

  // Filter index is codified in the first byte, the 4 LSB.
  return to_filter_index_type((message[0] & 0xf));
}

slot_symbol_point uplane_message_decoder_impl::peek_slot_symbol_point(span<const uint8_t> message) const
{
  // Slot is codified in the first 4 bytes of the Open Fronthaul message.
  if (message.size() < 4) {
    return {slot_point{}, 0, nof_symbols};
  }

  uint8_t  frame             = message[1];
  uint8_t  subframe_and_slot = message[2];
  uint8_t  subframe          = subframe_and_slot >> 4;
  unsigned slot_id           = 0;
  slot_id |= (subframe_and_slot & 0x0f) << 2;

  uint8_t  slot_and_symbol = message[3];
  unsigned symbol_id       = slot_and_symbol & 0x3f;
  slot_id |= slot_and_symbol >> 6;

  return {slot_point(to_numerology_value(scs), frame, subframe, slot_id), symbol_id, nof_symbols};
}
