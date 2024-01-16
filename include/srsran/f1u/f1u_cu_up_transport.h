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

/// This interface represents the NR-U entry point of a F1-U entity.
/// The lower-layer transport (GTP-U) will use this class to pass NR-U PDUs into F1-U.
class f1u_cu_up_lower_transport_interface
{
public:
  f1u_cu_up_lower_transport_interface()                                                       = default;
  virtual ~f1u_cu_up_lower_transport_interface()                                              = default;
  f1u_cu_up_lower_transport_interface(const f1u_cu_up_lower_transport_interface&)             = delete;
  f1u_cu_up_lower_transport_interface& operator=(const f1u_cu_up_lower_transport_interface&)  = delete;
  f1u_cu_up_lower_transport_interface(const f1u_cu_up_lower_transport_interface&&)            = delete;
  f1u_cu_up_lower_transport_interface& operator=(const f1u_cu_up_lower_transport_interface&&) = delete;

  virtual void handle_nr_u_pdu(byte_buffer pdu) = 0;
};

/// This interface represents the NR-U exit point of a F1-U entity.
/// The F1-U entity will use this class to pass NR-U PDUs to lower-layer transport (GTP-U).
class f1u_cu_up_lower_transport_notifier
{
public:
  f1u_cu_up_lower_transport_notifier()                                                      = default;
  virtual ~f1u_cu_up_lower_transport_notifier()                                             = default;
  f1u_cu_up_lower_transport_notifier(const f1u_cu_up_lower_transport_notifier&)             = delete;
  f1u_cu_up_lower_transport_notifier& operator=(const f1u_cu_up_lower_transport_notifier&)  = delete;
  f1u_cu_up_lower_transport_notifier(const f1u_cu_up_lower_transport_notifier&&)            = delete;
  f1u_cu_up_lower_transport_notifier& operator=(const f1u_cu_up_lower_transport_notifier&&) = delete;

  virtual void on_new_nr_u_pdu(byte_buffer pdu) = 0;
};

} // namespace srsran
