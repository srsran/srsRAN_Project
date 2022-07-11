/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PCAP_F1AP_PCAP_H
#define SRSGNB_PCAP_F1AP_PCAP_H

#include "pcap.h"

namespace srsgnb {

class f1ap_pcap : pcap_file_base
{
public:
  f1ap_pcap()= default;
  ~f1ap_pcap();
  f1ap_pcap(const f1ap_pcap& other) = delete;
  f1ap_pcap& operator=(const f1ap_pcap& other) = delete;
  f1ap_pcap(f1ap_pcap&& other)                 = delete;
  f1ap_pcap& operator=(f1ap_pcap&& other) = delete;

  void enable();
  void open(const char* filename_);
  void close();
  void write_pdu(const_span<uint8_t> pdu);
};

} // namespace srsran

#endif // SRSGNB_PCAP_F1AP_PCAP_H
