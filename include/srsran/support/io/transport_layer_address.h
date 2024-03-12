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

#include "fmt/format.h"
#include <netdb.h>
#include <string>
#include <sys/socket.h>

namespace srsran {

/// \brief Representation of an Transport Layer Address.
class transport_layer_address
{
public:
  /// Underlying native type used to store a transport layer address.
  using native_type = ::sockaddr_storage;

  transport_layer_address() = default;

  /// Creates a transport_layer_address object from a string with an IPv4 address with format X.X.X.X or with an IPv6
  /// address with format X:X:X:X:X:X:X:X.
  static transport_layer_address create_from_string(const std::string& ip_str);

  /// Creates a transport_layer_address object from a string of bits (each character is base 2).
  static transport_layer_address create_from_bitstring(const std::string& bit_str);

  /// Converts the transport_layer_address to an IPv4 or IPv6 string.
  std::string to_string() const { return fmt::format("{}", *this); }

  /// Converts the transport layer address to a string of bits (each character is base 2).
  std::string to_bitstring() const;

  /// Extracts the POSIX representation of the transport layer address.
  const native_type& native() const { return addr; }
  native_type&       native() { return addr; }

  /// Compares two transport_layer_addresses.
  bool operator==(const transport_layer_address& other) const;
  bool operator!=(const transport_layer_address& other) const { return not(*this == other); }
  bool operator==(const std::string& ip_str) const { return *this == create_from_string(ip_str); }
  bool operator!=(const std::string& ip_str) const { return not(*this == ip_str); }

  bool operator<(const transport_layer_address& other) const;
  bool operator<=(const transport_layer_address& other) const { return *this < other or *this == other; }
  bool operator>=(const transport_layer_address& other) const { return not(*this < other); }
  bool operator>(const transport_layer_address& other) const { return not(*this <= other); }

private:
  explicit transport_layer_address(const native_type& addr_) : addr(addr_) {}

private:
  native_type addr;
};

} // namespace srsran

namespace std {
template <>
struct hash<srsran::transport_layer_address> {
  size_t operator()(const srsran::transport_layer_address& s) const noexcept { return hash<string>{}(s.to_string()); }
};
} // namespace std

namespace fmt {

template <>
struct formatter<srsran::transport_layer_address> : public formatter<std::string> {
  template <typename FormatContext>
  auto format(const srsran::transport_layer_address& s, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    char ip_addr[NI_MAXHOST];
    if (::getnameinfo(reinterpret_cast<const sockaddr*>(&s.native()),
                      sizeof(srsran::transport_layer_address::native_type),
                      ip_addr,
                      sizeof(ip_addr),
                      nullptr,
                      0,
                      NI_NUMERICHOST) != 0) {
      return format_to(ctx.out(), "invalid_addr");
    }

    return format_to(ctx.out(), "{}", ip_addr);
  }
};

} // namespace fmt
