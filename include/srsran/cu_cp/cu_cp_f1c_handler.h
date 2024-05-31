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

#include "srsran/f1ap/common/f1ap_common.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

/// \brief Handler of the F1-C interface of the CU-CP.
///
/// This interface is used to forward F1AP messages or DU connection updates to the CU-CP.
class cu_cp_f1c_handler
{
public:
  virtual ~cu_cp_f1c_handler() = default;

  /// \brief Handles the establishment of a new DU-to-CU-CP TNL association.
  ///
  /// \param f1ap_tx_pdu_notifier Notifier that the CU-CP will use to push F1AP Tx PDUs to the F1-C GW. Once this
  /// notifier instance goes out of scope, the F1-C GW will be notified that the CU-CP wants to shutdown the connection.
  /// \return Notifier that the F1-C GW will use to forward F1AP PDUs to the CU-CP. If the caller lets the returned
  /// object go out of scope, the CU-CP will be notified that a GW event occurred that resulted in the association
  /// being shutdown.
  virtual std::unique_ptr<f1ap_message_notifier>
  handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
