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

#include "srsran/security/ciphering_engine.h"
#include "srsran/security/integrity_engine.h"
#include "srsran/security/security.h"
#include "srsran/security/security_engine.h"

namespace srsran {
namespace security {

class security_engine_generic : public security_engine
{
public:
  security_engine_generic(sec_128_as_config sec_cfg, uint8_t bearer_id, security_direction direction);
  ~security_engine_generic() = default;

  security_result encrypt_and_protect_integrity(byte_buffer buf, size_t offset, uint32_t count) override;
  security_result decrypt_and_verify_integrity(byte_buffer buf, size_t offset, uint32_t count) override;

private:
  std::unique_ptr<integrity_engine> integ_eng;
  std::unique_ptr<ciphering_engine> cipher_eng;
};

} // namespace security
} // namespace srsran
