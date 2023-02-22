/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "pcap_file_base.h"

namespace srsran {

class f1ap_pcap : pcap_file_base
{
public:
  f1ap_pcap() = default;
  ~f1ap_pcap();
  f1ap_pcap(const f1ap_pcap& other)            = delete;
  f1ap_pcap& operator=(const f1ap_pcap& other) = delete;
  f1ap_pcap(f1ap_pcap&& other)                 = delete;
  f1ap_pcap& operator=(f1ap_pcap&& other)      = delete;

  void enable();
  void open(const char* filename_);
  void close();
  void write_pdu(const_span<uint8_t> pdu);
};

} // namespace srsran
