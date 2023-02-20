/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2ap_pcap.h"
#include "srsgnb/adt/byte_buffer.h"
#include <stdint.h>

namespace srsran {

#define E2AP_DLT 155

struct e2ap_context_info_t {
  uint32_t message_type;
  uint32_t procedure_code;
  uint32_t protocol_ie_id;
  uint32_t protocol_extension_id;
};

e2ap_pcap::~e2ap_pcap()
{
  close();
}

void e2ap_pcap::open(const char* filename_)
{
  dlt_pcap_open(E2AP_DLT, filename_);
}
void e2ap_pcap::close()
{
  dlt_pcap_close();
}

void e2ap_pcap::write_pdu(srsran::const_span<uint8_t> pdu)
{
  if (!is_write_enabled() || pdu.empty()) {
    // skip
    return;
  }

  // write packet header
  write_pcap_header(pdu.size_bytes());

  // E2AP context currently not required for Wireshark
  // e2ap_context_info_t context = {};
  // fwrite(&context, 1, sizeof(context), pcap_file);

  // write PDU payload
  write_pcap_pdu(pdu);
}

} // namespace srsran
