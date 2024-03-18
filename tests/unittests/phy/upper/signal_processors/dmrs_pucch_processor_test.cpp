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

#include "../../support/resource_grid_test_doubles.h"
#include "dmrs_pucch_processor_test_data.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  const dmrs_pucch_processor::config_t& cc = test_case.config;
  fmt::print(os,
             "PUCCH Format {}, SCS {} kHz, intra-slot fr. hop. {}, shift {}, occ {}, +DM-RS {}, port {}",
             cc.format,
             15U * (1U << cc.slot.numerology()),
             (cc.intra_slot_hopping ? "ON" : "OFF"),
             cc.initial_cyclic_shift,
             cc.time_domain_occ,
             (cc.additional_dmrs ? "ON" : "OFF"),
             cc.ports[0]);
  return os;
}
} // namespace srsran

namespace {

class DmrsPucchProcessorFixture : public ::testing::TestWithParam<test_case_t>
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
      estimator_factory = create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory, port_chan_estimator_factory);
      ASSERT_NE(estimator_factory, nullptr) << "Cannot create PUCCH estimator factory.";
    }
  }

  void SetUp() override
  {
    // Check the factory again, since ASSERT_* is not fatal in SetUpTestSuite in old googletest releases.
    ASSERT_NE(estimator_factory, nullptr) << "Cannot create PUCCH estimator factory.";

    const test_case_t& test_case = GetParam();
    // Create DMRS-PUCCH processor.
    switch (test_case.config.format) {
      case pucch_format::FORMAT_1:
        dmrs_pucch = estimator_factory->create_format1();
        break;
      case pucch_format::FORMAT_2:
        dmrs_pucch = estimator_factory->create_format2();
      case pucch_format::FORMAT_0:
      case pucch_format::FORMAT_3:
      case pucch_format::FORMAT_4:
      case pucch_format::NOF_FORMATS:
      default:
        break;
    }
    ASSERT_NE(dmrs_pucch, nullptr) << "Cannot create PUCCH processor.";
  }

  static std::shared_ptr<dmrs_pucch_estimator_factory> estimator_factory;
  std::unique_ptr<dmrs_pucch_processor>                dmrs_pucch = nullptr;
};

std::shared_ptr<dmrs_pucch_estimator_factory> DmrsPucchProcessorFixture::estimator_factory = nullptr;

TEST_P(DmrsPucchProcessorFixture, DmrsPucchProcessorTest)
{
  // Fixed BW
  const unsigned nof_prb = 52;

  const test_case_t& test_case = GetParam();

  // Load Matlab generated symbols.
  const std::vector<resource_grid_reader_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

  // Create resource grid
  resource_grid_reader_spy grid;
  grid.write(testvector_symbols);

  // Object to store channel estimation results
  channel_estimate::channel_estimate_dimensions ch_est_dims = {};
  ch_est_dims.nof_prb                                       = nof_prb;
  ch_est_dims.nof_symbols                                   = get_nsymb_per_slot(test_case.config.cp);
  ch_est_dims.nof_tx_layers                                 = 1;
  ch_est_dims.nof_rx_ports                                  = 1;
  channel_estimate estimate(ch_est_dims);

  // Estimate channel
  dmrs_pucch->estimate(estimate, grid, test_case.config);

  ASSERT_NEAR(estimate.get_noise_variance(0), 0, 1e-2) << "Expected an ideal channel!";
}

INSTANTIATE_TEST_SUITE_P(DmrsPucchProcessorSuite,
                         DmrsPucchProcessorFixture,
                         ::testing::ValuesIn(dmrs_pucch_processor_test_data));
} // namespace