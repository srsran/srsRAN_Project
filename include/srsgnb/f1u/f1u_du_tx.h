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

/// This interface represents the data exit point of the transmitting side of a F1-U entity.
/// The F1-U will push PDCP PDUs to the lower layers using this interface.
/// The F1-U will also inform the lower layers of PDCP PDUs to be discarded.
class f1u_du_tx_lower_data_notifier
{
public:
  f1u_du_tx_lower_data_notifier()                                                 = default;
  virtual ~f1u_du_tx_lower_data_notifier()                                        = default;
  f1u_du_tx_lower_data_notifier(const f1u_du_tx_lower_data_notifier&)             = delete;
  f1u_du_tx_lower_data_notifier& operator=(const f1u_du_tx_lower_data_notifier&)  = delete;
  f1u_du_tx_lower_data_notifier(const f1u_du_tx_lower_data_notifier&&)            = delete;
  f1u_du_tx_lower_data_notifier& operator=(const f1u_du_tx_lower_data_notifier&&) = delete;

  virtual void on_new_pdcp_pdu(byte_buffer pdu, uint32_t count) = 0;
  virtual void on_discard_pdcp_pdu(uint32_t count)              = 0;
};

/// This interface represents the notification entry point of the transmitting side of a F1-U entity
/// through which the lower layers notify the PDCP transmitting entity of successful delivery of PDCP PDUs.
class f1u_du_tx_lower_data_interface
{
public:
  f1u_du_tx_lower_data_interface()                                                  = default;
  virtual ~f1u_du_tx_lower_data_interface()                                         = default;
  f1u_du_tx_lower_data_interface(const f1u_du_tx_lower_data_interface&)             = delete;
  f1u_du_tx_lower_data_interface& operator=(const f1u_du_tx_lower_data_interface&)  = delete;
  f1u_du_tx_lower_data_interface(const f1u_du_tx_lower_data_interface&&)            = delete;
  f1u_du_tx_lower_data_interface& operator=(const f1u_du_tx_lower_data_interface&&) = delete;

  virtual void notify_delivered_pdcp_pdu(uint32_t count) = 0;
};

} // namespace srsgnb
