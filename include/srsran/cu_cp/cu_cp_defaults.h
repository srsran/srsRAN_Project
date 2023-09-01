/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/cu_types.h"

namespace srsran {
namespace srs_cu_cp {

// TODO: Read these values from config
constexpr security_indication_t get_default_security_indication()
{
  security_indication_t default_security_indication          = {};
  default_security_indication.integrity_protection_ind       = integrity_protection_indication_t::not_needed;
  default_security_indication.confidentiality_protection_ind = confidentiality_protection_indication_t::required;
  return default_security_indication;
}

} // namespace srs_cu_cp
} // namespace srsran
