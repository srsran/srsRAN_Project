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
