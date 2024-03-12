/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/gtpu/gtpu_teid.h"
#include "srsran/support/io/transport_layer_address.h"

namespace srsran {

/// \brief Identifier for F1-U transport layer associated to a DRB.
struct up_transport_layer_info {
  transport_layer_address tp_address = transport_layer_address::create_from_string("0.0.0.0");
  gtpu_teid_t             gtp_teid;

  up_transport_layer_info() = default;

  up_transport_layer_info(const transport_layer_address& tp_address_, gtpu_teid_t gtp_teid_) :
    tp_address(tp_address_), gtp_teid(gtp_teid_)
  {
  }

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
up_transport_layer_info asn1_to_up_transport_layer_info(const Asn1Type& asn1obj)
{
  return {transport_layer_address::create_from_bitstring(asn1obj.gtp_tunnel().transport_layer_address.to_string()),
          int_to_gtpu_teid(asn1obj.gtp_tunnel().gtp_teid.to_number())};
}

} // namespace srsran

namespace std {

template <>
struct hash<srsran::up_transport_layer_info> {
  size_t operator()(const srsran::up_transport_layer_info& s) const noexcept
  {
    return (hash<srsran::transport_layer_address>{}(s.tp_address) ^
            (hash<srsran::gtpu_teid_t::value_type>{}(s.gtp_teid.value()) << 1U) >> 1U);
  }
};

} // namespace std

namespace fmt {

template <>
struct formatter<srsran::up_transport_layer_info> : public formatter<std::string> {
  template <typename FormatContext>
  auto format(const srsran::up_transport_layer_info& s, FormatContext& ctx)
  {
    return format_to(ctx.out(), "{{Addr={} TEID={}}}", s.tp_address, s.gtp_teid);
  }
};

} // namespace fmt
