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

#include "srsran/f1ap/cu_cp/f1ap_cu.h"

namespace srsran {
namespace srs_cu_cp {

/// Handler of a single DU connected to the CU-CP.
class du_f1c_handler
{
public:
  virtual ~du_f1c_handler() = default;

  /// \brief Get the Rx F1AP PDU handler for a given DU F1-C interface.
  /// \return The Rx F1AP PDU handler interface.
  virtual f1ap_message_handler& get_message_handler() = 0;
};

/// \brief Handler of the F1-C interface of the CU-CP.
///
/// This interface is used to forward F1AP messages or DU connection updates to the CU-CP.
class cu_cp_f1c_handler
{
public:
  virtual ~cu_cp_f1c_handler() = default;

  /// \brief Handles the start of a new DU connection.
  ///
  /// \param f1ap_tx_pdu_notifier Notifier that the CU-CP will use to push F1AP Tx messages to the DU. Once this
  /// object goes out of scope, the DU connection will be closed.
  /// \return Notifier that the F1-C will use to forward to the CU-CP received F1AP messages from the DU via F1-C. If
  /// the caller lets the returned object go out of scope, the DU connection will be closed.
  virtual std::unique_ptr<f1ap_message_notifier>
  handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier) = 0;

  /// \brief Handles a remove request. The corresponding DU processor object will be removed.
  /// \param[in] du_index The index of the DU processor object to delete.
  virtual void handle_du_remove_request(du_index_t du_index) = 0;

  /// \brief Get handler to a DU connected to the CU-CP.
  virtual du_f1c_handler& get_du(du_index_t du_index) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
