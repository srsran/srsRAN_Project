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

#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/plmn_identity.h"
#include <chrono>
#include <string>

namespace srsran {

namespace srs_cu_cp {

/// \brief NGAP context
struct ngap_context_t {
  gnb_id_t                             gnb_id = {0, 22};
  std::string                          ran_node_name;
  std::string                          amf_name;
  amf_index_t                          amf_index;
  std::vector<supported_tracking_area> supported_tas;
  std::vector<guami_t>                 served_guami_list;
  uint16_t                             default_paging_drx = 256;    // default paging drx
  std::chrono::seconds                 request_pdu_session_timeout; // timeout for requesting a PDU session in seconds
  byte_buffer                          lmf_routing_id;

  std::vector<plmn_identity> get_supported_plmns() const
  {
    std::vector<plmn_identity> supported_plmns;
    for (const auto& ta : supported_tas) {
      for (const auto& plmn : ta.plmn_list) {
        supported_plmns.push_back(plmn.plmn_id);
      }
    }

    return supported_plmns;
  }
};

} // namespace srs_cu_cp

} // namespace srsran
