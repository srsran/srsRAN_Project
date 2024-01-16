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

#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, const pdsch_mcs_table& mcs_table)
{
  switch (mcs_table) {
    case pdsch_mcs_table::qam64:
      fmt::print(os, "qam64");
      break;
    case pdsch_mcs_table::qam256:
      fmt::print(os, "qam256");
      break;
    case pdsch_mcs_table::qam64LowSe:
      fmt::print(os, "qam64LowSe");
      break;
  }
  return os;
}

} // namespace srsran

using namespace srsran;

// Combined test parameters. MCS table and index.
using pdsch_mcs_param = std::tuple<pdsch_mcs_table, unsigned>;

// Combined expected values from TS38.211 Tables 6.3.3.1-5, 6.3.3.1-6 and 6.3.3.1-7.
static const std::map<pdsch_mcs_param, std::tuple<unsigned, float, float>> pdsch_mcs_expected = {
    {{pdsch_mcs_table::qam64, 0}, {2, 120.0F, 0.2344F}},
    {{pdsch_mcs_table::qam64, 1}, {2, 157.0F, 0.3066F}},
    {{pdsch_mcs_table::qam64, 2}, {2, 193.0F, 0.3770F}},
    {{pdsch_mcs_table::qam64, 3}, {2, 251.0F, 0.4902F}},
    {{pdsch_mcs_table::qam64, 4}, {2, 308.0F, 0.6016F}},
    {{pdsch_mcs_table::qam64, 5}, {2, 379.0F, 0.7402F}},
    {{pdsch_mcs_table::qam64, 6}, {2, 449.0F, 0.8770F}},
    {{pdsch_mcs_table::qam64, 7}, {2, 526.0F, 1.0273F}},
    {{pdsch_mcs_table::qam64, 8}, {2, 602.0F, 1.1758F}},
    {{pdsch_mcs_table::qam64, 9}, {2, 679.0F, 1.3262F}},
    {{pdsch_mcs_table::qam64, 10}, {4, 340.0F, 1.3281F}},
    {{pdsch_mcs_table::qam64, 11}, {4, 378.0F, 1.4766F}},
    {{pdsch_mcs_table::qam64, 12}, {4, 434.0F, 1.6953F}},
    {{pdsch_mcs_table::qam64, 13}, {4, 490.0F, 1.9141F}},
    {{pdsch_mcs_table::qam64, 14}, {4, 553.0F, 2.1602F}},
    {{pdsch_mcs_table::qam64, 15}, {4, 616.0F, 2.4063F}},
    {{pdsch_mcs_table::qam64, 16}, {4, 658.0F, 2.5703F}},
    {{pdsch_mcs_table::qam64, 17}, {6, 438.0F, 2.5664F}},
    {{pdsch_mcs_table::qam64, 18}, {6, 466.0F, 2.7305F}},
    {{pdsch_mcs_table::qam64, 19}, {6, 517.0F, 3.0293F}},
    {{pdsch_mcs_table::qam64, 20}, {6, 567.0F, 3.3223F}},
    {{pdsch_mcs_table::qam64, 21}, {6, 616.0F, 3.6094F}},
    {{pdsch_mcs_table::qam64, 22}, {6, 666.0F, 3.9023F}},
    {{pdsch_mcs_table::qam64, 23}, {6, 719.0F, 4.2129F}},
    {{pdsch_mcs_table::qam64, 24}, {6, 772.0F, 4.5234F}},
    {{pdsch_mcs_table::qam64, 25}, {6, 822.0F, 4.8164F}},
    {{pdsch_mcs_table::qam64, 26}, {6, 873.0F, 5.1152F}},
    {{pdsch_mcs_table::qam64, 27}, {6, 910.0F, 5.3320F}},
    {{pdsch_mcs_table::qam64, 28}, {6, 948.0F, 5.5547F}},
    {{pdsch_mcs_table::qam64, 29}, {2, 0.0F, 0.0F}},
    {{pdsch_mcs_table::qam64, 30}, {4, 0.0F, 0.0F}},
    {{pdsch_mcs_table::qam64, 31}, {6, 0.0F, 0.0F}},
    {{pdsch_mcs_table::qam256, 0}, {2, 120.0F, 0.2344F}},
    {{pdsch_mcs_table::qam256, 1}, {2, 193.0F, 0.3770F}},
    {{pdsch_mcs_table::qam256, 2}, {2, 308.0F, 0.6016F}},
    {{pdsch_mcs_table::qam256, 3}, {2, 449.0F, 0.8770F}},
    {{pdsch_mcs_table::qam256, 4}, {2, 602.0F, 1.1758F}},
    {{pdsch_mcs_table::qam256, 5}, {4, 378.0F, 1.4766F}},
    {{pdsch_mcs_table::qam256, 6}, {4, 434.0F, 1.6953F}},
    {{pdsch_mcs_table::qam256, 7}, {4, 490.0F, 1.9141F}},
    {{pdsch_mcs_table::qam256, 8}, {4, 553.0F, 2.1602F}},
    {{pdsch_mcs_table::qam256, 9}, {4, 616.0F, 2.4063F}},
    {{pdsch_mcs_table::qam256, 10}, {4, 658.0F, 2.5703F}},
    {{pdsch_mcs_table::qam256, 11}, {6, 466.0F, 2.7305F}},
    {{pdsch_mcs_table::qam256, 12}, {6, 517.0F, 3.0293F}},
    {{pdsch_mcs_table::qam256, 13}, {6, 567.0F, 3.3223F}},
    {{pdsch_mcs_table::qam256, 14}, {6, 616.0F, 3.6094F}},
    {{pdsch_mcs_table::qam256, 15}, {6, 666.0F, 3.9023F}},
    {{pdsch_mcs_table::qam256, 16}, {6, 719.0F, 4.2129F}},
    {{pdsch_mcs_table::qam256, 17}, {6, 772.0F, 4.5234F}},
    {{pdsch_mcs_table::qam256, 18}, {6, 822.0F, 4.8164F}},
    {{pdsch_mcs_table::qam256, 19}, {6, 873.0F, 5.1152F}},
    {{pdsch_mcs_table::qam256, 20}, {8, 682.5F, 5.3320F}},
    {{pdsch_mcs_table::qam256, 21}, {8, 711.0F, 5.5547F}},
    {{pdsch_mcs_table::qam256, 22}, {8, 754.0F, 5.8906F}},
    {{pdsch_mcs_table::qam256, 23}, {8, 797.0F, 6.2266F}},
    {{pdsch_mcs_table::qam256, 24}, {8, 841.0F, 6.5703F}},
    {{pdsch_mcs_table::qam256, 25}, {8, 885.0F, 6.9141F}},
    {{pdsch_mcs_table::qam256, 26}, {8, 916.5F, 7.1602F}},
    {{pdsch_mcs_table::qam256, 27}, {8, 948.0F, 7.4063F}},
    {{pdsch_mcs_table::qam256, 28}, {2, 0.0F, 0.0F}},
    {{pdsch_mcs_table::qam256, 29}, {4, 0.0F, 0.0F}},
    {{pdsch_mcs_table::qam256, 30}, {6, 0.0F, 0.0F}},
    {{pdsch_mcs_table::qam256, 31}, {8, 0.0F, 0.0F}},
    {{pdsch_mcs_table::qam64LowSe, 0}, {2, 30.0F, 0.0586F}},
    {{pdsch_mcs_table::qam64LowSe, 1}, {2, 40.0F, 0.0781F}},
    {{pdsch_mcs_table::qam64LowSe, 2}, {2, 50.0F, 0.0977F}},
    {{pdsch_mcs_table::qam64LowSe, 3}, {2, 64.0F, 0.1250F}},
    {{pdsch_mcs_table::qam64LowSe, 4}, {2, 78.0F, 0.1523F}},
    {{pdsch_mcs_table::qam64LowSe, 5}, {2, 99.0F, 0.1934F}},
    {{pdsch_mcs_table::qam64LowSe, 6}, {2, 120.0F, 0.2344F}},
    {{pdsch_mcs_table::qam64LowSe, 7}, {2, 157.0F, 0.3066F}},
    {{pdsch_mcs_table::qam64LowSe, 8}, {2, 193.0F, 0.3770F}},
    {{pdsch_mcs_table::qam64LowSe, 9}, {2, 251.0F, 0.4902F}},
    {{pdsch_mcs_table::qam64LowSe, 10}, {2, 308.0F, 0.6016F}},
    {{pdsch_mcs_table::qam64LowSe, 11}, {2, 379.0F, 0.7402F}},
    {{pdsch_mcs_table::qam64LowSe, 12}, {2, 449.0F, 0.8770F}},
    {{pdsch_mcs_table::qam64LowSe, 13}, {2, 526.0F, 1.0273F}},
    {{pdsch_mcs_table::qam64LowSe, 14}, {2, 602.0F, 1.1758F}},
    {{pdsch_mcs_table::qam64LowSe, 15}, {4, 340.0F, 1.3281F}},
    {{pdsch_mcs_table::qam64LowSe, 16}, {4, 378.0F, 1.4766F}},
    {{pdsch_mcs_table::qam64LowSe, 17}, {4, 434.0F, 1.6953F}},
    {{pdsch_mcs_table::qam64LowSe, 18}, {4, 490.0F, 1.9141F}},
    {{pdsch_mcs_table::qam64LowSe, 19}, {4, 553.0F, 2.1602F}},
    {{pdsch_mcs_table::qam64LowSe, 20}, {4, 616.0F, 2.4063F}},
    {{pdsch_mcs_table::qam64LowSe, 21}, {6, 438.0F, 2.5664F}},
    {{pdsch_mcs_table::qam64LowSe, 22}, {6, 466.0F, 2.7305F}},
    {{pdsch_mcs_table::qam64LowSe, 23}, {6, 517.0F, 3.0293F}},
    {{pdsch_mcs_table::qam64LowSe, 24}, {6, 567.0F, 3.3223F}},
    {{pdsch_mcs_table::qam64LowSe, 25}, {6, 616.0F, 3.6094F}},
    {{pdsch_mcs_table::qam64LowSe, 26}, {6, 666.0F, 3.9023F}},
    {{pdsch_mcs_table::qam64LowSe, 27}, {6, 719.0F, 4.2129F}},
    {{pdsch_mcs_table::qam64LowSe, 28}, {6, 772.0F, 4.5234F}},
    {{pdsch_mcs_table::qam64LowSe, 29}, {2, 0.0F, 0.0F}},
    {{pdsch_mcs_table::qam64LowSe, 30}, {4, 0.0F, 0.0F}},
    {{pdsch_mcs_table::qam64LowSe, 31}, {6, 0.0F, 0.0F}},
};

