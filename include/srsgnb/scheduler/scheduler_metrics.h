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

#include "srsgnb/adt/span.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/sch_mcs.h"

namespace srsgnb {

/// \brief Snapshot of the metrics for a UE.
struct scheduler_ue_metrics {
  pci_t         pci;
  rnti_t        rnti;
  unsigned      cqi;
  sch_mcs_index dl_mcs;
  double        dl_brate_kbps;
  unsigned      dl_nof_ok;
  unsigned      dl_nof_nok;
  float         pusch_snr_db;
  float         pucch_snr_db;
  sch_mcs_index ul_mcs;
  double        ul_brate_kbps;
  unsigned      ul_nof_ok;
  unsigned      ul_nof_nok;
  unsigned      bsr;
};

/// \brief Notifier interface used by scheduler to report UE metrics.
class scheduler_ue_metrics_notifier
{
public:
  virtual ~scheduler_ue_metrics_notifier() = default;

  /// \brief This method will be called periodically by the scheduler to report the latest UE metrics statistics.
  virtual void report_metrics(span<const scheduler_ue_metrics> ue_metrics) = 0;
};

} // namespace srsgnb
