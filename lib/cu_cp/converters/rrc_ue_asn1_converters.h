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

/// @brief Convert a RRC Establishment Cause to a NGAP RRC Establishment Cause.
/// @param establishment_cause The RRC Establishment Cause.
/// @return The NGAP RRC Establishment Cause.
inline asn1::ngap::rrcestablishment_cause_opts rrc_establishment_cause_to_ngap_rrcestablishment_cause(
    const asn1::rrc_nr::establishment_cause_opts& establishment_cause)
{
  asn1::ngap::rrcestablishment_cause_opts rrcestablishment_cause = {};
  switch (establishment_cause.value) {
    case asn1::rrc_nr::establishment_cause_opts::options::emergency:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::emergency;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::high_prio_access:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::high_prio_access;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mt_access:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mt_access;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_sig:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_sig;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_data:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_data;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_voice_call:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_voice_call;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_video_call:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_video_call;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mo_sms:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mo_sms;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mps_prio_access:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mps_prio_access;
      break;
    case asn1::rrc_nr::establishment_cause_opts::options::mcs_prio_access:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::mcs_prio_access;
      break;
    default:
      rrcestablishment_cause.value = asn1::ngap::rrcestablishment_cause_opts::nulltype;
      break;
  }

  return rrcestablishment_cause;
}

/// @brief Convert CU-CP s-NSSAI to E1AP s-NSSAI.
/// @param cu_cp_snssai The CU-CP s-NSSAI.
/// @return The E1AP s-NSSAI.
inline asn1::e1ap::snssai_s common_snssai_to_e1ap_snssai(srsgnb::srs_cu_cp::cu_cp_s_nssai cu_cp_snssai)
{
  asn1::e1ap::snssai_s snssai;
  snssai.sst.from_number(cu_cp_snssai.sst);
  if (cu_cp_snssai.sd.has_value()) {
    snssai.sd_present = true;
    snssai.sd.from_number(cu_cp_snssai.sd.value());
  }

  return snssai;
}

/// @brief Convert CU-CP PDU session type to E1AP PDU session type.
/// @param cu_cp_session_type The CU-CP PDU session type.
/// @return The E1AP PDU session type.
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

/// @brief Convert CU-CP UL NGU UP TNL Information to E1AP UP TNL Information.
/// @param ul_ngu_up_tnl_info The CU-CP UL NGU UP TNL Information.
/// @return The E1AP UP TNL Information.
inline asn1::e1ap::up_tnl_info_c cu_cp_ul_ngu_up_tnl_info_to_e1ap_up_tnl_info(cu_cp_gtp_tunnel ul_ngu_up_tnl_info)
{
  asn1::e1ap::up_tnl_info_c e1ap_up_tnl_info;

  e1ap_up_tnl_info.set_gtp_tunnel();
  auto& e1ap_gtp_tunnel = e1ap_up_tnl_info.gtp_tunnel();
  e1ap_gtp_tunnel.gtp_teid.from_number(ul_ngu_up_tnl_info.gtp_teid);
  e1ap_gtp_tunnel.transport_layer_address.from_string(ul_ngu_up_tnl_info.transport_layer_address);

  return e1ap_up_tnl_info;
}

/// @brief Convert E1AP Cause to CU-CP Cause.
/// @param e1ap_cause The E1AP Cause.
/// @return The CU-CP Cause.
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

/// @brief Convert E1AP UP TNL Information to CU-CP Uptransport Layer Information.
/// @param e1ap_up_tnl_info The E1AP UP TNL Information.
/// @return The CU-CP Uptransport Layer Information.
inline cu_cp_gtp_tunnel cu_cp_uptransport_layer_info_from_e1ap_up_tnl_info(asn1::e1ap::up_tnl_info_c e1ap_up_tnl_info)
{
  cu_cp_gtp_tunnel cu_cp_uptransport_layer_info;

  cu_cp_uptransport_layer_info.gtp_teid = e1ap_up_tnl_info.gtp_tunnel().gtp_teid.to_number();
  cu_cp_uptransport_layer_info.transport_layer_address =
      e1ap_up_tnl_info.gtp_tunnel().transport_layer_address.to_string();

  return cu_cp_uptransport_layer_info;
}

/// @brief Convert E1AP UP Params Item to RRC UE GTP Tunnel.
/// @param up_param_item The E1AP UP Params Item.
/// @return The RRC UE GTP Tunnel.
inline rrc_ue_gtp_tunnel e1ap_up_param_item_to_rrc_ue_gtp_tunnel(asn1::e1ap::up_params_item_s up_param_item)
{
  rrc_ue_gtp_tunnel gtp_tunnel;
  gtp_tunnel.gtp_teid                = up_param_item.up_tnl_info.gtp_tunnel().gtp_teid.to_number();
  gtp_tunnel.transport_layer_address = up_param_item.up_tnl_info.gtp_tunnel().transport_layer_address.to_number();
  gtp_tunnel.cell_group_id           = up_param_item.cell_group_id;

  return gtp_tunnel;
}

inline asn1::f1ap::uluptnl_info_to_be_setup_item_s
rrc_ue_gtp_tunnel_to_f1ap_uluptnl_info_to_be_setup_item(rrc_ue_gtp_tunnel gtp_tunnel)
{
  asn1::f1ap::uluptnl_info_to_be_setup_item_s uluptnl_item;
  uluptnl_item.uluptnl_info.set_gtp_tunnel();
  uluptnl_item.uluptnl_info.gtp_tunnel().gtp_teid.from_number(gtp_tunnel.gtp_teid);
  uluptnl_item.uluptnl_info.gtp_tunnel().transport_layer_address.from_string(gtp_tunnel.transport_layer_address);

  return uluptnl_item;
}

} // namespace srs_cu_cp
} // namespace srsgnb
