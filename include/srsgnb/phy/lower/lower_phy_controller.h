/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

/// Describes the lower physical layer control interface.
class lower_phy_controller
{
public:
  /// Default destructor.
  virtual ~lower_phy_controller() = default;

  /// \brief Starts the lower physical layer operation.
  ///
  /// \param[in] realtime_task_executor Provides a task executor for real time execution.
  /// \note The real time task executor is exclusively dedicated to the lower physical layer operation.
  virtual void start(task_executor& realtime_task_executor) = 0;

  /// Stops the lower physical layer operation.
  virtual void stop() = 0;
};

} // namespace srsgnb
