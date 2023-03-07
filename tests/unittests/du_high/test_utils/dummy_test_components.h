/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/mac/mac_cell_result.h"
#include "srsran/pcap/pcap.h"

namespace srsran {

struct mac_pcap_dummy : public mac_pcap {
public:
  void open(const char* filename_) override {}
  void close() override {}
  bool is_write_enabled() override { return false; }
  void push_pdu(mac_nr_context_info context, const_span<uint8_t> pdu) override {}
  void push_pdu(mac_nr_context_info context, byte_buffer pdu) override {}
};

} // namespace srsran
