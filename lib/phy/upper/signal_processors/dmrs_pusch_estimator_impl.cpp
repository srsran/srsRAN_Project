#include "dmrs_pusch_estimator_impl.h"
#include <iostream>

using namespace srsgnb;

void dmrs_pusch_estimator_impl::estimate(channel_estimate&           estimate,
                                         const resource_grid_reader& grid,
                                         const configuration&        config)
{
  unsigned nof_tx_layers   = config.nof_tx_layers;
  unsigned nof_rx_antennas = config.rx_ports.size();

  symbols.resize(nof_tx_layers);
  coordinates.resize(nof_tx_layers);

  dmrs_gen.generate(symbols, coordinates, config);

  port_channel_estimator::configuration est_cfg = {};

  est_cfg.rb_mask       = config.rb_mask;
  est_cfg.c_prefix      = config.c_prefix;
  est_cfg.nof_tx_layers = nof_tx_layers;
  est_cfg.nof_rx_ports  = nof_rx_antennas;
  est_cfg.first_symbol  = config.first_symbol;
  est_cfg.nof_symbols   = config.nof_symbols;

  for (unsigned port_ind = 0; port_ind != nof_rx_antennas; ++port_ind) {
    std::cout << "Just a test version of dmrs_pusch_estimate!\n";

    ch_estimator->compute(estimate, grid, port_ind, symbols, coordinates, est_cfg);
  }
}
