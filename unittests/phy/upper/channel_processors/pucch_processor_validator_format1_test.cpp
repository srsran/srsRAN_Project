/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/equalization/equalization_factories.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsgnb;

namespace {

// Maximum channel dimensions used to construct the PUCCH processor.
static channel_estimate::channel_estimate_dimensions max_dimensions = {MAX_RB,
                                                                       MAX_NSYMB_PER_SLOT - 1,
                                                                       1,
                                                                       PUCCH_MAX_LAYERS};
// Maximum NID value for PUCCH Format 1.
static constexpr unsigned PUCCH_F1_MAX_NID = 1023;
// Maximum time domain OCC index for PUCCH Format 1.
static constexpr unsigned PUCCH_F1_MAX_OCC_IDX = 6;
// Maximum initial cyclic shift for PUCCH Format 1.
static constexpr unsigned PUCCH_F1_MAX_CYCLIC_SHIFT = 11;

const pucch_processor::format1_configuration base_format_1_config = {
    // Slot.
    {0, 9},
    // BWP size.
    50,
    // BWP start.
    10,
    // CP.
    cyclic_prefix::NORMAL,
    // Starting PRB.
    1,
    // Second hop PRB.
    {},
    // N_ID
    PUCCH_F1_MAX_NID,
    // Number of HARQ-ACK bits.
    PUCCH_FORMAT1_MAX_HARQ_ACK,
    // Rx Ports.
    {0},
    // Initial cyclic shift.
    PUCCH_F1_MAX_CYCLIC_SHIFT,
    // Number of OFDM symbols.
    PUCCH_FORMAT1_MIN_NSYMB,
    // Start symbol index.
    3,
    // Time domain OCC.
    0};

// Test case parameters structure.
struct test_params {
  pucch_processor::format1_configuration config;
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

// Test cases are implemented as lambda functions that generate and return an invalid PUCCH Format 1 configuration,
// along with the expected assert message.
const std::vector<test_case_t> pucch_processor_validator_test_data = {
    {
        [] {
          test_params entry = {};
          entry.config      = base_format_1_config;
          entry.config.cp   = cyclic_prefix::EXTENDED;
          set_slot_numerology(entry.config.slot, 0);
          entry.assert_message = R"(Extended CP is only supported for 60 kHz SCS\.)";
          return entry;
        },
    },
    {
        [] {
          test_params entry        = {};
          entry.config             = base_format_1_config;
          entry.config.bwp_size_rb = 0;
          entry.assert_message     = fmt::format(R"(BWP size cannot be zero\.)");
          return entry;
        },
    },
    {
        [] {
          test_params entry         = {};
          entry.config              = base_format_1_config;
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
          entry.config              = base_format_1_config;
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
          entry.config                = base_format_1_config;
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
          entry.config                    = base_format_1_config;
          entry.config.slot               = slot_point(2, 0);
          entry.config.cp                 = cyclic_prefix::EXTENDED;
          entry.config.nof_symbols        = PUCCH_FORMAT1_MIN_NSYMB;
          entry.config.start_symbol_index = get_nsymb_per_slot(entry.config.cp) - entry.config.nof_symbols + 1;
          entry.assert_message            = fmt::format(
              R"(OFDM symbol allocation goes up to symbol {}\, exceeding the number of symbols in the slot given {} CP\, i\.e\.\, {}\.)",
              entry.config.start_symbol_index + entry.config.nof_symbols,
              entry.config.cp.to_string(),
              get_nsymb_per_slot(entry.config.cp));
          return entry;
        },
    },
    {
        [] {
          test_params entry               = {};
          entry.config                    = base_format_1_config;
          entry.config.cp                 = cyclic_prefix::NORMAL;
          entry.config.nof_symbols        = PUCCH_FORMAT1_MIN_NSYMB;
          entry.config.start_symbol_index = max_dimensions.nof_symbols - PUCCH_FORMAT1_MIN_NSYMB + 1;
          entry.assert_message            = fmt::format(
              R"(OFDM symbol allocation goes up to symbol {}\, exceeding the configured maximum number of slot symbols\, i\.e\.\, {}\.)",
              entry.config.start_symbol_index + entry.config.nof_symbols,
              max_dimensions.nof_symbols);
          return entry;
        },
    },
    {
        [] {
          test_params entry               = {};
          entry.config                    = base_format_1_config;
          entry.config.start_symbol_index = 0;
          entry.config.nof_symbols        = PUCCH_FORMAT1_MAX_NSYMB + 1;
          entry.assert_message            = fmt::format(
              R"(Invalid number of OFDM Symbols\, i\.e\.\, {}\. PUCCH Format 1 occupies {} to {} symbols\.)",
              entry.config.nof_symbols,
              PUCCH_FORMAT1_MIN_NSYMB,
              PUCCH_FORMAT1_MAX_NSYMB);
          return entry;
        },
    },
    {
        [] {
          test_params entry               = {};
          entry.config                    = base_format_1_config;
          entry.config.start_symbol_index = 0;
          entry.config.nof_symbols        = PUCCH_FORMAT1_MIN_NSYMB - 1;
          entry.assert_message            = fmt::format(
              R"(Invalid number of OFDM Symbols\, i\.e\.\, {}\. PUCCH Format 1 occupies {} to {} symbols\.)",
              entry.config.nof_symbols,
              PUCCH_FORMAT1_MIN_NSYMB,
              PUCCH_FORMAT1_MAX_NSYMB);
          return entry;
        },
    },
    {
        [] {
          test_params entry    = {};
          entry.config         = base_format_1_config;
          entry.config.ports   = {};
          entry.assert_message = fmt::format(R"(number of receive ports cannot be zero\.)");
          return entry;
        },
    },
    {
        [] {
          test_params entry    = {};
          entry.config         = base_format_1_config;
          entry.config.ports   = {0, 1};
          entry.assert_message = fmt::format(
              R"(Number of receive ports\, i\.e\. {}\, exceeds the configured maximum number of receive ports\, i\.e\.\, {}\.)",
              entry.config.ports.size(),
              max_dimensions.nof_rx_ports);
          return entry;
        },
    },
    {
        [] {
          test_params entry            = {};
          entry.config                 = base_format_1_config;
          entry.config.time_domain_occ = PUCCH_F1_MAX_OCC_IDX + 1;
          entry.assert_message =
              fmt::format(R"(The time domain OCC index\, i\.e\.\, {}\, exceeds its maximum value of {}\.)",
                          entry.config.time_domain_occ,
                          PUCCH_F1_MAX_OCC_IDX);
          return entry;
        },
    },
    {
        [] {
          test_params entry                 = {};
          entry.config                      = base_format_1_config;
          entry.config.initial_cyclic_shift = PUCCH_F1_MAX_CYCLIC_SHIFT + 1;
          entry.assert_message =
              fmt::format(R"(The initial cyclic shift\, i\.e\.\, {}\, exceeds its maximum value of {}\.)",
                          entry.config.initial_cyclic_shift,
                          PUCCH_F1_MAX_CYCLIC_SHIFT);
          return entry;
        },
    },
    {
        [] {
          test_params entry    = {};
          entry.config         = base_format_1_config;
          entry.config.n_id    = PUCCH_F1_MAX_NID + 1;
          entry.assert_message = fmt::format(R"(The NID value\, i\.e\.\, {}\, exceeds its maximum value of {}\.)",
                                             entry.config.n_id,
                                             PUCCH_F1_MAX_NID);
          return entry;
        },
    },
    {
        [] {
          test_params entry         = {};
          entry.config              = base_format_1_config;
          entry.config.nof_harq_ack = PUCCH_FORMAT1_MAX_HARQ_ACK + 1;
          entry.assert_message      = fmt::format(
              R"(The number of requested HARQ-ACK, i\.e\.\, {}\, exceeds the PUCCH Format 1 number of HARQ-ACK\, i\.e\.\, {}\.)",
              entry.config.nof_harq_ack,
              PUCCH_FORMAT1_MAX_HARQ_ACK);
          return entry;
        },
    },
};

class PucchProcessorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static std::unique_ptr<pucch_processor>     pucch_proc;
  static std::unique_ptr<pucch_pdu_validator> pucch_validator;

