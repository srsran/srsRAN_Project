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
