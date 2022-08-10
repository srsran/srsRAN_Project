/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/bs_channel_bandwidth.h"
#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

TEST(test_get_band_from_arfcn, mix_bands)
{
  ASSERT_NE(nr_band_fr1::invalid, band_helper::get_band_from_dl_arfcn(530000U));
  ASSERT_EQ(nr_band_fr1::invalid, band_helper::get_band_from_dl_arfcn(5300000U));
  ASSERT_EQ(nr_band_fr1::n1, band_helper::get_band_from_dl_arfcn(423000U));
  ASSERT_EQ(nr_band_fr1::n3, band_helper::get_band_from_dl_arfcn(365000U));
  ASSERT_EQ(nr_band_fr1::n5, band_helper::get_band_from_dl_arfcn(175000U));
  ASSERT_EQ(nr_band_fr1::n7, band_helper::get_band_from_dl_arfcn(530000U));
  ASSERT_EQ(nr_band_fr1::n25, band_helper::get_band_from_dl_arfcn(399000U));
  ASSERT_EQ(nr_band_fr1::n34, band_helper::get_band_from_dl_arfcn(404000U));
  ASSERT_EQ(nr_band_fr1::n38, band_helper::get_band_from_dl_arfcn(515000U));
  ASSERT_EQ(nr_band_fr1::n41, band_helper::get_band_from_dl_arfcn(499900U));
  ASSERT_EQ(nr_band_fr1::n41, band_helper::get_band_from_dl_arfcn(504000U));
  ASSERT_EQ(nr_band_fr1::n50, band_helper::get_band_from_dl_arfcn(286500U));
  ASSERT_EQ(nr_band_fr1::n51, band_helper::get_band_from_dl_arfcn(285500U));
  ASSERT_EQ(nr_band_fr1::n70, band_helper::get_band_from_dl_arfcn(400000U));
  ASSERT_EQ(nr_band_fr1::n77, band_helper::get_band_from_dl_arfcn(630500U));
}

TEST(test_arfcn_freq_conversion, arfcn_to_freq)
{
  ASSERT_DOUBLE_EQ(3489.42e6, band_helper::nr_arfcn_to_freq(632628));
  // default refPointA.
  ASSERT_DOUBLE_EQ(3508.92e6, band_helper::nr_arfcn_to_freq(633928));
  // default ARFCN with freq divisible by 11.52 MHz.
  ASSERT_DOUBLE_EQ(3513.6e6, band_helper::nr_arfcn_to_freq(634240));
  // n28 n67
  ASSERT_DOUBLE_EQ(703.0e6, band_helper::nr_arfcn_to_freq(140600));
  ASSERT_DOUBLE_EQ(729.0e6, band_helper::nr_arfcn_to_freq(145800));
  ASSERT_DOUBLE_EQ(768.0e6, band_helper::nr_arfcn_to_freq(153600));
  ASSERT_DOUBLE_EQ(788.0e6, band_helper::nr_arfcn_to_freq(157600));
  // n20
  ASSERT_DOUBLE_EQ(791.0e6, band_helper::nr_arfcn_to_freq(158200));
  ASSERT_DOUBLE_EQ(801.0e6, band_helper::nr_arfcn_to_freq(160200));
  ASSERT_DOUBLE_EQ(811.0e6, band_helper::nr_arfcn_to_freq(162200));
  ASSERT_DOUBLE_EQ(842.0e6, band_helper::nr_arfcn_to_freq(168400));
  // n32 n75
  ASSERT_DOUBLE_EQ(1452.0e6, band_helper::nr_arfcn_to_freq(290400));
  ASSERT_DOUBLE_EQ(1472.0e6, band_helper::nr_arfcn_to_freq(294400));

  // n1
  ASSERT_DOUBLE_EQ(1920.0e6, band_helper::nr_arfcn_to_freq(384000));
  ASSERT_DOUBLE_EQ(1940.15e6, band_helper::nr_arfcn_to_freq(388030));
  ASSERT_DOUBLE_EQ(1959.15e6, band_helper::nr_arfcn_to_freq(391830));
  ASSERT_DOUBLE_EQ(2170.0e6, band_helper::nr_arfcn_to_freq(434000));
  // n3
  ASSERT_DOUBLE_EQ(1710.0e6, band_helper::nr_arfcn_to_freq(342000));
  ASSERT_DOUBLE_EQ(1740.0e6, band_helper::nr_arfcn_to_freq(348000));
  ASSERT_DOUBLE_EQ(1805.0e6, band_helper::nr_arfcn_to_freq(361000));
  ASSERT_DOUBLE_EQ(1842.5e6, band_helper::nr_arfcn_to_freq(368500));
  ASSERT_DOUBLE_EQ(1880.0e6, band_helper::nr_arfcn_to_freq(376000));
  // n5
  ASSERT_DOUBLE_EQ(881.5e6, band_helper::nr_arfcn_to_freq(176300));
  ASSERT_DOUBLE_EQ(836.5e6, band_helper::nr_arfcn_to_freq(167300));
  // n7 n38
  ASSERT_DOUBLE_EQ(2500.0e6, band_helper::nr_arfcn_to_freq(500000));
  ASSERT_DOUBLE_EQ(2540.0e6, band_helper::nr_arfcn_to_freq(508000));
  ASSERT_DOUBLE_EQ(2610.0e6, band_helper::nr_arfcn_to_freq(522000));
  ASSERT_DOUBLE_EQ(2690.0e6, band_helper::nr_arfcn_to_freq(538000));

  // n78
  ASSERT_DOUBLE_EQ(3513.6e6, band_helper::nr_arfcn_to_freq(634240));
}

