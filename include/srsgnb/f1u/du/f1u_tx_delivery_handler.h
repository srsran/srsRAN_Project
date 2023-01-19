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

/// \brief This interface represents the notification entry point of the transmitting side of a F1-U bearer of the DU
/// through which the lower layer (e.g. RLC) notifies the PDCP Tx entity in the CU-UP of transmit/delivery of PDCP PDUs.
class f1u_tx_delivery_handler
{
public:
  virtual ~f1u_tx_delivery_handler() = default;

  virtual void handle_transmit_notification(uint32_t highest_pdcp_sn) = 0;
  virtual void handle_delivery_notification(uint32_t highest_pdcp_sn) = 0;
};

} // namespace srs_du
} // namespace srsgnb
