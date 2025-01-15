/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
