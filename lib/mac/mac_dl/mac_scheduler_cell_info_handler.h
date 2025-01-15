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

#include "mac_dl_ue_repository.h"
#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_ue_control_information_handler.h"
#include "srsran/ran/du_types.h"

namespace srsran {

struct sched_result;

/// \brief Interface used by MAC Cell Processor to interact with the MAC scheduler.
class mac_scheduler_cell_info_handler : public mac_ue_control_information_handler
{
public:
  virtual ~mac_scheduler_cell_info_handler() = default;

  /// \brief Processes a new slot for a specific cell in the MAC scheduler.
  /// \param slot_tx SFN + slot index of the Transmit slot to be processed.
  /// \param cell_idx DU-specific index of the cell for which the slot is being processed.
  /// \return Result of the scheduling operation. It contains both DL and UL scheduling information.
  virtual const sched_result& slot_indication(slot_point slot_tx, du_cell_index_t cell_idx) = 0;

  /// \brief Processes an error indication for a specific cell in the MAC scheduler.
  /// \param slot_tx SFN + slot index of the Transmit slot to be processed.
  /// \param cell_idx DU-specific index of the cell for which the indication is being processed.
  /// \param event Effect that the errors in the lower layers had on the result provided by the scheduler.
  virtual void
  handle_error_indication(slot_point slot_tx, du_cell_index_t cell_idx, mac_cell_slot_handler::error_event event) = 0;

  /// \brief Update SIB1 PDU size in the scheduler.
  /// \param[in] cell_index Index of the cell being updated.
  /// \param[in] sib_version SIB1 PDU version. The version should always increase for each new SIB1 PDU provided.
  /// \param[in] new_payload_size Size of the SIB1 PDU payload.
  virtual void
  handle_sib1_update_indication(du_cell_index_t cell_index, unsigned sib_version, units::bytes new_payload_size) = 0;
};

} // namespace srsran
