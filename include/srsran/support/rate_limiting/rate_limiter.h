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

#include <cstdint>

namespace srsran {

/// \brief Interface for rate limiting classes.
class rate_limiter
{
public:
  virtual ~rate_limiter()               = default;
  virtual bool consume(uint32_t tokens) = 0;
  virtual void stop()                   = 0;
};

} // namespace srsran
