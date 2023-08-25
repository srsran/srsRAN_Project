/*
*
* Copyright 2021-2023 Software Radio Systems Limited
*
* By using this file, you agree to the terms and conditions set
* forth in the LICENSE file which can be found at the top level of
* the distribution.
*
*/

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_formatters.h"
#include "srsran/ran/csi_report/csi_report_on_pusch_helpers.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace {

// Valid CSI configuration.
const csi_report_configuration base_csi_configuration = {
    // Number of CSI RS resources.
    1,
    // PMI codebook.
    pmi_codebook_type::one,
    // RI restriction.
    ~ri_restriction_type(1),
    // CSI report quantities.
    csi_report_quantities::cri_ri_cqi};

// Valid CSI Part 1 payload.
const csi_report_packed base_csi1_packed =
    csi_report_packed(get_csi_report_pusch_size(base_csi_configuration).part1_size.value());

// CSI validator test case parameters.
struct csi_validator_test_params {
  csi_report_configuration config      = base_csi_configuration;
  csi_report_packed        csi1_packed = base_csi1_packed;
  csi_report_packed        csi2_packed = csi_report_packed(0);
  std::string              assert_message;
};

struct csi_validator_test_case {
  std::function<csi_validator_test_params()> get_test_params;
};

std::ostream& operator<<(std::ostream& os, const csi_validator_test_case& test_case)
{
  fmt::print(os, "{}", test_case.get_test_params().config);
  return os;
}

// Test cases are implemented as lambda functions that generate and return an invalid CSI payload, along with the
// expected assert message.
const std::vector<csi_validator_test_case> csi_pusch_validator_test_data = {
    {
        [] {
          csi_validator_test_params entry = {};
          entry.config.pmi_codebook       = pmi_codebook_type::other;
          entry.assert_message            = fmt::format(R"(Unsupported PMI codebook type\.)");
          return entry;
        },
    },
    {
        [] {
          csi_validator_test_params entry = {};
          entry.config.quantities         = csi_report_quantities::other;
          entry.assert_message            = fmt::format(R"(Invalid CSI report quantities\.)");
          return entry;
        },
    },
    {
        [] {
          csi_validator_test_params entry = {};
          entry.config.pmi_codebook       = pmi_codebook_type::two;
          entry.config.quantities         = csi_report_quantities::cri_ri_li_pmi_cqi;
          entry.assert_message =
              fmt::format(R"(PUSCH CSI Part 2 is required for more than one CSI-RS port when PMI is reported\.)");
          return entry;
        },
    },
    {
        [] {
          csi_validator_test_params entry = {};

          units::bits csi1_size         = get_csi_report_pusch_size(base_csi_configuration).part1_size;
          units::bits invalid_csi1_size = csi1_size + units::bits(1);
          entry.csi1_packed             = csi_report_packed(invalid_csi1_size.value());
          entry.assert_message          = fmt::format(
              R"(The number of packed bits for CSI Part 1 \(i\.e\.\, {}\) is not equal to the CSI Part 1 size \(i\.e\.\, {}\)\.)",
              invalid_csi1_size,
              csi1_size);
          return entry;
        },
    },
    {
        [] {
          csi_validator_test_params entry = {};

          // Enable CSI report Part 2.
          entry.config.pmi_codebook   = pmi_codebook_type::two;
          entry.config.quantities     = csi_report_quantities::cri_ri_pmi_cqi;
          entry.config.ri_restriction = ~ri_restriction_type(2);

          // Resize CSI report Part 1 to the valid dimensions.
          units::bits csi1_size = get_csi_report_pusch_size(entry.config).part1_size;
          entry.csi1_packed     = csi_report_packed(csi1_size.value());

          // Resize CSI report Part 2 to an invalid value.
          units::bits csi2_size         = get_csi_report_pusch_size(entry.config).part2_max_size;
          units::bits invalid_csi2_size = csi2_size + units::bits(1);
          entry.csi2_packed             = csi_report_packed(invalid_csi2_size.value());
          entry.assert_message          = fmt::format(
              R"(The number of packed bits for CSI Part 2 \(i\.e\.\, {}\) is not equal to the CSI Part 2 size \(i\.e\.\, {})\.)",
              invalid_csi2_size,
              csi2_size);
          return entry;
        },
    },
};

class CsiReportPuschValidatorFixture : public ::testing::TestWithParam<csi_validator_test_case>
{
protected:
  csi_report_configuration configuration;
  csi_report_packed        csi1_packed;
  csi_report_packed        csi2_packed;
  std::string              expected_assert_message;

  void SetUp() override
  {
    const csi_validator_test_params& test_params = GetParam().get_test_params();
    configuration                                = test_params.config;
    csi1_packed                                  = test_params.csi1_packed;
    csi2_packed                                  = test_params.csi2_packed;
    expected_assert_message                      = test_params.assert_message;
  }
};

TEST_P(CsiReportPuschValidatorFixture, csiReportPuschUnpackingDeathTest)
{
  // Use thread safe death test.
  ::testing::GTEST_FLAG(death_test_style) = "threadsafe";

  // Make sure the configuration is invalid.
  ASSERT_FALSE(validate_pusch_csi_payload(csi1_packed, csi2_packed, configuration));

  // Unpack CSI.
#ifdef ASSERTS_ENABLED
  ASSERT_DEATH({ csi_report_unpack_pusch(csi1_packed, csi2_packed, configuration); }, expected_assert_message);
#endif // ASSERTS_ENABLED
}

INSTANTIATE_TEST_SUITE_P(CsiReportPuschValidatorTest,
                         CsiReportPuschValidatorFixture,
                         ::testing::ValuesIn(csi_pusch_validator_test_data));
} // namespace