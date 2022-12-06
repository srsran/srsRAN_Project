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

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {
namespace srs_du {

/// \brief Interface used by F1 to push PDUs to transmit over-the-air to lower layers.
class f1c_tx_pdu_notifier
{
public:
  virtual ~f1c_tx_pdu_notifier() = default;

  virtual void on_tx_pdu(byte_buffer pdu) = 0;
};

} // namespace srs_du
} // namespace srsgnb