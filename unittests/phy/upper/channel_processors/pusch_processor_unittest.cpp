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
#include "../rx_softbuffer_test_doubles.h"
#include "../signal_processors/dmrs_pusch_estimator_test_doubles.h"
#include "pusch_decoder_test_doubles.h"
#include "pusch_demodulator_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "gtest/gtest.h"
#include <random>

using namespace srsgnb;

static std::mt19937                            rgen(5678);
static std::uniform_int_distribution<unsigned> slot_count_dist(0, 10240 * 8);
static std::uniform_int_distribution<uint16_t> rnti_dist(1, std::numeric_limits<uint16_t>::max());
static std::uniform_int_distribution<unsigned> bwp_size_dist(1, MAX_RB);
static std::uniform_int_distribution<unsigned> bwp_start_dist(0, MAX_RB - 1);
static std::uniform_int_distribution<unsigned> rv_dist(0, 3);
static std::uniform_int_distribution<unsigned> bool_dist(0, 1);
static std::uniform_int_distribution<uint8_t>  ldpc_base_graph_dist(0, 1);
static std::uniform_int_distribution<unsigned> n_id_dist(0, 1023);
static std::uniform_int_distribution<unsigned> nof_tx_layers_dist(1, 4);
static std::uniform_int_distribution<unsigned> nof_rx_ports_dist(1, 4);
static std::uniform_int_distribution<unsigned> dmrs_type_dist(0, 1);
static std::uniform_int_distribution<unsigned> scrambling_id_dist(0, 65535);
static std::uniform_int_distribution<unsigned> n_scid_dist(0, 1);
static std::uniform_int_distribution<unsigned> nof_cdm_groups_without_data_dist(1, 2);
static std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, MAX_NSYMB_PER_SLOT - 1);
static std::uniform_int_distribution<unsigned> nof_symbols_dist(1, MAX_NSYMB_PER_SLOT);
static std::uniform_int_distribution<unsigned> tbs_lbrm_bytes_dist(1, ldpc::MAX_CODEBLOCK_SIZE / 8);
static std::uniform_int_distribution<unsigned> tbs_dist(1, 1024);

namespace {

using PuschProcessorParams = std::tuple<subcarrier_spacing, cyclic_prefix, modulation_scheme, bool>;

class PuschProcessorFixture : public ::testing::TestWithParam<PuschProcessorParams>
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

TEST_P(PuschProcessorFixture, PuschProcessorUnittest)
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

  subcarrier_spacing scs                = std::get<0>(GetParam());
  cyclic_prefix      cyclic_prefix_conf = std::get<1>(GetParam());
  modulation_scheme  modulation         = std::get<2>(GetParam());
  bool               codeword_present   = std::get<3>(GetParam());

  pusch_processor::pdu_t pdu;
  pdu.slot = slot_point(to_numerology_value(scs), slot_count_dist(rgen) % (10240 * get_nof_slots_per_subframe(scs)));
  pdu.rnti = rnti_dist(rgen);
  pdu.bwp_start_rb = bwp_start_dist(rgen);
  pdu.bwp_size_rb  = std::min(MAX_RB - pdu.bwp_start_rb, bwp_size_dist(rgen));
  pdu.cp           = cyclic_prefix_conf;
  pdu.modulation   = modulation;

  if (codeword_present) {
    pdu.codeword.emplace();
    pdu.codeword.value().rv              = rv_dist(rgen);
    pdu.codeword.value().ldpc_base_graph = static_cast<ldpc_base_graph_type>(ldpc_base_graph_dist(rgen));
    pdu.codeword.value().new_data        = bool_dist(rgen) == 0;
  }

  unsigned rb_alloc_start = bwp_start_dist(rgen) % pdu.bwp_size_rb;
  unsigned rb_alloc_count = std::min(pdu.bwp_size_rb - rb_alloc_start, bwp_size_dist(rgen));

  pdu.n_id          = n_id_dist(rgen);
  pdu.nof_tx_layers = nof_tx_layers_dist(rgen);
  pdu.rx_ports.resize(nof_rx_ports_dist(rgen));
  std::iota(pdu.rx_ports.begin(), pdu.rx_ports.end(), 0);
  std::generate(pdu.dmrs_symbol_mask.begin(), pdu.dmrs_symbol_mask.end(), [&]() { return bool_dist(rgen) == 0; });
  pdu.dmrs                        = static_cast<dmrs_type::options>(dmrs_type_dist(rgen));
  pdu.scrambling_id               = scrambling_id_dist(rgen);
  pdu.n_scid                      = n_scid_dist(rgen);
  pdu.nof_cdm_groups_without_data = nof_cdm_groups_without_data_dist(rgen);
  pdu.freq_alloc                  = rb_allocation::make_type1(rb_alloc_start, rb_alloc_count);
  pdu.start_symbol_index          = start_symbol_index_dist(rgen);
  pdu.nof_symbols                 = std::min(MAX_NSYMB_PER_SLOT - pdu.start_symbol_index, nof_symbols_dist(rgen));
  pdu.tbs_lbrm_bytes              = tbs_lbrm_bytes_dist(rgen);

  // Generate transport block.
  std::vector<uint8_t> transport_block(tbs_dist(rgen));
  std::generate(transport_block.begin(), transport_block.end(), [&]() { return static_cast<uint8_t>(rgen()); });

  // Create receive softbuffer.
  rx_softbuffer_spy softbuffer_spy;

  // Resource grid spy.
  resource_grid_spy rg_spy;

  // Process PDU.
  pusch_proc->process(transport_block, softbuffer_spy, rg_spy, pdu);

  // Calling resource grid and softbuffer methods are not permitted.
  ASSERT_EQ(0, rg_spy.get_total_count());
  ASSERT_EQ(0, softbuffer_spy.get_total_count());

  // Assert channel estimator inputs.
  // ...

  // Assert demodulator inputs.
  // ...

  // Assert data decoder inputs if enabled.
  if (codeword_present) {
    // ...
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PuschProcessorUnittest,
                         PuschProcessorFixture,
                         ::testing::Combine(::testing::Values(subcarrier_spacing::kHz15,
                                                              subcarrier_spacing::kHz30,
                                                              subcarrier_spacing::kHz60),
                                            ::testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED),
                                            ::testing::Values(modulation_scheme::QPSK,
                                                              modulation_scheme::QAM16,
                                                              modulation_scheme::QAM64,
                                                              modulation_scheme::QAM256),
                                            ::testing::Values(true, false)));

} // namespace