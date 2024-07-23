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

#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handler of the E1 interface of the CU-CP.
///
/// This interface is used to forward E1AP messages or CU-UP connection updates to the CU-CP.
class cu_cp_e1_handler
{
public:
  virtual ~cu_cp_e1_handler() = default;

  /// \brief Handles the start of a new CU-UP connection.
  ///
  /// \param e1ap_tx_pdu_notifier Notifier that the CU-CP will use to push E1AP Tx messages to the CU-UP. Once this
  /// object goes out of scope, the CU-UP connection will be closed.
  /// \return Notifier that the E1AP will use to forward to the CU-CP received E1AP messages from the CU-UP via E1AP. If
  /// the caller lets the returned object go out of scope, the CU-UP connection will be closed.
  virtual std::unique_ptr<e1ap_message_notifier>
  handle_new_cu_up_connection(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
