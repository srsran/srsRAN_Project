/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "mbedtls/aes.h"
#include "srsran/security/integrity_engine.h"
#include "srsran/security/security.h"

namespace srsran {
namespace security {

class integrity_engine_nia2_non_cmac final : public integrity_engine
{
public:
  integrity_engine_nia2_non_cmac(sec_128_key k_128_int_, uint8_t bearer_id_, security_direction direction_);
  ~integrity_engine_nia2_non_cmac() = default;

  security_result protect_integrity(byte_buffer buf, uint32_t count) override;
  security_result verify_integrity(byte_buffer buf, uint32_t count) override;

private:
  expected<security::sec_mac, security_error> compute_mac(const byte_buffer_view v, uint32_t count);

  sec_128_key        k_128_int;
  uint8_t            bearer_id;
  security_direction direction;

  mbedtls_aes_context ctx;
  uint8_t             k1[16];
  uint8_t             k2[16];

  std::array<uint8_t, sec_max_pdu_size>    msg_buf;
  static constexpr std::array<uint8_t, 16> zeros = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};

} // namespace security
} // namespace srsran
