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

#include "srsran/pcap/pcap_rlc.h"

namespace srsran {

/// Dummy pcap writer.
class pcap_rlc_dummy : public pcap_rlc
{
public:
  ~pcap_rlc_dummy() = default;
  pcap_rlc_dummy()  = default;

  void open(const std::string& filename_) override {}
  void close() override {}
  bool is_write_enabled() override { return false; }
  void push_pdu(const pcap_rlc_pdu_context& context, const_span<uint8_t> pdu) override {}
  void push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer& pdu) override {}
};

} // namespace srsran
