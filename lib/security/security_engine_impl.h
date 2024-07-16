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

#include "srsran/security/ciphering_engine.h"
#include "srsran/security/integrity_engine.h"
#include "srsran/security/security.h"
#include "srsran/security/security_engine.h"

namespace srsran {
namespace security {

class security_engine_impl final : public security_engine_tx, public security_engine_rx
{
public:
  security_engine_impl(sec_128_as_config  sec_cfg,
                       uint8_t            bearer_id,
                       security_direction direction,
                       integrity_enabled  integrity_enabled,
                       ciphering_enabled  ciphering_enabled);
  ~security_engine_impl() = default;

  security_result encrypt_and_protect_integrity(byte_buffer buf, size_t offset, uint32_t count) override;
  security_result decrypt_and_verify_integrity(byte_buffer buf, size_t offset, uint32_t count) override;

private:
  std::unique_ptr<integrity_engine> integ_eng;
  std::unique_ptr<ciphering_engine> cipher_eng;
};

} // namespace security
} // namespace srsran