TEST(test_arfcn_freq_conversion, freq_to_arfcn)
{
  ASSERT_EQ(632628, band_helper::freq_to_nr_arfcn(3489.42e6));
  ASSERT_EQ(633928, band_helper::freq_to_nr_arfcn(3508.92e6));
  ASSERT_EQ(634240, band_helper::freq_to_nr_arfcn(3513.6e6));
  // n28 n67
  ASSERT_EQ(140600, band_helper::freq_to_nr_arfcn(703.0e6));
  ASSERT_EQ(145800, band_helper::freq_to_nr_arfcn(729.0e6));
  ASSERT_EQ(153600, band_helper::freq_to_nr_arfcn(768.0e6));
  ASSERT_EQ(157600, band_helper::freq_to_nr_arfcn(788.0e6));
  // n5
  ASSERT_EQ(176300, band_helper::freq_to_nr_arfcn(881.5e6));
}

TEST(get_ul_arfcn_from_dl_arfcn, mixed_frequencies)
{
  // n5
  ASSERT_EQ(167300, band_helper::get_ul_arfcn_from_dl_arfcn(176300));
}

TEST(test_get_abs_freq_point_a_arfcn, mixed_frequencies)
{
  // n3
  ASSERT_EQ(367564, band_helper::get_abs_freq_point_a_arfcn(52, 368500));
  // n5
  ASSERT_EQ(175364, band_helper::get_abs_freq_point_a_arfcn(52, 176300));
  // n7 n38
  ASSERT_EQ(528984, band_helper::get_abs_freq_point_a_arfcn(52, 529920));
  // n78
  ASSERT_EQ(633928, band_helper::get_abs_freq_point_a_arfcn(52, 634240));
}

TEST(test_arfcn_freq_conversion, arfcn_to_freq_corner_cases)
{
  const uint32_t max_valid_nr_arfcn = 3279165;

  // Max ARFCN is 3279165 at almost 10 GHz
  ASSERT_DOUBLE_EQ(99.99996e9, band_helper::nr_arfcn_to_freq(max_valid_nr_arfcn));

  // Invalid ARFCN
  ASSERT_DOUBLE_EQ(0.0, band_helper::nr_arfcn_to_freq(max_valid_nr_arfcn + 1));
}

