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
#include "srsran/rlc/rlc_metrics.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// This class is used to create a source object in the metrics hub to which one or more subscribers can be connected.
class metrics_hub_source
{
public:
  metrics_hub_source(std::string source_name_) : source_name(source_name_){};
  virtual ~metrics_hub_source() = default;

  void set_task_executor(task_executor* executor_) { executor = executor_; };

  std::string    source_name;
  task_executor* executor;
};

class scheduler_ue_metrics_source : public metrics_hub_source, public scheduler_ue_metrics_notifier
{
public:
  scheduler_ue_metrics_source(std::string source_name_) : metrics_hub_source(source_name_){};
  ~scheduler_ue_metrics_source() = default;
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;
  void add_subscriber(scheduler_ue_metrics_notifier& subscriber);

private:
  std::vector<scheduler_ue_metrics_notifier*> subscribers;
};

class rlc_metrics_source : public metrics_hub_source, public rlc_metrics_notifier
{
public:
  rlc_metrics_source(std::string source_name_) : metrics_hub_source(source_name_){};
  ~rlc_metrics_source() = default;
  void report_metrics(const rlc_metrics& metrics) override;
  void add_subscriber(rlc_metrics_notifier& subscriber);

private:
  std::vector<rlc_metrics_notifier*> subscribers;
};

class metrics_hub
{
public:
  metrics_hub(task_executor& executor_);

  /// \brief moves the new source object to the metrics hub.
  /// \param[in] metrics_hub_source object.
  void add_source(std::unique_ptr<metrics_hub_source> source);

  /// \brief retrieves a pointer to the Scheduler UE metric source with the given name.
  /// \param[in] name of desired Scheduler UE metric source.
  /// \return returns a pointer to the Scheduler UE metric source.
  scheduler_ue_metrics_source* get_scheduler_ue_metrics_source(std::string source_name_);

  /// \brief retrieves a pointer to the RLC metric source with the given name.
  /// \param[in] name of desired RLC metric source.
  /// \return returns a pointer to the RLC metric source.
  rlc_metrics_source* get_rlc_metrics_source(std::string source_name_);

private:
  std::vector<std::unique_ptr<metrics_hub_source>> sources;
  srslog::basic_logger&                            logger;
  task_executor&                                   executor;
};

} // namespace srsran
