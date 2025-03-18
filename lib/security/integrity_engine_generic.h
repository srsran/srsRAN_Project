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

#include "srsran/security/integrity_engine.h"
#include "srsran/security/security.h"

namespace srsran {
namespace security {

class integrity_engine_generic final : public integrity_engine
{
public:
  integrity_engine_generic(sec_128_key         k_128_int_,
                           uint8_t             bearer_id_,
                           security_direction  direction_,
                           integrity_algorithm integ_algo_);
  ~integrity_engine_generic() = default;

  security_result protect_integrity(byte_buffer buf, uint32_t count) override;
  security_result verify_integrity(byte_buffer buf, uint32_t count) override;

private:
  sec_128_key         k_128_int;
  uint8_t             bearer_id;
  security_direction  direction;
  integrity_algorithm integ_algo;

  srslog::basic_logger& logger;
};

} // namespace security
} // namespace srsran
