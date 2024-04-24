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

#include "../../../gnb/gnb_appconfig.h"
#include "../../../services/worker_manager.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/pcap/mac_pcap.h"
#include "srsran/pcap/rlc_pcap.h"

namespace srsran {
namespace modules {
namespace flexible_du {

/// Creates the DLT PCAP of the DU.
inline std::unique_ptr<dlt_pcap> create_dlt_pcap(const pcap_appconfig& pcap_cfg, worker_manager& workers)
{
  return pcap_cfg.f1ap.enabled ? create_f1ap_pcap(pcap_cfg.f1ap.filename, workers.get_executor("pcap_exec"))
                               : create_null_dlt_pcap();
}

/// Creates the MAC PCAP of the DU.
inline std::unique_ptr<mac_pcap> create_mac_pcap(const pcap_appconfig& pcap_cfg, worker_manager& workers)
{
  if (pcap_cfg.mac.type != "dlt" && pcap_cfg.mac.type != "udp") {
    report_error("Invalid type for MAC PCAP. type={}\n", pcap_cfg.mac.type);
  }
  return pcap_cfg.mac.enabled ? create_mac_pcap(pcap_cfg.mac.filename,
                                                pcap_cfg.mac.type == "dlt" ? mac_pcap_type::dlt : mac_pcap_type::udp,
                                                workers.get_executor("mac_pcap_exec"))
                              : create_null_mac_pcap();
}

/// Creates the RLC PCAP of the DU.
inline std::unique_ptr<rlc_pcap> create_rlc_pcap(const pcap_appconfig& pcap_cfg, worker_manager& workers)
{
  if (pcap_cfg.rlc.rb_type != "all" && pcap_cfg.rlc.rb_type != "srb" && pcap_cfg.rlc.rb_type != "drb") {
    report_error("Invalid rb_type for RLC PCAP. rb_type={}\n", pcap_cfg.rlc.rb_type);
  }

  return pcap_cfg.rlc.enabled ? create_rlc_pcap(pcap_cfg.rlc.filename,
                                                workers.get_executor("rlc_pcap_exec"),
                                                pcap_cfg.rlc.rb_type != "drb",
                                                pcap_cfg.rlc.rb_type != "srb")
                              : create_null_rlc_pcap();
}

} // namespace flexible_du
} // namespace modules
} // namespace srsran