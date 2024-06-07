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
struct cu_cp_unit_pcap_config {
  struct {
    std::string filename = "/tmp/cu_ngap.pcap";
    bool        enabled  = false;
  } ngap;
  struct {
    std::string filename = "/tmp/cu_e1ap.pcap";
    bool        enabled  = false;
  } e1ap;
  struct {
    std::string filename = "/tmp/cu_f1ap.pcap";
    bool        enabled  = false;
  } f1ap;

  /// helper method to set the filename prefix for different apps.
  /// This is used to provide different defaults depending on the app,
  /// e.g.: "/tmp/gnb_e1ap.pcap" or "/tmp/cu_e1ap.pcap"
  void set_default_filename(std::string prefix)
  {
    ngap.filename = fmt::format("{}_ngap.pcap", prefix);
    e1ap.filename = fmt::format("{}_e1ap.pcap", prefix);
    f1ap.filename = fmt::format("{}_f1u.pcap", prefix);
  }
};
} // namespace srsran
