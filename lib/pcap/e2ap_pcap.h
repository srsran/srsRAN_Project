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

#include "pcap_file_base.h"

namespace srsran {

class e2ap_pcap : pcap_file_base
{
public:
  e2ap_pcap() = default;
  ~e2ap_pcap();
  e2ap_pcap(const e2ap_pcap& other)            = delete;
  e2ap_pcap& operator=(const e2ap_pcap& other) = delete;
  e2ap_pcap(e2ap_pcap&& other)                 = delete;
  e2ap_pcap& operator=(e2ap_pcap&& other)      = delete;

  void enable();
  void open(const char* filename_);
  void close();
  void write_pdu(srsran::const_span<uint8_t> pdu);
};

} // namespace srsran
