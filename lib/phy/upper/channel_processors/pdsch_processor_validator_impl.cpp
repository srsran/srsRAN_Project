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

#include "pdsch_processor_validator_impl.h"
#include "srsran/ran/dmrs.h"

using namespace srsran;

/// Checks that reserved elements do not collide with DM-RS.
static bool check_dmrs_and_reserved_collision(const pdsch_processor::pdu_t& config)
{
  // Create an OFDM symbol mask of all reserved elements to check it doesn't collide with the DM-RS one.
  symbol_slot_mask reserved_symbol_mask(config.dmrs_symbol_mask.size());
  for (const re_pattern& reserved_pattern : config.reserved.get_re_patterns()) {
    reserved_symbol_mask |= reserved_pattern.symbols;
  }
  reserved_symbol_mask &= config.dmrs_symbol_mask;

  return reserved_symbol_mask.any();
}

void pdsch_processor_validator_impl::assert_pdu(const pdsch_processor::pdu_t& pdu)
{
  // Recover parameters from the PDU.
  unsigned         nof_layers       = pdu.precoding.get_nof_layers();
  unsigned         nof_codewords    = (nof_layers > 4) ? 2 : 1;
  unsigned         nof_symbols_slot = get_nsymb_per_slot(pdu.cp);
  dmrs_config_type dmrs_config = (pdu.dmrs == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2;

  srsran_assert(pdu.dmrs_symbol_mask.size() == nof_symbols_slot,
                "The DM-RS symbol mask size (i.e., {}), must be equal to the number of symbols in the slot (i.e., {}).",
                pdu.dmrs_symbol_mask.size(),
                nof_symbols_slot);
  srsran_assert(pdu.dmrs_symbol_mask.any(), "The number of OFDM symbols carrying DM-RS must be greater than zero.");
  srsran_assert(static_cast<unsigned>(pdu.dmrs_symbol_mask.find_lowest(true)) >= pdu.start_symbol_index,
                "The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to or greater than the "
                "first symbol allocated to transmission (i.e., {}).",
                pdu.dmrs_symbol_mask.find_lowest(true),
                pdu.start_symbol_index);
  srsran_assert(static_cast<unsigned>(pdu.dmrs_symbol_mask.find_highest(true)) <
                    (pdu.start_symbol_index + pdu.nof_symbols),
                "The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than or equal to the last "
                "symbol allocated to transmission (i.e., {}).",
                pdu.dmrs_symbol_mask.find_highest(true),
                pdu.start_symbol_index + pdu.nof_symbols - 1);
  srsran_assert((pdu.start_symbol_index + pdu.nof_symbols) <= nof_symbols_slot,
                "The transmission with time allocation [{}, {}) exceeds the slot boundary of {} symbols.",
                pdu.start_symbol_index,
                pdu.start_symbol_index + pdu.nof_symbols,
                nof_symbols_slot);
  srsran_assert(pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb),
                "Invalid BWP configuration [{}, {}) for the given frequency allocation {}.",
                pdu.bwp_start_rb,
                pdu.bwp_start_rb + pdu.bwp_size_rb,
                pdu.freq_alloc);
  srsran_assert(pdu.dmrs == dmrs_type::TYPE1, "Only DM-RS Type 1 is currently supported.");
  srsran_assert(pdu.freq_alloc.is_contiguous(), "Only contiguous allocation is currently supported.");
  srsran_assert(pdu.nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(dmrs_config),
                "The number of CDM groups without data (i.e., {}) must not exceed the maximum supported by the DM-RS "
                "type (i.e., {}).",
                pdu.nof_cdm_groups_without_data,
                get_max_nof_cdm_groups_without_data(dmrs_config));
  srsran_assert(nof_layers != 0, "No transmit layers are active.");
  srsran_assert(nof_layers <= 4, "Only 1 to 4 layers are currently supported. {} layers requested.", nof_layers);

  srsran_assert(pdu.codewords.size() == nof_codewords,
                "Expected {} codewords and got {} for {} layers.",
                nof_codewords,
                pdu.codewords.size(),
                nof_layers);
  srsran_assert(pdu.tbs_lbrm_bytes > 0 && pdu.tbs_lbrm_bytes <= ldpc::MAX_CODEBLOCK_SIZE / 8,
                "Invalid LBRM size ({} bytes). It must be non-zero, less than or equal to {} bytes",
                pdu.tbs_lbrm_bytes,
                ldpc::MAX_CODEBLOCK_SIZE / 8);

  srsran_assert(!check_dmrs_and_reserved_collision(pdu),
                "The DM-RS symbol mask must not collide with reserved elements.");
};

bool pdsch_processor_validator_impl::is_valid(const pdsch_processor::pdu_t& pdu) const
{
  unsigned         nof_symbols_slot = get_nsymb_per_slot(pdu.cp);
  dmrs_config_type dmrs_config = (pdu.dmrs == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2;

  // The frequency allocation is not compatible with the BWP parameters.
  if (!pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb)) {
    return false;
  }

  // The size of the DM-RS symbol mask must coincide with the number of symbols in the slot.
  if (pdu.dmrs_symbol_mask.size() != nof_symbols_slot) {
    return false;
  }

  // The number of OFDM symbols carrying DM-RS must be greater than zero.
  if (pdu.dmrs_symbol_mask.empty()) {
    return false;
  }

  // The index of the first OFDM symbol carrying DM-RS shall be equal to or greater than the first symbol allocated to
  // transmission.
  int first_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_lowest(true);
  if (static_cast<unsigned>(first_dmrs_symbol_index) < pdu.start_symbol_index) {
    return false;
  }

  // The index of the last OFDM symbol carrying DM-RS shall not be larger than the last symbol allocated to
  // transmission.
  int last_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_highest(true);
  if (static_cast<unsigned>(last_dmrs_symbol_index) >= (pdu.start_symbol_index + pdu.nof_symbols)) {
    return false;
  }

  // None of the occupied symbols must exceed the slot size.
  if (nof_symbols_slot < (pdu.start_symbol_index + pdu.nof_symbols)) {
    return false;
  }

  // Only DM-RS Type 1 is supported.
  if (pdu.dmrs != dmrs_type::TYPE1) {
    return false;
  }

  // The number of CDM groups without data must not exceed the maximum supported by the DM-RS type.
  if (pdu.nof_cdm_groups_without_data > get_max_nof_cdm_groups_without_data(dmrs_config)) {
    return false;
  }

  // Only contiguous allocation is currently supported.
  if (!pdu.freq_alloc.is_contiguous()) {
    return false;
  }

  // Only one to four antenna ports are currently supported.
  if ((pdu.precoding.get_nof_ports() == 0) || (pdu.precoding.get_nof_ports() > 4)) {
    return false;
  }

  // The number of layers cannot be zero or larger than the number of ports.
  if ((pdu.precoding.get_nof_layers() == 0) || (pdu.precoding.get_nof_layers() > pdu.precoding.get_nof_ports())) {
    return false;
  }

  // Only one codeword is currently supported.
  if (pdu.codewords.size() != 1) {
    return false;
  }

  // The limited buffer for rate matching size must be within limits.
  if ((pdu.tbs_lbrm_bytes == 0) || (pdu.tbs_lbrm_bytes > ldpc::MAX_CODEBLOCK_SIZE / 8)) {
    return false;
  }

  // Make sure the DM-RS and the reserved resource elements do not collide.
  if (check_dmrs_and_reserved_collision(pdu)) {
    return false;
  }

  return true;
}
