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
#include "srsran/security/security.h"
#include "srsran/security/ssl.h"

namespace srsran {
namespace security {

class ciphering_engine_nea2 final : public ciphering_engine
{
public:
  ciphering_engine_nea2(sec_128_key k_128_enc, uint8_t bearer_id_, security_direction direction_);
  virtual ~ciphering_engine_nea2() = default;

  security_result apply_ciphering(byte_buffer buf, size_t offset, uint32_t count) override;

private:
  uint8_t            bearer_id;
  security_direction direction;

  aes_context ctx;
};

} // namespace security
} // namespace srsran
