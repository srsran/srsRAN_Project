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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/srslog/srslog.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

struct cu_up_processor_config_t {
  std::string           name                  = "srs_cu_cp";
  cu_up_index_t         cu_up_index           = cu_up_index_t::invalid;
  unsigned              max_nof_supported_ues = MAX_NOF_CU_UES;
  srslog::basic_logger& logger                = srslog::fetch_basic_logger("CU-CP");
};

} // namespace srs_cu_cp
} // namespace srsran
