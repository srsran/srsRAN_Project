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

#pragma once

#include "srsran/adt/span.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sch_mcs.h"

namespace srsran {

/// \brief Snapshot of the metrics for a UE.
struct scheduler_ue_metrics {
  pci_t         pci;
  rnti_t        rnti;
  uint8_t       cqi;
  uint8_t       ri;
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
  unsigned      dl_bs;
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
