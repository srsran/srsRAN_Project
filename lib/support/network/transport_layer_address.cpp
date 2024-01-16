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

#include "srsran/support/io/transport_layer_address.h"
#include "srsran/support/srsran_assert.h"
#include <netinet/in.h>
#include <string>

using namespace srsran;

transport_layer_address& transport_layer_address::from_string(const std::string& ip_str)
{
  struct addrinfo* results;

  int err = getaddrinfo(ip_str.c_str(), nullptr, nullptr, &results);
  srsran_assert(err == 0, "Getaddrinfo error: {} - {}", ip_str, gai_strerror(err));

  // store address
  memcpy(&addr, results->ai_addr, results->ai_addrlen);

  freeaddrinfo(results);

  return *this;
}

transport_layer_address& transport_layer_address::from_bitstring(std::string bit_str)
{
  srsran_assert(bit_str.length() < 160, "Combined IPv4 and IPv6 addresses are currently not supported");

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

std::string transport_layer_address::to_bitstring() const
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

bool transport_layer_address::operator==(const transport_layer_address& other) const
{
  for (long unsigned int i = 0; i < sizeof(((sockaddr*)&addr)->sa_data); i++) {
    if (((sockaddr*)&addr)->sa_data[i] != ((sockaddr*)&other)->sa_data[i]) {
      return false;
    }
  }

  return true;
}

bool transport_layer_address::operator<(const transport_layer_address& other) const
{
  for (long unsigned int i = 0; i < sizeof(((sockaddr*)&addr)->sa_data); i++) {
    if (((sockaddr*)&addr)->sa_data[i] < ((sockaddr*)&other)->sa_data[i]) {
      return true;
    }
  }
  return false;
}
