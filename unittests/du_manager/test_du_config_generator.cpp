/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/du_manager/converters/config_generator.h"
#include "../du_manager/du_test_suite.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

using namespace srs_du;

// Define struct to pass parameters from test functions.
struct du_cfg_gen_input_params {
  unsigned           dl_arfcn;
  unsigned           nr_band;
  unsigned           n_rbs;
  subcarrier_spacing scs_common;
  subcarrier_spacing scs_ssb;
};

static bool compare_ssb_freq_location(const ssb_freq_location& lhs, const ssb_freq_location& rhs)
{
  bool result = lhs.is_valid && rhs.is_valid && lhs.k_ssb.to_uint() == rhs.k_ssb.to_uint() &&
                lhs.offset_to_point_A.to_uint() == rhs.offset_to_point_A.to_uint();
  return result;
}

static bool compare_ssb_coreset0_allocation(const du_ssb_sib1_location& lhs, const du_ssb_sib1_location& rhs)
{
  bool result = lhs.is_valid && rhs.is_valid && lhs.k_ssb.to_uint() == rhs.k_ssb.to_uint() &&
                lhs.offset_to_point_A.to_uint() == rhs.offset_to_point_A.to_uint() &&
                lhs.coreset0_idx == rhs.coreset0_idx && lhs.searchspace0_idx == rhs.searchspace0_idx;
  return result;
}

TEST(du_config_generator_tester, first_param_set)
{
  du_config_generator cfg_generator{/* DL-ARFCN*/ 365000,
                                    /* NR-band*/ 3,
                                    /* N_RBs */ 52,
                                    /* SCS_common */ subcarrier_spacing::kHz15,
                                    /* SCS_SSB */ subcarrier_spacing::kHz15};

  ssb_freq_location expected{true, 5, 2};
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 11;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 18;
  expected.k_ssb             = 6;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 25;
  expected.k_ssb             = 2;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 31;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  ASSERT_TRUE(not cfg_generator.get_next_ssb().is_valid);
}

TEST(du_config_generator_tester, second_param_set)
{
  du_config_generator cfg_generator{/* DL-ARFCN*/ 531720,
                                    /* NR-band*/ 7,
                                    /* N_RBs */ 25,
                                    /* SCS_common */ subcarrier_spacing::kHz15,
                                    /* SCS_SSB */ subcarrier_spacing::kHz15};

  ssb_freq_location expected{true, 0, 0};
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  ASSERT_FALSE(cfg_generator.get_next_ssb().is_valid);
}

TEST(du_config_generator_tester, scs_ssb_15kHz_common_30kHz)
{
  du_config_generator cfg_generator{/* DL-ARFCN*/ 391180,
                                    /* NR-band*/ 25,
                                    /* N_RBs */ 38,
                                    /* SCS_common */ subcarrier_spacing::kHz30,
                                    /* SCS_SSB */ subcarrier_spacing::kHz15};

  ssb_freq_location expected{true, 2, 2};
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 8;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 14;
  expected.k_ssb             = 18;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 22;
  expected.k_ssb             = 2;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 28;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 34;
  expected.k_ssb             = 18;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 42;
  expected.k_ssb             = 2;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 48;
  expected.k_ssb             = 10;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 54;
  expected.k_ssb             = 18;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));
}

TEST(du_config_generator_tester, third_param_set)
{
  du_config_generator cfg_generator{/* DL-ARFCN*/ 435740,
                                    /* NR-band*/ 66,
                                    /* N_RBs */ 51,
                                    /* SCS_common */ subcarrier_spacing::kHz30,
                                    /* SCS_SSB */ subcarrier_spacing::kHz30};

  ssb_freq_location expected{true, 0, 22};
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 8;
  expected.k_ssb             = 6;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 14;
  expected.k_ssb             = 14;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 20;
  expected.k_ssb             = 22;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 28;
  expected.k_ssb             = 6;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 34;
  expected.k_ssb             = 14;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 40;
  expected.k_ssb             = 22;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 48;
  expected.k_ssb             = 6;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 54;
  expected.k_ssb             = 14;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  expected.offset_to_point_A = 60;
  expected.k_ssb             = 22;
  ASSERT_TRUE(compare_ssb_freq_location(expected, cfg_generator.get_next_ssb()));

  ASSERT_FALSE(cfg_generator.get_next_ssb().is_valid);
}

