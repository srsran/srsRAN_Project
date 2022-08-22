#include "dmrs_pusch_estimator_impl.h"
#include "dmrs_helper.h"

using namespace srsgnb;

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
    mask[layer_ix].symbols = cfg.symbols_mask;
    mask[layer_ix].rb_mask = cfg.rb_mask;

    if (cfg.type == dmrs_type::TYPE1) {
      mask[layer_ix].re_mask = {true, false, true, false, true, false, true, false, true, false, true, false};
    } else {
      mask[layer_ix].re_mask = {true, true, false, false, false, false, true, true, false, false, false, false};
    }
  }
}

void dmrs_pusch_estimator_impl::estimate(channel_estimate&           estimate,
                                         const resource_grid_reader& grid,
                                         const configuration&        config)
{
  unsigned nof_tx_layers = config.nof_tx_layers;
  unsigned nof_rx_ports  = config.rx_ports.size();

  symbols.resize(nof_tx_layers);
  coordinates.resize(nof_tx_layers);

  dmrs_gen.generate(symbols, coordinates, config);

  port_channel_estimator::configuration est_cfg = {};

  est_cfg.rb_mask       = config.rb_mask;
  est_cfg.scs           = to_subcarrier_spacing(config.slot.numerology());
  est_cfg.c_prefix      = config.c_prefix;
  est_cfg.nof_tx_layers = nof_tx_layers;
  est_cfg.nof_rx_ports  = nof_rx_ports;
  est_cfg.first_symbol  = config.first_symbol;
  est_cfg.nof_symbols   = config.nof_symbols;

  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    ch_estimator->compute(estimate, grid, i_port, symbols, coordinates, est_cfg);
  }
}
