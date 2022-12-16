/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "adapters/gtpu_adapters.h"
#include "adapters/sdap_adapters.h"
#include "drb_context.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/gtpu/gtpu_tunnel_factory.h"

namespace srsgnb {
namespace srs_cu_up {

#define MAX_NUM_PDU_SESSIONS_PER_UE (8) /// Todo: find 3GPP spec reference

/// \brief Context for PDU session with session-wide parameters and all contained DRBs.
struct pdu_session {
  pdu_session(const asn1::e1ap::pdu_session_res_to_setup_item_s& session) :
    pdu_session_id(session.pdu_session_id),
    session_type(session.pdu_session_type),
    snssai(session.snssai),
    security_ind(session.security_ind),
    pdu_session_res_ambr(session.pdu_session_res_dl_ambr),
    tunnel_info(session.ng_ul_up_tnl_info){};

  std::unique_ptr<sdap_entity> sdap;
  std::unique_ptr<gtpu_tunnel> gtpu;

  // Adapters between SDAP and GTPU
  gtpu_sdap_adapter gtpu_to_sdap_adapter;
  sdap_gtpu_adapter sdap_to_gtpu_adapter;

  // Adapter SDAP->PDCP
  // FIXME: Currently, we assume only one DRB per PDU session and only one QoS flow per DRB.
  sdap_pdcp_adapter sdap_to_pdcp_adapter;

  uint8_t                        pdu_session_id; //< PDU session ID (0-255)
  asn1::e1ap::pdu_session_type_e session_type;
  asn1::e1ap::snssai_s           snssai;
  asn1::e1ap::security_ind_s     security_ind;
  uint64_t                       pdu_session_res_ambr = 0;

  // Tunneling info used by all DRBs/QoS flows in this PDU session
  asn1::e1ap::up_tnl_info_c tunnel_info; // the peer GTP-U address and TEID
  uint32_t                  local_teid;  // the local teid used by the gNB for this PDU session

  drb_context* default_drb = nullptr; // non-owning pointer to default DRB, if any

  std::map<int16_t, std::unique_ptr<drb_context>> drbs; // key is drb_id
};

} // namespace srs_cu_up
} // namespace srsgnb
