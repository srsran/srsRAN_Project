/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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