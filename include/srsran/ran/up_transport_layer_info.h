/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/support/network/transport_layer_address.h"

namespace srsran {

/// \brief Identifier for F1-U transport layer associated to a DRB.
struct up_transport_layer_info {
  transport_layer_address tp_address{"0.0.0.0"};
  gtpu_teid_t             gtp_teid;

  bool operator==(const up_transport_layer_info& other) const
  {
    return tp_address == other.tp_address and gtp_teid == other.gtp_teid;
  }
  bool operator!=(const up_transport_layer_info& other) const { return not(*this == other); }
  bool operator<(const up_transport_layer_info& other) const
  {
    return gtp_teid < other.gtp_teid or (gtp_teid == other.gtp_teid and tp_address < other.tp_address);
  }
};

/// \brief Converts type \c up_transport_layer_info to an ASN.1 type.
/// \param asn1obj ASN.1 object where the result of the conversion is stored.
/// \param up_tp_layer_info UP Transport Layer Info object.
template <typename Asn1Type>
void up_transport_layer_info_to_asn1(Asn1Type& asn1obj, const up_transport_layer_info& up_tp_layer_info)
{
  asn1obj.set_gtp_tunnel();
  asn1obj.gtp_tunnel().gtp_teid.from_number(up_tp_layer_info.gtp_teid.value());
  asn1obj.gtp_tunnel().transport_layer_address.from_string(up_tp_layer_info.tp_address.to_bitstring());
}

/// \brief Converts ASN.1 type into \c up_transport_layer_info.
/// \param asn1obj ASN.1 object which is going to be converted.
/// \return UP Transport Layer Info object where the result of the conversion is stored.
template <typename Asn1Type>
up_transport_layer_info asn1_to_up_transport_layer_info(Asn1Type& asn1obj)
{
  up_transport_layer_info ret;
  ret.gtp_teid = int_to_gtpu_teid(asn1obj.gtp_tunnel().gtp_teid.to_number());
  ret.tp_address.from_bitstring(asn1obj.gtp_tunnel().transport_layer_address.to_string());
  return ret;
}

} // namespace srsran

template <>
struct std::hash<srsran::up_transport_layer_info> {
  std::size_t operator()(const srsran::up_transport_layer_info& s) const noexcept
  {
    return (std::hash<srsran::transport_layer_address>{}(s.tp_address) ^
            (std::hash<srsran::gtpu_teid_t::value_type>{}(s.gtp_teid.value()) << 1U) >> 1U);
  }
};