TEST(test_center_freq_conversion, freq_center)
{
  ASSERT_DOUBLE_EQ(881.5e6, band_helper::get_center_freq_from_abs_freq_point_a(52, 175364));
  ASSERT_DOUBLE_EQ(836.5e6, band_helper::get_center_freq_from_abs_freq_point_a(52, 166364));
}

TEST(test_is_paired_spectrum, paired)
{
  ASSERT_TRUE(band_helper::is_paired_spectrum(nr_band_fr1::n1));
  ASSERT_TRUE(band_helper::is_paired_spectrum(nr_band_fr1::n3));
  ASSERT_TRUE(band_helper::is_paired_spectrum(nr_band_fr1::n7));
  ASSERT_TRUE(band_helper::is_paired_spectrum(nr_band_fr1::n25));
  ASSERT_TRUE(band_helper::is_paired_spectrum(nr_band_fr1::n28));
  ASSERT_TRUE(band_helper::is_paired_spectrum(nr_band_fr1::n66));
  ASSERT_TRUE(band_helper::is_paired_spectrum(nr_band_fr1::n74));
}

TEST(test_is_paired_spectrum, unpaired)
{
  ASSERT_FALSE(band_helper::is_paired_spectrum(nr_band_fr1::n34));
  ASSERT_FALSE(band_helper::is_paired_spectrum(nr_band_fr1::n40));
  ASSERT_FALSE(band_helper::is_paired_spectrum(nr_band_fr1::n51));
  ASSERT_FALSE(band_helper::is_paired_spectrum(nr_band_fr1::n75));
  ASSERT_FALSE(band_helper::is_paired_spectrum(nr_band_fr1::n76));
  ASSERT_FALSE(band_helper::is_paired_spectrum(nr_band_fr1::n77));
  ASSERT_FALSE(band_helper::is_paired_spectrum(nr_band_fr1::n79));
  ASSERT_FALSE(band_helper::is_paired_spectrum(nr_band_fr1::n81));
  ASSERT_FALSE(band_helper::is_paired_spectrum(nr_band_fr1::n84));
}

TEST(test_ssb_pattern, ssb_pattern_case_A)
{
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n1, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n3, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n25, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n34, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n38, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n39, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n41, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n51, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n66, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::A, band_helper::get_ssb_pattern(nr_band_fr1::n70, subcarrier_spacing::kHz15));
}

TEST(test_ssb_pattern, ssb_pattern_case_B)
{
  ASSERT_EQ(ssb_pattern_case::B, band_helper::get_ssb_pattern(nr_band_fr1::n5, subcarrier_spacing::kHz30));
  ASSERT_EQ(ssb_pattern_case::B, band_helper::get_ssb_pattern(nr_band_fr1::n5, subcarrier_spacing::kHz30));
  ASSERT_EQ(ssb_pattern_case::B, band_helper::get_ssb_pattern(nr_band_fr1::n66, subcarrier_spacing::kHz30));
}

TEST(test_ssb_pattern, ssb_pattern_case_C)
{
  ASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(nr_band_fr1::n34, subcarrier_spacing::kHz30));
  ASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(nr_band_fr1::n38, subcarrier_spacing::kHz30));
  ASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(nr_band_fr1::n39, subcarrier_spacing::kHz30));
  ASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(nr_band_fr1::n40, subcarrier_spacing::kHz30));
  ASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(nr_band_fr1::n41, subcarrier_spacing::kHz30));
  ASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(nr_band_fr1::n50, subcarrier_spacing::kHz30));
  ASSERT_EQ(ssb_pattern_case::C, band_helper::get_ssb_pattern(nr_band_fr1::n78, subcarrier_spacing::kHz30));
}

TEST(test_ssb_pattern, ssb_pattern_case_invalid)
{
  ASSERT_GT(ssb_pattern_case::invalid, band_helper::get_ssb_pattern(nr_band_fr1::n1, subcarrier_spacing::kHz15));
  ASSERT_EQ(ssb_pattern_case::invalid, band_helper::get_ssb_pattern(nr_band_fr1::n3, subcarrier_spacing::kHz30));
}

