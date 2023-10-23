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

#include "srsran/mac/mac_cell_result.h"
#include "srsran/pcap/pcap.h"

namespace srsran {

/// Dummy pcap writer.
struct mac_pcap_dummy : public mac_pcap {
public:
  void open(const std::string& filename_, mac_pcap_type type) override {}
  void close() override {}
  bool is_write_enabled() override { return false; }
  void push_pdu(mac_nr_context_info context, const_span<uint8_t> pdu) override {}
  void push_pdu(mac_nr_context_info context, byte_buffer pdu) override {}
};

} // namespace srsran
