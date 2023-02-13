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
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/support/executors/task_worker.h"

namespace srsgnb {

class ngap_pcap : public pcap_file_base
{
public:
  ngap_pcap();
  ~ngap_pcap();
  ngap_pcap(const ngap_pcap& other)            = delete;
  ngap_pcap& operator=(const ngap_pcap& other) = delete;
  ngap_pcap(ngap_pcap&& other)                 = delete;
  ngap_pcap& operator=(ngap_pcap&& other)      = delete;

  void open(const char* filename_);
  void close();
  void push_pdu(srsgnb::byte_buffer pdu);
  void push_pdu(srsgnb::const_span<uint8_t> pdu);

private:
  void        write_pdu(srsgnb::byte_buffer buf);
  task_worker worker;
};

} // namespace srsgnb
