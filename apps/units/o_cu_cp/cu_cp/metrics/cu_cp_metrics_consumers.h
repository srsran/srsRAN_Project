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

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/consumers/ngap_metrics_consumers.h"
#include "apps/units/o_cu_cp/cu_cp/metrics/consumers/rrc_metrics_consumers.h"
#include "srsran/srslog/log_channel.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// Consumer for the json CU-CP metrics.
class cu_cp_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit cu_cp_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the log CU-CP metrics.
class cu_cp_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit cu_cp_metrics_consumer_log(srslog::log_channel& log_chan_) :
    ngap_consumer(log_chan_), rrc_consumer(log_chan_)
  {
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  ngap_metrics_consumer_log ngap_consumer;
  rrc_metrics_consumer_log  rrc_consumer;
};

} // namespace srsran
