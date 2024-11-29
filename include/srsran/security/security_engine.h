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
