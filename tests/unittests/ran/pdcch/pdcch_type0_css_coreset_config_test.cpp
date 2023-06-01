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

#include "srsran/ran/band_helper.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;

static void test_table_13_1()
{
  // Choose 2 bands with different minimum channel BW for SCS 15kHz.
  for (nr_band band : {nr_band::n7, nr_band::n77}) {
    for (unsigned config_index = 0; config_index != 16; ++config_index) {
      pdcch_type0_css_coreset_description description =
          pdcch_type0_css_coreset_get(band, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15, config_index, 0);

      // Assert pattern.
      if (config_index < 15) {
        TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
      } else {
        TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
      }

      // Assert CORESET bandwidth.
      if (config_index < 6) {
        TESTASSERT_EQ(24, description.nof_rb_coreset);
      } else if (config_index < 12) {
        TESTASSERT_EQ(48, description.nof_rb_coreset);
      } else if (config_index < 15) {
        TESTASSERT_EQ(96, description.nof_rb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
      }

      // Assert number of symbols.
      if (config_index < 3 || config_index == 8 || config_index == 9 || config_index == 13) {
        TESTASSERT_EQ(2, description.nof_symb_coreset);
      } else if (config_index < 6 || config_index == 10 || config_index == 11 || config_index == 14) {
        TESTASSERT_EQ(3, description.nof_symb_coreset);
      } else if (config_index < 8 || config_index == 12) {
        TESTASSERT_EQ(1, description.nof_symb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
      }

      // Assert offset.
      if (config_index < 6) {
        TESTASSERT_EQ(static_cast<int8_t>(2 * (config_index % 3)), description.offset);
      } else if (config_index < 12) {
        TESTASSERT_EQ(static_cast<int8_t>(4 * ((config_index - 6) % 2) + 12), description.offset);
      } else if (config_index < 15) {
        TESTASSERT_EQ(38, description.offset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
      }
    }
  }
}

static void test_table_13_1_A()
{
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
        nr_band::n46, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15, config_index, 0);

    // Assert pattern.
    if (config_index < 8) {
      TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
    } else {
      TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
    }

    // Assert CORESET bandwidth.
    if (config_index < 8) {
      TESTASSERT_EQ(96, description.nof_rb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
    }

    // Assert number of symbols.
    if (config_index < 4) {
      TESTASSERT_EQ(1, description.nof_symb_coreset);
    } else if (config_index < 8) {
      TESTASSERT_EQ(2, description.nof_symb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
    }

    // Assert offset.
    if (config_index < 8) {
      TESTASSERT_EQ(static_cast<int8_t>(10 + 2 * (config_index % 4)), description.offset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
    }
  }
}

static void test_table_13_2()
{
  // There is no NR band with 5MHz minimum channel BW for SCS 30kHz.
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    pdcch_type0_css_coreset_description description =
        pdcch_type0_css_coreset_get(nr_band::n7, subcarrier_spacing::kHz15, subcarrier_spacing::kHz30, config_index, 0);

    // Assert pattern.
    if (config_index < 14) {
      TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
    } else {
      TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
    }

    // Assert CORESET bandwidth.
    if (config_index < 8) {
      TESTASSERT_EQ(24, description.nof_rb_coreset);
    } else if (config_index < 14) {
      TESTASSERT_EQ(48, description.nof_rb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
    }

    // Assert number of symbols.
    if (config_index < 4 || config_index == 10 || config_index == 11) {
      TESTASSERT_EQ(2, description.nof_symb_coreset);
    } else if (config_index < 8 || config_index == 12 || config_index == 13) {
      TESTASSERT_EQ(3, description.nof_symb_coreset);
    } else if (config_index < 10) {
      TESTASSERT_EQ(1, description.nof_symb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
    }

    // Assert offset.
    if (config_index < 8) {
      TESTASSERT_EQ(static_cast<int8_t>((config_index % 4) + 5), description.offset);
    } else if (config_index < 14) {
      TESTASSERT_EQ(static_cast<int8_t>(((config_index - 14) % 2) * 2 + 18), description.offset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
    }
  }
}

static void test_table_13_3()
{
  // Choose 2 bands with different minimum channel BW for SCS 15kHz.
  for (nr_band band : {nr_band::n7, nr_band::n77}) {
    for (unsigned config_index = 0; config_index != 16; ++config_index) {
      pdcch_type0_css_coreset_description description =
          pdcch_type0_css_coreset_get(band, subcarrier_spacing::kHz30, subcarrier_spacing::kHz15, config_index, 0);

      // Assert pattern.
      if (config_index < 9) {
        TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
      } else {
        TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
      }

      // Assert CORESET bandwidth.
      if (config_index < 6) {
        TESTASSERT_EQ(48, description.nof_rb_coreset);
      } else if (config_index < 9) {
        TESTASSERT_EQ(96, description.nof_rb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
      }

      // Assert number of symbols.
      if (config_index < 2 || config_index == 6) {
        TESTASSERT_EQ(1, description.nof_symb_coreset);
      } else if (config_index < 4 || config_index == 7) {
        TESTASSERT_EQ(2, description.nof_symb_coreset);
      } else if (config_index < 9) {
        TESTASSERT_EQ(3, description.nof_symb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
      }

      // Assert offset.
      if (config_index < 6) {
        TESTASSERT_EQ(static_cast<int8_t>((config_index % 2) * 4 + 2), description.offset);
      } else if (config_index < 9) {
        TESTASSERT_EQ(28, description.offset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
      }
    }
  }
}

static void test_table_13_4()
{
  // There is no NR band with 5MHz minimum channel BW for SCS 30kHz.
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    pdcch_type0_css_coreset_description description =
        pdcch_type0_css_coreset_get(nr_band::n7, subcarrier_spacing::kHz30, subcarrier_spacing::kHz30, config_index, 0);

    // Assert pattern.
    TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);

    // Assert CORESET bandwidth.
    if (config_index < 10) {
      TESTASSERT_EQ(24, description.nof_rb_coreset);
    } else {
      TESTASSERT_EQ(48, description.nof_rb_coreset);
    }

    // Assert number of symbols.
    if (config_index < 5) {
      TESTASSERT_EQ(2, description.nof_symb_coreset);
    } else if (config_index < 10) {
      TESTASSERT_EQ(3, description.nof_symb_coreset);
    } else if (config_index < 13) {
      TESTASSERT_EQ(1, description.nof_symb_coreset);
    } else {
      TESTASSERT_EQ(2, description.nof_symb_coreset);
    }

    // Assert offset.
    if (config_index < 10) {
      TESTASSERT_EQ(static_cast<int8_t>(config_index % 5), description.offset);
    } else {
      TESTASSERT_EQ(static_cast<int8_t>(12 + ((config_index - 10) % 3) * 2), description.offset);
    }
  }
}

static void test_table_13_4_A()
{
  // Choose 2 bands with different minimum channel BW for SCS 15kHz.

  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
        nr_band::n46, subcarrier_spacing::kHz30, subcarrier_spacing::kHz30, config_index, 0);

    // Assert pattern.
    if (config_index < 8) {
      TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
    } else {
      TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
    }

    // Assert CORESET bandwidth.
    if (config_index < 8) {
      TESTASSERT_EQ(48, description.nof_rb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
    }

    // Assert number of symbols.
    if (config_index < 4) {
      TESTASSERT_EQ(1, description.nof_symb_coreset);
    } else if (config_index < 8) {
      TESTASSERT_EQ(2, description.nof_symb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
    }

    // Assert offset.
    if (config_index < 8) {
      TESTASSERT_EQ(static_cast<int8_t>(config_index % 4), description.offset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
    }
  }
}

static void test_table_13_5()
{
  // Choose a band with has minimum channel BW 40MHz or equivalent (As per TS 38.101, Table 5.2-1, only bands where Note
  // 17 applies, i.e., n79 or n104).
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
        srsran::nr_band::n79, subcarrier_spacing::kHz30, subcarrier_spacing::kHz15, config_index, 0);

    // Assert pattern.
    if (config_index < 9) {
      TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
    } else {
      TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
    }

    // Assert CORESET bandwidth.
    if (config_index < 3) {
      TESTASSERT_EQ(48, description.nof_rb_coreset);
    } else if (config_index < 9) {
      TESTASSERT_EQ(96, description.nof_rb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
    }

    // Assert number of symbols.
    if (config_index < 3) {
      TESTASSERT_EQ(config_index + 1, description.nof_symb_coreset);
    } else if (config_index < 9) {
      TESTASSERT_EQ(((config_index - 3) / 2) + 1, description.nof_symb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
    }

    // Assert offset.
    if (config_index < 3) {
      TESTASSERT_EQ(4, description.offset);
    } else if (config_index < 9) {
      TESTASSERT_EQ(static_cast<int8_t>(((config_index - 3) % 2) * 56), description.offset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
    }
  }
}

static void test_table_13_6()
{
  // Choose a band with has minimum channel BW 40MHz or equivalent (As per TS 38.101, Table 5.2-1, only bands where Note
  // 17 applies, i.e., n79 or n104).
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
        nr_band::n79, subcarrier_spacing::kHz30, subcarrier_spacing::kHz30, config_index, 0);

    // Assert pattern.
    if (config_index < 10) {
      TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
    } else {
      TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
    }

    // Assert CORESET bandwidth.
    if (config_index < 4) {
      TESTASSERT_EQ(24, description.nof_rb_coreset);
    } else if (config_index < 10) {
      TESTASSERT_EQ(48, description.nof_rb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
    }

    // Assert number of symbols.
    if (config_index < 10) {
      TESTASSERT_EQ((config_index / 2 + 1) % 3 + 1, description.nof_symb_coreset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
    }

    // Assert offset.
    if (config_index < 5) {
      TESTASSERT_EQ(static_cast<int8_t>((config_index % 2) * 4), description.offset);
    } else if (config_index < 10) {
      TESTASSERT_EQ(static_cast<int8_t>((config_index % 2) * 28), description.offset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
    }
  }
}

static void test_table_13_7()
{
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    for (uint8_t subcarrier_offset = 0; subcarrier_offset != 2; ++subcarrier_offset) {
      pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
          nr_band::n257, subcarrier_spacing::kHz120, subcarrier_spacing::kHz60, config_index, subcarrier_offset);

      // Assert pattern.
      if (config_index < 8) {
        TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
      } else if (config_index < 12) {
        TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern2);
      } else {
        TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
      }

      // Assert CORESET bandwidth.
      if (config_index < 6) {
        TESTASSERT_EQ(48, description.nof_rb_coreset);
      } else if (config_index < 8) {
        TESTASSERT_EQ(96, description.nof_rb_coreset);
      } else if (config_index < 10) {
        TESTASSERT_EQ(48, description.nof_rb_coreset);
      } else if (config_index < 12) {
        TESTASSERT_EQ(96, description.nof_rb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
      }

      // Assert number of symbols.
      if (config_index < 6) {
        TESTASSERT_EQ((config_index / 2) % 3 + 1, description.nof_symb_coreset);
      } else if (config_index < 8) {
        TESTASSERT_EQ(config_index - 5, description.nof_symb_coreset);
      } else if (config_index < 12) {
        TESTASSERT_EQ(1, description.nof_symb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
      }

      // Assert offset.
      if (config_index < 6) {
        TESTASSERT_EQ(static_cast<int8_t>((config_index % 2) * 8), description.offset);
      } else if (config_index < 8) {
        TESTASSERT_EQ(28, description.offset);
      } else if (config_index == 8 || config_index == 10) {
        TESTASSERT_EQ((subcarrier_offset == 0) ? -41 : -42, description.offset);
      } else if (config_index == 9) {
        TESTASSERT_EQ(49, description.offset);
      } else if (config_index == 11) {
        TESTASSERT_EQ(97, description.offset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
      }
    }
  }
}

static void test_table_13_8()
{
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    for (uint8_t subcarrier_offset = 0; subcarrier_offset != 2; ++subcarrier_offset) {
      pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
          nr_band::n257, subcarrier_spacing::kHz120, subcarrier_spacing::kHz120, config_index, subcarrier_offset);

      // Assert pattern.
      if (config_index < 4) {
        TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
      } else if (config_index < 8) {
        TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern3);
      } else {
        TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
      }

      // Assert CORESET bandwidth.
      if (config_index < 8) {
        TESTASSERT_EQ(((config_index / 2) % 2) * 24 + 24, description.nof_rb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
      }

      // Assert number of symbols.
      if (config_index == 2) {
        TESTASSERT_EQ(1, description.nof_symb_coreset);
      } else if (config_index < 8) {
        TESTASSERT_EQ(2, description.nof_symb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
      }

      // Assert offset.
      if (config_index < 2) {
        TESTASSERT_EQ(static_cast<int8_t>(config_index * 4), description.offset);
      } else if (config_index < 4) {
        TESTASSERT_EQ(14, description.offset);
      } else if (config_index == 4 || config_index == 6) {
        TESTASSERT_EQ((subcarrier_offset == 0) ? -20 : -21, description.offset);
      } else if (config_index == 5) {
        TESTASSERT_EQ(24, description.offset);
      } else if (config_index == 7) {
        TESTASSERT_EQ(48, description.offset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
      }
    }
  }
}

static void test_table_13_9()
{
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    for (uint8_t subcarrier_offset = 0; subcarrier_offset != 2; ++subcarrier_offset) {
      pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
          nr_band::n257, subcarrier_spacing::kHz240, subcarrier_spacing::kHz60, config_index, subcarrier_offset);

      // Assert pattern.
      if (config_index < 4) {
        TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
      } else {
        TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
      }

      // Assert CORESET bandwidth.
      if (config_index < 4) {
        TESTASSERT_EQ(96, description.nof_rb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
      }

      // Assert number of symbols.
      if (config_index < 4) {
        TESTASSERT_EQ((config_index / 2) % 2 + 1, description.nof_symb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
      }

      // Assert offset.
      if (config_index < 4) {
        TESTASSERT_EQ(static_cast<int8_t>((config_index % 2) * 16), description.offset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
      }
    }
  }
}

static void test_table_13_10()
{
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    for (uint8_t subcarrier_offset = 0; subcarrier_offset != 2; ++subcarrier_offset) {
      pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
          nr_band::n257, subcarrier_spacing::kHz240, subcarrier_spacing::kHz120, config_index, subcarrier_offset);

      // Assert pattern.
      if (config_index < 4) {
        TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1);
      } else if (config_index < 8) {
        TESTASSERT(description.pattern == ssb_coreset0_mplex_pattern::mplx_pattern2);
      } else {
        TESTASSERT(description.pattern == PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern);
      }

      // Assert CORESET bandwidth.
      if (config_index == 4 || config_index == 5) {
        TESTASSERT_EQ(24, description.nof_rb_coreset);
      } else if (config_index < 8) {
        TESTASSERT_EQ(48, description.nof_rb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_rb_coreset, description.nof_rb_coreset);
      }

      // Assert number of symbols.
      if (config_index == 2 || config_index == 3) {
        TESTASSERT_EQ((config_index / 2) % 2 + 1, description.nof_symb_coreset);
      } else if (config_index < 8) {
        TESTASSERT_EQ(1, description.nof_symb_coreset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.nof_symb_coreset, description.nof_symb_coreset);
      }

      // Assert offset.
      if (config_index < 4) {
        TESTASSERT_EQ(static_cast<int8_t>((config_index % 2) * 8), description.offset);
      } else if (config_index == 4 || config_index == 6) {
        TESTASSERT_EQ((subcarrier_offset == 0) ? -41 : -42, description.offset);
      } else if (config_index == 5) {
        TESTASSERT_EQ(25, description.offset);
      } else if (config_index == 7) {
        TESTASSERT_EQ(49, description.offset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
      }
    }
  }
}

int main()
{
  test_table_13_1();
  test_table_13_1_A();
  test_table_13_2();
  test_table_13_3();
  test_table_13_4();
  test_table_13_4_A();
  test_table_13_5();
  test_table_13_6();
  test_table_13_7();
  test_table_13_8();
  test_table_13_9();
  test_table_13_10();
}