class pdsch_mcs_table_test : public ::testing::TestWithParam<pdsch_mcs_param>
{
protected:
  static std::tuple<unsigned, float, float> get_expected()
  {
    // Get parameter.
    pdsch_mcs_param param = GetParam();

    // Return the expected value.
    return pdsch_mcs_expected.at(param);
  }
};

TEST_P(pdsch_mcs_table_test, check_combinations)
{
  // Get combined parameter.
  pdsch_mcs_param params = GetParam();

  // Extract parameters.
  pdsch_mcs_table table = std::get<0>(params);
  sch_mcs_index   mcs   = std::get<1>(params);

  // Get number of cyclic shifts.
  sch_mcs_description descr = pdsch_mcs_get_config(table, mcs);

  std::tuple<unsigned, float, float> expected = get_expected();

  // Compare with expected.
  ASSERT_EQ(std::get<0>(expected), get_bits_per_symbol(descr.modulation));
  ASSERT_EQ(std::get<1>(expected), descr.target_code_rate);
  ASSERT_NEAR(std::get<2>(expected), descr.get_spectral_efficiency(), 1e-4);
  ASSERT_NEAR(std::get<0>(expected) * std::get<1>(expected) / 1024.0F, std::get<2>(expected), 1e-4);
}

INSTANTIATE_TEST_SUITE_P(
    pdsch_mcs_table,
    pdsch_mcs_table_test,
    ::testing::Combine(::testing::Values(pdsch_mcs_table::qam64, pdsch_mcs_table::qam256, pdsch_mcs_table::qam64LowSe),
                       ::testing::Range(0U, 32U)));
