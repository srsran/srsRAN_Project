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

#include "srsgnb/sdap/sdap_rx_pdu_unpack.h"
#include <cstdio>

namespace srsgnb {

class sdap_rx_pdu_unpack_impl : public sdap_rx_pdu_unpack
{
public:
  bool decapsulate(byte_buffer& data) override
  {
    std::printf("[SDAP-RX-PDU-UNPACK] Removing SDAP header from packet of size = %u\n", (unsigned)data.length());
    data.trim_head(3);
    std::printf("[SDAP-RX-PDU-UNPACK] New size after removing SDAP header is %u bytes\n", (unsigned)data.length());
    return true;
  }
};

} // namespace srsgnb
