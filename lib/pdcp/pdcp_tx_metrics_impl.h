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

#include "srsran/pdcp/pdcp_tx_metrics.h"
#include <mutex>

namespace srsran {

class pdcp_tx_metrics : pdcp_tx_metrics_interface
{
  pdcp_tx_metrics_container metrics = {};
  std::mutex                metrics_mutex;

public:
  void metrics_add_sdus(uint32_t num_sdus_, size_t num_sdu_bytes_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_sdus += num_sdus_;
    metrics.num_sdu_bytes += num_sdu_bytes_;
  }

  void metrics_add_pdus(uint32_t num_pdus_, size_t num_pdu_bytes_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_pdus += num_pdus_;
    metrics.num_pdu_bytes += num_pdu_bytes_;
  }

  void metrics_add_discard_timouts(uint32_t num_discard_timeouts_)
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_discard_timeouts += num_discard_timeouts_;
  }

  pdcp_tx_metrics_container get_metrics() final
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    return metrics;
  }

  pdcp_tx_metrics_container get_metrics_and_reset() final
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    pdcp_tx_metrics_container   ret = metrics;
    metrics                         = {};
    return ret;
  }

  void reset_metrics() final
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics = {};
  }
};
} // namespace srsran
