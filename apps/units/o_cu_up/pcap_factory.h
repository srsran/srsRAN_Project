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
#include "apps/units/o_cu_up/o_cu_up_unit_config.h"
#include "srsran/pcap/dlt_pcap.h"

namespace srsran {

struct o_cu_up_dlt_pcaps {
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

/// Creates the DLT PCAPs of the O-RAN CU-UP.
inline o_cu_up_dlt_pcaps create_o_cu_up_dlt_pcaps(const o_cu_up_unit_config&      unit_cfg,
                                                  worker_manager_executor_getter& exec_getter)
{
  o_cu_up_dlt_pcaps pcaps;

  const auto& cu_pcaps = unit_cfg.cu_up_cfg.pcap_cfg;
  pcaps.e1ap = cu_pcaps.e1ap.enabled ? create_e1ap_pcap(cu_pcaps.e1ap.filename, exec_getter.get_executor("pcap_exec"))
                                     : create_null_dlt_pcap();

  pcaps.n3 = cu_pcaps.n3.enabled ? create_gtpu_pcap(cu_pcaps.n3.filename, exec_getter.get_executor("n3_pcap_exec"))
                                 : create_null_dlt_pcap();

  pcaps.f1u = cu_pcaps.f1u.enabled ? create_gtpu_pcap(cu_pcaps.f1u.filename, exec_getter.get_executor("f1u_pcap_exec"))
                                   : create_null_dlt_pcap();

  pcaps.e2ap = unit_cfg.e2_cfg.pcaps.enabled
                   ? create_e2ap_pcap(unit_cfg.e2_cfg.pcaps.filename, exec_getter.get_executor("pcap_exec"))
                   : create_null_dlt_pcap();

  return pcaps;
}

} // namespace srsran
