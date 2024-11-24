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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/rrc/meas_types.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// NRPPA notifier to the CU-CP UE
class nrppa_cu_cp_ue_notifier
{
public:
  virtual ~nrppa_cu_cp_ue_notifier() = default;

  /// \brief Get the UE index of the UE.
  virtual ue_index_t get_ue_index() = 0;

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

/// This interface is used to push UE measurements to the NRPPA interface.
class nrppa_measurement_handler
{
public:
  virtual ~nrppa_measurement_handler() = default;

  /// Handle the incoming UE measurement.
  virtual void handle_ue_measurement(ue_index_t ue_index, const rrc_meas_results& meas_result) = 0;
};

/// Combined entry point for the NRPPA object.
class nrppa_interface : public nrppa_message_handler, public nrppa_measurement_handler
{
public:
  virtual ~nrppa_interface() = default;

  virtual nrppa_message_handler&     get_nrppa_message_handler()     = 0;
  virtual nrppa_measurement_handler& get_nrppa_measurement_handler() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
