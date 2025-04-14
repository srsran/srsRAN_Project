/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/ran/du_types.h"

namespace srsran {
namespace srs_du {

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

/// \brief Class used to retrieve F1AP metrics.
class f1ap_metrics_collector
{
public:
  virtual ~f1ap_metrics_collector() = default;

  /// \brief Generate an F1AP metrics report.
  /// \param report Report to fill.
  virtual void handle_metrics_report_request(f1ap_metrics_report& report) = 0;
};

} // namespace srs_du
} // namespace srsran
