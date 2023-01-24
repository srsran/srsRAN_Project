/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/e1/cu_cp/e1_cu_cp.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Converts a hex string (e.g. 01FA02) to a sec_as_key.
security::sec_as_key make_sec_as_key(std::string hex_str);

/// Converts a hex string (e.g. 01FA02) to a sec_128_as_key.
security::sec_128_as_key make_sec_128_as_key(std::string hex_str);

/// \brief Constructs full RRC Reconfig request with radioBearerConfig, masterCellGroup and NAS PDU
cu_cp_rrc_reconfiguration_procedure_request generate_rrc_reconfiguration_procedure_request();

} // namespace srs_cu_cp
} // namespace srsgnb
