/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "apps/units/flexible_o_du/o_du_high/du_high/metrics/consumers/scheduler_metrics_consumers.h"

using namespace srsran;

/// \file
/// \brief Unit test metrics plotter writing to stdout.
///
/// This is meant to visually and (currently still) manually verify the correct
/// metrics formatting and plotting.

static scheduler_metrics_report reports;

void fill_metrics_single_ue()
{
  scheduler_ue_metrics ue;
  ue.pci  = 500;
  ue.rnti = to_rnti(0x4601);
  ue.cqi_stats.update(4);
  ue.dl_ri_stats.update(1);
  ue.ul_ri_stats.update(1);
  ue.dl_mcs        = 28;
  ue.dl_brate_kbps = 1 * 1024; // 1Mbit
  ue.dl_nof_ok     = 900;
  ue.dl_nof_nok    = 100;
  ue.pusch_snr_db  = 25.0;
  ue.pusch_rsrp_db = -10.0;
  ue.pucch_snr_db  = 23.0;
  ue.ul_mcs        = 15;
  ue.ul_brate_kbps = 1;
  ue.ul_nof_ok     = 99;
  ue.ul_nof_nok    = 1;
  ue.bsr           = 8192;
  ue.dl_bs         = 100000;
  auto& cell       = reports.cells.emplace_back();
  cell.ue_metrics.push_back(ue);
}

int main()
{
  scheduler_cell_metrics_consumer_stdout plotter;

  fill_metrics_single_ue();

  plotter.handle_metric(reports);

  return 0;
}
