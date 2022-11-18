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

#include "drb_context.h"
#include "srsgnb/asn1/e1ap.h"

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

  uint16_t                       pdu_session_id;
  asn1::e1ap::pdu_session_type_e session_type;
  asn1::e1ap::snssai_s           snssai;
  asn1::e1ap::security_ind_s     security_ind;
  uint64_t                       pdu_session_res_ambr = 0;
  asn1::e1ap::up_tnl_info_c      tunnel_info; // used by all DRBs/QoS flows

  drb_context* default_drb = nullptr; // non-owning pointer to default DRB, if any

  std::map<int16_t, std::unique_ptr<drb_context>> drbs; // key is drb_id
};

} // namespace srs_cu_up
} // namespace srsgnb
