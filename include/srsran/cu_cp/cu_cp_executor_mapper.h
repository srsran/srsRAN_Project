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

#include <memory>

namespace srsran {

class task_executor;

namespace srs_cu_cp {

/// \brief Interface used to access different executors used in the CU-CP.
class cu_cp_executor_mapper
{
public:
  virtual ~cu_cp_executor_mapper() = default;

  /// \brief Gets sequential task executor that used for processing procedures and other CU-CP tasks.
  virtual task_executor& ctrl_executor() = 0;

  /// \brief Gets sequential task executor that used for E2 tasks associated with the CU-CP.
  virtual task_executor& e2_executor() = 0;

  /// \brief Gets task executor used to parallelize the reception of N2 messages.
  virtual task_executor& n2_rx_executor() = 0;
};

/// Configuration of a cu_cp_executor_mapper that instantiates strands associated with the same thread pool.
struct strand_based_executor_config {
  /// \brief Executor to which CU-CP strands will be associated.
  task_executor& pool_executor;
};

/// \brief Creates an executor mapper for the CU-CP that is based on strands of a worker pool.
std::unique_ptr<cu_cp_executor_mapper> make_cu_cp_executor_mapper(const strand_based_executor_config& config);

} // namespace srs_cu_cp
} // namespace srsran
