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
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/ran/gtp_tunnel.h"
#include "srsgnb/ran/nr_cgi.h"
#include <string>
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

inline asn1::f1ap::uluptnl_info_to_be_setup_item_s
gtp_tunnel_to_f1ap_uluptnl_info_to_be_setup_item(const gtp_tunnel& gtp_tunnel)
{
  asn1::f1ap::uluptnl_info_to_be_setup_item_s uluptnl_item;
  uluptnl_item.uluptnl_info.set_gtp_tunnel();
  uluptnl_item.uluptnl_info.gtp_tunnel().gtp_teid.from_number(gtp_tunnel.gtp_teid);
  uluptnl_item.uluptnl_info.gtp_tunnel().transport_layer_address.from_string(gtp_tunnel.transport_layer_address);

  return uluptnl_item;
}

/// @brief Convert F1AP Cause to CU-CP Cause.
/// @param f1ap_cause The F1AP Cause.
/// @return The CU-CP Cause.
inline cu_cp_cause_t f1ap_cause_to_cu_cp_cause(asn1::f1ap::cause_c f1ap_cause)
{
  cu_cp_cause_t cu_cp_cause;

  switch (f1ap_cause.type()) {
    case asn1::f1ap::cause_c::types_opts::radio_network:
      cu_cp_cause = cu_cp_cause_t::radio_network;
      return cu_cp_cause;
      break;
    case asn1::f1ap::cause_c::types_opts::transport:
      cu_cp_cause = cu_cp_cause_t::transport;
      return cu_cp_cause;
      break;
    case asn1::f1ap::cause_c::types_opts::protocol:
      cu_cp_cause = cu_cp_cause_t::protocol;
      return cu_cp_cause;
      break;
    case asn1::f1ap::cause_c::types_opts::misc:
      cu_cp_cause = cu_cp_cause_t::misc;
      return cu_cp_cause;
      break;
    default:
      cu_cp_cause = cu_cp_cause_t::nulltype;
      return cu_cp_cause;
  }
}

inline gtp_tunnel
f1ap_dluptnl_info_to_be_setup_item_to_cu_cp_gtp_tunnel(asn1::f1ap::dluptnl_info_to_be_setup_item_s f1ap_item)
{
  gtp_tunnel gtp_tunnel;
  gtp_tunnel.gtp_teid                = f1ap_item.dluptnl_info.gtp_tunnel().gtp_teid.to_number();
  gtp_tunnel.transport_layer_address = f1ap_item.dluptnl_info.gtp_tunnel().transport_layer_address.to_string();

  return gtp_tunnel;
}

/// @brief Convert F1AP NRCGI to NR Cell Identity.
/// @param f1ap_cgi The F1AP NRCGI.
/// @return The NR Cell Identity.
inline nr_cell_id_t f1ap_nrcgi_to_nr_cell_identity(asn1::f1ap::nrcgi_s& f1ap_cgi)
{
  nr_cell_id_t nci;
  nci.packed = f1ap_cgi.nrcell_id.to_number();

  nci.cell_idenity = (f1ap_cgi.nrcell_id.to_number() & 0xfffc0000);
  nci.gnb_identity = (f1ap_cgi.nrcell_id.to_number() & 0X3ff0);

  return nci;
}

} // namespace srs_cu_cp
} // namespace srsgnb
