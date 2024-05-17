/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "sockets.h"
#include "fmt/format.h"
#include <string>

namespace srsran {

/// \brief Representation of an Transport Layer Address.
class transport_layer_address
{
  using storage_type = ::sockaddr_storage;

public:
  /// Underlying native type used to store a transport layer address.
  struct native_type {
    struct sockaddr* addr;
    socklen_t        addrlen;
  };

  transport_layer_address() = default;

  /// Creates a transport_layer_address object from a string with an IPv4 address with format X.X.X.X or with an IPv6
  /// address with format X:X:X:X:X:X:X:X.
  static transport_layer_address create_from_string(const std::string& ip_str);

  /// Creates a transport_layer_address object from a string of bits (each character is base 2).
  static transport_layer_address create_from_bitstring(const std::string& bit_str);

  static transport_layer_address create_from_sockaddr(const struct sockaddr& sockaddr, socklen_t addr_len);
  static transport_layer_address create_from_sockaddr(native_type addr);

  /// Converts the transport_layer_address to an IPv4 or IPv6 string.
  std::string to_string() const { return fmt::format("{}", *this); }

  /// Converts the transport layer address to a string of bits (each character is base 2).
  std::string to_bitstring() const;

  /// Extracts the POSIX representation of the transport layer address.
  native_type native() const { return {(struct sockaddr*)&addr_storage, addrlen}; }

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
  explicit transport_layer_address(const struct sockaddr& addr_, socklen_t socklen);

  storage_type addr_storage;
  socklen_t    addrlen = 0;
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
    std::array<char, NI_MAXHOST> ip_addr;
    int                          port;
    auto                         native_repr = s.native();
    if (not srsran::getnameinfo(*native_repr.addr, native_repr.addrlen, ip_addr, port)) {
      return format_to(ctx.out(), "invalid");
    }
    if (port != 0) {
      return format_to(ctx.out(), "{}:{}", ip_addr.data(), port);
    }
    return format_to(ctx.out(), "{}", ip_addr.data());
  }
};

} // namespace fmt
