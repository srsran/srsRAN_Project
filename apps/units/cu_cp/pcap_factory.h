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

#include "../../gnb/gnb_appconfig.h"
#include "../../services/worker_manager.h"
#include "srsran/pcap/dlt_pcap.h"

namespace srsran {
namespace modules {
namespace cu_cp {

/// Creates the DLT PCAP of the CU-CP.
inline std::unique_ptr<dlt_pcap> create_dlt_pcap(const pcap_appconfig& pcap_cfg, worker_manager& workers)
{
  // return pcap_cfg.ngap.enabled ? create_ngap_pcap(pcap_cfg.ngap.filename, workers.get_executor("pcap_exec"))
  //                              : create_null_dlt_pcap();
  return create_null_dlt_pcap();
}

} // namespace cu_cp
} // namespace modules
} // namespace srsran
