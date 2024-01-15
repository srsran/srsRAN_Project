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

#include "srsran/ran/pusch/pusch_mcs.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, const pusch_mcs_table& mcs_table)
{
  switch (mcs_table) {
    case pusch_mcs_table::qam64:
      fmt::print(os, "qam64");
      break;
    case pusch_mcs_table::qam256:
      fmt::print(os, "qam256");
      break;
    case pusch_mcs_table::qam64LowSe:
      fmt::print(os, "qam64LowSe");
      break;
    case pusch_mcs_table::qam64_tp:
      fmt::print(os, "qam64TP");
      break;
    case pusch_mcs_table::qam64LowSe_tp:
      fmt::print(os, "qam64LowSeTP");
      break;
  }
  return os;
}

} // namespace srsran

using namespace srsran;

// Combined test parameters. MCS table and index.
using pusch_mcs_param = std::tuple<pusch_mcs_table, unsigned, bool>;

// Combined expected values from TS38.211 Tables 6.3.3.1-5, 6.3.3.1-6 and 6.3.3.1-7.
static const std::map<pusch_mcs_param, std::tuple<unsigned, float, float>> pusch_mcs_expected = {
    {{pusch_mcs_table::qam64, 0, false}, {2, 120.0F, 0.2344F}},
    {{pusch_mcs_table::qam64, 1, false}, {2, 157.0F, 0.3066F}},
    {{pusch_mcs_table::qam64, 2, false}, {2, 193.0F, 0.3770F}},
    {{pusch_mcs_table::qam64, 3, false}, {2, 251.0F, 0.4902F}},
    {{pusch_mcs_table::qam64, 4, false}, {2, 308.0F, 0.6016F}},
    {{pusch_mcs_table::qam64, 5, false}, {2, 379.0F, 0.7402F}},
    {{pusch_mcs_table::qam64, 6, false}, {2, 449.0F, 0.8770F}},
    {{pusch_mcs_table::qam64, 7, false}, {2, 526.0F, 1.0273F}},
    {{pusch_mcs_table::qam64, 8, false}, {2, 602.0F, 1.1758F}},
    {{pusch_mcs_table::qam64, 9, false}, {2, 679.0F, 1.3262F}},
    {{pusch_mcs_table::qam64, 10, false}, {4, 340.0F, 1.3281F}},
    {{pusch_mcs_table::qam64, 11, false}, {4, 378.0F, 1.4766F}},
    {{pusch_mcs_table::qam64, 12, false}, {4, 434.0F, 1.6953F}},
    {{pusch_mcs_table::qam64, 13, false}, {4, 490.0F, 1.9141F}},
    {{pusch_mcs_table::qam64, 14, false}, {4, 553.0F, 2.1602F}},
    {{pusch_mcs_table::qam64, 15, false}, {4, 616.0F, 2.4063F}},
    {{pusch_mcs_table::qam64, 16, false}, {4, 658.0F, 2.5703F}},
    {{pusch_mcs_table::qam64, 17, false}, {6, 438.0F, 2.5664F}},
    {{pusch_mcs_table::qam64, 18, false}, {6, 466.0F, 2.7305F}},
    {{pusch_mcs_table::qam64, 19, false}, {6, 517.0F, 3.0293F}},
    {{pusch_mcs_table::qam64, 20, false}, {6, 567.0F, 3.3223F}},
    {{pusch_mcs_table::qam64, 21, false}, {6, 616.0F, 3.6094F}},
    {{pusch_mcs_table::qam64, 22, false}, {6, 666.0F, 3.9023F}},
    {{pusch_mcs_table::qam64, 23, false}, {6, 719.0F, 4.2129F}},
    {{pusch_mcs_table::qam64, 24, false}, {6, 772.0F, 4.5234F}},
    {{pusch_mcs_table::qam64, 25, false}, {6, 822.0F, 4.8164F}},
    {{pusch_mcs_table::qam64, 26, false}, {6, 873.0F, 5.1152F}},
    {{pusch_mcs_table::qam64, 27, false}, {6, 910.0F, 5.3320F}},
    {{pusch_mcs_table::qam64, 28, false}, {6, 948.0F, 5.5547F}},
    {{pusch_mcs_table::qam64, 29, false}, {2, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64, 30, false}, {4, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64, 31, false}, {6, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam256, 0, false}, {2, 120.0F, 0.2344F}},
    {{pusch_mcs_table::qam256, 1, false}, {2, 193.0F, 0.3770F}},
    {{pusch_mcs_table::qam256, 2, false}, {2, 308.0F, 0.6016F}},
    {{pusch_mcs_table::qam256, 3, false}, {2, 449.0F, 0.8770F}},
    {{pusch_mcs_table::qam256, 4, false}, {2, 602.0F, 1.1758F}},
    {{pusch_mcs_table::qam256, 5, false}, {4, 378.0F, 1.4766F}},
    {{pusch_mcs_table::qam256, 6, false}, {4, 434.0F, 1.6953F}},
    {{pusch_mcs_table::qam256, 7, false}, {4, 490.0F, 1.9141F}},
    {{pusch_mcs_table::qam256, 8, false}, {4, 553.0F, 2.1602F}},
    {{pusch_mcs_table::qam256, 9, false}, {4, 616.0F, 2.4063F}},
    {{pusch_mcs_table::qam256, 10, false}, {4, 658.0F, 2.5703F}},
    {{pusch_mcs_table::qam256, 11, false}, {6, 466.0F, 2.7305F}},
    {{pusch_mcs_table::qam256, 12, false}, {6, 517.0F, 3.0293F}},
    {{pusch_mcs_table::qam256, 13, false}, {6, 567.0F, 3.3223F}},
    {{pusch_mcs_table::qam256, 14, false}, {6, 616.0F, 3.6094F}},
    {{pusch_mcs_table::qam256, 15, false}, {6, 666.0F, 3.9023F}},
    {{pusch_mcs_table::qam256, 16, false}, {6, 719.0F, 4.2129F}},
    {{pusch_mcs_table::qam256, 17, false}, {6, 772.0F, 4.5234F}},
    {{pusch_mcs_table::qam256, 18, false}, {6, 822.0F, 4.8164F}},
    {{pusch_mcs_table::qam256, 19, false}, {6, 873.0F, 5.1152F}},
    {{pusch_mcs_table::qam256, 20, false}, {8, 682.5F, 5.3320F}},
    {{pusch_mcs_table::qam256, 21, false}, {8, 711.0F, 5.5547F}},
    {{pusch_mcs_table::qam256, 22, false}, {8, 754.0F, 5.8906F}},
    {{pusch_mcs_table::qam256, 23, false}, {8, 797.0F, 6.2266F}},
    {{pusch_mcs_table::qam256, 24, false}, {8, 841.0F, 6.5703F}},
    {{pusch_mcs_table::qam256, 25, false}, {8, 885.0F, 6.9141F}},
    {{pusch_mcs_table::qam256, 26, false}, {8, 916.5F, 7.1602F}},
    {{pusch_mcs_table::qam256, 27, false}, {8, 948.0F, 7.4063F}},
    {{pusch_mcs_table::qam256, 28, false}, {2, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam256, 29, false}, {4, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam256, 30, false}, {6, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam256, 31, false}, {8, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64LowSe, 0, false}, {2, 30.0F, 0.0586F}},
    {{pusch_mcs_table::qam64LowSe, 1, false}, {2, 40.0F, 0.0781F}},
    {{pusch_mcs_table::qam64LowSe, 2, false}, {2, 50.0F, 0.0977F}},
    {{pusch_mcs_table::qam64LowSe, 3, false}, {2, 64.0F, 0.1250F}},
    {{pusch_mcs_table::qam64LowSe, 4, false}, {2, 78.0F, 0.1523F}},
    {{pusch_mcs_table::qam64LowSe, 5, false}, {2, 99.0F, 0.1934F}},
    {{pusch_mcs_table::qam64LowSe, 6, false}, {2, 120.0F, 0.2344F}},
    {{pusch_mcs_table::qam64LowSe, 7, false}, {2, 157.0F, 0.3066F}},
    {{pusch_mcs_table::qam64LowSe, 8, false}, {2, 193.0F, 0.3770F}},
    {{pusch_mcs_table::qam64LowSe, 9, false}, {2, 251.0F, 0.4902F}},
    {{pusch_mcs_table::qam64LowSe, 10, false}, {2, 308.0F, 0.6016F}},
    {{pusch_mcs_table::qam64LowSe, 11, false}, {2, 379.0F, 0.7402F}},
    {{pusch_mcs_table::qam64LowSe, 12, false}, {2, 449.0F, 0.8770F}},
    {{pusch_mcs_table::qam64LowSe, 13, false}, {2, 526.0F, 1.0273F}},
    {{pusch_mcs_table::qam64LowSe, 14, false}, {2, 602.0F, 1.1758F}},
    {{pusch_mcs_table::qam64LowSe, 15, false}, {4, 340.0F, 1.3281F}},
    {{pusch_mcs_table::qam64LowSe, 16, false}, {4, 378.0F, 1.4766F}},
    {{pusch_mcs_table::qam64LowSe, 17, false}, {4, 434.0F, 1.6953F}},
    {{pusch_mcs_table::qam64LowSe, 18, false}, {4, 490.0F, 1.9141F}},
    {{pusch_mcs_table::qam64LowSe, 19, false}, {4, 553.0F, 2.1602F}},
    {{pusch_mcs_table::qam64LowSe, 20, false}, {4, 616.0F, 2.4063F}},
    {{pusch_mcs_table::qam64LowSe, 21, false}, {6, 438.0F, 2.5664F}},
    {{pusch_mcs_table::qam64LowSe, 22, false}, {6, 466.0F, 2.7305F}},
    {{pusch_mcs_table::qam64LowSe, 23, false}, {6, 517.0F, 3.0293F}},
    {{pusch_mcs_table::qam64LowSe, 24, false}, {6, 567.0F, 3.3223F}},
    {{pusch_mcs_table::qam64LowSe, 25, false}, {6, 616.0F, 3.6094F}},
    {{pusch_mcs_table::qam64LowSe, 26, false}, {6, 666.0F, 3.9023F}},
    {{pusch_mcs_table::qam64LowSe, 27, false}, {6, 719.0F, 4.2129F}},
    {{pusch_mcs_table::qam64LowSe, 28, false}, {6, 772.0F, 4.5234F}},
    {{pusch_mcs_table::qam64LowSe, 29, false}, {2, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64LowSe, 30, false}, {4, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64LowSe, 31, false}, {6, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64_tp, 0, false}, {1, 240.0F, 0.2344F}},
    {{pusch_mcs_table::qam64_tp, 1, false}, {1, 314.0F, 0.3066F}},
    {{pusch_mcs_table::qam64_tp, 2, false}, {2, 193.0F, 0.3770F}},
    {{pusch_mcs_table::qam64_tp, 3, false}, {2, 251.0F, 0.4902F}},
    {{pusch_mcs_table::qam64_tp, 4, false}, {2, 308.0F, 0.6016F}},
    {{pusch_mcs_table::qam64_tp, 5, false}, {2, 379.0F, 0.7402F}},
    {{pusch_mcs_table::qam64_tp, 6, false}, {2, 449.0F, 0.8770F}},
    {{pusch_mcs_table::qam64_tp, 7, false}, {2, 526.0F, 1.0273F}},
    {{pusch_mcs_table::qam64_tp, 8, false}, {2, 602.0F, 1.1758F}},
    {{pusch_mcs_table::qam64_tp, 9, false}, {2, 679.0F, 1.3262F}},
    {{pusch_mcs_table::qam64_tp, 10, false}, {4, 340.0F, 1.3281F}},
    {{pusch_mcs_table::qam64_tp, 11, false}, {4, 378.0F, 1.4766F}},
    {{pusch_mcs_table::qam64_tp, 12, false}, {4, 434.0F, 1.6953F}},
    {{pusch_mcs_table::qam64_tp, 13, false}, {4, 490.0F, 1.9141F}},
    {{pusch_mcs_table::qam64_tp, 14, false}, {4, 553.0F, 2.1602F}},
    {{pusch_mcs_table::qam64_tp, 15, false}, {4, 616.0F, 2.4063F}},
    {{pusch_mcs_table::qam64_tp, 16, false}, {4, 658.0F, 2.5703F}},
    {{pusch_mcs_table::qam64_tp, 17, false}, {6, 466.0F, 2.7305F}},
    {{pusch_mcs_table::qam64_tp, 18, false}, {6, 517.0F, 3.0293F}},
    {{pusch_mcs_table::qam64_tp, 19, false}, {6, 567.0F, 3.3223F}},
    {{pusch_mcs_table::qam64_tp, 20, false}, {6, 616.0F, 3.6094F}},
    {{pusch_mcs_table::qam64_tp, 21, false}, {6, 666.0F, 3.9023F}},
    {{pusch_mcs_table::qam64_tp, 22, false}, {6, 719.0F, 4.2129F}},
    {{pusch_mcs_table::qam64_tp, 23, false}, {6, 772.0F, 4.5234F}},
    {{pusch_mcs_table::qam64_tp, 24, false}, {6, 822.0F, 4.8164F}},
    {{pusch_mcs_table::qam64_tp, 25, false}, {6, 873.0F, 5.1152F}},
    {{pusch_mcs_table::qam64_tp, 26, false}, {6, 910.0F, 5.3320F}},
    {{pusch_mcs_table::qam64_tp, 27, false}, {6, 948.0F, 5.5547F}},
    {{pusch_mcs_table::qam64_tp, 28, false}, {2, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64_tp, 29, false}, {2, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64_tp, 30, false}, {4, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64_tp, 31, false}, {6, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64LowSe_tp, 0, false}, {1, 60.0F, 0.0586F}},
    {{pusch_mcs_table::qam64LowSe_tp, 1, false}, {1, 80.0F, 0.0781F}},
    {{pusch_mcs_table::qam64LowSe_tp, 2, false}, {1, 100.0F, 0.0977F}},
    {{pusch_mcs_table::qam64LowSe_tp, 3, false}, {1, 128.0F, 0.1250F}},
    {{pusch_mcs_table::qam64LowSe_tp, 4, false}, {1, 156.0F, 0.1523F}},
    {{pusch_mcs_table::qam64LowSe_tp, 5, false}, {1, 198.0F, 0.1934F}},
    {{pusch_mcs_table::qam64LowSe_tp, 6, false}, {2, 120.0f, 0.2344F}},
    {{pusch_mcs_table::qam64LowSe_tp, 7, false}, {2, 157.0f, 0.3066F}},
    {{pusch_mcs_table::qam64LowSe_tp, 8, false}, {2, 193.0f, 0.3770F}},
    {{pusch_mcs_table::qam64LowSe_tp, 9, false}, {2, 251.0f, 0.4902F}},
    {{pusch_mcs_table::qam64LowSe_tp, 10, false}, {2, 308.0f, 0.6016F}},
    {{pusch_mcs_table::qam64LowSe_tp, 11, false}, {2, 379.0f, 0.7402F}},
    {{pusch_mcs_table::qam64LowSe_tp, 12, false}, {2, 449.0f, 0.8770F}},
    {{pusch_mcs_table::qam64LowSe_tp, 13, false}, {2, 526.0f, 1.0273F}},
    {{pusch_mcs_table::qam64LowSe_tp, 14, false}, {2, 602.0f, 1.1758F}},
    {{pusch_mcs_table::qam64LowSe_tp, 15, false}, {2, 679.0f, 1.3262F}},
    {{pusch_mcs_table::qam64LowSe_tp, 16, false}, {4, 378.0f, 1.4766F}},
    {{pusch_mcs_table::qam64LowSe_tp, 17, false}, {4, 434.0f, 1.6953F}},
    {{pusch_mcs_table::qam64LowSe_tp, 18, false}, {4, 490.0f, 1.9141F}},
    {{pusch_mcs_table::qam64LowSe_tp, 19, false}, {4, 553.0f, 2.1602F}},
    {{pusch_mcs_table::qam64LowSe_tp, 20, false}, {4, 616.0f, 2.4063F}},
    {{pusch_mcs_table::qam64LowSe_tp, 21, false}, {4, 658.0f, 2.5703F}},
    {{pusch_mcs_table::qam64LowSe_tp, 22, false}, {4, 699.0f, 2.7305F}},
    {{pusch_mcs_table::qam64LowSe_tp, 23, false}, {4, 772.0f, 3.0156F}},
    {{pusch_mcs_table::qam64LowSe_tp, 24, false}, {6, 567.0f, 3.3223F}},
    {{pusch_mcs_table::qam64LowSe_tp, 25, false}, {6, 616.0f, 3.6094F}},
    {{pusch_mcs_table::qam64LowSe_tp, 26, false}, {6, 666.0f, 3.9023F}},
    {{pusch_mcs_table::qam64LowSe_tp, 27, false}, {6, 772.0f, 4.5234F}},
    {{pusch_mcs_table::qam64LowSe_tp, 28, false}, {2, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64LowSe_tp, 29, false}, {2, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64LowSe_tp, 30, false}, {4, 0.0F, 0.0F}},
    {{pusch_mcs_table::qam64LowSe_tp, 31, false}, {6, 0.0F, 0.0F}},
};

