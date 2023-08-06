/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

class metrics_hub : public scheduler_ue_metrics_notifier
{
public:
  metrics_hub(task_executor& du_executor_);
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;

  /// \brief  adds a subscriber to the metrics hub.
  /// \param[in] subscriber reference to the new subscriber.
  /// \return returns a pointer to the new subscriber.
  scheduler_ue_metrics_notifier* add_subscriber(scheduler_ue_metrics_notifier& subscriber);

  /// \brief creates a new souce object in the metrics hub.
  /// \param[in]  gives the source a name.
  /// \return returns the index of the new source.
  unsigned add_source(std::string source_name);

  /// \brief connects a subscriber to a source.
  /// \param[in] index of the source to be connected.
  /// \param[in] pointer to the subscriber to be connected.
  void connect_subscriber_to_source(unsigned source_idx, scheduler_ue_metrics_notifier* subscriber);

  /// \brief retrives a pointer to the source notifier at the given index.
  /// \param[in] index of desired source notifier.
  /// \return returns a pointer to the source notifier.
  scheduler_ue_metrics_notifier* get_source_notifier(unsigned idx);

  /// \brief retrives a pointer to the subscriber notifier at the given index.
  /// \param index of the desired subscriber.
  /// \return returns a pointer to the subscriber notifier.
  scheduler_ue_metrics_notifier* get_subscriber_notifier(unsigned idx);

  /// This class is used to create a source object in the metrics hub to which one or more subscribers can be connected.
  class metrics_hub_source : public scheduler_ue_metrics_notifier
  {
  public:
    metrics_hub_source(task_executor& du_executor_, std::string _source_name) :
      du_executor(du_executor_), source_name(_source_name)
    {
    }
    void           report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;
    void           add_subscriber(scheduler_ue_metrics_notifier& subscriber);
    task_executor& du_executor;
    std::string    source_name;

  private:
    std::vector<scheduler_ue_metrics_notifier*> subscribers;
  };

private:
  std::vector<scheduler_ue_metrics_notifier*>      subscribers;
  std::vector<std::unique_ptr<metrics_hub_source>> sources;
  srslog::basic_logger&                            logger;
  task_executor&                                   du_executor;
};

} // namespace srsran
