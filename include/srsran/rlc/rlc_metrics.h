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

#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/rlc/rlc_rx_metrics.h"
#include "srsran/rlc/rlc_tx_metrics.h"

namespace srsran {

/// Container to hold TX/RX metrics
struct rlc_metrics {
  uint32_t       du_index;
  du_ue_index_t  ue_index;
  rb_id_t        rb_id;
  rlc_tx_metrics tx;
  rlc_rx_metrics rx;
  unsigned       counter;
};

/// \brief Notifier interface used to report RLC metrics.
class rlc_metrics_notifier
{
public:
  virtual ~rlc_metrics_notifier() = default;

  /// \brief This method will be called periodically to report the latest RLC metrics statistics.
  virtual void report_metrics(const rlc_metrics& metrics) = 0;
};
} // namespace srsran
