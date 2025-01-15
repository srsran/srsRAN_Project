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

#include <vector>

namespace srslog {

class sink;

/// Contains the registered sinks used by the loggers.
class sink_repository
{
public:
  virtual ~sink_repository() = default;

  /// Returns the contents of the sink repository.
  virtual std::vector<sink*> contents() = 0;
};

} // namespace srslog
