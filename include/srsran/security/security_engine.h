/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include <cstdint>

namespace srsran {
namespace security {

enum class security_error { buffer_failure, integrity_failure, ciphering_failure };

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
