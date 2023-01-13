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

#include "pcap.h"

namespace srsgnb {

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
  void write_pdu(srsgnb::const_span<uint8_t> pdu);
};

} // namespace srsgnb