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

#include "srsran/cu_cp/du_processor.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"

namespace srsran {
namespace srs_cu_cp {

/// Handler of a single DU connected to the CU-CP.
class du_handler
{
public:
  virtual ~du_handler() = default;

  /// \brief Get the F1AP message handler interface of the DU processor object.
  /// \return The F1AP message handler interface of the DU processor object.
  virtual f1ap_message_handler& get_f1ap_message_handler() = 0;

  /// \brief Get the F1AP statistics handler interface of the DU processor object.
  /// \return The F1AP statistics handler interface of the DU processor object.
  virtual f1ap_statistics_handler& get_f1ap_statistics_handler() = 0;

  /// \brief Get the mobility handler.
  virtual du_processor_mobility_handler& get_mobility_handler() = 0;

  /// \brief Get the UE task handler
  virtual du_processor_ue_task_handler& get_du_processor_ue_task_handler() = 0;

  /// \brief Retrieve the F1AP UE notifier of the DU processor.
  virtual du_processor_f1ap_ue_context_notifier& get_f1ap_ue_context_notifier() = 0;

  /// \brief Retrieve the UE Context notifier of the DU processor.
  virtual du_processor_ue_context_notifier& get_du_processor_ue_context_notifier() = 0;
};

/// Interface used to access and interact with the context of the DUs currently connected to the CU-CP.
class du_repository
{
public:
  virtual ~du_repository() = default;

  /// \brief Get the number of DUs connected to the CU-CP.
  /// \return The number of DUs.
  virtual size_t get_nof_dus() const = 0;

  /// \brief Get the number of UEs connected to the all DUs of the CU-CP.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;

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
  virtual du_handler& get_du(du_index_t du_index) = 0;

  /// \brief Checks whether a cell with the specified PCI is served by any of the connected DUs.
  /// \param[out] The index of the DU serving the given PCI.
  virtual du_index_t find_du(pci_t pci) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
