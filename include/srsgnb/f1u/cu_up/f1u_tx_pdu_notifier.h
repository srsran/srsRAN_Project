/*
 *
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
namespace srs_cu_up {

/// \brief This interface represents the NR-U exit point of the F1-U bearer
/// through which it passes NR-U PDUs to lower-layer transport (e.g. GTP-U).
class f1u_tx_pdu_notifier
{
public:
  virtual ~f1u_tx_pdu_notifier() = default;

  virtual void on_new_pdu(byte_buffer pdu) = 0;
};

} // namespace srs_cu_up
} // namespace srsgnb
