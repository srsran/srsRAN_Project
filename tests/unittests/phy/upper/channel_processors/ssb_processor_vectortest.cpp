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

#include "ssb_processor_test_data.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/signal_processors/pss_processor.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"
#include "srsran/phy/upper/signal_processors/sss_processor.h"

using namespace srsran;

int main()
{
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
  TESTASSERT(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<polar_factory> polar_factory = create_polar_factory_sw();
  TESTASSERT(polar_factory);

  std::shared_ptr<pbch_encoder_factory> pbch_enc_factory =
      create_pbch_encoder_factory_sw(crc_calc_factory, prg_factory, polar_factory);
  TESTASSERT(polar_factory);

  std::shared_ptr<pbch_modulator_factory> pbch_mod_factory =
      create_pbch_modulator_factory_sw(modulator_factory, prg_factory);
  TESTASSERT(pbch_mod_factory);

  std::shared_ptr<dmrs_pbch_processor_factory> dmrs_pbch_proc_factory =
      create_dmrs_pbch_processor_factory_sw(prg_factory);
  TESTASSERT(dmrs_pbch_proc_factory, "Invalid DMRS PBCH factory.");

  std::shared_ptr<pss_processor_factory> pss_proc_factory = create_pss_processor_factory_sw();
  TESTASSERT(pss_proc_factory, "Invalid PSS factory.");

  std::shared_ptr<sss_processor_factory> sss_proc_factory = create_sss_processor_factory_sw();
  TESTASSERT(sss_proc_factory, "Invalid SSS factory.");

  // Create channel processors - SSB
  ssb_processor_factory_sw_configuration ssb_factory_config;
  ssb_factory_config.encoder_factory                 = pbch_enc_factory;
  ssb_factory_config.modulator_factory               = pbch_mod_factory;
  ssb_factory_config.dmrs_factory                    = dmrs_pbch_proc_factory;
  ssb_factory_config.pss_factory                     = pss_proc_factory;
  ssb_factory_config.sss_factory                     = sss_proc_factory;
  std::shared_ptr<ssb_processor_factory> ssb_factory = create_ssb_processor_factory_sw(ssb_factory_config);
  TESTASSERT(ssb_factory);

  // Create SSB processor.
  std::unique_ptr<ssb_processor> ssb = ssb_factory->create();
  TESTASSERT(ssb);

  for (const test_case_t& test_case : ssb_processor_test_data) {
    // The test data was generated with 64 ports, the symbol indexes are relative to half frame and the bandwidth is
    // limited to the SSB.
    resource_grid_writer_spy grid(64, MAX_NSYMB_PER_SLOT * 5, SSB_BW_RB);

    // Process PDU
    ssb->process(grid, test_case.config);

    // Load output golden data
    const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Assert resource grid entries.
    grid.assert_entries(testvector_symbols);
  }

  return 0;
}
