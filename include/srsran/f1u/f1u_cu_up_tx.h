/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"

namespace srsran {

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
  virtual void handle_pdcp_pdu(byte_buffer pdu, uint32_t count) = 0;
  virtual void discard_pdcp_pdu(uint32_t count)                 = 0;
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

  virtual void on_delivered_pdcp_pdu(uint32_t count) = 0;
};

} // namespace srsran
