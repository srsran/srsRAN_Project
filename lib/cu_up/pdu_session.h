/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "adapters/gtpu_adapters.h"
#include "adapters/sdap_adapters.h"
#include "drb_context.h"
#include "srsran/asn1/e1ap/e1ap.h"
#include "srsran/gtpu/gtpu_tunnel_factory.h"
#include "srsran/ran/up_transport_layer_info.h"

namespace srsran {
namespace srs_cu_up {

#define MAX_NUM_PDU_SESSIONS_PER_UE (8) /// Todo: find 3GPP spec reference

/// \brief Context for PDU session with session-wide parameters and all contained DRBs.
struct pdu_session {
  pdu_session(const e1ap_pdu_session_res_to_setup_item& session) :
    pdu_session_id(session.pdu_session_id),
    session_type(session.pdu_session_type),
    snssai(session.snssai),
    security_ind(session.security_ind),
    ul_tunnel_info(session.ng_ul_up_tnl_info)
  {
    if (session.pdu_session_res_dl_ambr.has_value())
      pdu_session_res_ambr = session.pdu_session_res_dl_ambr.value();
  };

  std::unique_ptr<sdap_entity> sdap;
  std::unique_ptr<gtpu_tunnel> gtpu;

  // Adapters between SDAP and GTPU
  gtpu_sdap_adapter gtpu_to_sdap_adapter;
  sdap_gtpu_adapter sdap_to_gtpu_adapter;

  // Adapter SDAP->PDCP
  // FIXME: Currently, we assume only one DRB per PDU session and only one QoS flow per DRB.
  sdap_pdcp_adapter sdap_to_pdcp_adapter;
  pdcp_f1u_adapter  pdcp_to_f1u_adapter;
  pdu_session_id_t  pdu_session_id; //< PDU session ID (0-255)
  std::string       session_type;
  s_nssai_t         snssai;
  e1ap_security_ind security_ind;
  uint64_t          pdu_session_res_ambr = 0;

  // Tunneling info used by all DRBs/QoS flows in this PDU session
  up_transport_layer_info ul_tunnel_info; // the peer GTP-U address and TEID
  uint32_t                local_teid;     // the local teid used by the gNB for this PDU session

  drb_context* default_drb = nullptr; // non-owning pointer to default DRB, if any

  std::map<drb_id_t, std::unique_ptr<drb_context>> drbs; // key is drb_id
};

} // namespace srs_cu_up
} // namespace srsran
