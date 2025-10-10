/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1u_cli11_validator.h"
#include "f1u_appconfig.h"

using namespace srsran;

bool srsran::validate_f1u_sockets_appconfig(const f1u_sockets_appconfig& config)
{
  for (const f1u_socket_appconfig& f1u_socket : config.f1u_socket_cfg) {
    if (f1u_socket.sst.has_value() && not f1u_socket.five_qi.has_value()) {
      if (f1u_socket.sd.has_value()) {
        fmt::println(
            "F1-U socket has S-NSSAI configured, but no associated 5QI. Please, configure 5QI too. sst={} sd={}",
            *f1u_socket.sst,
            *f1u_socket.sd);
      } else {
        fmt::println("F1-U socket has S-NSSAI configured, but no associated 5QI. Please, configure 5QI too. sst={}",
                     *f1u_socket.sst);
      }
      return false;
    }
    if (f1u_socket.five_qi.has_value() && not f1u_socket.sst.has_value()) {
      fmt::println("F1-U socket has 5QI configured, but no associated S-NSSAI. Please, configure the SST too and "
                   "(optionaly) the SD. {}",
                   *f1u_socket.five_qi);
      return false;
    }
  }
  return true;
}
