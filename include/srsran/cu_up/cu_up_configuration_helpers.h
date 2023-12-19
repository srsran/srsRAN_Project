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

#include "srsran/cu_up/cu_up_types.h"
#include "srsran/ran/five_qi.h"
#include <map>

namespace srsran {
namespace config_helpers {

/// Generates default QoS configuration used by gNB CU-UP.
inline std::map<five_qi_t, srs_cu_up::cu_up_qos_config> make_default_cu_up_qos_config_list()
{
  std::map<five_qi_t, srs_cu_up::cu_up_qos_config> qos_list = {};
  {
    // 5QI=7
    srs_cu_up::cu_up_qos_config cfg{};
    cfg.pdcp_custom = pdcp_custom_config{}; // defaults are configured as member-initialization within the struct

    qos_list[uint_to_five_qi(7)] = cfg;
  }
  {
    // 5QI=9
    srs_cu_up::cu_up_qos_config cfg{};
    cfg.pdcp_custom = pdcp_custom_config{}; // defaults are configured as member-initialization within the struct

    qos_list[uint_to_five_qi(9)] = cfg;
  }
  return qos_list;
}

} // namespace config_helpers
} // namespace srsran
