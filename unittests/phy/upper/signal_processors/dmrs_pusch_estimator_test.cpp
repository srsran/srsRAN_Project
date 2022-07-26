/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../../lib/phy/upper/signal_processors/dmrs_pusch_estimator_impl.h"
#include "../../resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

class port_channel_estimator_dummy : public port_channel_estimator
{
public:
  void compute(channel_estimate& estimate,
               const resource_grid_reader& /**/,
               unsigned port,
               span<const dmrs_symbol_list> /**/,
               span<const dmrs_mask> /**/,
               const configuration& cfg) override
  {
    for (unsigned i_layer = 0; i_layer != cfg.nof_tx_layers; ++i_layer) {
      float marker = static_cast<float>(10 * port + i_layer);
      // Get a view to the RE channel estimate corresponding to the port-i_layer path.
      span<cf_t> ch_est = estimate.get_path_ch_estimate(port, i_layer);
      srsgnb_assert(ch_est.size() == cfg.nof_symbols * cfg.rb_mask.count() * NRE,
                    "The size of the channel estimate view does not match with the configuration parameters.");
      std::fill(ch_est.begin(), ch_est.end(), marker);
    }
  };
};

// Dummy DM-RS generation.
void dmrs_pusch_generator_impl::generate(span<dmrs_symbol_list> symbols, span<dmrs_mask> mask, const configuration& cfg)
{
  unsigned nof_tx_layers = cfg.nof_tx_layers;
  srsgnb_assert(nof_tx_layers <= pusch_constants::MAX_NOF_LAYERS, "Too many layers.");

  unsigned slot_syms = get_nsymb_per_slot(cfg.c_prefix);
  srsgnb_assert(cfg.first_symbol + cfg.nof_symbols <= slot_syms,
                "The slot has {} total symbols, requested {} symbols starting from symbol {}.",
                slot_syms,
                cfg.nof_symbols,
                cfg.first_symbol);
  srsgnb_assert(cfg.nof_symbols > 4, "For this test, the PUSCH should occupy at least 4 symbols.");

  unsigned nof_dmrs_per_symbol = cfg.rb_mask.count() * NRE;

  srsgnb_assert(symbols.size() == nof_tx_layers,
                "The number of DM-RS symbol lists do not match the number of Tx layers.");
  srsgnb_assert(mask.size() == nof_tx_layers, "The number of DM-RS masks do not match the number of Tx layers.");

  for (unsigned layer_ix = 0; layer_ix != nof_tx_layers; ++layer_ix) {
    symbols[layer_ix].resize(nof_dmrs_per_symbol, -1);
    mask[layer_ix].symbols.resize(cfg.nof_symbols, false);
    // Second and last-but-one symbols are used for DM-RS.
    mask[layer_ix].symbols[1]                   = true;
    mask[layer_ix].symbols[cfg.nof_symbols - 2] = true;
    mask[layer_ix].res_elements.resize(nof_dmrs_per_symbol / 2);
    mask[layer_ix].res_elements.reset();
    for (unsigned i_el = 0, max_el = mask[layer_ix].res_elements.size(); i_el < max_el; i_el += 2) {
      mask[layer_ix].res_elements.set(i_el);
    }
  }
}

std::unique_ptr<resource_grid_reader> create_rg_reader()
{
  return std::make_unique<resource_grid_reader_spy>();
}

std::unique_ptr<dmrs_pusch_estimator> create_dmrs_pusch_estimator()
{
  std::unique_ptr<port_channel_estimator> ch_est = std::make_unique<port_channel_estimator_dummy>();
  return std::make_unique<dmrs_pusch_estimator_impl>(std::move(ch_est));
}

int main()
{
  // SETUP
  // Total number of resource blocks.
  unsigned nof_rb = 20;
  // Cyclic prefix.
  cyclic_prefix cp = cyclic_prefix::NORMAL;

  // Channel estimation container.
  channel_estimate::channel_estimate_dimensions ch_est_dims = {};

  ch_est_dims.nof_tx_layers = 2;
  ch_est_dims.nof_rx_ports  = 4;
  ch_est_dims.nof_symbols   = 10;
  ch_est_dims.nof_prb       = 14;

  channel_estimate ch_est(ch_est_dims);

  for (unsigned i_layer = 0; i_layer != ch_est_dims.nof_tx_layers; ++i_layer) {
    for (unsigned i_port = 0; i_port != ch_est_dims.nof_rx_ports; ++i_port) {
      span<const cf_t> path_ch_est = ch_est.get_path_ch_estimate(i_port, i_layer);
      TESTASSERT(std::all_of(path_ch_est.begin(), path_ch_est.end(), [](cf_t a) { return (a == 1.0F); }),
                 "Channel estimate not properly initialized.");
    }
  }

  // PUSCH channel estimator instance.
  std::unique_ptr<dmrs_pusch_estimator> pusch_est_test = create_dmrs_pusch_estimator();

  // PUSCH channel estimator configuration object.
  dmrs_pusch_estimator::configuration cfg = {};
  cfg.rb_mask                             = {false, false, false, false, true, true, true, true, true,  true,
                                             true,  true,  true,  true,  true, true, true, true, false, false};
  srsgnb_assert(cfg.rb_mask.size() == nof_rb, "Check rm_mask!");
  srsgnb_assert(cfg.rb_mask.count() == ch_est_dims.nof_prb, "Check rm_mask!");
  cfg.nof_symbols  = ch_est_dims.nof_symbols;
  cfg.first_symbol = 2;
  cfg.rx_ports.resize(ch_est_dims.nof_rx_ports);
  cfg.nof_tx_layers = ch_est_dims.nof_tx_layers;
  cfg.c_prefix      = cp;

  unsigned nof_slot_symbols = get_nsymb_per_slot(cp);
  srsgnb_assert(nof_slot_symbols >= cfg.first_symbol + cfg.nof_symbols, "Check OFDM symbols.");

  std::unique_ptr<resource_grid_reader> grid = create_rg_reader();

  // RUN ESTIMATION ROUTINE
  pusch_est_test->estimate(ch_est, *grid, cfg);

  // Check the results. The dummy channel estimator sets all RE elements corresponding to one TX-RX path to a number
  // equal to 10 * Rx port index + layer index.
  unsigned nof_rx_ports = cfg.rx_ports.size();
  for (unsigned i_layer = 0; i_layer != cfg.nof_tx_layers; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      span<const cf_t> path   = ch_est.get_path_ch_estimate(i_port, i_layer);
      float            marker = static_cast<float>(10 * i_port + i_layer);
      TESTASSERT(
          std::all_of(path.begin(), path.end(), [marker](cf_t a) { return ((a.real() == marker) && (a.imag() == 0)); }),
          "Something went wrong with the channel estimation.");
    }
  }
}
