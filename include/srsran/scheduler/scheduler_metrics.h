/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sch/sch_mcs.h"
#include <optional>

namespace srsran {

/// \brief Snapshot of the metrics for a UE.
struct scheduler_ue_metrics {
  pci_t                        pci;
  unsigned                     nof_prbs;
  rnti_t                       rnti;
  uint8_t                      cqi;
  uint8_t                      ri;
  sch_mcs_index                dl_mcs;
  double                       dl_prbs_used;
  double                       dl_brate_kbps;
  unsigned                     dl_nof_ok;
  unsigned                     dl_nof_nok;
  float                        pusch_snr_db;
  float                        pusch_rsrp_db;
  float                        pucch_snr_db;
  sch_mcs_index                ul_mcs;
  double                       ul_prbs_used;
  double                       ul_brate_kbps;
  unsigned                     ul_nof_ok;
  unsigned                     ul_nof_nok;
  unsigned                     bsr;
  unsigned                     dl_bs;
  std::optional<phy_time_unit> last_ta;
  std::optional<int>           last_phr;
};

/// \brief Notifier interface used by scheduler to report UE metrics.
class scheduler_ue_metrics_notifier
{
public:
  virtual ~scheduler_ue_metrics_notifier() = default;

  /// \brief This method will be called periodically by the scheduler to report the latest UE metrics statistics.
  virtual void report_metrics(span<const scheduler_ue_metrics> ue_metrics) = 0;
};

} // namespace srsran
