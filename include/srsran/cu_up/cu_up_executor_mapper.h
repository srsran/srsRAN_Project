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

#include "srsran/support/async/async_task.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <optional>

namespace srsran {
namespace srs_cu_up {

/// \brief This class holds the executors available to a given UE in the CU-UP.
class ue_executor_mapper
{
public:
  virtual ~ue_executor_mapper() = default;

  /// \brief Initiate the shutdown of the executors associated with the respective UE context.
  /// \return Returns an asynchronous task that is only completed when all the UE executors finished their pending
  /// tasks.
  virtual async_task<void> stop() = 0;

  /// \brief Returns the task executor appropriate for the control aspects of the UE context (e.g. timers, deletion).
  virtual task_executor& ctrl_executor() = 0;

  /// \brief Returns the task executor to handle UL PDUs in the user plane.
  ///
  /// The returned executor should be of low priority, but optimized for high computational loads.
  virtual task_executor& ul_pdu_executor() = 0;

  /// \brief Returns the task executor to handle DL PDUs in the user plane.
  ///
  /// The returned executor should be of low priority, but optimized for high computational loads.
  virtual task_executor& dl_pdu_executor() = 0;

  /// \brief Returns the task executor to accelerate cryptography of PDUs in the user plane.
  ///
  /// The returned executor should run tasks on the next free worker without synchronizing with previous or next tasks.
  virtual task_executor& crypto_executor() = 0;
};

/// \brief Interface used to access different executors used in the CU-UP.
///
/// Tasks dispatched to executors with the same key will be executed sequentially.
class cu_up_executor_mapper
{
public:
  virtual ~cu_up_executor_mapper() = default;

  /// \brief Gets task executor that used for common tasks that are not related with any UE in particular.
  virtual task_executor& ctrl_executor() = 0;

  /// \brief Gets task executor that is used to write UL PDUs into the gateway.
  virtual task_executor& io_ul_executor() = 0;

  /// \brief Gets task executor that is used by the E2 CU-UP agent.
  virtual task_executor& e2_executor() = 0;

  /// \brief Gets task executor used to read N3 data from the IO. This executor allows parallelization.
  virtual task_executor& n3_rx_executor() = 0;

  /// \brief Gets task executor used to read E1 data from the IO. This executor allows parallelization.
  virtual task_executor& e1_rx_executor() = 0;

  /// \brief Gets task executor used to read E2 data from the IO. This executor allows parallelization.
  virtual task_executor& e2_rx_executor() = 0;

  /// \brief Gets task executor used to read F1-U data from the IO. This executor allows parallelization.
  virtual task_executor& f1u_rx_executor() = 0;

  /// \brief Instantiate executors for a created UE in the CU-UP.
  virtual std::unique_ptr<ue_executor_mapper> create_ue_executor_mapper() = 0;
};

/// Configuration of a cu_up_executor_mapper that instantiates multiple strands associated with the same thread pool.
struct strand_based_executor_config {
  /// \brief Maximum number of UE-dedicated strands instantiated in the CU-UP. The choice of \c max_nof_ue_strands is a
  /// tradeoff. A low \c max_nof_ue_strands will lead to UEs potentially sharing the same strand (and respective
  /// task queue), which will reduce inter-UE isolation and the ability to parallelize across UEs. If
  /// \c max_nof_ue_strands is high, more memory will be used to instantiate the UE strand queues.
  unsigned max_nof_ue_strands;
  /// \brief Default size for the task queues of the strands created in the CU-UP.
  unsigned default_task_queue_size;
  /// \brief Size for the task queues of the strands created in the CU-UP for UE DL PDU processing.
  unsigned dl_ue_task_queue_size;
  /// \brief Size for the task queues of the strands created in the CU-UP for UE UL PDU processing.
  unsigned ul_ue_task_queue_size;
  /// \brief Size for the task queues of the strands created in the CU-UP for UE control tasks.
  unsigned ctrl_ue_task_queue_size;
  /// \brief Maximum number of tasks that run in a strand before yeilding.
  unsigned strand_batch_size;
  /// \brief Executor to which CU-UP strands and crypto tasks will be associated.
  task_executor& medium_prio_executor;
  /// \brief Executor to which CU-UP packet reception tasks will be associated.
  task_executor& low_prio_executor;
  /// \brief Executor used to read SCTP messages from the E1/E2 sockets.
  task_executor& sctp_io_reader_executor;
  /// \brief Whether to instantiate a dedicated strand for sending UL PDUs to the IO.
  bool dedicated_io_strand;
  /// \brief Timers used by the application.
  timer_manager* timers;
  /// \brief Enable CU-UP executor tracing.
  bool tracing_enabled;
  /// \brief CU-UP executor metrics period.
  std::optional<std::chrono::milliseconds> metrics_period;
};

/// \brief Creates an executor mapper for the CU-UP that is based on strands of a worker pool.
std::unique_ptr<cu_up_executor_mapper> make_cu_up_executor_mapper(const strand_based_executor_config& config);

} // namespace srs_cu_up
} // namespace srsran
