#include "dmrs_pusch_estimator_impl.h"
#include <iostream>

using namespace srsgnb;

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
  est_cfg.scs           = config.scs;
  est_cfg.c_prefix      = config.c_prefix;
  est_cfg.nof_tx_layers = nof_tx_layers;
  est_cfg.nof_rx_ports  = nof_rx_ports;
  est_cfg.first_symbol  = config.first_symbol;
  est_cfg.nof_symbols   = config.nof_symbols;

  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    std::cout << "Just a test version of dmrs_pusch_estimate!\n";

    ch_estimator->compute(estimate, grid, i_port, symbols, coordinates, est_cfg);
  }
}
