/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/ran/ssb_freq_position_generator.h"
#include "ssb_coreset0_freq_pos_checker.h"
#include "srsgnb/ran/band_helper.h"
#include <gtest/gtest.h>

using namespace srsgnb;

// Define struct to pass parameters from test functions.
struct cfg_gen_input_params {
  unsigned           dl_arfcn;
  nr_band            band;
  unsigned           n_rbs;
  subcarrier_spacing scs_common;
  subcarrier_spacing scs_ssb;
};

// Helper that compares the SSB and Coreset0 paramters returned by du_config_generator.
static bool compare_ssb_freq_location(const ssb_freq_location& lhs, const ssb_freq_location& rhs)
{
  bool result = lhs.is_valid && rhs.is_valid && lhs.k_ssb.to_uint() == rhs.k_ssb.to_uint() &&
                lhs.offset_to_point_A.to_uint() == rhs.offset_to_point_A.to_uint();
  return result;
}

// Helper that compares the SSB parameters returned by du_config_generator.
static bool compare_ssb_coreset0_allocation(const band_helper::ssb_coreset0_freq_location& lhs,
                                            const band_helper::ssb_coreset0_freq_location& rhs)
{
  bool result = lhs.is_valid && rhs.is_valid && lhs.k_ssb.to_uint() == rhs.k_ssb.to_uint() &&
                lhs.offset_to_point_A.to_uint() == rhs.offset_to_point_A.to_uint() &&
                lhs.coreset0_idx == rhs.coreset0_idx && lhs.searchspace0_idx == rhs.searchspace0_idx;
  return result;
}

/*
 *      ===========    TEST GENERATION OF SSB POSITION WITHIN THE BAND    ===========
 */

// Test all possible SSB position within a given band returned by the config generator.
TEST(ssb_freq_position_generation_test, band_3)
{
  ssb_freq_position_generator cfg_generator{/* DL-ARFCN*/ 365000,
                                            /* NR-band*/ nr_band::n3,
                                            /* N_RBs */ 52,
                                            /* SCS_common */ subcarrier_spacing::kHz15,
                                            /* SCS_SSB */ subcarrier_spacing::kHz15};

  ssb_freq_location expected{true, 5, 2};
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  // Get next SSB position and verify its position in terms of offsetToPointA and k_SSB.
  expected.offset_to_point_A = 11;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 18;
  expected.k_ssb             = 6;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 25;
  expected.k_ssb             = 2;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 31;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  // Verify there are no possible SSB positions within the band.
  ASSERT_TRUE(not cfg_generator.get_next_ssb_location().is_valid);
}

// Test all possible SSB position within a given band returned by the config generator.
TEST(ssb_freq_position_generation_test, band_7)
{
  ssb_freq_position_generator cfg_generator{/* DL-ARFCN*/ 531720,
                                            /* NR-band*/ nr_band::n7,
                                            /* N_RBs */ 25,
                                            /* SCS_common */ subcarrier_spacing::kHz15,
                                            /* SCS_SSB */ subcarrier_spacing::kHz15};

  ssb_freq_location expected{true, 0, 0};
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  // Get next SSB position and verify its position in terms of offsetToPointA and k_SSB.
  ASSERT_FALSE(cfg_generator.get_next_ssb_location().is_valid);
}

// Test all possible SSB position within a given band returned by the config generator.
TEST(ssb_freq_position_generation_test, band_25)
{
  ssb_freq_position_generator cfg_generator{/* DL-ARFCN*/ 391180,
                                            /* NR-band*/ nr_band::n25,
                                            /* N_RBs */ 38,
                                            /* SCS_common */ subcarrier_spacing::kHz30,
                                            /* SCS_SSB */ subcarrier_spacing::kHz15};

  ssb_freq_location expected{true, 2, 2};
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  // Get next SSB position and verify its position in terms of offsetToPointA and k_SSB.
  expected.offset_to_point_A = 8;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 14;
  expected.k_ssb             = 18;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 22;
  expected.k_ssb             = 2;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 28;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 34;
  expected.k_ssb             = 18;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 42;
  expected.k_ssb             = 2;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 48;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 54;
  expected.k_ssb             = 18;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  // Verify there are no possible SSB positions within the band.
  ASSERT_FALSE(cfg_generator.get_next_ssb_location().is_valid);
}

// Test all possible SSB position within a given band returned by the config generator.
TEST(ssb_freq_position_generation_test, band_51)
{
  ssb_freq_position_generator cfg_generator{/* DL-ARFCN*/ 435740,
                                            /* NR-band*/ nr_band::n66,
                                            /* N_RBs */ 51,
                                            /* SCS_common */ subcarrier_spacing::kHz30,
                                            /* SCS_SSB */ subcarrier_spacing::kHz30};

  ssb_freq_location expected{true, 0, 22};
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  // Get next SSB position(s) and verify its position in terms of offsetToPointA and k_SSB.
  expected.offset_to_point_A = 8;
  expected.k_ssb             = 6;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 14;
  expected.k_ssb             = 14;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 20;
  expected.k_ssb             = 22;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 28;
  expected.k_ssb             = 6;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 34;
  expected.k_ssb             = 14;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 40;
  expected.k_ssb             = 22;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 48;
  expected.k_ssb             = 6;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 54;
  expected.k_ssb             = 14;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  expected.offset_to_point_A = 60;
  expected.k_ssb             = 22;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb_location()));

  // Verify there are no possible SSB positions within the band.
  ASSERT_FALSE(cfg_generator.get_next_ssb_location().is_valid);
}

