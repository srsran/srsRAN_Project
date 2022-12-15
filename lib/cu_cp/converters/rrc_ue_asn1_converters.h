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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include <string>
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

inline asn1::e1ap::pdu_session_type_e cu_cp_pdu_session_type_to_e1ap_pdu_session_type(std::string cu_cp_session_type)
{
  asn1::e1ap::pdu_session_type_e e1ap_session_type;

  if (cu_cp_session_type == "ethernet") {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ethernet;
    return e1ap_session_type;
  } else if (cu_cp_session_type == "ipv4") {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ipv4;
    return e1ap_session_type;
  } else if (cu_cp_session_type == "ipv4v6") {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ipv4v6;
    return e1ap_session_type;
  } else if (cu_cp_session_type == "ipv6") {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::ipv6;
    return e1ap_session_type;
  } else {
    e1ap_session_type = asn1::e1ap::pdu_session_type_opts::options::nulltype;
    return e1ap_session_type;
  }
}

inline cu_cp_cause_t e1ap_cause_to_cu_cp_cause(asn1::e1ap::cause_c e1ap_cause)
{
  cu_cp_cause_t cu_cp_cause;

  switch (e1ap_cause.type()) {
    case asn1::e1ap::cause_c::types_opts::radio_network:
      cu_cp_cause = cu_cp_cause_t::radio_network;
      return cu_cp_cause;
      break;
    case asn1::e1ap::cause_c::types_opts::transport:
      cu_cp_cause = cu_cp_cause_t::transport;
      return cu_cp_cause;
      break;
    case asn1::e1ap::cause_c::types_opts::protocol:
      cu_cp_cause = cu_cp_cause_t::protocol;
      return cu_cp_cause;
      break;
    default:
      cu_cp_cause = cu_cp_cause_t::misc;
      return cu_cp_cause;
      break;
      break;
  }
}

} // namespace srs_cu_cp
} // namespace srsgnb
