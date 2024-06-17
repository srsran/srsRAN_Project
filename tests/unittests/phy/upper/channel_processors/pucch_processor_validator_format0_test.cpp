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
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsran;

namespace {

// Maximum channel dimensions used to construct the PUCCH processor. It is irrelevant for Format 0.
static channel_estimate::channel_estimate_dimensions max_dimensions = {1, 1, 1, 1};

// Valid PUCCH Format 0 configuration.
const pucch_processor::format0_configuration base_format_0_config = {
    // Context.
    std::nullopt,
    // Slot.
    {0, 9},
    // CP.
    cyclic_prefix::NORMAL,
    // BWP size.
    50,
    // BWP start.
    10,
    // Starting PRB.
    1,
    // Second hop PRB.
    {},
    // Start symbol index.
    3,
    // Number of OFDM symbols.
    1,
    // Initial cyclic shift.
    0,
    // N_ID
    0,
    // Number of HARQ-ACK bits.
    1,
    // SR opportunity.
    false,
    // Rx Ports.
    {0},
};

// Test case parameters structure.
struct test_params {
  pucch_processor::format0_configuration config;
  std::string                            assert_message;
};

struct test_case_t {
  std::function<test_params()> get_test_params;
};

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.get_test_params().config);
  return os;
}

// Test cases are implemented as lambda functions that generate and return an invalid PUCCH Format 0 configuration,
// along with the expected assert message.
const std::vector<test_case_t> pucch_processor_validator_test_data = {
    {
        [] {
          test_params entry         = {};
          entry.config              = base_format_0_config;
          entry.config.bwp_start_rb = 10;
          entry.config.bwp_size_rb  = MAX_RB - entry.config.bwp_start_rb + 1;
          entry.assert_message      = fmt::format(
              R"(BWP allocation goes up to PRB {}\, exceeding the configured maximum grid RB size\, i\.e\.\, {}\.)",
              entry.config.bwp_start_rb + entry.config.bwp_size_rb,
              MAX_RB);
          return entry;
        },
    },
    {
        [] {
          test_params entry         = {};
          entry.config              = base_format_0_config;
          entry.config.starting_prb = entry.config.bwp_size_rb;
          entry.assert_message =
              fmt::format(R"(PRB allocation within the BWP goes up to PRB {}\, exceeding BWP size\, i\.e\.\, {}\.)",
                          entry.config.starting_prb + 1,
                          entry.config.bwp_size_rb);
          return entry;
        },
    },
    {
        [] {
          test_params entry           = {};
          entry.config                = base_format_0_config;
          entry.config.second_hop_prb = entry.config.bwp_size_rb;
          entry.assert_message        = fmt::format(
              R"(Second hop PRB allocation within the BWP goes up to PRB {}\, exceeding BWP size\, i\.e\.\, {}\.)",
              entry.config.second_hop_prb.value() + 1,
              entry.config.bwp_size_rb);
          return entry;
        },
    },
    {
        [] {
          test_params entry               = {};
          entry.config                    = base_format_0_config;
          entry.config.slot               = slot_point(2, 0);
          entry.config.cp                 = cyclic_prefix::EXTENDED;
          entry.config.nof_symbols        = pucch_constants::format0_nof_symbols_range.start();
          entry.config.start_symbol_index = get_nsymb_per_slot(entry.config.cp) - entry.config.nof_symbols + 1;
          entry.assert_message            = fmt::format(
              R"(OFDM symbol allocation goes up to symbol {}\, exceeding the number of symbols in the given slot with {} CP\, i\.e\.\, {}\.)",
              entry.config.start_symbol_index + entry.config.nof_symbols,
              entry.config.cp.to_string(),
              get_nsymb_per_slot(entry.config.cp));
          return entry;
        },
    },
    {
        [] {
          test_params entry        = {};
          entry.config             = base_format_0_config;
          entry.config.nof_symbols = pucch_constants::format0_nof_symbols_range.stop() + 1;
          entry.assert_message     = R"(Number of symbols \(i\.e\., 3\) is out of the range \[1\.\.2\]\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry                 = {};
          entry.config                      = base_format_0_config;
          entry.config.initial_cyclic_shift = pucch_constants::format0_initial_cyclic_shift_range.stop() + 1;
          entry.assert_message = R"(The initial cyclic shift \(i\.e\., 13\) is out of the range \[0\.\.12\)\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry = {};
          entry.config      = base_format_0_config;
          entry.config.n_id = pucch_constants::n_id_range.stop() + 1;
          entry.assert_message =
              R"(The sequence hopping identifier \(i\.e\., 1025\) is out of the range \[0\.\.1024\)\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry         = {};
          entry.config              = base_format_0_config;
          entry.config.nof_harq_ack = pucch_constants::format0_nof_harq_ack_range.stop() + 1;
          entry.assert_message      = R"(The number of HARQ-ACK bits \(i\.e\., 3\) is out of the range \[0\.\.2\]\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry           = {};
          entry.config                = base_format_0_config;
          entry.config.nof_harq_ack   = 0;
          entry.config.sr_opportunity = false;
          entry.assert_message        = R"(No payload\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry    = {};
          entry.config         = base_format_0_config;
          entry.config.ports   = {};
          entry.assert_message = R"(The number of receive ports cannot be zero\.)";
          return entry;
        },
    },
};

