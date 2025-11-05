/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
        expected<slice_differentiator> exp = slice_differentiator::create(*sd);
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
