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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/rrc/meas_types.h"
#include "srsran/support/async/async_task.h"
#include <map>

namespace srsran {
namespace srs_cu_cp {

struct cell_measurement_positioning_info {
  struct cell_measurement_item_t {
    nr_cell_global_id_t nr_cgi;
    uint32_t            nr_arfcn;
    rrc_meas_result_nr  meas_result;
  };

  nr_cell_global_id_t                                 serving_cell_id;
  std::map<nr_cell_identity, cell_measurement_item_t> cell_measurements;
};

/// NRPPA notifier to the CU-CP UE
class nrppa_cu_cp_ue_notifier
{
public:
  virtual ~nrppa_cu_cp_ue_notifier() = default;

  /// \brief Get the UE index of the UE.
  virtual ue_index_t get_ue_index() = 0;

  /// \brief Get the measurement results of the UE.
  virtual std::optional<cell_measurement_positioning_info>& on_measurement_results_required() = 0;

  /// \brief Schedule an async task for the UE.
  virtual bool schedule_async_task(async_task<void> task) = 0;
};

/// Methods used by NRPPa to signal events to the CU-CP.
class nrppa_cu_cp_notifier
{
public:
  virtual ~nrppa_cu_cp_notifier() = default;

  /// \brief Notifies the CU-CP about a new NRPPA UE.
  /// \param[in] ue_index The index of the new NRPPA UE.
  /// \returns Pointer to the NRPPA UE notifier.
  virtual nrppa_cu_cp_ue_notifier* on_new_nrppa_ue(ue_index_t ue_index) = 0;

  /// \brief Notifies about a NRPPa PDU.
  /// \param[in] nrppa_pdu The NRPPa PDU.
  /// \param[in] ue_index For UE associated messages the index of the UE.
  virtual void on_ul_nrppa_pdu(const byte_buffer& nrppa_pdu, std::optional<ue_index_t> ue_index) = 0;
};

/// This interface is used to push NRPPA messages to the NRPPA interface.
class nrppa_message_handler
{
public:
  virtual ~nrppa_message_handler() = default;

  /// Handle the incoming NRPPA message.
  virtual void handle_new_nrppa_pdu(const byte_buffer& nrppa_pdu, std::optional<ue_index_t> ue_index) = 0;
};

/// Handle ue context removal.
class nrppa_ue_context_removal_handler
{
public:
  virtual ~nrppa_ue_context_removal_handler() = default;

  /// \brief Remove the context of an UE.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void remove_ue_context(ue_index_t ue_index) = 0;
};

/// Combined entry point for the NRPPA object.
class nrppa_interface
{
public:
  virtual ~nrppa_interface() = default;

  virtual nrppa_message_handler&            get_nrppa_message_handler()            = 0;
  virtual nrppa_ue_context_removal_handler& get_nrppa_ue_context_removal_handler() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran