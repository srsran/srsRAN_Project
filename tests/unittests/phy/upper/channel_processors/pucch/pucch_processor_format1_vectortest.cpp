/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "pucch_processor_format1_test_data.h"
#include "pucch_processor_test_fixture.h"
#include "srsran/phy/upper/channel_processors/pucch/formatters.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor_result.h"
#include "srsran/phy/upper/channel_processors/uci/uci_status.h"
#include "srsran/phy/upper/channel_state_information.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "fmt/format.h"
#include <cstdint>
#include <gtest/gtest.h>

using namespace srsran;

using PucchProcessorFormat1Param   = test_case_t;
using PucchProcessorFormat1Fixture = PucchProcessorTestFixture<PucchProcessorFormat1Param>;
template <>
std::unique_ptr<pucch_processor> PucchProcessorFormat1Fixture::processor = nullptr;
template <>
std::unique_ptr<pucch_pdu_validator> PucchProcessorFormat1Fixture::validator = nullptr;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& tc)
{
  os << fmt::format("{:s} n_ports={} scs={} tx_mode={} n_mux_pucch={}",
                    tc.common_config,
                    tc.common_config.ports.size(),
                    to_string(to_subcarrier_spacing(tc.common_config.slot.numerology())),
                    tc.tx_mode,
                    tc.payloads.size());
  return os;
}

std::ostream& operator<<(std::ostream& os, const uci_status& status)
{
  switch (status) {
    case uci_status::unknown:
      return os << "unknown";
    case uci_status::valid:
      return os << "valid";
    case uci_status::invalid:
    default:
      return os << "invalid";
  }
}

std::ostream& operator<<(std::ostream& os, span<const uint8_t> data)
{
  return os << fmt::format("{}", data);
}

} // namespace srsran

