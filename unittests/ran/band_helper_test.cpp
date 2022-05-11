/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

/// Tests the basic function of band helper.
void test_band_helper_basic()
{
  // Test get_band_from_dl_arfcn().
  TESTASSERT(band_helper::get_band_from_dl_arfcn(530000U) < UINT16_MAX);
  TESTASSERT_EQ(UINT16_MAX, band_helper::get_band_from_dl_arfcn(5300000U));
  TESTASSERT_EQ(1, band_helper::get_band_from_dl_arfcn(423000U));
  TESTASSERT_EQ(3, band_helper::get_band_from_dl_arfcn(365000U));
  TESTASSERT_EQ(5, band_helper::get_band_from_dl_arfcn(175000U));
  TESTASSERT_EQ(7, band_helper::get_band_from_dl_arfcn(530000U));
  TESTASSERT_EQ(25, band_helper::get_band_from_dl_arfcn(399000U));
  TESTASSERT_EQ(34, band_helper::get_band_from_dl_arfcn(404000U));
  TESTASSERT_EQ(38, band_helper::get_band_from_dl_arfcn(515000U));
  TESTASSERT_EQ(41, band_helper::get_band_from_dl_arfcn(499900U));
  TESTASSERT_EQ(41, band_helper::get_band_from_dl_arfcn(504000U));
  TESTASSERT_EQ(50, band_helper::get_band_from_dl_arfcn(286500U));
  TESTASSERT_EQ(51, band_helper::get_band_from_dl_arfcn(285500U));
  TESTASSERT_EQ(70, band_helper::get_band_from_dl_arfcn(400000U));
  TESTASSERT_EQ(77, band_helper::get_band_from_dl_arfcn(630500U));

  // Test get_ssb_pattern().
  TESTASSERT(band_helper::get_ssb_pattern(1, subcarrier_spacing::kHz15) < ssb_pattern_case::invalid);
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(1, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(3, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::invalid, band_helper::get_ssb_pattern(3, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::B, band_helper::get_ssb_pattern(5, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::B, band_helper::get_ssb_pattern(5, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(25, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(34, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(34, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(38, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(38, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(39, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(39, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(40, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(41, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(41, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(50, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(51, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(66, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::B, band_helper::get_ssb_pattern(66, subcarrier_spacing::kHz30));
  TESTASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(70, subcarrier_spacing::kHz15));
  TESTASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(78, subcarrier_spacing::kHz30));

  // Test is_paired_spectrum().
  TESTASSERT_EQ(true, band_helper::is_paired_spectrum(1));
  TESTASSERT_EQ(true, band_helper::is_paired_spectrum(3));
  TESTASSERT_EQ(true, band_helper::is_paired_spectrum(7));
  TESTASSERT_EQ(true, band_helper::is_paired_spectrum(25));
  TESTASSERT_EQ(true, band_helper::is_paired_spectrum(28));
  TESTASSERT_EQ(false, band_helper::is_paired_spectrum(34));
  TESTASSERT_EQ(false, band_helper::is_paired_spectrum(40));
  TESTASSERT_EQ(false, band_helper::is_paired_spectrum(51));
  TESTASSERT_EQ(true, band_helper::is_paired_spectrum(66));
  TESTASSERT_EQ(true, band_helper::is_paired_spectrum(74));
  TESTASSERT_EQ(false, band_helper::is_paired_spectrum(75));
  TESTASSERT_EQ(false, band_helper::is_paired_spectrum(76));
  TESTASSERT_EQ(false, band_helper::is_paired_spectrum(77));
  TESTASSERT_EQ(false, band_helper::is_paired_spectrum(79));
  TESTASSERT_EQ(false, band_helper::is_paired_spectrum(81));
  TESTASSERT_EQ(false, band_helper::is_paired_spectrum(84));
}

int main()
{
  test_band_helper_basic();
}
