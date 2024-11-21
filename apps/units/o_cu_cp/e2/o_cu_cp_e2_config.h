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

#include "srsran/e2/e2ap_configuration.h"

namespace srsran {

/// O-RAN CU-CP E2 PCAP configuration.
struct o_cu_cp_e2_pcap_config {
  std::string filename = "/tmp/cu_cp_e2ap.pcap";
  bool        enabled  = false;
};

/// O-RAN CU-CP E2 configuration.
struct o_cu_cp_e2_config {
  e2_config              base_config;
  o_cu_cp_e2_pcap_config pcaps;
};

} // namespace srsran
