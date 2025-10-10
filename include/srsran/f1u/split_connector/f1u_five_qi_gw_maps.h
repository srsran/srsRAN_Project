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

  void add_gtpu_gateway(std::optional<uint8_t>        sst,
                        std::optional<uint32_t>       sd,
                        std::optional<five_qi_t>      five_qi,
                        std::unique_ptr<gtpu_gateway> gtpu_gw)
  {
    if (not five_qi.has_value() || not sst.has_value()) {
      default_gws.push_back(std::move(gtpu_gw));
    } else {
      s_nssai_t s_nssai{slice_service_type{*sst}, {}};
      if (sd.has_value()) {
        auto exp = slice_differentiator::create(*sd);
        if (not exp.has_value()) {
          report_error("Invalid F1U socket configuration. SD is invalid");
        }
        s_nssai.sd = *exp;
      }
      gw_maps[s_nssai][five_qi.value()].push_back(std::move(gtpu_gw));
    }
  }
};

} // namespace srsran
