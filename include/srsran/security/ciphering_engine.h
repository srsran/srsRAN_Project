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

struct ciphering_result {
  expected<byte_buffer> buf;
  uint32_t              count;
};

class ciphering_engine
{
public:
  virtual ~ciphering_engine() = default;

  virtual security_result apply_ciphering(byte_buffer buf, size_t offset, uint32_t count) = 0;
};

} // namespace security
} // namespace srsran
