/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1ap/f1ap_message_notifier.h"
#include <memory>

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
