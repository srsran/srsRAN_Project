/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dl_processor_test_data.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_processor.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdcch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsgnb/srsvec/bit.h"

using namespace srsgnb;

static void process_test_case_pdsch(const test_case_t& test_case, pdsch_processor& pdsch)
{
  for (const pdsch_transmission& pdsch_data : test_case.pdsch) {
    // Prepare PDSCH resource grid entries.
    std::vector<rg_entry> pdsch_rg_entries;
    {
      std::vector<rg_entry> pdsch_data_symbols = pdsch_data.data_symbols.read();
      std::vector<rg_entry> pdsch_dmrs_symbols = pdsch_data.dmrs_symbols.read();
      pdsch_rg_entries.insert(pdsch_rg_entries.end(), pdsch_dmrs_symbols.begin(), pdsch_dmrs_symbols.end());
      pdsch_rg_entries.insert(pdsch_rg_entries.end(), pdsch_data_symbols.begin(), pdsch_data_symbols.end());
    }

    // Prepare PDSCH transport block.
    std::vector<uint8_t> transport_block = pdsch_data.transport_block.read();
    TESTASSERT(transport_block.size(), "Failed to load transport block.");
    std::vector<uint8_t> transport_block_packed(transport_block.size() / 8);
    srsvec::bit_pack(transport_block_packed, transport_block);

    // Prepare transport blocks views.
    static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> transport_blocks;
    transport_blocks.emplace_back(transport_block_packed);

    // Create fresh resource grid spy.
    resource_grid_writer_spy pdsch_rg(
        1, get_nsymb_per_slot(pdsch_data.pdu.cp), pdsch_data.pdu.bwp_start_rb + pdsch_data.pdu.bwp_size_rb, "warning");

    // Process PDSCH.
    pdsch.process(pdsch_rg, transport_blocks, pdsch_data.pdu);

    // Validate PDSCH.
    pdsch_rg.assert_entries(pdsch_rg_entries);
  }
}

static void process_test_case_pdcch(const test_case_t& test_case, pdcch_processor& pdcch)
{
  for (const pdcch_transmission& pdcch_data : test_case.pdcch) {
    // Prepare PDSCH resource grid entries.
    std::vector<rg_entry> pdsch_rg_entries;
    {
      std::vector<rg_entry> pdcch_data_symbols = pdcch_data.data_symbols.read();
      std::vector<rg_entry> pdcch_dmrs_symbols = pdcch_data.dmrs_symbols.read();
      pdsch_rg_entries.insert(pdsch_rg_entries.end(), pdcch_dmrs_symbols.begin(), pdcch_dmrs_symbols.end());
      pdsch_rg_entries.insert(pdsch_rg_entries.end(), pdcch_data_symbols.begin(), pdcch_data_symbols.end());
    }

    // Create fresh resource grid spy.
    resource_grid_writer_spy pdcch_rg(1,
                                      get_nsymb_per_slot(pdcch_data.pdu.cp),
                                      pdcch_data.pdu.coreset.bwp_start_rb + pdcch_data.pdu.coreset.bwp_size_rb,
                                      "warning");

    // Process PDSCH.
    pdcch.process(pdcch_rg, pdcch_data.pdu);

    // Validate PDSCH.
    pdcch_rg.assert_entries(pdsch_rg_entries);
  }
}

int main()
{
  std::shared_ptr<crc_calculator_factory> crc_calculator_factory = create_crc_calculator_factory_sw();
  TESTASSERT(crc_calculator_factory);

  std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("generic");
  TESTASSERT(ldpc_encoder_factory);

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
  TESTASSERT(ldpc_rate_matcher_factory);

  std::shared_ptr<ldpc_segmenter_tx_factory> ldpc_segmenter_tx_factory =
      create_ldpc_segmenter_tx_factory_sw(crc_calculator_factory);
  TESTASSERT(ldpc_segmenter_tx_factory);

  pdsch_encoder_factory_sw_configuration pdsch_encoder_factory_config = {};
  pdsch_encoder_factory_config.encoder_factory                        = ldpc_encoder_factory;
  pdsch_encoder_factory_config.rate_matcher_factory                   = ldpc_rate_matcher_factory;
  pdsch_encoder_factory_config.segmenter_factory                      = ldpc_segmenter_tx_factory;
  std::shared_ptr<pdsch_encoder_factory> pdsch_encoder_factory =
      create_pdsch_encoder_factory_sw(pdsch_encoder_factory_config);
  TESTASSERT(ldpc_encoder_factory);

  std::shared_ptr<polar_factory> polar_code_factory = create_polar_factory_sw();
  TESTASSERT(polar_code_factory);

  std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
  TESTASSERT(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_factory =
      create_dmrs_pdsch_processor_factory_sw(prg_factory);
  TESTASSERT(dmrs_pdsch_factory);

  std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_pdcch_factory =
      create_dmrs_pdcch_processor_factory_sw(prg_factory);
  TESTASSERT(dmrs_pdcch_factory);

  std::shared_ptr<pdcch_modulator_factory> pdcch_modulator_factory =
      create_pdcch_modulator_factory_sw(modulator_factory, prg_factory);
  TESTASSERT(pdcch_modulator_factory);

  std::shared_ptr<pdsch_modulator_factory> pdsch_modulator_factory =
      create_pdsch_modulator_factory_sw(modulator_factory, prg_factory);
  TESTASSERT(pdsch_modulator_factory);

  std::shared_ptr<pdcch_encoder_factory> pdcch_encoder_factory =
      create_pdcch_encoder_factory_sw(crc_calculator_factory, polar_code_factory);
  TESTASSERT(pdcch_encoder_factory);

  std::shared_ptr<pdcch_processor_factory> pdcch_proc_factory =
      create_pdcch_processor_factory_sw(pdcch_encoder_factory, pdcch_modulator_factory, dmrs_pdcch_factory);
  TESTASSERT(pdcch_proc_factory);

  std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory =
      create_pdsch_processor_factory_sw(pdsch_encoder_factory, pdsch_modulator_factory, dmrs_pdsch_factory);
  TESTASSERT(pdsch_proc_factory);

  std::unique_ptr<pdcch_processor> pdcch = pdcch_proc_factory->create();
  TESTASSERT(pdcch);

  std::unique_ptr<pdsch_processor> pdsch = pdsch_proc_factory->create();
  TESTASSERT(pdsch);

  // Iterate all test cases.
  for (const test_case_t& test_case : dl_processor_test_data) {
    // Set the next line to 1 for printing the test case description.
#if 0
    fmt::print("[{} {} {} {}] {}\n",
               test_case.test_model.test_model,
               test_case.test_model.bandwidth,
               test_case.test_model.subcarrier_spacing,
               test_case.test_model.duplex_mode,
               test_case.test_model.description);
#endif

    // Process PDCCH PDUs.
    process_test_case_pdcch(test_case, *pdcch);

    // Process PDSCH PDUs.
    process_test_case_pdsch(test_case, *pdsch);
  }
}