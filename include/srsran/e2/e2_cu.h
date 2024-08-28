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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e2/e2.h"
#include "srsran/pdcp/pdcp_metrics.h"

namespace srsran {

class e2_cu_metrics_notifier : public pdcp_metrics_notifier
{
public:
  virtual ~e2_cu_metrics_notifier() = default;

  using pdcp_metrics_notifier::report_metrics;
};

class e2_cu_metrics_interface
{
public:
  virtual ~e2_cu_metrics_interface() = default;

  /// \brief connects e2_cu_metric_provider
  /// \param[in] meas_provider pointer to the e2_cu_metric_provider
  virtual void connect_e2_cu_meas_provider(std::unique_ptr<e2_cu_metrics_notifier> meas_provider) = 0;
};

/// Methods used by E2 to signal handover events to the CU-CP.
class e2_mobility_notifier
{
public:
  virtual ~e2_mobility_notifier() = default;

  /// \brief Notify the CU-CP about an required intra-CU handover.
  virtual async_task<srs_cu_cp::cu_cp_intra_cu_handover_response>
  on_intra_cu_handover_required(const srs_cu_cp::cu_cp_intra_cu_handover_request& request,
                                srs_cu_cp::du_index_t                             source_du_index,
                                srs_cu_cp::du_index_t                             target_du_index) = 0;
};

} // namespace srsran
