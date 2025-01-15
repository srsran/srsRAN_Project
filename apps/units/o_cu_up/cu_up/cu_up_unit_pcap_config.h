/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srsran {

/// Configuration of packet capture functionalities.
struct cu_up_unit_pcap_config {
  struct {
    std::string filename;
    bool        enabled = false;
  } n3;
  struct {
    std::string filename;
    bool        enabled = false;
  } f1u;
  struct {
    std::string filename;
    bool        enabled = false;
  } e1ap;

  /// When using the gNB app, there is no point in instantiating
  /// E1 pcaps twice. This function force disables them in the CU-UP.
  /// TODO: revisit
  void disable_e1_pcaps() { e1ap.enabled = false; }
};

} // namespace srsran
