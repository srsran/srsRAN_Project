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

#include <cstdint>

namespace srsgnb {
namespace srs_du {

/// \brief This interface represents the notification entry point of the transmitting side of a F1-U bearer
/// through which the lower layers notify the PDCP transmitting entity of successful delivery of PDCP PDUs.
class f1u_tx_pdu_delivery_handler
{
public:
  virtual ~f1u_tx_pdu_delivery_handler() = default;

  virtual void handle_delivered_tx_pdu(uint32_t count) = 0;
};

} // namespace srs_du
} // namespace srsgnb
