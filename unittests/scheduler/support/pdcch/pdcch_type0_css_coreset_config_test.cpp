/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/support/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

static void test_table_13_1()
{
  for (unsigned minimum_bandwidth : {5, 10}) {
    for (unsigned config_index = 0; config_index != 16; ++config_index) {
      pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
          minimum_bandwidth, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15, config_index);

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
        TESTASSERT_EQ(2 * (config_index % 3), description.offset);
      } else if (config_index < 12) {
        TESTASSERT_EQ(4 * ((config_index - 6) % 2) + 12, description.offset);
      } else if (config_index < 15) {
        TESTASSERT_EQ(38, description.offset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
      }
    }
  }
}

static void test_table_13_2()
{
  for (unsigned minimum_bandwidth : {5, 10}) {
    for (unsigned config_index = 0; config_index != 16; ++config_index) {
      pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
          minimum_bandwidth, subcarrier_spacing::kHz15, subcarrier_spacing::kHz30, config_index);

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
        TESTASSERT_EQ((config_index % 4) + 5, description.offset);
      } else if (config_index < 14) {
        TESTASSERT_EQ(((config_index - 14) % 2) * 2 + 18, description.offset);
      } else {
        TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
      }
    }
  }
}

static void test_table_13_3()
{
  for (unsigned minimum_bandwidth : {5, 10}) {
    for (unsigned config_index = 0; config_index != 16; ++config_index) {
      pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
          minimum_bandwidth, subcarrier_spacing::kHz30, subcarrier_spacing::kHz15, config_index);

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
        TESTASSERT_EQ((config_index % 2) * 4 + 2, description.offset);
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
  for (unsigned minimum_bandwidth : {5, 10}) {
    for (unsigned config_index = 0; config_index != 16; ++config_index) {
      pdcch_type0_css_coreset_description description = pdcch_type0_css_coreset_get(
          minimum_bandwidth, subcarrier_spacing::kHz30, subcarrier_spacing::kHz30, config_index);

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
        TESTASSERT_EQ(config_index % 5, description.offset);
      } else {
        TESTASSERT_EQ(12 + ((config_index - 10) % 3) * 2, description.offset);
      }
    }
  }
}

static void test_table_13_5()
{
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    pdcch_type0_css_coreset_description description =
        pdcch_type0_css_coreset_get(40, subcarrier_spacing::kHz30, subcarrier_spacing::kHz15, config_index);

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
      TESTASSERT_EQ(((config_index - 3) % 2) * 56, description.offset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
    }
  }
}

static void test_table_13_6()
{
  for (unsigned config_index = 0; config_index != 16; ++config_index) {
    pdcch_type0_css_coreset_description description =
        pdcch_type0_css_coreset_get(40, subcarrier_spacing::kHz30, subcarrier_spacing::kHz30, config_index);

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
      TESTASSERT_EQ((config_index % 2) * 4, description.offset);
    } else if (config_index < 10) {
      TESTASSERT_EQ((config_index % 2) * 28, description.offset);
    } else {
      TESTASSERT_EQ(PDCCH_TYPE0_CSS_CORESET_RESERVED.offset, description.offset);
    }
  }
}

int main()
{
  test_table_13_1();
  test_table_13_2();
  test_table_13_3();
  test_table_13_4();
  test_table_13_5();
  test_table_13_6();
}