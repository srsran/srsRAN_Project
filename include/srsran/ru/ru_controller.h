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

/// \brief Radio Unit - control interface.
///
/// Allows starting and stopping the Radio Unit.
///
/// \remark The methods \c start() and \c stop() must be called in order and only once; other uses will result in
/// undefined behavior.
class ru_controller
{
public:
  /// Default destructor.
  virtual ~ru_controller() = default;

  /// \brief Starts the Radio Unit operation.
  virtual void start() = 0;

  /// \brief Stops the Radio Unit operation.
  ///
  /// Waits for all asynchronous processes to be over before returning.
  virtual void stop() = 0;
};

} // namespace srsran
