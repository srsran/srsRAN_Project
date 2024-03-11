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

/// \brief This class holds the executors available to a given UE PDU session in the CU-CP.
class pdu_session_executor_mapper
{
public:
  virtual ~pdu_session_executor_mapper() = default;

  /// \brief Returns the task executor appropriate for the control aspects of the PDU session (e.g. timers, deletion).
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

  /// \brief Instantiate executors for a new PDU session.
  virtual std::unique_ptr<pdu_session_executor_mapper> create_pdu_session() = 0;
};

/// \brief Creates an executor mapper for the CU-UP.
std::unique_ptr<cu_up_executor_pool> make_cu_up_executor_mapper(span<task_executor*> dl_pdu_executors,
                                                                span<task_executor*> ul_pdu_executors,
                                                                span<task_executor*> ctrl_executors);

} // namespace srsran