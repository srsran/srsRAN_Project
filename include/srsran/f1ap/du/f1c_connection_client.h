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

#include "srsran/f1ap/common/f1ap_common.h"

namespace srsran {
namespace srs_du {

/// Interface used by the DU to establish a new connection with a CU-CP.
class f1c_connection_client
{
public:
  virtual ~f1c_connection_client() = default;

  /// Establish a new connection with a CU-CP.
  ///
  /// \param du_rx_pdu_notifier Notifier that will be used to forward the F1AP PDUs sent by the CU-CP to the DU.
  /// \return Notifier that the DU can use to send F1AP Tx PDUs to the CU-CP it connected to.
  virtual std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) = 0;
};

} // namespace srs_du
} // namespace srsran