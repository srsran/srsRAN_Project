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

#pragma once

#include "srsran/cu_cp/mobility_management_metrics.h"
#include "srsran/ngap/ngap_metrics.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"
#include "srsran/rrc/rrc_metrics.h"
#include "srsran/rrc/rrc_ue.h"

namespace srsran::srs_cu_cp {

/// CU-CP Metrics report.
struct metrics_report {
  struct ue_info {
    /// Current C-RNTI of the UE.
    rnti_t rnti;
    /// ID of the DU the UE is connected to.
    gnb_du_id_t du_id;
    /// PCI of the UE's PCell.
    pci_t pci;
    /// RRC connection state of the UE.
    rrc_state rrc_connection_state;
  };
  struct cell_info {
    nr_cell_global_id_t cgi;
    pci_t               pci;
  };

  struct du_info {
    /// ID of the DU connected to the CU-CP.
    gnb_du_id_t            id;
    std::vector<cell_info> cells;

    rrc_du_metrics rrc_metrics;
  };

  std::vector<ue_info>        ues;
  std::vector<du_info>        dus;
  std::vector<ngap_info>      ngaps;
  mobility_management_metrics mobility;
};

/// Interface used by the CU-CP to report metrics.
class metrics_report_notifier
{
public:
  virtual ~metrics_report_notifier() = default;

  /// \brief Used to notify a new metrics report.
  virtual void notify_metrics_report_request(const metrics_report& report) = 0;
};

} // namespace srsran::srs_cu_cp