  static void SetUpTestSuite()
  {
    if (!(pucch_proc && pucch_validator)) {
      // Create factories required by the PUCCH demodulator factory.
      std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_factory_zf();
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

      // Create channel estimator factory.
      std::shared_ptr<dmrs_pucch_estimator_factory> estimator_factory =
          create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory);
      ASSERT_NE(estimator_factory, nullptr) << "Cannot create DM-RS PUCCH estimator factory.";

      // Create PUCCH detector factory.
      std::shared_ptr<pucch_detector_factory> detector_factory =
          create_pucch_detector_factory_sw(lpc_factory, prg_factory);
      ASSERT_NE(detector_factory, nullptr) << "Cannot create PUCCH detector factory.";

      // Create UCI decoder factory.
      std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
      ASSERT_NE(short_block_det_factory, nullptr) << "Cannot create short block detector factory.";

      uci_decoder_factory_sw_configuration decoder_factory_config = {};
      decoder_factory_config.decoder_factory                      = short_block_det_factory;

      std::shared_ptr<uci_decoder_factory> decoder_factory = create_uci_decoder_factory_sw(decoder_factory_config);
      ASSERT_NE(decoder_factory, nullptr) << "Cannot create UCI decoder factory.";

      // Create PUCCH processor factory.
      std::shared_ptr<pucch_processor_factory> processor_factory = create_pucch_processor_factory_sw(
          estimator_factory, detector_factory, pucch_demod_factory, decoder_factory, max_dimensions);
      ASSERT_NE(processor_factory, nullptr) << "Cannot create PUCCH processor factory.";

      // Create PUCCH processor.
      pucch_proc = processor_factory->create();
      ASSERT_NE(pucch_proc, nullptr) << "Cannot create PUCCH processor.";

      // Create PUCCH processor validator.
      pucch_validator = processor_factory->create_validator();
      ASSERT_NE(pucch_validator, nullptr) << "Cannot create PUCCH processor validator.";
    }
  }
};

std::unique_ptr<pucch_processor>     PucchProcessorFixture::pucch_proc;
std::unique_ptr<pucch_pdu_validator> PucchProcessorFixture::pucch_validator;

TEST_P(PucchProcessorFixture, PucchProcessorValidatortest)
{
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
                         PucchProcessorFixture,
                         ::testing::ValuesIn(pucch_processor_validator_test_data));

} // namespace
