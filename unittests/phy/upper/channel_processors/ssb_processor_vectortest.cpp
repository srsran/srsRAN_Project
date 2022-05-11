/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_processors/pbch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pbch_modulator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pbch_processor.h"
#include "srsgnb/phy/upper/signal_processors/pss_processor.h"
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
  ssb_processor_config config = {};
  config.encoder              = create_pbch_encoder();
  config.modulator            = create_pbch_modulator();
  config.dmrs                 = create_dmrs_pbch_processor();
  config.pss                  = create_pss_processor();
  config.sss                  = create_sss_processor();

  std::unique_ptr<ssb_processor> ssb = create_ssb_processor(config);

  for (const test_case_t& test_case : ssb_processor_test_data) {
    resource_grid_writer_spy grid;

    // Process PDU
    ssb->process(test_case.config, grid);

    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }

  return 0;
}