class du_config_generator_fixture
  : public ::testing::TestWithParam<std::pair<du_ssb_sib1_location, du_cfg_gen_input_params>>
{
protected:
  ssb_freq_location get_ssb_freq_location(du_cfg_gen_input_params in_param)
  {
    du_config_generator cfg_generator{
        in_param.dl_arfcn, in_param.nr_band, in_param.n_rbs, in_param.scs_common, in_param.scs_ssb};
    return cfg_generator.get_next_ssb();
  }
  du_ssb_sib1_location get_ssb_coreset0_location(du_cfg_gen_input_params in_param)
  {
    return get_ssb_sib1_freq_location(
        in_param.dl_arfcn, in_param.nr_band, in_param.n_rbs, in_param.scs_common, in_param.scs_ssb);
  }
};

TEST_P(du_config_generator_fixture, test_du_ssb_coreset0_idx)
{
  std::pair<du_ssb_sib1_location, du_cfg_gen_input_params> params = GetParam();

  du_ssb_sib1_location    expected_res = params.first;
  du_cfg_gen_input_params input_params = params.second;

  du_ssb_sib1_location ssb_coreset0_alloc = get_ssb_coreset0_location(input_params);

  //
  test_ssb_coreset0_allocation(input_params.dl_arfcn,
                               input_params.nr_band,
                               input_params.n_rbs,
                               input_params.scs_common,
                               input_params.scs_ssb,
                               ssb_coreset0_alloc);

  compare_ssb_coreset0_allocation(expected_res, ssb_coreset0_alloc);
}

INSTANTIATE_TEST_SUITE_P(
    Generate_ssb_coreset0_test,
    du_config_generator_fixture,
    testing::Values(

        std::make_pair(du_ssb_sib1_location{true, 5, 2, 0, 0},
                       du_cfg_gen_input_params{365000, 3, 52, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15}),
        std::make_pair(du_ssb_sib1_location{true, 0, 0, 0, 0},
                       du_cfg_gen_input_params{531720, 7, 25, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15}),
        std::make_pair(du_ssb_sib1_location{true, 8, 7, 1, 0},
                       du_cfg_gen_input_params{643265, 78, 52, subcarrier_spacing::kHz15, subcarrier_spacing::kHz30}),
        std::make_pair(du_ssb_sib1_location{true, 0, 22, 0, 0},
                       du_cfg_gen_input_params{435740, 66, 51, subcarrier_spacing::kHz30, subcarrier_spacing::kHz30}),
        std::make_pair(du_ssb_sib1_location{true, 14, 18, 0, 0},
                       du_cfg_gen_input_params{391180, 25, 38, subcarrier_spacing::kHz30, subcarrier_spacing::kHz15})));

class du_config_ssb_coreset0_fixture : public ::testing::TestWithParam<du_cfg_gen_input_params>
{
protected:
  du_ssb_sib1_location get_ssb_coreset0_allocation(du_cfg_gen_input_params in_param)
  {
    return get_ssb_sib1_freq_location(
        in_param.dl_arfcn, in_param.nr_band, in_param.n_rbs, in_param.scs_common, in_param.scs_ssb);
  }
};

TEST_P(du_config_ssb_coreset0_fixture, test_du_ssb_coreset0_consitency)
{
  du_cfg_gen_input_params params = GetParam();

  du_cfg_gen_input_params input_params = params;

  du_ssb_sib1_location ssb_coreset0_alloc = get_ssb_coreset0_allocation(input_params);

  // This function test the SSB and CORESET0 allocation consitency.
  test_ssb_coreset0_allocation(input_params.dl_arfcn,
                               input_params.nr_band,
                               input_params.n_rbs,
                               input_params.scs_common,
                               input_params.scs_ssb,
                               ssb_coreset0_alloc);
}

INSTANTIATE_TEST_SUITE_P(
    ssb_coreset0_consistency_test,
    du_config_ssb_coreset0_fixture,
    testing::Values(du_cfg_gen_input_params{365000, 3, 52, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15},
                    du_cfg_gen_input_params{531720, 7, 25, subcarrier_spacing::kHz15, subcarrier_spacing::kHz15},
                       du_cfg_gen_input_params{643265, 78, 52, subcarrier_spacing::kHz15, subcarrier_spacing::kHz30},
                       du_cfg_gen_input_params{435740, 66, 51, subcarrier_spacing::kHz30, subcarrier_spacing::kHz30},
                       du_cfg_gen_input_params{391180, 25, 38, subcarrier_spacing::kHz30, subcarrier_spacing::kHz15}
                    ));