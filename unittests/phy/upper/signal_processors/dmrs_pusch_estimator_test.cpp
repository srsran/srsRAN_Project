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
    // For now, channel estimate is a long array with indices that vary with i) subcarriers, ii) OFDM symbols, iii) Rx
    // port, and iv) Tx layer.
    unsigned re_per_layer_ant = cfg.rb_mask.count() * NRE * cfg.nof_symbols;
    unsigned re_per_layer     = re_per_layer_ant * cfg.nof_rx_ports;

    for (unsigned i_layer = 0; i_layer != cfg.nof_tx_layers; ++i_layer) {
      float      marker = static_cast<float>(10 * port + i_layer);
      unsigned   skip   = re_per_layer * i_layer + re_per_layer_ant * port;
      span<cf_t> ch_est = span<cf_t>(estimate.ce).subspan(skip, re_per_layer_ant);
      std::fill(ch_est.begin(), ch_est.end(), marker);
    }
  };
};

// Dummy DM-RS generation.
void dmrs_pusch_generator_impl::generate(span<dmrs_symbol_list> symbols, span<dmrs_mask> mask, const configuration& cfg)
{
  unsigned nof_tx_layers = cfg.nof_tx_layers;
  srsran_assert(nof_tx_layers <= pusch_constants::MAX_NOF_LAYERS, "Too many layers.");

  unsigned slot_syms = get_nsymb_per_slot(cfg.c_prefix);
  srsran_assert(cfg.first_symbol + cfg.nof_symbols <= slot_syms,
                "The slot has {} total symbols, requested {} symbols starting from symbol {}.",
                slot_syms,
                cfg.nof_symbols,
                cfg.first_symbol);
  srsran_assert(cfg.nof_symbols > 4, "For this test, the PUSCH should occupy at least 4 symbols.");

  unsigned n_dmrs_symbols = cfg.rb_mask.count() * NRE;

  unsigned nof_rx_ports = symbols.size();
  srsran_assert(mask.size() == nof_rx_ports, "Sizes of inputs symbols and coordinates do not match.");

  for (unsigned layer_ix = 0; layer_ix != nof_tx_layers; ++layer_ix) {
    symbols[layer_ix].resize(n_dmrs_symbols);
    mask[layer_ix].symbols.resize(cfg.nof_symbols, false);
    // Second and last-but-one symbols are used for DM-RS.
    mask[layer_ix].symbols[1]                   = true;
    mask[layer_ix].symbols[cfg.nof_symbols - 2] = true;
    mask[layer_ix].res_elements.resize(n_dmrs_symbols / 2);
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

  TESTASSERT(std::all_of(ch_est.ce.begin(), ch_est.ce.end(), [](cf_t a) { return (a == 1.0F); }),
             "Channel estimate not properly initialized.");

  // PUSCH channel estimator instance.
  std::unique_ptr<dmrs_pusch_estimator> pusch_est_test = create_dmrs_pusch_estimator();

  // PUSCH channel estimator configuration object.
  dmrs_pusch_estimator::configuration cfg = {};
  cfg.rb_mask                             = {false, false, false, false, true, true, true, true, true,  true,
                                             true,  true,  true,  true,  true, true, true, true, false, false};
  srsran_assert(cfg.rb_mask.size() == nof_rb, "Check rm_mask!");
  srsran_assert(cfg.rb_mask.count() == ch_est_dims.nof_prb, "Check rm_mask!");
  cfg.nof_symbols  = ch_est_dims.nof_symbols;
  cfg.first_symbol = 2;
  cfg.rx_ports.resize(ch_est_dims.nof_rx_ports);
  cfg.nof_tx_layers = ch_est_dims.nof_tx_layers;
  cfg.c_prefix      = cp;

  unsigned nof_slot_symbols = get_nsymb_per_slot(cp);
  srsran_assert(nof_slot_symbols >= cfg.first_symbol + cfg.nof_symbols, "Check OFDM symbols.");

  std::unique_ptr<resource_grid_reader> grid = create_rg_reader();

  // RUN ESTIMATION ROUTINE
  pusch_est_test->estimate(ch_est, *grid, cfg);

  // Check the results. The dummy channel estimator sets all RE elements corresponding to one TX-RX path to a number
  // equal to 10 * Rx port index + layer index.
  unsigned nof_rx_ports = cfg.rx_ports.size();
  unsigned nof_re_path  = ch_est_dims.nof_symbols * ch_est_dims.nof_prb * NRE;
  unsigned skip         = 0;
  for (unsigned i_layer = 0; i_layer != cfg.nof_tx_layers; ++i_layer) {
    for (unsigned i_ant = 0; i_ant != nof_rx_ports; ++i_ant) {
      span<cf_t> path   = span<cf_t>(ch_est.ce).subspan(skip, nof_re_path);
      float      marker = static_cast<float>(10 * i_ant + i_layer);

      TESTASSERT(
          std::all_of(path.begin(), path.end(), [marker](cf_t a) { return ((a.real() == marker) && (a.imag() == 0)); }),
          "Something went wrong with the channel estimation.");

      skip += nof_re_path;
    }
  }
}
