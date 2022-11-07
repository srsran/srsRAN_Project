/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../phy_test_utils.h"
#include "../rx_softbuffer_test_doubles.h"
#include "pusch_processor_test_data.h"
#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"
#include <random>

using namespace srsgnb;

namespace srsgnb {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "FRC={} for {} MHz",
             test_case.description.fix_reference_channel,
             test_case.description.channel_bandwidth_MHz);
  return os;
}

} // namespace srsgnb

namespace {

using PuschProcessorParams = test_case_t;

class channel_equalizer_spy : public channel_equalizer
{
public:
  void equalize(re_list&        eq_symbols,
                noise_var_list& eq_noise_vars,
                const re_list&  ch_symbols,
                const ch_est_list& /**/,
                span<const float> /**/,
                float /**/) override
  {
    for (unsigned i_layer = 0, i_layer_end = eq_noise_vars.get_dimension_size(re_list::dims::slice);
         i_layer != i_layer_end;
         ++i_layer) {
      span<float> layer_noise_var = eq_noise_vars.get_view<>({i_layer});
      std::fill(layer_noise_var.begin(), layer_noise_var.end(), 1e-2);

      srsvec::copy(eq_symbols.get_view<>({i_layer}), ch_symbols.get_view<>({i_layer}));
    }
  }
};

PHY_SPY_FACTORY_TEMPLATE(channel_equalizer);

class PuschProcessorFixture : public ::testing::TestWithParam<PuschProcessorParams>
{
protected:
  std::unique_ptr<pusch_processor> pusch_proc;
  std::unique_ptr<resource_grid>   grid;

