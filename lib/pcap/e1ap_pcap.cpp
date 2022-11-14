/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/pcap/e1ap_pcap.h"
#include "srsgnb/adt/byte_buffer.h"
#include <stdint.h>

namespace srsgnb {

#define E1AP_DLT 153

struct e1ap_context_info_t {
  uint32_t message_type;
  uint32_t procedure_code;
  uint32_t protocol_ie_id;
  uint32_t protocol_extension_id;
};

e1ap_pcap::~e1ap_pcap()
{
  close();
}

void e1ap_pcap::open(const char* filename_)
{
  dlt_pcap_open(E1AP_DLT, filename_);
}
void e1ap_pcap::close()
{
  dlt_pcap_close();
}

void e1ap_pcap::write_pdu(srsgnb::const_span<uint8_t> pdu)
{
  if (!is_write_enabled() || pdu.empty()) {
    // skip
    return;
  }

  // write packet header
  write_pcap_header(pdu.size_bytes());

  // E1AP context currently not required for Wireshark
  // e1ap_context_info_t context = {};
  // fwrite(&context, 1, sizeof(context), pcap_file);

  // write PDU payload
  write_pcap_pdu(pdu);
}

} // namespace srsgnb
