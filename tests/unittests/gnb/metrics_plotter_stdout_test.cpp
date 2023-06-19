/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "metrics_plotter_stdout.h"

using namespace srsran;

/// \file
/// \brief Unit test metrics plotter writing to stdout.
///
/// This is meant to visually and (currently still) manually verify the correct
/// metrics formatting and plotting.

static std::vector<scheduler_ue_metrics> reports;

void fill_metrics_single_ue()
{
  scheduler_ue_metrics ue;
  ue.pci           = 500;
  ue.rnti          = to_rnti(0x4601);
  ue.cqi           = 4;
  ue.ri            = 1;
  ue.dl_mcs        = 28;
  ue.dl_brate_kbps = 1 * 1024; // 1Mbit
  ue.dl_nof_ok     = 900;
  ue.dl_nof_nok    = 100;
  ue.pusch_snr_db  = 25.0;
  ue.pucch_snr_db  = 23.0;
  ue.ul_mcs        = 15;
  ue.ul_brate_kbps = 1;
  ue.ul_nof_ok     = 99;
  ue.ul_nof_nok    = 1;
  ue.bsr           = 8192;
  reports.push_back(ue);
}

int main()
{
  srsran::metrics_plotter_stdout plotter;
  plotter.toggle_print();

  fill_metrics_single_ue();

  plotter.report_metrics(reports);

  return 0;
}