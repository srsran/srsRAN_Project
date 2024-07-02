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

#include "security_engine_generic.h"
#include "ciphering_engine_generic.h"
#include "integrity_engine_generic.h"

using namespace srsran;
using namespace security;

security_engine_generic::security_engine_generic(security::sec_128_as_config sec_cfg,
                                                 uint8_t                     bearer_id,
                                                 security_direction          direction)
{
  if (sec_cfg.integ_algo.has_value() && sec_cfg.k_128_int.has_value()) {
    integ_eng = std::make_unique<integrity_engine_generic>(
        sec_cfg.k_128_int.value(), bearer_id, direction, sec_cfg.integ_algo.value());
  }
  cipher_eng = std::make_unique<ciphering_engine_generic>(sec_cfg.k_128_enc, bearer_id, direction, sec_cfg.cipher_algo);
}

security_result security_engine_generic::encrypt_and_protect_integrity(byte_buffer buf, size_t offset, uint32_t count)
{
  security_result result{.buf = std::move(buf), .count = count};

  // apply integrity protection if activated
  if (integ_eng != nullptr) {
    result = integ_eng->protect_integrity(std::move(result.buf.value()), result.count);
    if (!result.buf.has_value()) {
      return result;
    }
  }

  // apply ciphering
  result = cipher_eng->apply_ciphering(std::move(result.buf.value()), offset, result.count);

  return result;
}

security_result security_engine_generic::decrypt_and_verify_integrity(byte_buffer buf, size_t offset, uint32_t count)
{
  security_result result;
  return result;
}
