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

#include "srsran/cu_up/cu_up_executor_mapper.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/e1ap/gateways/e1_connection_client.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/ngu_gateway.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/timers.h"
#include <map>

namespace srsran {

class io_broker;

namespace srs_cu_up {

struct network_interface_config {
  /// Port of UPF for NG-U connection (TODO: Refactor to use UPF port that we get from E1).

  /// Local IP address to bind for connection from UPF to receive downlink user-plane traffic (N3 interface).
  std::string n3_bind_addr = "127.0.1.1";

  /// External IP address that is advertised to receive GTP-U packets from UPF via N3 interface.
  /// It defaults to \c n3_bind_addr but may differ in case the CU-UP is behind a NAT.
  std::string n3_ext_addr = "auto";

  /// Interface name to bind the N3. `auto` does not force a specific interface and uses a normal `bind()`.
  std::string n3_bind_interface = "auto";

  /// Local port to bind for connection from UPF to receive downlink user-plane traffic (N3 interface).
  int n3_bind_port = GTPU_PORT; // TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs

  /// Maximum amount of packets received in a single syscall.
  int n3_rx_max_mmsg = 256;

  /// Pool occupancy threshold after which we drop packets.
  float pool_threshold = 0.9;

  /// Local IP address to bind for connection from DU to receive uplink user-plane traffic.
  std::string f1u_bind_addr = "127.0.2.1";

  /// Local port to bind for connection from DU to receive uplink user-plane traffic.
  int f1u_bind_port = GTPU_PORT;
};

struct n3_interface_config {
  int                       upf_port = GTPU_PORT;  // TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs
  std::chrono::milliseconds gtpu_reordering_timer; // N3 reordering timer
  bool                      warn_on_drop;
};

struct cu_up_test_mode_config {
  bool     enabled           = false;
  bool     integrity_enabled = true;
  bool     ciphering_enabled = true;
  uint16_t nea_algo          = 2;
  uint16_t nia_algo          = 2;
};

/// CU-UP configuration.
struct cu_up_config {
  /// 5QI as key.
  std::map<five_qi_t, cu_up_qos_config> qos;
  /// N3 configuration.
  n3_interface_config n3_cfg;
  /// Test mode configuration.
  cu_up_test_mode_config test_mode_cfg;
  /// CU-UP identifier.
  unsigned cu_up_id = 0;
  /// CU-UP name.
  std::string cu_up_name = "srs_cu_up_01";
  /// Full PLMN as string (without possible filler digit) e.g. "00101".
  std::string plmn = "00101";
  /// CU-UP statistics report period in seconds.
  std::chrono::seconds statistics_report_period;
};

/// CU-UP dependencies.
struct cu_up_dependencies {
  /// Executor mapper.
  cu_up_executor_mapper* exec_mapper = nullptr;
  /// F1-U gateway.
  f1u_cu_up_gateway* f1u_gateway = nullptr;
  /// Time manager.
  timer_manager* timers = nullptr;
  /// PCAP.
  dlt_pcap* gtpu_pcap = nullptr;
  /// PDCP metrics notifier.
  pdcp_metrics_notifier* pdcp_metric_notifier = nullptr;
  /// E1AP connection client.
  e1_connection_client* e1_conn_client = nullptr;
  /// NG-U gateways
  std::vector<srs_cu_up::ngu_gateway*> ngu_gws;
};

} // namespace srs_cu_up
} // namespace srsran

namespace fmt {

// Network config formatter
template <>
struct formatter<srsran::srs_cu_up::network_interface_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_up::network_interface_config& cfg, FormatContext& ctx)
  {
    return format_to(ctx.out(),
                     "n3_bind_addr={}, n3_bind_port={}, f1u_bind_addr={}, f1u_bind_port={}",
                     cfg.n3_bind_addr,
                     cfg.n3_bind_port,
                     cfg.f1u_bind_addr,
                     cfg.f1u_bind_port);
  }
};
} // namespace fmt
