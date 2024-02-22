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
    logger.info("Dropped received Open Fronthaul message as it is not an uplink message");

    return false;
  }

  if (version != OFH_PAYLOAD_VERSION) {
    logger.info(
        "Dropped received Open Fronthaul message as its payload version is '{}' but only version '{}' is supported",
        version,
        OFH_PAYLOAD_VERSION);

    return false;
  }

  if (params.filter_index == filter_index_type::reserved) {
    logger.info("Dropped received Open Fronthaul message as its filter index value is reserved '{}'",
                params.filter_index);

    return false;
  }

  if (params.symbol_id >= nof_symbols) {
    logger.info("Dropped received Open Fronthaul message as its symbol index is '{}' and this decoder supports a "
                "maximum of '{}' symbols",
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
    logger.info("Dropped received Open Fronthaul message as its size is '{}' bytes and it is smaller than the message "
                "header size",
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

  // No need to check the frame property, as its range is [0,256), and the slot_point frame range is [0,1024).

  // Check the subframe property.
  if (subframe >= NOF_SUBFRAMES_PER_FRAME) {
    logger.info("Dropped received Open Fronthaul message as the decoded subframe property '{}' is invalid", subframe);

    return false;
  }

  // Check the slot property.
  if (slot_id >= slot_point(scs, 0).nof_slots_per_subframe()) {
    logger.info("Dropped received Open Fronthaul message as the decoded slot property '{}' is invalid", slot_id);

    return false;
  }

  params.slot = slot_point(to_numerology_value(scs), frame, subframe, slot_id);

  return is_header_valid(params, logger, nof_symbols, version);
}

bool uplane_message_decoder_impl::decode_all_sections(uplane_message_decoder_results&    results,
                                                      network_order_binary_deserializer& deserializer)
{
  // Decode sections while the message has bytes remaining.
  while (deserializer.remaining_bytes()) {
    // Try to decode section.
    decoded_section_status status = decode_section(results, deserializer);

    // Incomplete sections force the exit of the loop.
    if (status == decoded_section_status::incomplete) {
      break;
    }

    if (status == decoded_section_status::malformed) {
      logger.info(
          "Dropped received Open Fronthaul message as a malformed section was decoded for slot '{}' and symbol '{}'",
          results.params.slot,
          results.params.symbol_id);

      return false;
    }

    if (results.sections.full()) {
      logger.info("Dropped received Open Fronthaul message as this deserializer only supports '{}' section for slot "
                  "'{}' and symbol '{}'",
                  MAX_NOF_SUPPORTED_SECTIONS,
                  results.params.slot,
                  results.params.symbol_id);

      return false;
    }
  }

  bool is_result_valid = !results.sections.empty();
  if (!is_result_valid) {
    logger.info(
        "Dropped received Open Fronthaul message as no section was decoded correctly for slot '{}' and symbol '{}'",
        results.params.slot,
        results.params.symbol_id);
  }

  return is_result_valid;
}

/// Fills the given User-Plane section parameters using the given decoded User-Plane section parameters.
static void fill_results_from_decoder_section(uplane_section_params&               results,
                                              const decoder_uplane_section_params& decoded_results)
{
  results.section_id                = decoded_results.section_id;
  results.is_every_rb_used          = decoded_results.is_every_rb_used;
  results.use_current_symbol_number = decoded_results.use_current_symbol_number;
  results.start_prb                 = decoded_results.start_prb;
  results.nof_prbs                  = decoded_results.nof_prbs;
  results.ud_comp_hdr               = decoded_results.ud_comp_hdr;
  results.ud_comp_len               = decoded_results.ud_comp_len;
  results.ud_comp_param             = decoded_results.ud_comp_param;
}

/// Returns true if the compression parameter is present based on the given compression type.
static bool is_ud_comp_param_present(compression_type comp)
{
  switch (comp) {
    case compression_type::BFP:
    case compression_type::block_scaling:
    case compression_type::mu_law:
    case compression_type::bfp_selective:
    case compression_type::mod_selective:
      return true;
    case compression_type::none:
    case compression_type::modulation:
      return false;
    default:
      srsran_assert(0, "Invalid compression type '{}'", comp);
  }

  SRSRAN_UNREACHABLE;
}

/// \brief Returns true when the given deserializer contains enough bytes to decode the IQ samples defined by PRB IQ
/// data size, otherwise false.
///
/// \param[out] nof_prb Number of PRBs.
/// \param[in] deserializer Deserializer.
/// \param[in] compression_params Compression parameters.
/// \param[in] logger Logger.
/// \return True on success, false otherwise.
static bool check_iq_data_size(unsigned                           nof_prb,
                               network_order_binary_deserializer& deserializer,
                               const ru_compression_params&       compression_params,
                               srslog::basic_logger&              logger)
{
  units::bytes prb_iq_data_size(
      units::bits(NOF_SUBCARRIERS_PER_RB * 2 * compression_params.data_width).round_up_to_bytes().value());

  // Add one byte when the udCompParam is present.
  if (is_ud_comp_param_present(compression_params.type)) {
    prb_iq_data_size = prb_iq_data_size + units::bytes(1);
  }

  if (deserializer.remaining_bytes() < prb_iq_data_size.value() * nof_prb) {
    logger.info("Received Open Fronthaul message size is '{}' bytes and it is smaller than the expected IQ samples "
                "size of '{}'",
                deserializer.remaining_bytes(),
                prb_iq_data_size.value() * nof_prb);

    return false;
  }

  return true;
}

uplane_message_decoder_impl::decoded_section_status
uplane_message_decoder_impl::decode_section(uplane_message_decoder_results&    results,
                                            network_order_binary_deserializer& deserializer)
{
  // Add a section to the results.
  decoder_uplane_section_params decoder_ofh_up_section;

  decoded_section_status status = decode_section_header(decoder_ofh_up_section, deserializer);

  if (status != decoded_section_status::ok) {
    return status;
  }

  status = decode_compression_header(decoder_ofh_up_section, deserializer);
  if (status != decoded_section_status::ok) {
    return status;
  }

  status = decode_compression_length(decoder_ofh_up_section, deserializer, decoder_ofh_up_section.ud_comp_hdr);
  if (status != decoded_section_status::ok) {
    return status;
  }

  // Check the message contains the required IQ data.
  if (!check_iq_data_size(decoder_ofh_up_section.nof_prbs, deserializer, decoder_ofh_up_section.ud_comp_hdr, logger)) {
    return decoded_section_status::incomplete;
  }

  // Add new section.
  auto& section = results.sections.emplace_back();
  fill_results_from_decoder_section(section, decoder_ofh_up_section);

  // Decode the IQ data.
  decode_iq_data(section, deserializer, section.ud_comp_hdr);

  return decoded_section_status::ok;
}

uplane_message_decoder_impl::decoded_section_status
uplane_message_decoder_impl::decode_section_header(decoder_uplane_section_params&     results,
                                                   network_order_binary_deserializer& deserializer)
{
  if (deserializer.remaining_bytes() < SECTION_ID_HEADER_NO_COMPRESSION_SIZE) {
    logger.info("Received Open Fronthaul message size is '{}' bytes and is smaller than the section header size",
                deserializer.remaining_bytes());

    return decoded_section_status::incomplete;
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

  return decoded_section_status::ok;
}

/// \brief Decodes the compressed PRBs from the deserializer.
///
/// This function skips the udCompParam field.
///
/// \param[out] comp_prb Compressed PRBs to decode.
/// \param[in] deserializer Deserializer.
/// \param[in] prb_iq_data_size PRB size in bits.
static void decode_prbs_no_ud_comp_param_field(span<compressed_prb>               comp_prb,
                                               network_order_binary_deserializer& deserializer,
                                               units::bits                        prb_iq_data_size)
{
  unsigned nof_bytes = prb_iq_data_size.round_up_to_bytes().value();

  // Read the samples from the deserializer.
  for (auto& prb : comp_prb) {
    // No need to read the udCompParam field.
    prb.set_compression_param(0);

    deserializer.read(prb.get_byte_buffer().first(nof_bytes));
    prb.set_stored_size(nof_bytes);
  }
}

/// \brief Decodes the compressed PRBs from the deserializer.
///
/// This function decodes the udCompParam field.
///
/// \param[out] comp_prb Compressed PRBs to decode.
/// \param[in] deserializer Deserializer.
/// \param[in] prb_iq_data_size PRB size in bits.
static void decode_prbs_with_ud_comp_param_field(span<compressed_prb>               comp_prb,
                                                 network_order_binary_deserializer& deserializer,
                                                 units::bits                        prb_iq_data_size)
{
  unsigned nof_bytes = prb_iq_data_size.round_up_to_bytes().value();

  // For each PRB, udCompParam must be decoded.
  for (auto& prb : comp_prb) {
    // Decode udComParam.
    prb.set_compression_param(deserializer.read<uint8_t>());

    // Decode IQ data.
    deserializer.read(prb.get_byte_buffer().first(nof_bytes));
    prb.set_stored_size(nof_bytes);
  }
}

uplane_message_decoder_impl::decoded_section_status
uplane_message_decoder_impl::decode_compression_length(decoder_uplane_section_params&     results,
                                                       network_order_binary_deserializer& deserializer,
                                                       const ru_compression_params&       compression_params)
{
  switch (compression_params.type) {
    case compression_type::none:
    case compression_type::BFP:
    case compression_type::block_scaling:
    case compression_type::mu_law:
    case compression_type::modulation:
      return decoded_section_status::ok;
    default:
      break;
  }

  if (deserializer.remaining_bytes() < sizeof(uint16_t)) {
    logger.info(
        "Received Open Fronthaul message size is '{}' bytes and is smaller than the user data compression length",
        deserializer.remaining_bytes());

    return decoded_section_status::incomplete;
  }

  results.ud_comp_len.emplace(deserializer.read<uint16_t>());

  return decoded_section_status::ok;
}

void uplane_message_decoder_impl::decode_iq_data(uplane_section_params&             results,
                                                 network_order_binary_deserializer& deserializer,
                                                 const ru_compression_params&       compression_params)
{
  std::array<compressed_prb, MAX_NOF_PRBS> comp_prbs_buffer;
  span<compressed_prb>                     comp_prbs(comp_prbs_buffer.data(), results.nof_prbs);
  units::bits prb_iq_data_size_bits(NOF_SUBCARRIERS_PER_RB * 2 * compression_params.data_width);

  // udCompParam field is not present when compression type is none or modulation.
  if (is_ud_comp_param_present(compression_params.type)) {
    decode_prbs_with_ud_comp_param_field(comp_prbs, deserializer, prb_iq_data_size_bits);
  } else {
    decode_prbs_no_ud_comp_param_field(comp_prbs, deserializer, prb_iq_data_size_bits);
  }

  // Decompress the samples.
  results.iq_samples.resize(results.nof_prbs * NOF_SUBCARRIERS_PER_RB);
  decompressor->decompress(results.iq_samples, comp_prbs, compression_params);
}

filter_index_type srsran::ofh::uplane_peeker::peek_filter_index(span<const uint8_t> message)
{
  if (message.empty()) {
    return filter_index_type::reserved;
  }

  // Filter index is codified in the first byte, the 4 LSB.
  return to_filter_index_type(message[0] & 0xf);
}

slot_symbol_point srsran::ofh::uplane_peeker::peek_slot_symbol_point(span<const uint8_t> message,
                                                                     unsigned            nof_symbols,
                                                                     subcarrier_spacing  scs)
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

  if (subframe >= NOF_SUBFRAMES_PER_FRAME) {
    return {slot_point{}, 0, nof_symbols};
  }

  // Check the slot property.
  if (slot_id >= slot_point(scs, 0).nof_slots_per_subframe()) {
    return {slot_point{}, 0, nof_symbols};
  }

  return {slot_point(to_numerology_value(scs), frame, subframe, slot_id), symbol_id, nof_symbols};
}
