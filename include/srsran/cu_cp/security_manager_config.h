/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/security/security.h"

namespace srsran {
namespace srs_cu_cp {

struct security_manager_config {
  security::preferred_integrity_algorithms int_algo_pref_list; ///< Integrity protection algorithms preference list
  security::preferred_ciphering_algorithms enc_algo_pref_list; ///< Encryption algorithms preference list
};

} // namespace srs_cu_cp
} // namespace srsran
