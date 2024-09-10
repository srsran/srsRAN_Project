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

#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_cell_rach_handler.h"
#include "srsran/mac/mac_cell_slot_handler.h"
#include "srsran/mac/mac_pdu_handler.h"
#include "srsran/ran/du_types.h"

namespace srsran {
namespace srs_du {

class du_configurator;

/// \brief Interface to DU-high class, which owns and manages the interaction between MAC, RLC and F1 layers.
class du_high
{
public:
  virtual ~du_high() = default;

  virtual void start() = 0;

  virtual void stop() = 0;

  /// \brief Get the handler for F1AP Rx PDUs coming from the CU-CP.
  virtual f1ap_message_handler& get_f1ap_message_handler() = 0;

  /// \brief Returns handler of slot indications for a cell with provided cell_index.
  /// \param cell_index Index of cell currently activated in the DU.
  virtual mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) = 0;

  /// Returns handler of PRACHs for a given cell.
  virtual mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index) = 0;

  /// Returns MAC Rx PDU handler.
  virtual mac_pdu_handler& get_pdu_handler() = 0;

  /// Returns handler for UL and DL control information for a given cell.
  virtual mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) = 0;

  /// Returns handler to configure and control the UEs attached to this DU.
  virtual du_configurator& get_du_configurator() = 0;
};

} // namespace srs_du
} // namespace srsran
