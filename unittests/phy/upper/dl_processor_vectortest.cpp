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
#include "srsgnb/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsgnb/srsvec/bit.h"

using namespace srsgnb;

namespace srsgnb {

struct pdsch_modulator_config_t {
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;
};
std::unique_ptr<pdsch_modulator> create_pdsch_modulator(pdsch_modulator_config_t& config);

struct pdsch_processor_configuration {
  std::unique_ptr<pdsch_encoder>        encoder;
  std::unique_ptr<pdsch_modulator>      modulator;
  std::unique_ptr<dmrs_pdsch_processor> dmrs;
};
std::unique_ptr<pdsch_processor> create_pdsch_processor(pdsch_processor_configuration& config);

} // namespace srsgnb

void process_test_case_pdsch(const test_case_t& test_case, pdsch_processor& pdsch)
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
    resource_grid_writer_spy pdsch_rg("warning");

    // Process PDSCH.
    pdsch.process(pdsch_rg, transport_blocks, pdsch_data.pdu);

    // Validate PDSCH.
    pdsch_rg.assert_entries(pdsch_rg_entries);
  }
}

int main()
{
  // Create PDSCH processor.
  std::unique_ptr<pdsch_processor> pdsch = nullptr;
  {
    pdsch_modulator_config_t modulator_config;
    modulator_config.modulator = create_modulation_mapper();
    modulator_config.scrambler = create_pseudo_random();

    pdsch_processor_configuration processor_config;
    processor_config.encoder   = create_pdsch_encoder();
    processor_config.modulator = create_pdsch_modulator(modulator_config);
    processor_config.dmrs      = create_dmrs_pdsch_processor();

    pdsch = create_pdsch_processor(processor_config);
    TESTASSERT(pdsch);
  }

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

    // Process PDSCH PDUs.
    process_test_case_pdsch(test_case, *pdsch);
  }
}