namespace {

TEST_P(PucchProcessorFormat1Fixture, FromVector)
{
  // Prepare resource grid.
  resource_grid_reader_spy grid(0, 0, 0);
  grid.write(GetParam().grid.read());

  const PucchProcessorFormat1Param& param = GetParam();

  pucch_processor::format1_configuration tmp_conf = param.common_config;

  ASSERT_EQ(grid.get_nof_ports(), tmp_conf.ports.size())
      << fmt::format("The number of ports of the resource grid {} and the number of configured ports {} do not match.",
                     grid.get_nof_ports(),
                     tmp_conf.ports.size());
  pucch_processor::format1_batch_configuration batch_config(tmp_conf);
  batch_config.entries.clear();

  for (const auto& mux_pucch : param.payloads) {
    tmp_conf.initial_cyclic_shift = mux_pucch.initial_cyclic_shift;
    tmp_conf.time_domain_occ      = mux_pucch.time_domain_occ;
    tmp_conf.nof_harq_ack         = mux_pucch.nof_harq_ack;

    // Make sure configuration is valid.
    ASSERT_TRUE(validator->is_valid(tmp_conf));

    batch_config.entries.insert(
        tmp_conf.initial_cyclic_shift, tmp_conf.time_domain_occ, {std::nullopt, tmp_conf.nof_harq_ack});
  }

  const pucch_format1_map<pucch_processor_result>& results = processor->process(grid, batch_config);

  ASSERT_EQ(results.size(), batch_config.entries.size())
      << "The number of configured and processed PUCCHs does not match.";

  unsigned nof_false_alarms = 0;
  for (const auto& mux_pucch : param.payloads) {
    unsigned initial_cyclic_shift = mux_pucch.initial_cyclic_shift;
    unsigned time_domain_occ      = mux_pucch.time_domain_occ;

    ASSERT_TRUE(results.contains(initial_cyclic_shift, time_domain_occ)) << fmt::format(
        "PUCCH with ICS {} and OCCI {} is missing from the results.", initial_cyclic_shift, time_domain_occ);

    const pucch_processor_result& this_result = results.get(initial_cyclic_shift, time_domain_occ);

    std::optional<float> epre_dB = this_result.csi.get_epre_dB();
    ASSERT_TRUE(epre_dB.has_value()) << fmt::format(
        "PUCCH with ICS {} and OCCI {} has an invalid EPRE.", initial_cyclic_shift, time_domain_occ);
    ASSERT_NEAR(convert_dB_to_power(*epre_dB), param.epre, param.epre * 0.01)
        << fmt::format("PUCCH with ICS {} and OCCI {} has a wrong EPRE value.", initial_cyclic_shift, time_domain_occ);

    std::optional<float> rsrp_dB = this_result.csi.get_rsrp_dB();
    ASSERT_TRUE(rsrp_dB.has_value()) << fmt::format(
        "PUCCH with ICS {} and OCCI {} has an invalid RSRP.", initial_cyclic_shift, time_domain_occ);
    ASSERT_NEAR(convert_dB_to_power(*rsrp_dB), mux_pucch.rsrp, mux_pucch.rsrp * 0.01)
        << fmt::format("PUCCH with ICS {} and OCCI {} has a wrong RSRP value.", initial_cyclic_shift, time_domain_occ);

    ASSERT_TRUE(this_result.csi.get_sinr_dB().has_value())
        << fmt::format("PUCCH with ICS {} and OCCI {} has an invalid SNR.", initial_cyclic_shift, time_domain_occ);

    ASSERT_FALSE(this_result.csi.get_time_alignment()) << fmt::format(
        "PUCCH with ICS {} and OCCI {} has a non-null time alignment.", initial_cyclic_shift, time_domain_occ);

    ASSERT_TRUE(this_result.detection_metric.has_value()) << fmt::format(
        "PUCCH with ICS {} and OCCI {} has an invalid detection metric.", initial_cyclic_shift, time_domain_occ);
    EXPECT_NEAR(this_result.detection_metric.value(), mux_pucch.detection_metric, mux_pucch.detection_metric * 0.01)
        << fmt::format("PUCCH ({}, {}): the detection metric {} and the expected metric {} do not match.",
                       initial_cyclic_shift,
                       time_domain_occ,
                       this_result.detection_metric.value(),
                       mux_pucch.detection_metric);

    span<const uint8_t> expected_bits(mux_pucch.payload);
    span<const uint8_t> actual_bits = this_result.message.get_harq_ack_bits();
    if (expected_bits.empty()) {
      nof_false_alarms += (this_result.message.get_status() == uci_status::valid);
    } else {
      ASSERT_EQ(this_result.message.get_status(), uci_status::valid)
          << fmt::format("PUCCH ({}, {}) should be valid, actual status is {}",
                         initial_cyclic_shift,
                         time_domain_occ,
                         fmt::underlying(this_result.message.get_status()));
      ASSERT_EQ(this_result.message.get_csi_part1_bits().size(), 0) << "There should not be CSI Part 1.";
      ASSERT_EQ(this_result.message.get_csi_part2_bits().size(), 0) << "There should not be CSI Part 2.";
      if (param.tx_mode == "ACK") {
        ASSERT_EQ(actual_bits.size(), expected_bits.size())
            << fmt::format("PUCCH ({}, {}): expected {} bits, received {}.",
                           initial_cyclic_shift,
                           time_domain_occ,
                           expected_bits.size(),
                           actual_bits.size());
        ASSERT_EQ(actual_bits, expected_bits);
      } else {
        ASSERT_EQ(actual_bits.size(), 0) << fmt::format("SR PUCCH ({}, {}): expected 0 bits, received {}.",
                                                        initial_cyclic_shift,
                                                        time_domain_occ,
                                                        actual_bits.size());
      }
    }
  }
  ASSERT_LE(nof_false_alarms, 1) << "Too many false alarms.";
}

INSTANTIATE_TEST_SUITE_P(PucchProcessorFormat1,
                         PucchProcessorFormat1Fixture,
                         ::testing::ValuesIn(pucch_processor_format1_test_data));

} // namespace
