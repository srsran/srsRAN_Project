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

#include "apps/services/worker_manager_worker_getter.h"
#include "apps/units/cu_cp/cu_cp_unit_pcap_config.h"
#include "srsran/pcap/dlt_pcap.h"

namespace srsran::modules::cu_cp {

/// Creates the DLT PCAP of the CU-CP.
inline std::unique_ptr<dlt_pcap> create_dlt_pcap(const cu_cp_unit_pcap_config&   pcap_cfg,
                                                 worker_manager_executor_getter* exec_getter)
{
  return pcap_cfg.ngap.enabled ? create_ngap_pcap(pcap_cfg.ngap.filename, exec_getter->get_executor("pcap_exec"))
                               : create_null_dlt_pcap();
}

} // namespace srsran::modules::cu_cp
