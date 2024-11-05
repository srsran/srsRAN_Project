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

#include "apps/services/worker_manager/worker_manager_worker_getter.h"
#include "apps/units/cu_up/cu_up_unit_pcap_config.h"
#include "srsran/pcap/dlt_pcap.h"

namespace srsran {

struct cu_up_dlt_pcaps {
  std::unique_ptr<dlt_pcap> n3;
  std::unique_ptr<dlt_pcap> f1u;
  std::unique_ptr<dlt_pcap> e1ap;
  std::unique_ptr<dlt_pcap> e2ap;
  void                      close()
  {
    n3.reset();
    f1u.reset();
    e1ap.reset();
  }
};

/// Creates the DLT PCAPs of the CU-UP.
inline cu_up_dlt_pcaps create_cu_up_dlt_pcaps(const cu_up_unit_pcap_config&   pcap_cfg,
                                              worker_manager_executor_getter& exec_getter)
{
  cu_up_dlt_pcaps pcaps;

  pcaps.e1ap = pcap_cfg.e1ap.enabled ? create_e1ap_pcap(pcap_cfg.e1ap.filename, exec_getter.get_executor("pcap_exec"))
                                     : create_null_dlt_pcap();

  pcaps.n3 = pcap_cfg.n3.enabled ? create_gtpu_pcap(pcap_cfg.n3.filename, exec_getter.get_executor("n3_pcap_exec"))
                                 : create_null_dlt_pcap();

  pcaps.f1u = pcap_cfg.f1u.enabled ? create_gtpu_pcap(pcap_cfg.f1u.filename, exec_getter.get_executor("f1u_pcap_exec"))
                                   : create_null_dlt_pcap();

  pcaps.e2ap = pcap_cfg.e2ap.enabled ? create_e2ap_pcap(pcap_cfg.e2ap.filename, exec_getter.get_executor("pcap_exec"))
                                     : create_null_dlt_pcap();

  return pcaps;
}

} // namespace srsran
