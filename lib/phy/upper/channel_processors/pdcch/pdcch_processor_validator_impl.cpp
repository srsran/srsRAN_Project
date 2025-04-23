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

#include "pdcch_processor_validator_impl.h"

using namespace srsran;

error_type<std::string> pdcch_processor_validator_impl::is_valid(const pdcch_processor::pdu_t& pdu) const

{
  static constexpr interval<unsigned, true> coreset_duration_range(1, pdcch_constants::MAX_CORESET_DURATION);
  if (!coreset_duration_range.contains(pdu.coreset.duration)) {
    return make_unexpected(fmt::format(
        "The CORESET duration (i.e., {}) is out of the range {}-", pdu.coreset.duration, coreset_duration_range));
  }

  unsigned slot_duration = get_nsymb_per_slot(pdu.cp);
  if (pdu.coreset.start_symbol_index + pdu.coreset.duration > slot_duration) {
    return make_unexpected(fmt::format("The CORESET start symbol index (i.e., {}) plus the duration (i.e., {}) "
                                       "exceeds the slot duration (i.e., {}).",
                                       pdu.coreset.start_symbol_index,
                                       pdu.coreset.duration,
                                       slot_duration));
  }

  bool use_interleaver_mapping =
      (pdu.coreset.cce_to_reg_mapping == pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED);
  if (use_interleaver_mapping &&
      (((pdu.coreset.duration == 3) && (pdu.coreset.reg_bundle_size != 3)) ||
       ((pdu.coreset.duration != 3) && (pdu.coreset.reg_bundle_size != 2))) &&
      (pdu.coreset.reg_bundle_size != 6)) {
    return make_unexpected(fmt::format("Invalid REG bundle size (i.e., {}) for CORESET duration of {}.",
                                       pdu.coreset.reg_bundle_size,
                                       pdu.coreset.duration));
  }

  if (use_interleaver_mapping && (pdu.coreset.interleaver_size != 2) && (pdu.coreset.interleaver_size != 3) &&
      (pdu.coreset.interleaver_size != 6)) {
    return make_unexpected(fmt::format("Invalid interleaver size (i.e., {}).", pdu.coreset.interleaver_size));
  }

  if ((pdu.dci.aggregation_level != 1) && (pdu.dci.aggregation_level != 2) && (pdu.dci.aggregation_level != 4) &&
      (pdu.dci.aggregation_level != 8) && (pdu.dci.aggregation_level != 16)) {
    return make_unexpected(fmt::format("Invalid aggregation level (i.e., {}).", pdu.dci.aggregation_level));
  }

  unsigned nof_coreset_cce = pdu.coreset.frequency_resources.count() * pdu.coreset.duration;
  if (pdu.dci.cce_index + pdu.dci.aggregation_level > nof_coreset_cce) {
    return make_unexpected(fmt::format(
        "The CCE index (i.e., {}) plus the aggregation level (i.e., {}) exceeds CORESET capacity (i.e., {}).",
        pdu.dci.cce_index,
        pdu.dci.aggregation_level,
        nof_coreset_cce));
  }

  if (pdu.dci.payload.empty()) {
    return make_unexpected("Empty payload.");
  }

  if (pdu.dci.precoding.get_nof_layers() != 1) {
    return make_unexpected(
        fmt::format("Precoding number of layers (i.e., {}) must be one.", pdu.dci.precoding.get_nof_layers()));
  }

  return default_success_t();
}
