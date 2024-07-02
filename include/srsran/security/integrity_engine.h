/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "security_engine.h"

namespace srsran {
namespace security {

class integrity_engine
{
public:
  virtual ~integrity_engine() = default;

  virtual security_result protect_integrity(byte_buffer buf, uint32_t count) = 0;
  virtual security_result verify_integrity(byte_buffer buf, uint32_t count)  = 0;
};

} // namespace security
} // namespace srsran
