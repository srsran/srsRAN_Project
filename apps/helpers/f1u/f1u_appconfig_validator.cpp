/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "f1u_appconfig_validator.h"
#include "f1u_appconfig.h"

using namespace srsran;

bool srsran::validate_f1u_sockets_appconfig(const f1u_sockets_appconfig& config)
{
  for (const f1u_socket_appconfig& f1u_socket : config.f1u_socket_cfg) {
    if (f1u_socket.sst.has_value() && not f1u_socket.five_qi.has_value()) {
      if (f1u_socket.sd.has_value()) {
        fmt::println(
            "F1-U socket has S-NSSAI configured, but no associated 5QI. Please, configure 5QI too. sst={} sd={:#x}",
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