TEST(test_get_point_a_from_f_req, scs_kHz15)
{
  // Band n1, BWs are {5MHz, 10MHz, 15MHz, 20MHz}.
  ASSERT_DOUBLE_EQ(2122150000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2124400000.0, 25, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2105420000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2110100000.0, 52, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2108090000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2115200000.0, 79, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2150860000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2160400000.0, 106, subcarrier_spacing::kHz15));

  // Band n78, BWs are {10MHz, 15MHz, 20MHz, 30MHz, 40MHz, 50MHz}.
  ASSERT_DOUBLE_EQ(3615675000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3620355000.0, 52, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3613245000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3620355000.0, 79, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3610815000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3620355000.0, 106, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3605955000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3620355000.0, 160, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3600915000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3620355000.0, 216, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3596055000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3620355000.0, 270, subcarrier_spacing::kHz15));
}

TEST(test_get_point_a_from_f_req, scs_kHz30)
{
  // Band n3, BWs are {10MHz, 15MHz, 20MHz, 25MHz, 30MHz}.
  ASSERT_DOUBLE_EQ(1821380000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(1825700000.0, 24, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1844460000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(1851300000.0, 38, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1860220000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(1869400000.0, 51, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1837800000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(1849500000.0, 65, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1837800000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(1849500000.0, 65, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1842060000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(1856100000.0, 78, subcarrier_spacing::kHz30));

  // Band n78, BWs that are tested are {15MHz, 20MHz, 30MHz, 50MHz, 80MHz, 100MHz}.
  ASSERT_DOUBLE_EQ(3697320000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 38, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3694980000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 51, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3690120000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 78, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3680220000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 133, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3665100000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 217, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3655020000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 273, subcarrier_spacing::kHz30));
}

TEST(test_get_point_a_from_f_req, scs_kHz60)
{
  // Band n41, BWs are {40MHz, 50MHz, 60MHz, 70MHz, 80MHz, 90MHz, 100MHz}.
  ASSERT_DOUBLE_EQ(2671620000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2689980000.0, 51, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2666580000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2689980000.0, 65, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2661540000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2689980000.0, 79, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2656500000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2689980000.0, 93, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2651460000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2689980000.0, 107, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2646420000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2689980000.0, 121, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2641380000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(2689980000.0, 135, subcarrier_spacing::kHz60));

  // Band n78, BWs that are tested are {10MHz, 40MHz, 60MHz, 70MHz, 90MHz}.
  ASSERT_DOUBLE_EQ(3700200000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 11, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3685800000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 51, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3675720000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 79, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3670680000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 93, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3660600000.0,
                   band_helper::get_abs_freq_point_a_from_f_ref(3704160000.0, 121, subcarrier_spacing::kHz60));
}

TEST(test_get_f_req_from_f_req_point_a, scs_kHz15)
{
  // Band n1, BWs are {5MHz, 10MHz, 15MHz, 20MHz}.
  ASSERT_DOUBLE_EQ(2124400000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2122150000.0, 25, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2110100000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2105420000.0, 52, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2115200000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2108090000.0, 79, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(2160400000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2150860000.0, 106, subcarrier_spacing::kHz15));

  // Band n78, BWs are {10MHz, 15MHz, 20MHz, 30MHz, 40MHz, 50MHz}.
  ASSERT_DOUBLE_EQ(3620355000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3615675000.0, 52, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3613245000.0, 79, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3610815000.0, 106, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3605955000.0, 160, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3600915000.0, 216, subcarrier_spacing::kHz15));
  ASSERT_DOUBLE_EQ(3620355000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3596055000.0, 270, subcarrier_spacing::kHz15));
}

TEST(test_get_f_req_from_f_req_point_a, scs_kHz30)
{
  // Band n3, BWs are {10MHz, 15MHz, 20MHz, 25MHz, 30MHz}.
  ASSERT_DOUBLE_EQ(1825700000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(1821380000.0, 24, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1851300000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(1844460000.0, 38, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1869400000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(1860220000.0, 51, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1849500000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(1837800000.0, 65, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1849500000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(1837800000.0, 65, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(1856100000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(1842060000.0, 78, subcarrier_spacing::kHz30));

  // Band n78, BWs that are tested are {15MHz, 20MHz, 30MHz, 50MHz, 80MHz, 100MHz}.
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3697320000.0, 38, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3694980000.0, 51, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3690120000.0, 78, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3680220000.0, 133, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3665100000.0, 217, subcarrier_spacing::kHz30));
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3655020000.0, 273, subcarrier_spacing::kHz30));
}

TEST(test_get_f_req_from_f_req_point_a, scs_kHz60)
{
  // Band n41, BWs are {40MHz, 50MHz, 60MHz, 70MHz, 80MHz, 90MHz, 100MHz}.
  ASSERT_DOUBLE_EQ(2689980000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2671620000.0, 51, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2666580000.0, 65, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2661540000.0, 79, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2656500000.0, 93, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2651460000.0, 107, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2646420000.0, 121, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(2689980000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(2641380000.0, 135, subcarrier_spacing::kHz60));

  // Band n78, BWs that are tested are {10MHz, 40MHz, 60MHz, 70MHz, 90MHz}.
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3700200000.0, 11, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3685800000.0, 51, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3675720000.0, 79, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3670680000.0, 93, subcarrier_spacing::kHz60));
  ASSERT_DOUBLE_EQ(3704160000.0,
                   band_helper::get_f_ref_from_abs_freq_point_a(3660600000.0, 121, subcarrier_spacing::kHz60));
}

TEST(test_get_n_rbs_from_bw, scs_15kHz)
{
  ASSERT_EQ(
      25,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz5, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(
      52,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz10, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(
      79,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz15, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(
      106,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz20, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(
      133,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz25, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(
      160,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz30, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(
      216,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz40, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(
      270,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz50, subcarrier_spacing::kHz15, frequency_range::FR1));
  ASSERT_EQ(
      0,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz60, subcarrier_spacing::kHz15, frequency_range::FR1));
}

TEST(test_get_n_rbs_from_bw, scs_30kHz)
{
  ASSERT_EQ(
      11,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz5, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      24,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz10, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      38,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz15, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      51,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz20, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      65,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz25, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      78,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz30, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      133,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz50, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      162,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz60, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      189,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz70, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      217,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz80, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(
      245,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz90, subcarrier_spacing::kHz30, frequency_range::FR1));
  ASSERT_EQ(273,
            band_helper::get_n_rbs_from_bw(
                bs_channel_bandwidth_fr1::MHz100, subcarrier_spacing::kHz30, frequency_range::FR1));
}

TEST(test_get_n_rbs_from_bw, scs_60kHz)
{
  ASSERT_EQ(
      0,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz5, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      11,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz10, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      18,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz15, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      24,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz20, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      31,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz25, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      38,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz30, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      51,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz40, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      65,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz50, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      79,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz60, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      93,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz70, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      107,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz80, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(
      121,
      band_helper::get_n_rbs_from_bw(bs_channel_bandwidth_fr1::MHz90, subcarrier_spacing::kHz60, frequency_range::FR1));
  ASSERT_EQ(135,
            band_helper::get_n_rbs_from_bw(
                bs_channel_bandwidth_fr1::MHz100, subcarrier_spacing::kHz60, frequency_range::FR1));
}

TEST(test_get_n_rbs_from_bw, invalid_cases)
{
  ASSERT_EQ(0,
            band_helper::get_n_rbs_from_bw(
                bs_channel_bandwidth_fr1::MHz100, subcarrier_spacing::kHz60, frequency_range::FR2));
  ASSERT_EQ(0,
            band_helper::get_n_rbs_from_bw(
                bs_channel_bandwidth_fr1::MHz100, subcarrier_spacing::kHz120, frequency_range::FR1));
}