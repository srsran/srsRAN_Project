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
};

/// \brief Interface used to access different executors used in the CU-UP.
///
/// Tasks dispatched to executors with the same key will be executed sequentially.
class cu_up_executor_pool
{
public:
  virtual ~cu_up_executor_pool() = default;

  /// \brief Instantiate executors for a created UE in the CU-UP.
  virtual std::unique_ptr<ue_executor_mapper> create_ue_executor_mapper() = 0;
};

/// \brief Creates an executor mapper for the CU-UP.
std::unique_ptr<cu_up_executor_pool> make_cu_up_executor_mapper(task_executor&       cu_up_main_executor,
                                                                span<task_executor*> dl_pdu_executors,
                                                                span<task_executor*> ul_pdu_executors,
                                                                span<task_executor*> ctrl_executors);

} // namespace srs_cu_up
} // namespace srsran