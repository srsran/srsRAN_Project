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

#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_message_handler;
class ngap_event_handler;

/// Handler of N2 connection between CU-CP and AMF.
class n2_connection_client : public ngap_message_notifier
{
public:
  /// \brief Connect the CU-CP to the N2 connection client.
  virtual void connect_cu_cp(ngap_message_handler& msg_handler_, ngap_event_handler& ev_handler_) = 0;

  /// \brief Disconnect the CU-CP from the N2 connection client.
  virtual void disconnect() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran