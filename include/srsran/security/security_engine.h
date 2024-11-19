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

#include "srsran/adt/byte_buffer.h"
#include "fmt/format.h"
#include <cstdint>

namespace srsran {
namespace security {

enum class security_error { buffer_failure, engine_failure, integrity_failure, ciphering_failure };

struct security_result {
  expected<byte_buffer, security_error> buf;
  uint32_t                              count;
};

class security_engine_tx
{
public:
  virtual ~security_engine_tx() = default;

  virtual security_result encrypt_and_protect_integrity(byte_buffer buf, size_t offset, uint32_t count) = 0;
};

class security_engine_rx
{
public:
  virtual ~security_engine_rx() = default;

  virtual security_result decrypt_and_verify_integrity(byte_buffer buf, size_t offset, uint32_t count) = 0;
};

} // namespace security
} // namespace srsran
