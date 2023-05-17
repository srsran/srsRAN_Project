/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

/// \brief Radio unit - control interface.
///
/// Provides a start and stop methods.
///
/// \remark The methods start() and stop() must be called in order and only once, otherwise it will result in undefined
/// behaviour.
class ru_controller
{
public:
  /// Default destructor.
  virtual ~ru_controller() = default;

  /// \brief Starts the Radio Unit operation.
  virtual void start() = 0;

  /// \brief Stops the Radio unit operation.
  ///
  /// Requests the Radio unit to stop and it waits that all asynchronous execution is finished before returning.
  virtual void stop() = 0;
};

} // namespace srsran
