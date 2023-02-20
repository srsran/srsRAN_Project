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

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

/// NR user plane protocol uplink message as described in TS 38.425.
struct nr_up_ul_message {
  //: TODO: add here dl data delivery status fields of 38.425
  byte_buffer pdu;
};

/// NR user plane protocol downlink message as described in TS 38.425.
struct nr_up_dl_message {
  //: TODO: add here dl user data fields of 38.425
  byte_buffer pdu;
};

} // namespace srsgnb
