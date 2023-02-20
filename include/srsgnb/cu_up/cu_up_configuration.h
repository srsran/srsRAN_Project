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

#include "srsgnb/e1ap/common/e1ap_common.h"
#include "srsgnb/f1u/cu_up/f1u_gateway.h"
#include "srsgnb/gtpu/gtpu_config.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/io_broker/io_broker.h"

namespace srsgnb {
namespace srs_cu_up {

struct network_interface_config {
  /// Port of UPF for NG-U connection (TODO: Refactor to use UPF port that we get from E1).
  int upf_port = GTPU_PORT; // TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs

  /// Local IP address to bind for connection from UPF to receive downlink user-plane traffic (N3 interface).
  std::string n3_bind_addr = "127.0.1.1";

  /// Local port to bind for connection from UPF to receive downlink user-plane traffic (N3 interface).
  int n3_bind_port = GTPU_PORT; // TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs

  /// Local IP address to bind for connection from DU to receive uplink user-plane traffic.
  std::string f1u_bind_addr = "127.0.2.1";

  /// Local port to bind for connection from DU to receive uplink user-plane traffic.
  int f1u_bind_port = GTPU_PORT;
};

/// Configuration passed to CU-UP.
struct cu_up_configuration {
  task_executor*         cu_up_executor    = nullptr;
  task_executor*         gtpu_pdu_executor = nullptr;
  e1ap_message_notifier* e1ap_notifier     = nullptr; ///< Callback for incoming E1AP messages.
  f1u_cu_up_gateway*     f1u_gateway       = nullptr;
  io_broker*             epoll_broker      = nullptr; ///< IO broker to receive messages from a network gateway

  network_interface_config net_cfg;

  unsigned    cu_up_id   = 0;
  std::string cu_up_name = "srs_cu_up_01";
  std::string plmn; ///< Full PLMN as string (without possible filler digit) e.g. "00101"
};

} // namespace srs_cu_up
} // namespace srsgnb

namespace fmt {

// Network config formatter
template <>
struct formatter<srsgnb::srs_cu_up::network_interface_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::srs_cu_up::network_interface_config cfg, FormatContext& ctx)
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
