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

#include "../../../support/resource_grid_test_doubles.h"
#include "dmrs_pucch_estimator_test_data.h"
#include "srsran/phy/upper/signal_processors/pucch/factories.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const dmrs_pucch_estimator::common_configuration& cc)
{
  fmt::print(os,
             "SCS {} kHz, intra-slot fr. hop. {}, port {}",
             15U * (1U << cc.slot.numerology()),
             (cc.second_hop_prb.has_value() ? "ON" : "OFF"),
             cc.ports[0]);
  return os;
}

std::ostream& operator<<(std::ostream& os, const dmrs_pucch_estimator::format2_configuration& config)
{
  fmt::print(os,
             "PUCCH Format 2 {}, PRBs {}, nid0 {}",
             static_cast<const dmrs_pucch_estimator::common_configuration&>(config),
             config.nof_prb,
             config.n_id_0);
  return os;
}

std::ostream& operator<<(std::ostream& os, const dmrs_pucch_estimator::format3_configuration& config)
{
  fmt::print(os,
             "PUCCH Format 3 {}, PRBs {}, +DMRS {}",
             static_cast<const dmrs_pucch_estimator::common_configuration&>(config),
             config.nof_prb,
             (config.additional_dmrs ? "ON" : "OFF"));
  return os;
}

std::ostream& operator<<(std::ostream& os, const dmrs_pucch_estimator::format4_configuration& config)
{
  fmt::print(os,
             "PUCCH Format 4 {}, +DMRS {}, OCC {}",
             static_cast<const dmrs_pucch_estimator::common_configuration&>(config),
             (config.additional_dmrs ? "ON" : "OFF"),
             config.occ_index);
  return os;
}

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  std::visit([&os](const auto& config) { fmt::print(os, "{}", config); }, test_case.config);
  return os;
}

} // namespace srsran

template <>
struct fmt::formatter<srsran::dmrs_pucch_estimator::common_configuration> : ostream_formatter {};
template <>
struct fmt::formatter<srsran::dmrs_pucch_estimator::format2_configuration> : ostream_formatter {};
template <>
struct fmt::formatter<srsran::dmrs_pucch_estimator::format3_configuration> : ostream_formatter {};
template <>
struct fmt::formatter<srsran::dmrs_pucch_estimator::format4_configuration> : ostream_formatter {};

namespace {

class DmrsPucchEstimatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  // The factory only needs to be created once.
  static void SetUpTestSuite()
  {
    if (!estimator_factory) {
      // Create pseudo-random sequence generator factory.
      std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
      ASSERT_NE(prg_factory, nullptr) << "Cannot create pseudorandom generator factory.";

      // Create low PAPR sequence generator factory.
      std::shared_ptr<low_papr_sequence_generator_factory> lpg_factory =
          create_low_papr_sequence_generator_sw_factory();
      ASSERT_NE(lpg_factory, nullptr) << "Cannot create Low-PAPR sequence generator factory.";

      // Create low PAPR collection factory.
      std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory =
          create_low_papr_sequence_collection_sw_factory(lpg_factory);
      ASSERT_NE(lpc_factory, nullptr) << "Cannot create Low-PAPR sequence collection factory.";

      std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
      if (!dft_factory) {
        dft_factory = create_dft_processor_factory_generic();
      }
      ASSERT_NE(dft_factory, nullptr) << "Cannot create DFT factory.";

      std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory =
          create_time_alignment_estimator_dft_factory(dft_factory);
      ASSERT_NE(ta_estimator_factory, nullptr) << "Cannot create TA estimator factory.";

      // Create channel estimator factory.
      std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
          create_port_channel_estimator_factory_sw(ta_estimator_factory);
      ASSERT_NE(port_chan_estimator_factory, nullptr) << "Cannot create port channel estimator factory.";

      // Create DM-RS for PUCCH estimator factory.
      estimator_factory =
          create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory, lpg_factory, port_chan_estimator_factory);
      ASSERT_NE(estimator_factory, nullptr) << "Cannot create PUCCH estimator factory.";
    }
  }

  void SetUp() override
  {
    // Check the factory again, since ASSERT_* is not fatal in SetUpTestSuite in old googletest releases.
    ASSERT_NE(estimator_factory, nullptr) << "Cannot create PUCCH estimator factory.";

    dmrs_pucch = estimator_factory->create();
    ASSERT_NE(dmrs_pucch, nullptr) << "Cannot create PUCCH estimator.";
  }

  static std::shared_ptr<dmrs_pucch_estimator_factory> estimator_factory;
  std::unique_ptr<dmrs_pucch_estimator>                dmrs_pucch = nullptr;
};

std::shared_ptr<dmrs_pucch_estimator_factory> DmrsPucchEstimatorFixture::estimator_factory = nullptr;

TEST_P(DmrsPucchEstimatorFixture, DmrsPucchEstimatorTest)
{
  // Fixed BW
  const unsigned nof_prb = 52;

  const test_case_t& test_case = GetParam();

  // Load Matlab generated symbols.
  const std::vector<resource_grid_reader_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

  // Create resource grid
  resource_grid_reader_spy grid(MAX_PORTS, MAX_NSYMB_PER_SLOT, MAX_NOF_PRBS);
  grid.write(testvector_symbols);

  std::visit(
      [this, &grid](const auto& config) {
        // Object to store channel estimation results
        channel_estimate::channel_estimate_dimensions ch_est_dims = {};
        ch_est_dims.nof_prb                                       = nof_prb;
        ch_est_dims.nof_symbols                                   = get_nsymb_per_slot(config.cp);
        ch_est_dims.nof_tx_layers                                 = 1;
        ch_est_dims.nof_rx_ports                                  = 1;
        channel_estimate estimate(ch_est_dims);

        dmrs_pucch->estimate(estimate, grid, config);

        ASSERT_NEAR(estimate.get_noise_variance(0), 0, 1e-2) << "Expected an ideal channel!";
      },
      test_case.config);
}

INSTANTIATE_TEST_SUITE_P(DmrsPucchEstimatorSuite,
                         DmrsPucchEstimatorFixture,
                         ::testing::ValuesIn(dmrs_pucch_estimator_test_data));
} // namespace
