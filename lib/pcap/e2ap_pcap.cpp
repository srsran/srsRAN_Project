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

#include "e2ap_pcap.h"
#include "srsran/adt/byte_buffer.h"
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
