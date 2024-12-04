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
#include "apps/units/o_cu_cp/o_cu_cp_unit_config.h"
#include "srsran/pcap/dlt_pcap.h"

namespace srsran {

struct o_cu_cp_dlt_pcaps {
  std::unique_ptr<dlt_pcap> ngap;
  std::unique_ptr<dlt_pcap> f1ap;
  std::unique_ptr<dlt_pcap> e1ap;
  std::unique_ptr<dlt_pcap> e2ap;

  /// \brief Close (and flush) the PCAPs without destroying the objects.
  void close()
  {
    if (ngap) {
      ngap->close();
    }
    if (f1ap) {
      f1ap->close();
    }
    if (e1ap) {
      e1ap->close();
    }
    if (e2ap) {
      e2ap->close();
    }
  }

  /// \brief Destroy (close and flush) the PCAPs.
  void reset()
  {
    ngap.reset();
    f1ap.reset();
    e1ap.reset();
    e2ap.reset();
  }
};

/// Creates the DLT PCAPs of the O-RAN CU-CP.
inline o_cu_cp_dlt_pcaps create_o_cu_cp_dlt_pcap(const o_cu_cp_unit_config&      config,
                                                 worker_manager_executor_getter& exec_getter,
                                                 signal_observable&              signal_source)
{
  o_cu_cp_dlt_pcaps             pcaps;
  const cu_cp_unit_pcap_config& pcap_cfg = config.cucp_cfg.pcap_cfg;
  pcaps.ngap = pcap_cfg.ngap.enabled ? create_ngap_pcap(pcap_cfg.ngap.filename, exec_getter.get_executor("pcap_exec"))
                                     : create_null_dlt_pcap();
  signal_source.attach(pcaps.ngap.get());

  pcaps.f1ap = pcap_cfg.f1ap.enabled ? create_f1ap_pcap(pcap_cfg.f1ap.filename, exec_getter.get_executor("pcap_exec"))
                                     : create_null_dlt_pcap();
  signal_source.attach(pcaps.f1ap.get());

  pcaps.e1ap = pcap_cfg.e1ap.enabled ? create_e1ap_pcap(pcap_cfg.e1ap.filename, exec_getter.get_executor("pcap_exec"))
                                     : create_null_dlt_pcap();
  signal_source.attach(pcaps.e1ap.get());

  pcaps.e2ap = config.e2_cfg.pcaps.enabled
                   ? create_e2ap_pcap(config.e2_cfg.pcaps.filename, exec_getter.get_executor("pcap_exec"))
                   : create_null_dlt_pcap();
  signal_source.attach(pcaps.e2ap.get());

  return pcaps;
}

} // namespace srsran
