/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/blocking_queue.h"
#include "srsgnb/ran/pci.h"

namespace srsgnb {

/// \brief Snapshot of the metrics for a UE.
struct scheduler_ue_metrics {
  pci_t    pci;
  rnti_t   rnti;
  unsigned cqi;
  unsigned dl_mcs;
  double   dl_brate_kbps;
  unsigned dl_nof_ok;
  unsigned dl_nof_nok;
  float    pusch_snr;
  float    pucch_snr;
  unsigned ul_mcs;
  double   ul_brate_kbps;
  unsigned ul_nof_ok;
  unsigned ul_nof_nok;
  unsigned bsr;
};

/// \brief Channel used by the scheduler to send metrics.
using scheduler_metrics_queue = blocking_queue<scheduler_ue_metrics>;

} // namespace srsgnb