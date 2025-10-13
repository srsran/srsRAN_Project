/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <chrono>
#include <memory>
#include <optional>

namespace srsran {

class task_executor;
class executor_metrics_channel_registry;

namespace srs_cu_cp {

/// Interface used to access different executors used in the CU-CP.
class cu_cp_executor_mapper
{
public:
  virtual ~cu_cp_executor_mapper() = default;

  /// Gets sequential task executor that used for processing procedures and other CU-CP tasks.
  virtual task_executor& ctrl_executor() = 0;

  /// Gets sequential task executor that used for E2 tasks associated with the CU-CP.
  virtual task_executor& e2_executor() = 0;

  /// Gets task executor used to parallelize the reception of N2 messages.
  virtual task_executor& n2_rx_executor() = 0;

  /// Gets task executor used to parallelize the reception of F1-C messages.
  virtual task_executor& f1c_rx_executor() = 0;

  /// Gets task executor used to parallelize the reception of E1 messages.
  virtual task_executor& e1_rx_executor() = 0;

  /// Gets task executor used to parallelize the reception of F1-C messages.
  virtual task_executor& e2_rx_executor() = 0;
};

/// Configuration of a cu_cp_executor_mapper that instantiates strands associated with the same thread pool.
struct strand_based_executor_config {
  /// Executor to which CU-CP strands will be associated.
  task_executor& pool_executor;
  /// \brief Optional executor metrics channel registry.
  ///
  /// If it is initialized, the executor mapper wraps the executors with metric decorators.
  executor_metrics_channel_registry* exec_metrics_channel_registry = nullptr;
};

/// \brief Creates an executor mapper for the CU-CP that is based on strands of a worker pool.
std::unique_ptr<cu_cp_executor_mapper> make_cu_cp_executor_mapper(const strand_based_executor_config& config);

} // namespace srs_cu_cp
} // namespace srsran
