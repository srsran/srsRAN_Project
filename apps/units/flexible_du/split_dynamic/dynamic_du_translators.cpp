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

#include "dynamic_du_translators.h"
#include "dynamic_du_unit_config.h"
#include "srsran/du/du_cell_config.h"

using namespace srsran;

ru_dummy_configuration srsran::generate_ru_dummy_config(const ru_dummy_unit_config& ru_cfg,
                                                        span<const du_cell_config>  du_cells,
                                                        unsigned                    max_processing_delay_slots,
                                                        unsigned                    nof_prach_ports)
{
  ru_dummy_configuration out_cfg;

  const du_cell_config& cell = du_cells.front();

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