class pusch_mcs_table_test : public ::testing::TestWithParam<pusch_mcs_param>
{
protected:
  static std::tuple<unsigned, float, float> get_expected()
  {
    // Get parameter.
    pusch_mcs_param param = GetParam();

    // Table ignores pi2_bpsk_present flag. It indexes with false always.
    bool pi2_bpsk_present = std::get<2>(param);
    std::get<2>(param)    = false;

    // Return the expected value as pi2_bpsk_present is present.
    std::tuple<unsigned, float, float> expected = pusch_mcs_expected.at(param);

    // Correct for pi2_bpsk_present.
    if (!pi2_bpsk_present && std::get<0>(expected) == 1) {
      std::get<0>(expected) = 2;
      std::get<1>(expected) /= 2;
    }

    return expected;
  }
};

TEST_P(pusch_mcs_table_test, check_combinations)
{
  // Get combined parameter.
  pusch_mcs_param params = GetParam();

  // Extract parameters.
  pusch_mcs_table table            = std::get<0>(params);
  sch_mcs_index   mcs              = std::get<1>(params);
  bool            pi2_bpsk_enabled = std::get<1>(params);

  // Get number of cyclic shifts.
  sch_mcs_description descr = pusch_mcs_get_config(table, mcs, pi2_bpsk_enabled);

  std::tuple<unsigned, float, float> expected = get_expected();

  // Compare with expected.
  ASSERT_EQ(std::get<0>(expected), get_bits_per_symbol(descr.modulation));
  ASSERT_EQ(std::get<1>(expected), descr.target_code_rate);
  ASSERT_NEAR(std::get<2>(expected), descr.get_spectral_efficiency(), 1e-4);
  ASSERT_NEAR(std::get<0>(expected) * std::get<1>(expected) / 1024.0F, std::get<2>(expected), 1e-4);
}

INSTANTIATE_TEST_SUITE_P(
    pusch_mcs_table,
    pusch_mcs_table_test,
    ::testing::Combine(::testing::Values(pusch_mcs_table::qam64, pusch_mcs_table::qam256, pusch_mcs_table::qam64LowSe),
                       ::testing::Range(0U, 32U),
                       ::testing::Values(false, true)));
