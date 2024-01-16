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

#include "srsran/ran/csi_report/csi_report_configuration.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/csi_report/csi_report_formatters.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
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

// Valid CSI payload.
const csi_report_packed base_csi_packed = csi_report_packed(get_csi_report_pucch_size(base_csi_configuration).value());

// CSI validator test case parameters.
struct csi_validator_test_params {
  csi_report_configuration config     = base_csi_configuration;
  csi_report_packed        csi_packed = base_csi_packed;
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
const std::vector<csi_validator_test_case> csi_pucch_validator_test_data = {
    {
        [] {
          csi_validator_test_params entry   = {};
          entry.config.nof_csi_rs_resources = 0;
          entry.assert_message              = fmt::format(
              R"(The number of CSI-RS resources in the resource set\, i\.e\.\, {} exceeds the valid range \[1\.\.64\]\.)",
              entry.config.nof_csi_rs_resources);
          return entry;
        },
    },
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
          entry.config.pmi_codebook       = pmi_codebook_type::two;
          entry.config.ri_restriction.resize(1);
          entry.assert_message = fmt::format(
              R"(The RI restriction set size\, i\.e\.\, {}\, is smaller than the number of CSI-RS ports\, i\.e\.\, 2\.)",
              entry.config.ri_restriction.size());
          return entry;
        },
    },
    {
        [] {
          csi_validator_test_params entry = {};
          entry.config.pmi_codebook       = pmi_codebook_type::two;
          entry.config.ri_restriction.resize(3);
          entry.config.ri_restriction.set(2);
          entry.assert_message = fmt::format(
              R"(The RI restriction set\, i\.e\.\, {}\, allows higher rank values than the number of CSI-RS ports\, i\.e\.\, 2\.)",
              entry.config.ri_restriction);
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

          units::bits csi_size         = get_csi_report_pucch_size(base_csi_configuration);
          units::bits invalid_csi_size = csi_size + units::bits(1);
          entry.csi_packed             = csi_report_packed(invalid_csi_size.value());
          entry.assert_message         = fmt::format(
              R"(The number of packed bits \(i\.e\.\, {}\) is not equal to the CSI report size \(i\.e\.\, {}\)\.)",
              invalid_csi_size,
              csi_size);
          return entry;
        },
    },
    {
        [] {
          csi_validator_test_params entry = {};
          // This results in 4 CSI-RS ports and an RI size of 2 bit.
          entry.config.pmi_codebook = pmi_codebook_type::typeI_single_panel_4ports_mode1;
          entry.csi_packed.resize(6);

          // Allow three different ranks, therefore RI can go from 0b00 to 0b10.
          entry.config.ri_restriction.resize(4);
          entry.config.ri_restriction.set(0);
          entry.config.ri_restriction.set(1);
          entry.config.ri_restriction.set(3);

          // Set the RI to a value of 0b11, which is not valid.
          entry.csi_packed.set(0);
          entry.csi_packed.set(1);

          entry.assert_message = fmt::format(
              R"(Packed RI\, i\.e\.\, 3\, is out of bounds given the number of allowed rank values\, i\.e\.\, 3\.)");
          return entry;
        },
    },
};

class CsiReportPucchValidatorFixture : public ::testing::TestWithParam<csi_validator_test_case>
{
protected:
  csi_report_configuration configuration;
  csi_report_packed        csi_packed;
  csi_report_packed        csi2_packed;
  std::string              expected_assert_message;

  void SetUp() override
  {
    const csi_validator_test_params& test_params = GetParam().get_test_params();
    configuration                                = test_params.config;
    csi_packed                                   = test_params.csi_packed;
    expected_assert_message                      = test_params.assert_message;
  }
};

TEST_P(CsiReportPucchValidatorFixture, csiReportPucchUnpackingDeathTest)
{
  // Use thread safe death test.
  ::testing::GTEST_FLAG(death_test_style) = "threadsafe";

  // Make sure the configuration is invalid.
  ASSERT_FALSE(validate_pucch_csi_payload(csi_packed, configuration));

  // Unpack CSI.
#ifdef ASSERTS_ENABLED
  ASSERT_DEATH({ csi_report_unpack_pucch(csi_packed, configuration); }, expected_assert_message);
#endif // ASSERTS_ENABLED
}

INSTANTIATE_TEST_SUITE_P(CsiReportPucchValidatorTest,
                         CsiReportPucchValidatorFixture,
                         ::testing::ValuesIn(csi_pucch_validator_test_data));
} // namespace