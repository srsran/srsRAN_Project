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

#include "apps/services/worker_manager_worker_getter.h"
#include "apps/units/cu_cp/cu_cp_unit_pcap_config.h"
#include "srsran/pcap/dlt_pcap.h"

namespace srsran::modules::cu_cp {

struct cu_cp_dlt_pcaps {
  std::unique_ptr<dlt_pcap> ngap;
  std::unique_ptr<dlt_pcap> f1ap;
  std::unique_ptr<dlt_pcap> e1ap;
  void                      close()
  {
    ngap.reset();
    f1ap.reset();
    e1ap.reset();
  }
};

/// Creates the DLT PCAP of the CU-CP.
inline cu_cp_dlt_pcaps create_dlt_pcap(const cu_cp_unit_pcap_config&   pcap_cfg,
                                       worker_manager_executor_getter& exec_getter)
{
  cu_cp_dlt_pcaps pcaps;
  pcaps.ngap = pcap_cfg.ngap.enabled ? create_ngap_pcap(pcap_cfg.ngap.filename, exec_getter.get_executor("pcap_exec"))
                                     : create_null_dlt_pcap();
  pcaps.f1ap = pcap_cfg.f1ap.enabled ? create_f1ap_pcap(pcap_cfg.f1ap.filename, exec_getter.get_executor("pcap_exec"))
                                     : create_null_dlt_pcap();
  pcaps.e1ap = pcap_cfg.e1ap.enabled ? create_e1ap_pcap(pcap_cfg.e1ap.filename, exec_getter.get_executor("pcap_exec"))
                                     : create_null_dlt_pcap();
  return pcaps;
}

} // namespace srsran::modules::cu_cp
