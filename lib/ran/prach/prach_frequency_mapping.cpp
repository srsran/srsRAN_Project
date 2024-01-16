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

#include "srsran/ran/prach/prach_frequency_mapping.h"

using namespace srsran;

prach_frequency_mapping_information srsran::prach_frequency_mapping_get(prach_subcarrier_spacing prach_scs,
                                                                        subcarrier_spacing       pusch_scs)
{
  prach_frequency_mapping_information result = PRACH_FREQUENCY_MAPPING_INFORMATION_RESERVED;
  switch (prach_scs) {
    case prach_subcarrier_spacing::kHz1_25:
      if (pusch_scs == subcarrier_spacing::kHz15) {
        result.nof_rb_ra = 6;
        result.k_bar     = 7;
      } else if (pusch_scs == subcarrier_spacing::kHz30) {
        result.nof_rb_ra = 3;
        result.k_bar     = 1;
      } else if (pusch_scs == subcarrier_spacing::kHz60) {
        result.nof_rb_ra = 2;
        result.k_bar     = 133;
      }
      break;
    case prach_subcarrier_spacing::kHz5:
      if (pusch_scs == subcarrier_spacing::kHz15) {
        result.nof_rb_ra = 24;
        result.k_bar     = 12;
      } else if (pusch_scs == subcarrier_spacing::kHz30) {
        result.nof_rb_ra = 12;
        result.k_bar     = 10;
      } else if (pusch_scs == subcarrier_spacing::kHz60) {
        result.nof_rb_ra = 6;
        result.k_bar     = 7;
      }
      break;
    case prach_subcarrier_spacing::kHz15:
      if (pusch_scs == subcarrier_spacing::kHz15) {
        result.nof_rb_ra = 12;
        result.k_bar     = 2;
      } else if (pusch_scs == subcarrier_spacing::kHz30) {
        result.nof_rb_ra = 6;
        result.k_bar     = 2;
      } else if (pusch_scs == subcarrier_spacing::kHz60) {
        result.nof_rb_ra = 3;
        result.k_bar     = 2;
      }
      break;
    case prach_subcarrier_spacing::kHz30:
      if (pusch_scs == subcarrier_spacing::kHz15) {
        result.nof_rb_ra = 24;
        result.k_bar     = 2;
      } else if (pusch_scs == subcarrier_spacing::kHz30) {
        result.nof_rb_ra = 12;
        result.k_bar     = 2;
      } else if (pusch_scs == subcarrier_spacing::kHz60) {
        result.nof_rb_ra = 6;
        result.k_bar     = 2;
      }
      break;
    case prach_subcarrier_spacing::kHz60:
      if (pusch_scs == subcarrier_spacing::kHz60) {
        result.nof_rb_ra = 12;
        result.k_bar     = 2;
      } else if (pusch_scs == subcarrier_spacing::kHz120) {
        result.nof_rb_ra = 6;
        result.k_bar     = 2;
      }
      break;
    case prach_subcarrier_spacing::kHz120:
      if (pusch_scs == subcarrier_spacing::kHz60) {
        result.nof_rb_ra = 24;
        result.k_bar     = 2;
      } else if (pusch_scs == subcarrier_spacing::kHz120) {
        result.nof_rb_ra = 12;
        result.k_bar     = 2;
      }
      break;
    case prach_subcarrier_spacing::invalid:
      // Ignore.
      break;
  }

  return result;
}