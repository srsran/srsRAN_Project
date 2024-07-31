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

#include "security_engine_impl.h"
#include "ciphering_engine_nea1.h"
#include "ciphering_engine_nea2.h"
#include "ciphering_engine_nea3.h"
#include "integrity_engine_generic.h"
#include "integrity_engine_nia2_cmac.h"
#include "integrity_engine_nia2_non_cmac.h"

using namespace srsran;
using namespace security;

security_engine_impl::security_engine_impl(security::sec_128_as_config sec_cfg,
                                           uint8_t                     bearer_id,
                                           security_direction          direction,
                                           security::integrity_enabled integrity_enabled,
                                           security::ciphering_enabled ciphering_enabled)
{
  if (integrity_enabled == security::integrity_enabled::on) {
    srsran_assert(sec_cfg.integ_algo.has_value(), "Cannot enable integrity protection: No algorithm selected");
    srsran_assert(sec_cfg.k_128_int.has_value(), "Cannot enable integrity protection: No key");
    switch (sec_cfg.integ_algo.value()) {
      case integrity_algorithm::nia2:
#ifdef MBEDTLS_CMAC_C
        integ_eng = std::make_unique<integrity_engine_nia2_cmac>(sec_cfg.k_128_int.value(), bearer_id, direction);
#else
        integ_eng = std::make_unique<integrity_engine_nia2_non_cmac>(sec_cfg.k_128_int.value(), bearer_id, direction);
#endif
        break;
      default:
        integ_eng = std::make_unique<integrity_engine_generic>(
            sec_cfg.k_128_int.value(), bearer_id, direction, sec_cfg.integ_algo.value());
        break;
    }
  }
  if (ciphering_enabled == security::ciphering_enabled::on) {
    switch (sec_cfg.cipher_algo) {
      case ciphering_algorithm::nea1:
        cipher_eng = std::make_unique<ciphering_engine_nea1>(sec_cfg.k_128_enc, bearer_id, direction);
        break;
      case ciphering_algorithm::nea2:
        cipher_eng = std::make_unique<ciphering_engine_nea2>(sec_cfg.k_128_enc, bearer_id, direction);
        break;
      case ciphering_algorithm::nea3:
        cipher_eng = std::make_unique<ciphering_engine_nea3>(sec_cfg.k_128_enc, bearer_id, direction);
        break;
      default:
        // no cipher_eng for NEA0
        break;
    }
  }
}

security_result security_engine_impl::encrypt_and_protect_integrity(byte_buffer buf, size_t offset, uint32_t count)
{
  security_result result{.buf = std::move(buf), .count = count};

  // apply integrity protection if activated
  if (integ_eng != nullptr) {
    result = integ_eng->protect_integrity(std::move(result.buf.value()), result.count);
    if (!result.buf.has_value()) {
      return result;
    }
  }

  // apply ciphering if activated
  if (cipher_eng != nullptr) {
    result = cipher_eng->apply_ciphering(std::move(result.buf.value()), offset, result.count);
  }

  return result;
}

security_result security_engine_impl::decrypt_and_verify_integrity(byte_buffer buf, size_t offset, uint32_t count)
{
  security_result result;
  return result;
}
