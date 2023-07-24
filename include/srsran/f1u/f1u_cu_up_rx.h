/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/byte_buffer_chain.h"

namespace srsran {

/// This interface represents the data exit point of the receiving side of a F1-U entity.
/// The F1-U entity will use this class to pass PDCP PDUs into the PDCP.
class f1u_cu_up_rx_upper_data_notifier
{
public:
  f1u_cu_up_rx_upper_data_notifier()                                                    = default;
  virtual ~f1u_cu_up_rx_upper_data_notifier()                                           = default;
  f1u_cu_up_rx_upper_data_notifier(const f1u_cu_up_rx_upper_data_notifier&)             = delete;
  f1u_cu_up_rx_upper_data_notifier& operator=(const f1u_cu_up_rx_upper_data_notifier&)  = delete;
  f1u_cu_up_rx_upper_data_notifier(const f1u_cu_up_rx_upper_data_notifier&&)            = delete;
  f1u_cu_up_rx_upper_data_notifier& operator=(const f1u_cu_up_rx_upper_data_notifier&&) = delete;

  virtual void on_new_pdcp_pdu(byte_buffer_chain pdu) = 0;
};

} // namespace srsran
