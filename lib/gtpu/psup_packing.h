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

#include "srsran/adt/span.h"
#include "srsran/gtpu/psup_message.h"

namespace srsran {

class psup_packing
{
public:
  static bool unpack(psup_dl_pdu_session_information& dl_pdu_session_information, const span<uint8_t> container)
  {
    return true;
  }
};

} // namespace srsran
