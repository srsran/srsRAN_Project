/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/prach/prach_frequency_mapping.h"

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
  }

  return result;
}