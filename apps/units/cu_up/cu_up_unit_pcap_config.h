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

#include "fmt/core.h"
#include <string>

namespace srsran {

/// Configuration of packet capture functionalities.
struct cu_up_unit_pcap_config {
  struct {
    std::string filename = "/tmp/cu_n3.pcap";
    bool        enabled  = false;
  } n3;
  struct {
    std::string filename = "/tmp/cu_f1u.pcap";
    bool        enabled  = false;
  } f1u;
  struct {
    std::string filename = "/tmp/cu_e1ap.pcap";
    bool        enabled  = false;
  } e1ap;

  /// helper method to set the prefix for different apps.
  void set_prefix(std::string prefix)
  {
    n3.filename   = fmt::format("{}_n3.pcap", prefix);
    f1u.filename  = fmt::format("{}_f1u.pcap", prefix);
    e1ap.filename = fmt::format("{}_e1ap.pcap", prefix);
  }
};

} // namespace srsran
