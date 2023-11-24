
#pragma once

#include "srsran/ran/slot_point.h"

namespace srsran {

/// Interface used to handle slot indications specific to a cell.
class mac_cell_slot_handler
{
public:
  virtual ~mac_cell_slot_handler() = default;

  /// \brief Processing of a new Slot Indication. This operation involves the following sub-tasks:
  /// - The scheduler generates the scheduling grants for the provided slot and cell.
  /// - The scheduling grants are passed to the PHY.
  /// - The MAC DL PDUs are generated and passed to the PHY as well.
  virtual void handle_slot_indication(slot_point sl_tx) = 0;

  /// \brief Handles an error indication sent by the lower layers for a given slot.
  ///
  /// This indication may occur when the MAC took too long to send its slot result to the lower layers or when the
  /// lower layers were not able to process that slot result.
  virtual void handle_error_indication(slot_point sl_tx) = 0;
};

} // namespace srsran
