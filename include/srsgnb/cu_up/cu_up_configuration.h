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

#include "srsgnb/e1/common/e1_common.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/f1u/cu_up/f1u_gateway.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/io_broker/io_broker.h"

namespace srsgnb {
namespace srs_cu_up {

struct network_interface_config {
  /// IP address of UPF for NG-U connection (TODO: Refactor to use UPF IP that we get from E1).
  std::string upf_addr = "0.0.0.0";

  /// Port of UPF for NG-U connection (TODO: Refactor to use UPF port that we get from E1).
  int upf_port = 2152; // TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs

  /// Local IP address to bind for connection from UPF to receive downlink user-plane traffic (N3 interface).
  std::string n3_bind_addr = "127.0.1.1";

  /// Local port to bind for connection from UPF to receive downlink user-plane traffic (N3 interface).
  int n3_bind_port = 2152; // TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs

  /// Local IP address to bind for connection from DU to receive uplink user-plane traffic.
  std::string f1u_bind_addr = "127.0.1.1";

  /// Local port to bind for connection from DU to receive uplink user-plane traffic.
  int f1u_bind_port = 2152; // TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs
};

/// Configuration passed to CU-UP.
struct cu_up_configuration {
  task_executor*       cu_up_executor = nullptr;
  e1_message_notifier* e1_notifier    = nullptr; ///< Callback for incoming E1 messages.
  f1u_cu_up_gateway*   f1u_gateway    = nullptr;
  io_broker*           epoll_broker   = nullptr; ///< IO broker to receive messages from a network gateway

  network_interface_config net_cfg;

  unsigned    cu_up_id   = 0;
  std::string cu_up_name = "srs_cu_up_01";
  std::string plmn; ///< Full PLMN as string (without possible filler digit) e.g. "00101"
};

} // namespace srs_cu_up
} // namespace srsgnb