  void SetUp() override
  {
    // Create pseudo-random sequence generator.
    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_TRUE(prg_factory);

    // Create demodulator mapper factory.
    std::shared_ptr<channel_modulation_factory> chan_modulation_factory = create_channel_modulation_sw_factory();
    ASSERT_TRUE(chan_modulation_factory);

    // Create CRC calculator factory.
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw();
    ASSERT_TRUE(crc_calc_factory);

    // Create LDPC decoder factory.
    std::shared_ptr<ldpc_decoder_factory> ldpc_dec_factory = create_ldpc_decoder_factory_sw("generic");
    ASSERT_TRUE(ldpc_dec_factory);

    // Create LDPC rate dematcher factory.
    std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rm_factory = create_ldpc_rate_dematcher_factory_sw();
    ASSERT_TRUE(ldpc_rm_factory);

    // Create LDPC desegmenter factory.
    std::shared_ptr<ldpc_segmenter_rx_factory> ldpc_segm_rx_factory = create_ldpc_segmenter_rx_factory_sw();
    ASSERT_TRUE(ldpc_segm_rx_factory);

    // Create port channel estimator factory.
    std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
        create_port_channel_estimator_factory_sw();
    ASSERT_TRUE(port_chan_estimator_factory);

    // Create DM-RS for PUSCH channel estimator.
    std::shared_ptr<dmrs_pusch_estimator_factory> dmrs_pusch_chan_estimator_factory =
        create_dmrs_pusch_estimator_factory_sw(prg_factory, port_chan_estimator_factory);
    ASSERT_TRUE(dmrs_pusch_chan_estimator_factory);

    // Create channel equalizer factory.
    std::shared_ptr<channel_equalizer_factory> eq_factory = std::make_shared<channel_equalizer_factory_spy>();
    ASSERT_TRUE(eq_factory);

    // Create PUSCH demodulator factory.
    std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory =
        create_pusch_demodulator_factory_sw(eq_factory, chan_modulation_factory, prg_factory);
    ASSERT_TRUE(pusch_demod_factory);

    // Create PUSCH decoder.
    pusch_decoder_factory_sw_configuration pusch_dec_config;
    pusch_dec_config.crc_factory                             = crc_calc_factory;
    pusch_dec_config.decoder_factory                         = ldpc_dec_factory;
    pusch_dec_config.dematcher_factory                       = ldpc_rm_factory;
    pusch_dec_config.segmenter_factory                       = ldpc_segm_rx_factory;
    std::shared_ptr<pusch_decoder_factory> pusch_dec_factory = create_pusch_decoder_factory_sw(pusch_dec_config);
    ASSERT_TRUE(pusch_dec_factory);

    // Create PUSCH processor.
    pusch_processor_factory_sw_configuration pusch_proc_factory_config;
    pusch_proc_factory_config.estimator_factory                    = dmrs_pusch_chan_estimator_factory;
    pusch_proc_factory_config.demodulator_factory                  = pusch_demod_factory;
    pusch_proc_factory_config.decoder_factory                      = pusch_dec_factory;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_prb       = MAX_RB;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_rx_ports  = 1;
    pusch_proc_factory_config.ch_estimate_dimensions.nof_tx_layers = 1;
    std::shared_ptr<pusch_processor_factory> pusch_proc_factory =
        create_pusch_processor_factory_sw(pusch_proc_factory_config);
    ASSERT_TRUE(pusch_proc_factory);

    // Create actual PUSCH processor.
    pusch_proc = pusch_proc_factory->create();
    ASSERT_TRUE(pusch_proc);

    // Create resource grid.
    grid = create_resource_grid(1, MAX_NSYMB_PER_SLOT, MAX_RB * NRE);
    ASSERT_TRUE(grid);
  }
};

TEST_P(PuschProcessorFixture, PuschProcessorUnittest)
{
  const test_case_t& test_case = GetParam();

  // For each slot.
  for (const fix_reference_channel_slot& slot_config : test_case.slots) {
    // Prepare resource grid.
    std::vector<rg_entry> rg_data_entries = slot_config.data.read();
    std::vector<rg_entry> rg_dmrs_entries = slot_config.dmrs.read();
    std::vector<rg_entry> rg_entries;
    rg_entries.reserve(rg_data_entries.size() + rg_dmrs_entries.size());
    rg_entries.insert(rg_entries.end(), rg_data_entries.begin(), rg_data_entries.end());
    rg_entries.insert(rg_entries.end(), rg_dmrs_entries.begin(), rg_dmrs_entries.end());

    for (unsigned i_port = 0, i_port_end = slot_config.config.rx_ports.size(); i_port != i_port_end; ++i_port) {
      // Create vector of coordinates and values for the port.
      std::vector<resource_grid_coordinate> coordinates(0);
      std::vector<cf_t>                     values(0);

      // Reserve to avoid continuous memory allocation.
      coordinates.reserve(rg_entries.size());
      values.reserve(rg_entries.size());

      // Select the grid entries that match the port.
      for (const resource_grid_reader_spy::expected_entry_t& rg_entry : rg_entries) {
        if (rg_entry.port == i_port) {
          resource_grid_coordinate coordinate;
          coordinate.subcarrier = rg_entry.subcarrier;
          coordinate.symbol     = rg_entry.symbol;
          coordinates.emplace_back(coordinate);
          values.emplace_back(rg_entry.value);
        }
      }

      // Put elements in the grid for the selected port.
      grid->put(i_port, coordinates, values);
    }

    // Read expected data.
    std::vector<uint8_t> expected_data = slot_config.sch_data.read();

    // Prepare receive data.
    std::vector<uint8_t> data(expected_data.size());

    // Prepare softbuffer.
    rx_softbuffer_spy softbuffer(
        ldpc::MAX_CODEBLOCK_SIZE,
        ldpc::compute_nof_codeblocks(expected_data.size(), slot_config.config.codeword.value().ldpc_base_graph));

    // Process PUSCH PDU.
    pusch_processor_result result = pusch_proc->process(data, softbuffer, *grid, slot_config.config);

    // Verify results.
    ASSERT_TRUE(result.data.has_value());
    ASSERT_TRUE(result.data.value().tb_crc_ok);
    ASSERT_EQ(1, result.data.value().ldpc_decoder_stats.get_max());
    ASSERT_EQ(expected_data, data);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PuschProcessorUnittest, PuschProcessorFixture, ::testing::ValuesIn(pusch_processor_test_data));

} // namespace
