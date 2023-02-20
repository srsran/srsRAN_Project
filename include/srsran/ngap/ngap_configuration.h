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

#include "srsran/cu_cp/cu_cp_types.h"
#include <map>
#include <string>

namespace srsran {

namespace srs_cu_cp {

/// \brief NGAP configuration
struct ngap_configuration {
  unsigned    gnb_id = 0;
  std::string ran_node_name;
  std::string plmn; /// Full PLMN as string (without possible filler digit) e.g. "00101"
  unsigned    tac;

  std::map<uint8_t, cu_cp_qos_config> qos_config;
};

} // namespace srs_cu_cp

} // namespace srsran
