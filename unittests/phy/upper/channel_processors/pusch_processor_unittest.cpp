/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../signal_processors/dmrs_pusch_estimator_test_doubles.h"
#include "pusch_decoder_test_doubles.h"
#include "pusch_demodulator_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "gtest/gtest.h"

using namespace srsgnb;

namespace {

class PuschProcessorFixture : public ::testing::Test
{
protected:
  std::shared_ptr<dmrs_pusch_estimator_factory_spy> estimator_factory_spy;
  std::shared_ptr<pusch_demodulator_factory_spy>    demodulator_factory_spy;
  std::shared_ptr<pusch_decoder_factory_spy>        decoder_factory_spy;
  std::shared_ptr<pusch_processor_factory>          pusch_proc_factory;

  std::unique_ptr<pusch_processor> pusch_proc;

  PuschProcessorFixture()
  {
    // Create spy factories.
    estimator_factory_spy   = std::make_shared<dmrs_pusch_estimator_factory_spy>();
    demodulator_factory_spy = std::make_shared<pusch_demodulator_factory_spy>();
    decoder_factory_spy     = std::make_shared<pusch_decoder_factory_spy>();

    // Create PUSCH processor factory.
    pusch_processor_factory_sw_configuration proc_factory_config;
    proc_factory_config.estimator_factory                    = estimator_factory_spy;
    proc_factory_config.demodulator_factory                  = demodulator_factory_spy;
    proc_factory_config.decoder_factory                      = decoder_factory_spy;
    proc_factory_config.ch_estimate_dimensions.nof_prb       = MAX_RB;
    proc_factory_config.ch_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
    proc_factory_config.ch_estimate_dimensions.nof_rx_ports  = 1;
    proc_factory_config.ch_estimate_dimensions.nof_tx_layers = 1;
    pusch_proc_factory                                       = create_pusch_processor_factory_sw(proc_factory_config);

    // Create actual PUSCH processor.
    pusch_proc = pusch_proc_factory->create();
  }
};

TEST_F(PuschProcessorFixture, PuschProcessorCreation)
{
  // Make sure PUSCH processor is created.
  ASSERT_TRUE(pusch_proc);

  // DMRS PUSCH estimator creation.
  dmrs_pusch_estimator_spy* estimator_spy = estimator_factory_spy->get_entries().front();
  ASSERT_TRUE(estimator_spy);
  // Demodulator creation.
  pusch_demodulator_spy* demodulator_spy = demodulator_factory_spy->get_entries().front();
  ASSERT_TRUE(demodulator_spy);
  // Decoder creation.
  pusch_decoder_spy* decoder_spy = decoder_factory_spy->get_entries().front();
  ASSERT_TRUE(decoder_spy);
}

} // namespace