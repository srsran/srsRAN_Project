
#pragma once

#include "srsran/ran/slot_point.h"

namespace srsran {

struct mac_cell_timing_context;

/// Interface used to handle slot indications specific to a cell.
class mac_cell_slot_handler
{
public:
  /// The effect that the lower layer error(s) had on the results provided by the MAC during a slot indication.
  struct error_event {
    bool pdcch_discarded : 1;
    bool pdsch_discarded : 1;
    bool pusch_and_pucch_discarded : 1;
  };

  virtual ~mac_cell_slot_handler() = default;

  /// \brief Processing of a new Slot Indication.
  /// \param[in] context Indication context.
  ///
  /// This operation involves the following sub-tasks:
  /// - The scheduler generates the scheduling grants for the provided slot and cell.
  /// - The scheduling grants are passed to the PHY.
  /// - The MAC DL PDUs are generated and passed to the PHY as well.
  virtual void handle_slot_indication(const mac_cell_timing_context& context) = 0;

  /// \brief Handles an error indication sent by the lower layers for a given slot.
  ///
  /// This indication may occur when the MAC took too long to send its slot result to the lower layers or when the
  /// lower layers were not able to process that slot result.
  virtual void handle_error_indication(slot_point sl_tx, error_event event) = 0;

  /// \brief Handles a stop indication sent by the lower layers for a given slot.
  ///
  /// This function is called after the lower layers receive a STOP request for a cell in RUNNING state and after
  /// the lower layers stop all the pending RX/TX operations.
  virtual void handle_stop_indication() = 0;
};

} // namespace srsran
