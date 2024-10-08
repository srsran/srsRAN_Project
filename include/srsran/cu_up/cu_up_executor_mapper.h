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

#include "srsran/adt/span.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/executors/task_executor.h"

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
  virtual task_executor& io_executor() = 0;

  /// \brief Gets task executor that is used by the E2 CU-UP agent.
  virtual task_executor& e2_executor() = 0;

  /// \brief Instantiate executors for a created UE in the CU-UP.
  virtual std::unique_ptr<ue_executor_mapper> create_ue_executor_mapper() = 0;
};

struct strand_based_executor_config {
  unsigned       max_nof_ue_strands;
  unsigned       default_task_queue_size;
  unsigned       gtpu_task_queue_size;
  task_executor& worker_pool_executor;
  bool           dedicated_io_strand;
};

/// \brief Creates an executor mapper for the CU-UP that is based on strands of a worker pool.
std::unique_ptr<cu_up_executor_mapper> make_cu_up_executor_mapper(const strand_based_executor_config& config);

} // namespace srs_cu_up
} // namespace srsran
