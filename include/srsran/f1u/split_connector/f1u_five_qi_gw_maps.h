/*
 *
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gtpu/gtpu_gateway.h"
#include "srsran/ran/qos/five_qi.h"
#include "srsran/ran/s_nssai.h"
#include <map>

namespace srsran {

using f1u_gw_list        = std::vector<std::unique_ptr<gtpu_gateway>>;
using f1u_five_qi_gw_map = std::map<five_qi_t, f1u_gw_list>;
using f1u_s_nssai_gw_map = std::map<s_nssai_t, f1u_five_qi_gw_map>;

struct gtpu_gateway_maps {
  f1u_gw_list        default_gws;
  f1u_s_nssai_gw_map gw_maps;
};

} // namespace srsran
