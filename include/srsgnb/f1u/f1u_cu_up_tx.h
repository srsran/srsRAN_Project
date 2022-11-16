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

/// This interface represents the data entry point of the transmitting side of a F1-U entity.
/// The upper-layers will use this class to pass PDCP PDUs into the TX entity.
/// The upper-layers will use this class to inform lower layers of PDCP PDUs to be discarded.
class f1u_cu_up_tx_upper_data_interface
{
public:
  f1u_cu_up_tx_upper_data_interface()                                                     = default;
  virtual ~f1u_cu_up_tx_upper_data_interface()                                            = default;
  f1u_cu_up_tx_upper_data_interface(const f1u_cu_up_tx_upper_data_interface&)             = delete;
  f1u_cu_up_tx_upper_data_interface& operator=(const f1u_cu_up_tx_upper_data_interface&)  = delete;
  f1u_cu_up_tx_upper_data_interface(const f1u_cu_up_tx_upper_data_interface&&)            = delete;
  f1u_cu_up_tx_upper_data_interface& operator=(const f1u_cu_up_tx_upper_data_interface&&) = delete;

  /// \brief Interface for higher layers to pass PDCP PDUs to lower layers
  /// \param pdu PDCP PDU to be handled
  virtual void handle_pdu(byte_buffer pdu, uint32_t count) = 0;
  virtual void discard_pdu(uint32_t count)                 = 0;
};

/// This interface represents the notification exit point of the transmitting side of a F1-U entity
/// through which it notifies the PDCP of successful delivery of PDCP PDUs.
class f1u_cu_up_tx_upper_data_notifier
{
public:
  f1u_cu_up_tx_upper_data_notifier()                                                    = default;
  virtual ~f1u_cu_up_tx_upper_data_notifier()                                           = default;
  f1u_cu_up_tx_upper_data_notifier(const f1u_cu_up_tx_upper_data_notifier&)             = delete;
  f1u_cu_up_tx_upper_data_notifier& operator=(const f1u_cu_up_tx_upper_data_notifier&)  = delete;
  f1u_cu_up_tx_upper_data_notifier(const f1u_cu_up_tx_upper_data_notifier&&)            = delete;
  f1u_cu_up_tx_upper_data_notifier& operator=(const f1u_cu_up_tx_upper_data_notifier&&) = delete;

  virtual void on_delivered_pdu(uint32_t count) = 0;
};

} // namespace srsgnb
