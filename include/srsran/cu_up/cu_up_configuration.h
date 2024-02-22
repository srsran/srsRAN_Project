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

#include "srsran/cu_up/cu_up_executor_pool.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/e1ap/cu_up/e1ap_connection_client.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <map>

namespace srsran {

class io_broker;

namespace srs_cu_up {

struct network_interface_config {
  /// Port of UPF for NG-U connection (TODO: Refactor to use UPF port that we get from E1).
  int upf_port = GTPU_PORT; // TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs

  /// Local IP address to bind for connection from UPF to receive downlink user-plane traffic (N3 interface).
  std::string n3_bind_addr = "127.0.1.1";

  /// Interface name to bind the N3. `auto` does not force a specific interface and uses a normal `bind()`.
  std::string n3_bind_interface = "auto";

  /// Local port to bind for connection from UPF to receive downlink user-plane traffic (N3 interface).
  int n3_bind_port = GTPU_PORT; // TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs

  /// Maximum amount of packets received in a single syscall.
  int n3_rx_max_mmsg = 256;

  /// Local IP address to bind for connection from DU to receive uplink user-plane traffic.
  std::string f1u_bind_addr = "127.0.2.1";

  /// Local port to bind for connection from DU to receive uplink user-plane traffic.
  int f1u_bind_port = GTPU_PORT;
};

struct n3_interface_config {
  std::chrono::milliseconds gtpu_reordering_timer; // N3 reordering timer
  bool                      warn_on_drop;
};

struct e1ap_config_params {
  e1ap_connection_client*  e1ap_conn_client = nullptr;
  e1ap_connection_manager* e1ap_conn_mng    = nullptr;
};

/// Configuration passed to CU-UP.
struct cu_up_configuration {
  cu_up_executor_pool* ue_exec_pool   = nullptr;
  task_executor*       ctrl_executor  = nullptr; ///< CU-UP executor for control
  task_executor*       io_ul_executor = nullptr; ///< CU-UP executor for UL data IO
  task_executor*       cu_up_e2_exec  = nullptr;
  e1ap_config_params   e1ap;
  f1u_cu_up_gateway*   f1u_gateway  = nullptr;
  io_broker*           epoll_broker = nullptr; ///< IO broker to receive messages from a network gateway
  timer_manager*       timers       = nullptr;
  dlt_pcap*            gtpu_pcap    = nullptr;

  std::map<five_qi_t, cu_up_qos_config> qos; // 5QI as key

  network_interface_config net_cfg;
  n3_interface_config      n3_cfg;

  unsigned    cu_up_id   = 0;
  std::string cu_up_name = "srs_cu_up_01";
  std::string plmn       = "00101"; ///< Full PLMN as string (without possible filler digit) e.g. "00101"

  std::chrono::seconds statistics_report_period; // CU-UP statistics report period in seconds
};

} // namespace srs_cu_up
} // namespace srsran

namespace fmt {

// Network config formatter
template <>
struct formatter<srsran::srs_cu_up::network_interface_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_up::network_interface_config cfg, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "upf_port={}, n3_bind_addr={}, n3_bind_port={}, f1u_bind_addr={}, f1u_bind_port={}",
                     cfg.upf_port,
                     cfg.n3_bind_addr,
                     cfg.n3_bind_port,
                     cfg.f1u_bind_addr,
                     cfg.f1u_bind_port);
  }
};
} // namespace fmt
