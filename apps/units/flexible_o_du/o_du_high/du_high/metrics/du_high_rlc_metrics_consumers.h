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

#include "du_high_rlc_metrics.h"
#include "srsran/srslog/log_channel.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Consumer for the json RLC metrics.
class rlc_metrics_consumer_json : public app_services::metrics_consumer
{
public:
  explicit rlc_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the log RLC metrics.
class rlc_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit rlc_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    srsran_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  srslog::log_channel& log_chan;
};

/// Consumer for the E2 RLC metrics.
class rlc_metrics_consumer_e2 : public app_services::metrics_consumer
{
public:
  explicit rlc_metrics_consumer_e2(rlc_metrics_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override;

private:
  rlc_metrics_notifier& notifier;
};

} // namespace srsran
