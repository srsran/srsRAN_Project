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

#include "srsgnb/adt/strong_type.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>

namespace srsgnb {

/// \brief GTP Tunnel Identifier.
struct gtp_teid_tag {};
using gtp_teid_t = strong_type<uint32_t, struct gtp_teid_tag, strong_equality, strong_equality_with<uint32_t>>;

/// \brief Convert integer to GTP TEID value.
inline gtp_teid_t int_to_gtp_teid(uint32_t teid_val)
{
  return gtp_teid_t{teid_val};
}

/// \brief Representation of an Transport Layer Address.
class transport_layer_address
{
public:
  transport_layer_address() = default;
  transport_layer_address(const std::string& ip_str) { from_string(ip_str); }

  transport_layer_address& from_string(const std::string& ip_str)
  {
    struct addrinfo* results;

    int err = getaddrinfo(ip_str.c_str(), nullptr, nullptr, &results);
    srsgnb_assert(err == 0, "Getaddrinfo error: {} - {}", ip_str, gai_strerror(err));

    // store address
    memcpy(&addr, results->ai_addr, results->ai_addrlen);

    freeaddrinfo(results);

    return *this;
  }

  std::string to_string() const { return fmt::format("{}", *this); }

  transport_layer_address& from_bitstring(std::string bit_str)
  {
    srsgnb_assert(bit_str.length() < 160, "Combined IPv4 and IPv6 addresses are currently not supported");

    std::string ip_str;

    if (bit_str.length() == 32) { // see see TS 38.414: 32 bits in case of IPv4 address according to IETF RFC 791
      ip_str = fmt::format("{}.{}.{}.{}",
                           std::stoi(bit_str.substr(0, 8), nullptr, 2),
                           std::stoi(bit_str.substr(8, 8), nullptr, 2),
                           std::stoi(bit_str.substr(16, 8), nullptr, 2),
                           std::stoi(bit_str.substr(24, 8), nullptr, 2));
    } else if (bit_str.length() ==
               128) { // see see TS 38.414: 128 bits in case of IPv6 address according to IETF RFC 8200
      ip_str = fmt::format("{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}",
                           std::stoi(bit_str.substr(0, 16), nullptr, 2),
                           std::stoi(bit_str.substr(16, 16), nullptr, 2),
                           std::stoi(bit_str.substr(32, 16), nullptr, 2),
                           std::stoi(bit_str.substr(48, 16), nullptr, 2),
                           std::stoi(bit_str.substr(64, 16), nullptr, 2),
                           std::stoi(bit_str.substr(80, 16), nullptr, 2),
                           std::stoi(bit_str.substr(96, 16), nullptr, 2),
                           std::stoi(bit_str.substr(112, 16), nullptr, 2));

    } else { // see see TS 38.414: 160 bits if both IPv4 and IPv6 addresses are signalled, in which case the IPv4
             // address is contained in the first 32 bits
      // TODO: Support 160 bit transport layer addresses
    }

    return from_string(ip_str);
  }

  std::string to_bitstring() const
  {
    std::string bitstr;

    char ip_addr[NI_MAXHOST];
    getnameinfo((sockaddr*)&addr, sizeof(addr), ip_addr, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);

    if (((sockaddr*)&addr)->sa_family == AF_INET) {
      std::string ip_str = ip_addr;
      std::string delim  = ".";

      auto start = 0U;
      auto end   = ip_str.find(delim);
      while (end != std::string::npos) {
        bitstr = bitstr + fmt::format("{:08b}", std::stoul(ip_str.substr(start, end - start)));
        start  = end + delim.length();
        end    = ip_str.find(delim, start);
      }
      bitstr = bitstr + fmt::format("{:08b}", std::stoul(ip_str.substr(start, end)));
    } else {
      std::string ip_str = ip_addr;
      std::string delim  = ":";

      auto start = 0U;
      auto end   = ip_str.find(delim);
      while (end != std::string::npos) {
        bitstr = bitstr + fmt::format("{:016b}", std::stoul(ip_str.substr(start, end - start), nullptr, 16));
        start  = end + delim.length();
        end    = ip_str.find(delim, start);
      }
      bitstr = bitstr + fmt::format("{:016b}", std::stoul(ip_str.substr(start, end), nullptr, 16));
    }

    return bitstr;
  }

  const struct sockaddr_storage& native() const { return addr; }
  struct sockaddr_storage&       native() { return addr; }

  bool operator==(const transport_layer_address& other) const
  {
    for (long unsigned int i = 0; i < sizeof(((sockaddr*)&addr)->sa_data); i++) {
      if (((sockaddr*)&addr)->sa_data[i] != ((sockaddr*)&other)->sa_data[i]) {
        return false;
      }
    }

    return true;
  }
  bool operator!=(const transport_layer_address& other) const { return not(*this == other); }
  bool operator==(const std::string& ip_str) const { return *this == transport_layer_address(ip_str); }
  bool operator!=(const std::string& ip_str) const { return not(*this == ip_str); }

private:
  struct sockaddr_storage addr;
};

/// \brief Identifier for F1 transport layer associated to a DRB.
struct up_transport_layer_info {
  transport_layer_address tp_address{"0.0.0.0"};
  gtp_teid_t              gtp_teid;
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
  ret.gtp_teid = int_to_gtp_teid(asn1obj.gtp_tunnel().gtp_teid.to_number());
  ret.tp_address.from_bitstring(asn1obj.gtp_tunnel().transport_layer_address.to_string());
  return ret;
}

} // namespace srsgnb

namespace fmt {

template <>
struct formatter<srsgnb::transport_layer_address> : public formatter<std::string> {
  template <typename FormatContext>
  auto format(const srsgnb::transport_layer_address& s, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    char ip_addr[NI_MAXHOST];
    getnameinfo((sockaddr*)&s, sizeof(s), ip_addr, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);

    return format_to(ctx.out(), "{}", ip_addr);
  }
};

} // namespace fmt