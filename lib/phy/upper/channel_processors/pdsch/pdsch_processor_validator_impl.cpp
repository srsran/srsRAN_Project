/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

error_type<std::string> pdsch_processor_validator_impl::is_valid(const pdsch_processor::pdu_t& pdu) const
{
  using namespace units::literals;

  unsigned         nof_symbols_slot = get_nsymb_per_slot(pdu.cp);
  dmrs_config_type dmrs_config = (pdu.dmrs == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2;

  // The frequency allocation is not compatible with the BWP parameters.
  if (!pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb)) {
    return make_unexpected(
        fmt::format("Invalid BWP configuration, i.e., [{}, {}) for the given RB allocation, i.e., {}.",
                    pdu.bwp_start_rb,
                    pdu.bwp_start_rb + pdu.bwp_size_rb,
                    pdu.freq_alloc));
  }

  // The size of the DM-RS symbol mask must coincide with the number of symbols in the slot.
  if (pdu.dmrs_symbol_mask.size() != nof_symbols_slot) {
    return make_unexpected(
        fmt::format("The DM-RS symbol mask size (i.e., {}) must be equal to the slot size (i.e., {}).",
                    pdu.dmrs_symbol_mask.size(),
                    nof_symbols_slot));
  }

  // The number of OFDM symbols carrying DM-RS must be greater than zero.
  if (pdu.dmrs_symbol_mask.none()) {
    return make_unexpected("The number of OFDM symbols carrying DM-RS must be greater than zero.");
  }

  // The index of the first OFDM symbol carrying DM-RS shall be equal to or greater than the first symbol allocated to
  // transmission.
  int first_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_lowest(true);
  if (static_cast<unsigned>(first_dmrs_symbol_index) < pdu.start_symbol_index) {
    return make_unexpected(fmt::format("The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to "
                                       "or greater than the first symbol allocated to transmission (i.e., {}).",
                                       first_dmrs_symbol_index,
                                       pdu.start_symbol_index));
  }

  // The index of the last OFDM symbol carrying DM-RS shall not be larger than the last symbol allocated to
  // transmission.
  int last_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_highest(true);
  if (static_cast<unsigned>(last_dmrs_symbol_index) >= (pdu.start_symbol_index + pdu.nof_symbols)) {
    return make_unexpected(fmt::format("The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than "
                                       "or equal to the last symbol allocated to transmission (i.e., {}).",
                                       last_dmrs_symbol_index,
                                       (pdu.start_symbol_index + pdu.nof_symbols - 1)));
  }

  // None of the occupied symbols must exceed the slot size.
  if (nof_symbols_slot < (pdu.start_symbol_index + pdu.nof_symbols)) {
    return make_unexpected(fmt::format("The symbol allocation (i.e., [{}, {})) exceeds the slot size (i.e., {}).",
                                       pdu.start_symbol_index,
                                       pdu.start_symbol_index + pdu.nof_symbols,
                                       nof_symbols_slot));
  }

  // Only DM-RS Type 1 is supported.
  if (pdu.dmrs != dmrs_type::TYPE1) {
    return make_unexpected("Only DM-RS Type 1 is currently supported.");
  }

  // The number of CDM groups without data must not exceed the maximum supported by the DM-RS type.
  if (pdu.nof_cdm_groups_without_data > get_max_nof_cdm_groups_without_data(dmrs_config)) {
    return make_unexpected(fmt::format(
        "The number of CDM groups without data (i.e., {}) must not exceed the maximum supported by the DM-RS "
        "type (i.e., {}).",
        pdu.nof_cdm_groups_without_data,
        get_max_nof_cdm_groups_without_data(dmrs_config)));
  }

  // Only contiguous allocation is currently supported.
  if (!pdu.freq_alloc.is_vrb_mask_contiguous()) {
    return make_unexpected("Only contiguous VRB mask allocation is currently supported.");
  }

  // Only one codeword is currently supported.
  if (pdu.codewords.size() != 1) {
    return make_unexpected("Only one codeword is currently supported.");
  }

  // The limited buffer for rate matching size must not be zero.
  if (pdu.tbs_lbrm == 0_bytes) {
    return make_unexpected("Invalid LBRM size (0 bytes).");
  }

  // Make sure the DM-RS and the reserved resource elements do not collide.
  if (check_dmrs_and_reserved_collision(pdu)) {
    return make_unexpected("Reserved REs collide with DM-RS.");
  }

  return default_success_t();
}
