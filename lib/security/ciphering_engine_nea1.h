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
#include "srsran/security/security.h"

namespace srsran {
namespace security {

class ciphering_engine_nea1 final : public ciphering_engine
{
public:
  ciphering_engine_nea1(sec_128_key k_128_enc_, uint8_t bearer_id_, security_direction direction_);
  virtual ~ciphering_engine_nea1() = default;

  security_result apply_ciphering(byte_buffer buf, size_t offset, uint32_t count) override;

private:
  sec_128_key        k_128_enc;
  uint8_t            bearer_id;
  security_direction direction;
};

} // namespace security
} // namespace srsran
