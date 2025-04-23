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

#include "srsran/adt/static_vector.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/ran/du_types.h"

namespace srsran {
namespace srs_du {

/// Metrics report of the DU F1AP.
struct f1ap_metrics_report {
  struct ue_metrics {
    du_ue_index_t       ue_index;
    gnb_du_ue_f1ap_id_t ue_id;
  };

  /// Number of PDUs received by the F1AP-DU.
  unsigned nof_rx_pdus = 0;
  /// List of UE metrics.
  static_vector<ue_metrics, MAX_NOF_DU_UES> ues;
};

} // namespace srs_du
} // namespace srsran
