/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srsgnb {

struct cu_appconfig {
  std::string amf_addr      = "";
  uint16_t    amf_port      = 38412;
  std::string amf_bind_addr = "";
};

/// Program parameters exposed through command line/config. Default values are captured here.
struct gnb_appconfig {
  std::string config_file           = "~/srsgnb.conf";
  std::string selected_profile_name = "zmq_20MHz_n7";
  std::string log_level             = "info";
  float       baseband_gain_dB      = -2.5F;
  bool        enable_clipping       = false;
  bool        printconfig           = false;

  cu_appconfig cu;
};

} // namespace srsgnb