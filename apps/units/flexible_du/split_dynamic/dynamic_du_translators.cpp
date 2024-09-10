/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dynamic_du_translators.h"
#include "dynamic_du_unit_config.h"
#include "srsran/du/du_cell_config.h"

using namespace srsran;

ru_dummy_configuration srsran::generate_ru_dummy_config(const ru_dummy_unit_config&        ru_cfg,
                                                        span<const srs_du::du_cell_config> du_cells,
                                                        unsigned                           max_processing_delay_slots,
                                                        unsigned                           nof_prach_ports)
{
  ru_dummy_configuration out_cfg;

  const srs_du::du_cell_config& cell = du_cells.front();

  // Derive parameters.
  unsigned channel_bw_prb = band_helper::get_n_rbs_from_bw(
      MHz_to_bs_channel_bandwidth(cell.dl_carrier.carrier_bw_mhz), cell.scs_common, frequency_range::FR1);

  // Fill configuration parameters.
  out_cfg.scs                        = cell.scs_common;
  out_cfg.nof_sectors                = du_cells.size();
  out_cfg.rx_rg_nof_prb              = channel_bw_prb;
  out_cfg.rx_rg_nof_ports            = cell.ul_carrier.nof_ant;
  out_cfg.rx_prach_nof_ports         = nof_prach_ports;
  out_cfg.max_processing_delay_slots = max_processing_delay_slots;
  out_cfg.dl_processing_delay        = ru_cfg.dl_processing_delay;

  return out_cfg;
}
