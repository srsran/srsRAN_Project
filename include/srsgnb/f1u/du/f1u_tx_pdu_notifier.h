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
namespace srs_du {

/// \brief This interface represents the data exit point of the transmitting side of a F1-U entity.
/// The F1-U will push PDUs to the lower layers using this interface.
/// The F1-U will also inform the lower layers of PDUs to be discarded.
class f1u_tx_pdu_notifier
{
public:
  virtual ~f1u_tx_pdu_notifier() = default;

  virtual void on_tx_pdu(byte_buffer pdu, uint32_t count) = 0;
  virtual void on_discard_tx_pdu(uint32_t count)          = 0;
};

} // namespace srs_du
} // namespace srsgnb