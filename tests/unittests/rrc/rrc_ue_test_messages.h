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

#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"

namespace srsran {
namespace srs_cu_cp {

/// Converts a hex string (e.g. 01FA02) to a sec_as_key.
security::sec_as_key make_sec_as_key(std::string hex_str);

/// Converts a hex string (e.g. 01FA02) to a sec_128_as_key.
security::sec_128_as_key make_sec_128_as_key(std::string hex_str);

/// \brief Constructs full RRC Reconfig request with radioBearerConfig, masterCellGroup and NAS PDU
cu_cp_rrc_reconfiguration_procedure_request generate_rrc_reconfiguration_procedure_request();

} // namespace srs_cu_cp
} // namespace srsran
