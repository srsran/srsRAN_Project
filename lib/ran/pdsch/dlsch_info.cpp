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

#include "srsran/ran/pdsch/dlsch_info.h"
#include "srsran/phy/constants.h"

using namespace srsran;

dlsch_information srsran::get_dlsch_information(const dlsch_configuration& config)
{
  using namespace units::literals;

  dlsch_information result = {};

  // Get shared channel parameters.
  result.sch = get_sch_segmentation_info(config.tbs, config.mcs_descr.get_normalised_target_code_rate());

  // Check whether the number of CDM groups without data is valid.
  srsran_assert(config.nof_cdm_groups_without_data >= 1 &&
                    config.nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(config.dmrs_type),
                "The number of CDM groups without data (i.e., {}) is invalid (min. 1, max. {}).",
                config.nof_cdm_groups_without_data,
                get_max_nof_cdm_groups_without_data(config.dmrs_type));

  // Make sure the OFDM symbol that carry DM-RS are within the time allocation.
  srsran_assert(config.dmrs_symbol_mask.count() > 0,
                "The number of OFDM symbols carrying DM-RS RE must be greater than zero.");
  srsran_assert(
      static_cast<unsigned>(config.dmrs_symbol_mask.find_lowest(true)) >= config.start_symbol_index,
      "The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to or greater than the first symbol "
      "allocated to transmission (i.e., {}).",
      config.dmrs_symbol_mask.find_lowest(true),
      config.start_symbol_index);
  srsran_assert(static_cast<unsigned>(config.dmrs_symbol_mask.find_highest(true)) <
                    (config.start_symbol_index + config.nof_symbols),
                "The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than or equal to the last "
                "symbol allocated "
                "to transmission (i.e., {}).",
                config.dmrs_symbol_mask.find_highest(true),
                config.start_symbol_index + config.nof_symbols - 1);
  srsran_assert(config.dmrs_symbol_mask.size() >= (config.start_symbol_index + config.nof_symbols),
                "The DM-RS symbol mask size (i.e., {}) must be the same as the number of symbols allocated to the "
                "transmission within the slot (i.e., {}).",
                config.dmrs_symbol_mask.size(),
                config.start_symbol_index + config.nof_symbols);

  // Count number of OFDM symbols that contain DM-RS.
  unsigned nof_symbols_dmrs = config.dmrs_symbol_mask.count();

  // Count number of RE used for DM-RS.
  unsigned nof_re_dmrs_per_rb =
      nof_symbols_dmrs * config.nof_cdm_groups_without_data * get_nof_re_per_prb(config.dmrs_type);

  // Count number of RE used for CSI-RS.
  unsigned nof_re_csi_rs_per_rb = 0;

  // Count number of RE used for PT-RS.
  unsigned nof_pt_rs_per_rb = 0;

  // Count total number of resource elements available for DL-SCH.
  unsigned nof_re_dl_sch =
      config.nof_rb * (config.nof_symbols * NRE - nof_re_dmrs_per_rb - nof_re_csi_rs_per_rb - nof_pt_rs_per_rb);

  // Retrieve the modulation order.
  unsigned modulation_order = get_bits_per_symbol(config.mcs_descr.modulation);

  // Number of bits used for shared channel.
  result.nof_dl_sch_bits = units::bits(nof_re_dl_sch * config.nof_layers * modulation_order);

  // Count the number of rate matched bits that overlap with the DC position.
  if (config.contains_dc) {
    result.nof_dc_overlap_bits = units::bits(config.nof_symbols * modulation_order);
  } else {
    result.nof_dc_overlap_bits = 0_bits;
  }

  return result;
}
