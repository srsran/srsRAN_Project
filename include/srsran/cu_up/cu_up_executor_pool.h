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
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// \brief Interface used to access different executors used in the CU-UP.
///
/// Tasks dispatched to executors with the same key will be executed sequentially.
class cu_up_executor_pool
{
public:
  using ptr = std::unique_ptr<task_executor, unique_function<void(task_executor*)>>;

  virtual ~cu_up_executor_pool() = default;

  /// \brief Creates a task executor to handle UL PDUs in the user plane.
  ///
  /// The returned executor should be of low priority, but optimized for high computational loads.
  virtual ptr create_ul_pdu_executor() = 0;

  /// \brief Creates a task executor to handle DL PDUs in the user plane.
  ///
  /// The returned executor should be of low priority, but optimized for high computational loads.
  virtual ptr create_dl_pdu_executor() = 0;

  /// \brief Creates a task executor appropriate for the control plane.
  virtual ptr create_ctrl_executor() = 0;
};

/// \brief Creates an executor mapper for the CU-UP.
std::unique_ptr<cu_up_executor_pool> make_cu_up_executor_mapper(span<task_executor*> dl_pdu_executors,
                                                                span<task_executor*> ul_pdu_executors,
                                                                span<task_executor*> ctrl_executors);

} // namespace srsran