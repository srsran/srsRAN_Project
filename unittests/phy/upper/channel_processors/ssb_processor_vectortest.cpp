/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/signal_processors/pss_processor.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include "srsgnb/phy/upper/signal_processors/sss_processor.h"
#include "ssb_processor_test_data.h"

namespace srsgnb {
struct ssb_processor_config {
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;
};

std::unique_ptr<ssb_processor> create_ssb_processor(ssb_processor_config& config);
} // namespace srsgnb

using namespace srsgnb;

int main()
{
  std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
  TESTASSERT(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<pbch_modulator_factory> pbch_modulator_factory =
      create_pbch_modulator_factory_sw(modulator_factory, prg_factory);
  TESTASSERT(modulator_factory);

  ssb_processor_config config = {};
  config.encoder              = create_pbch_encoder();
  config.modulator            = pbch_modulator_factory->create();
  config.dmrs                 = create_dmrs_pbch_processor();
  config.pss                  = create_pss_processor();
  config.sss                  = create_sss_processor();

  std::unique_ptr<ssb_processor> ssb = create_ssb_processor(config);

  for (const test_case_t& test_case : ssb_processor_test_data) {
    // The test data was generated with 64 ports, the symbol indexes are relative to half frame and the bandwidth is
    // limited to the SSB.
    resource_grid_writer_spy grid(64, MAX_NSYMB_PER_SLOT * 5, SSB_BW_RB);

    // Process PDU
    ssb->process(test_case.config, grid);

    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }

  return 0;
}
