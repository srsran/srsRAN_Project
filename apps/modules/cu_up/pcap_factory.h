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

#include "../../gnb/gnb_appconfig.h"
#include "../../services/worker_manager.h"
#include "srsran/pcap/dlt_pcap.h"

namespace srsran {
namespace modules {
namespace cu_up {

/// Types of PCAPs in the CU-UP
enum class pcap_type { E1_AP, GTPU, last };

/// Converts the given PCAP type to an unsigned.
inline unsigned to_value(pcap_type value)
{
  return static_cast<unsigned>(value);
}

/// Creates the DLT PCAPs of the CU-UP.
inline std::vector<std::unique_ptr<dlt_pcap>> create_dlt_pcaps(const pcap_appconfig& pcap_cfg, worker_manager& workers)
{
  std::vector<std::unique_ptr<dlt_pcap>> pcaps(to_value(pcap_type::last));

  pcaps[to_value(pcap_type::E1_AP)] = pcap_cfg.e1ap.enabled
                                          ? create_e1ap_pcap(pcap_cfg.e1ap.filename, workers.get_executor("pcap_exec"))
                                          : create_null_dlt_pcap();

  pcaps[to_value(pcap_type::GTPU)] = pcap_cfg.gtpu.enabled
                                         ? create_gtpu_pcap(pcap_cfg.gtpu.filename, workers.get_executor("pcap_exec"))
                                         : create_null_dlt_pcap();

  return pcaps;
}

} // namespace cu_up
} // namespace modules
} // namespace srsran
