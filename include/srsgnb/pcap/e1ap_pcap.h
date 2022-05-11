/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_E1AP_PCAP_H
#define SRSGNB_E1AP_PCAP_H

#include "pcap.h"

namespace srsgnb {

class e1ap_pcap : pcap_file_base
{
public:
  e1ap_pcap()= default;
  ~e1ap_pcap();
  e1ap_pcap(const e1ap_pcap& other) = delete;
  e1ap_pcap& operator=(const e1ap_pcap& other) = delete;
  e1ap_pcap(e1ap_pcap&& other)                 = delete;
  e1ap_pcap& operator=(e1ap_pcap&& other) = delete;

  void enable();
  void open(const char* filename_);
  void close();
  void write_pdu(srsgnb::const_span<uint8_t> pdu);
};

} // namespace srsran

#endif // SRSGNB_E1AP_PCAP_H
