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

#include "mbedtls/cipher.h"
#include "mbedtls/cmac.h" // Nested include of MBEDTLS_CMAC_C from either config.h (v2) or mbedtls_config.h (v3)
#include "srsran/security/integrity_engine.h"
#include "srsran/security/security.h"

namespace srsran {
namespace security {

#ifdef MBEDTLS_CMAC_C

class integrity_engine_nia2_cmac final : public integrity_engine
{
public:
  integrity_engine_nia2_cmac(sec_128_key k_128_int_, uint8_t bearer_id_, security_direction direction_);
  ~integrity_engine_nia2_cmac();

  security_result protect_integrity(byte_buffer buf, uint32_t count) override;
  security_result verify_integrity(byte_buffer buf, uint32_t count) override;

private:
  expected<security::sec_mac, security_error> compute_mac(const byte_buffer_view v, uint32_t count);

  sec_128_key        k_128_int;
  uint8_t            bearer_id;
  security_direction direction;

  srslog::basic_logger&        logger;
  const mbedtls_cipher_info_t* cipher_info;
  mbedtls_cipher_context_t     ctx;
};

#endif // MBEDTLS_CMAC_C

} // namespace security
} // namespace srsran
