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

#include "srsran/security/ciphering_engine.h"
#include "srsran/security/security.h"

namespace srsran {
namespace security {

class ciphering_engine_nea3 final : public ciphering_engine
{
public:
  ciphering_engine_nea3(sec_128_key k_128_enc_, uint8_t bearer_id_, security_direction direction_);
  virtual ~ciphering_engine_nea3() = default;

  security_result apply_ciphering(byte_buffer buf, size_t offset, uint32_t count) override;

private:
  sec_128_key        k_128_enc;
  uint8_t            bearer_id;
  security_direction direction;
};

} // namespace security
} // namespace srsran