/*
 *      =====    TEST GENERATION OF SSB AND CORESET0 POSITIONS WITHIN THE BAND    =====
 */

class ssb_coreset0_param_generator_test
  : public ::testing::TestWithParam<std::pair<band_helper::ssb_coreset0_freq_location, cfg_gen_input_params>>
{
protected:
  band_helper::ssb_coreset0_freq_location get_ssb_coreset0_location(cfg_gen_input_params in_param)
  {
    return band_helper::get_ssb_coreset0_freq_location(
        in_param.dl_arfcn, in_param.band, in_param.n_rbs, in_param.scs_common, in_param.scs_ssb);
  }
};

// Test whether the position of SSB and Coreset0/SS0 indices are correctly generated.
TEST_P(ssb_coreset0_param_generator_test, test_du_ssb_coreset0_idx)
{
  std::pair<band_helper::ssb_coreset0_freq_location, cfg_gen_input_params> params = GetParam();

  band_helper::ssb_coreset0_freq_location expected_res = params.first;
  cfg_gen_input_params                    input_params = params.second;

  band_helper::ssb_coreset0_freq_location ssb_coreset0_alloc = get_ssb_coreset0_location(input_params);

  compare_ssb_coreset0_allocation(expected_res, ssb_coreset0_alloc);
}

INSTANTIATE_TEST_SUITE_P(
    du_ssb_coreset0_idx,
    ssb_coreset0_param_generator_test,
    testing::Values(

        std::make_pair(
            band_helper::ssb_coreset0_freq_location{true, 5, 2, 0, 0},
            cfg_gen_input_params{365000, nr_band::n3, 52, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15}),
        std::make_pair(
            band_helper::ssb_coreset0_freq_location{true, 0, 0, 0, 0},
            cfg_gen_input_params{531720, nr_band::n7, 25, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15}),
        std::make_pair(
            band_helper::ssb_coreset0_freq_location{true, 8, 7, 1, 0},
            cfg_gen_input_params{643265, nr_band::n78, 52, subcarrier_spacing::kHz15, subcarrier_spacing::kHz30}),
        std::make_pair(
            band_helper::ssb_coreset0_freq_location{true, 0, 22, 0, 0},
            cfg_gen_input_params{435740, nr_band::n66, 51, subcarrier_spacing::kHz30, subcarrier_spacing::kHz30}),
        std::make_pair(
            band_helper::ssb_coreset0_freq_location{true, 14, 18, 0, 0},
            cfg_gen_input_params{391180, nr_band::n25, 38, subcarrier_spacing::kHz30, subcarrier_spacing::kHz15})));

/*
 *      =====    TEST SSB IS WITHIN SYNC RASTER AND CORESET0 IS WITHIN THE BAND    =====
 */

class ssb_coreset0_compatibility_with_raster_and_band : public ::testing::TestWithParam<cfg_gen_input_params>
{
protected:
  band_helper::ssb_coreset0_freq_location get_ssb_coreset0_allocation(cfg_gen_input_params in_param)
  {
    return band_helper::get_ssb_coreset0_freq_location(
        in_param.dl_arfcn, in_param.band, in_param.n_rbs, in_param.scs_common, in_param.scs_ssb);
  }
};

// Test whether the SSB f_ref correspond to sync raster and Coreset0 is within the band.
TEST_P(ssb_coreset0_compatibility_with_raster_and_band, test_ssb_raster_and_cset0_band)
{
  cfg_gen_input_params params = GetParam();

  cfg_gen_input_params input_params = params;

  band_helper::ssb_coreset0_freq_location ssb_coreset0_alloc = get_ssb_coreset0_allocation(input_params);

  // This function test the SSB and CORESET0 allocation consitency.
  test_ssb_coreset0_allocation(input_params.dl_arfcn,
                               input_params.band,
                               input_params.n_rbs,
                               input_params.scs_common,
                               input_params.scs_ssb,
                               ssb_coreset0_alloc);
}

INSTANTIATE_TEST_SUITE_P(
    ssb_raster_and_cset0_band,
    ssb_coreset0_compatibility_with_raster_and_band,
    testing::Values(
        cfg_gen_input_params{365000, nr_band::n3, 52, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15},
        cfg_gen_input_params{531720, nr_band::n7, 25, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15},
        cfg_gen_input_params{643265, nr_band::n78, 52, subcarrier_spacing::kHz15, subcarrier_spacing::kHz30},
        cfg_gen_input_params{435740, nr_band::n66, 51, subcarrier_spacing::kHz30, subcarrier_spacing::kHz30},
        cfg_gen_input_params{391180, nr_band::n25, 38, subcarrier_spacing::kHz30, subcarrier_spacing::kHz15}));