class PucchProcessorFormat0Fixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static std::unique_ptr<pucch_processor>     pucch_proc;
  static std::unique_ptr<pucch_pdu_validator> pucch_validator;

  static void SetUpTestSuite()
  {
    if (!(pucch_proc && pucch_validator)) {
      // Create factories required by the PUCCH demodulator factory.
      std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_generic_factory();
      ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory.";

      std::shared_ptr<channel_modulation_factory> demod_factory = create_channel_modulation_sw_factory();
      ASSERT_NE(demod_factory, nullptr) << "Cannot create channel modulation factory.";

      std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
      ASSERT_NE(prg_factory, nullptr) << "Cannot create pseudo-random generator factory.";

      // Create PUCCH demodulator factory.
      std::shared_ptr<pucch_demodulator_factory> pucch_demod_factory =
          create_pucch_demodulator_factory_sw(equalizer_factory, demod_factory, prg_factory);
      ASSERT_NE(pucch_demod_factory, nullptr) << "Cannot create PUCCH demodulator factory.";

      // Create factories required by the PUCCH channel estimator factory.
      std::shared_ptr<low_papr_sequence_generator_factory> lpg_factory =
          create_low_papr_sequence_generator_sw_factory();
      ASSERT_NE(lpg_factory, nullptr) << "Cannot create low PAPR sequence generator factory.";

      std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory =
          create_low_papr_sequence_collection_sw_factory(lpg_factory);
      ASSERT_NE(lpc_factory, nullptr) << "Cannot create low PAPR sequence collection factory.";

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

      std::shared_ptr<dmrs_pucch_estimator_factory> estimator_factory =
          create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory, port_chan_estimator_factory);
      ASSERT_NE(estimator_factory, nullptr) << "Cannot create DM-RS PUCCH estimator factory.";

      // Create PUCCH detector factory.
      std::shared_ptr<pucch_detector_factory> detector_factory =
          create_pucch_detector_factory_sw(lpc_factory, prg_factory, equalizer_factory);
      ASSERT_NE(detector_factory, nullptr) << "Cannot create PUCCH detector factory.";

      // Create short block detector factory.
      std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
      ASSERT_NE(short_block_det_factory, nullptr) << "Cannot create short block detector factory.";

      // Create polar decoder factory.
      std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
      ASSERT_NE(polar_dec_factory, nullptr) << "Invalid polar decoder factory.";

      // Create CRC calculator factory.
      std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
      ASSERT_NE(crc_calc_factory, nullptr) << "Invalid CRC calculator factory.";

      // Create UCI decoder factory.
      std::shared_ptr<uci_decoder_factory> uci_dec_factory =
          create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
      ASSERT_NE(uci_dec_factory, nullptr) << "Cannot create UCI decoder factory.";

      // Create PUCCH processor factory.
      std::shared_ptr<pucch_processor_factory> processor_factory = create_pucch_processor_factory_sw(
          estimator_factory, detector_factory, pucch_demod_factory, uci_dec_factory, max_dimensions);
      ASSERT_NE(processor_factory, nullptr) << "Cannot create PUCCH processor factory.";

      // Create PUCCH processor.
      pucch_proc = processor_factory->create();
      ASSERT_NE(pucch_proc, nullptr) << "Cannot create PUCCH processor.";

      // Create PUCCH processor validator.
      pucch_validator = processor_factory->create_validator();
      ASSERT_NE(pucch_validator, nullptr) << "Cannot create PUCCH validator.";
    }
  }
};

std::unique_ptr<pucch_processor>     PucchProcessorFormat0Fixture::pucch_proc;
std::unique_ptr<pucch_pdu_validator> PucchProcessorFormat0Fixture::pucch_validator;

TEST_P(PucchProcessorFormat0Fixture, PucchProcessorValidatortest)
{
  ASSERT_NE(pucch_proc, nullptr) << "PUCCH processor not created.";
  ASSERT_NE(pucch_validator, nullptr) << "PUCCH validator not created.";

  const test_case_t& param = GetParam();

  // Make sure the configuration is invalid.
  ASSERT_FALSE(pucch_validator->is_valid(param.get_test_params().config));

  // Prepare resource grid.
  resource_grid_reader_spy grid;

  // Process PUCCH PDU.
#ifdef ASSERTS_ENABLED
  ASSERT_DEATH({ pucch_proc->process(grid, param.get_test_params().config); }, param.get_test_params().assert_message);
#endif // ASSERTS_ENABLED
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PucchProcessorValidatortest,
                         PucchProcessorFormat0Fixture,
                         ::testing::ValuesIn(pucch_processor_validator_test_data));

} // namespace
