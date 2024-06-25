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

#include <cstdint>
#include <optional>

namespace srsran::srs_cu_up {

/// \brief Public interface to the main CU-UP class
class cu_up_interface
{
public:
  virtual ~cu_up_interface() = default;

  virtual void start() = 0;

  /// \brief Stop the CU-UP. This call is blocking and only returns once all tasks in the CU-UP are completed.
  virtual void stop() = 0;

  /// \brief Get the N3 bind port.
  virtual std::optional<uint16_t> get_n3_bind_port() = 0;
};

} // namespace srsran::srs_cu_up
