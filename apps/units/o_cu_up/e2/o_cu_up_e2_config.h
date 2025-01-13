/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/e2/e2ap_configuration.h"

namespace srsran {

/// O-RAN CU-UP E2 PCAP config.
struct o_cu_up_e2_pcap_config {
  std::string filename = "/tmp/cu_up_e2ap.pcap";
  bool        enabled  = false;
};

/// O-RAN CU-UP E2 configuration.
struct o_cu_up_e2_config {
  e2_config              base_config;
  o_cu_up_e2_pcap_config pcaps;
};

} // namespace srsran
