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

/// O-RAN DU high E2 PCAP configuration.
struct o_du_high_e2_pcap_config {
  std::string filename = "/tmp/du_e2ap.pcap";
  bool        enabled  = false;
};

/// O-RAN DU high configuration.
struct o_du_high_e2_config {
  /// E2 configuration.
  e2_config base_cfg;
  /// PCAPs.
  o_du_high_e2_pcap_config pcaps;
};

} // namespace srsran
