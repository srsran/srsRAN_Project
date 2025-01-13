/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "fmt/format.h"
#include <cstdint>

namespace srsran {
namespace security {

enum class security_error { buffer_failure, engine_failure, integrity_failure, ciphering_failure };
inline const char* to_string(security_error sec_err)
{
  switch (sec_err) {
    case security_error::buffer_failure:
      return "buffer failure";
    case security_error::engine_failure:
      return "engine failure";
    case security_error::integrity_failure:
      return "integrity failure";
    case security_error::ciphering_failure:
      return "ciphering failure";
    default:
      return "invalid";
  }
}

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

namespace fmt {

template <>
struct formatter<srsran::security::security_error> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::security::security_error sec_err, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}", to_string(sec_err));
  }
};
} // namespace fmt
