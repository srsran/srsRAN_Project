/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_cell_manager.h"
#include "srsran/mac/mac_cell_rach_handler.h"
#include "srsran/mac/mac_cell_slot_handler.h"
#include "srsran/mac/mac_paging_information_handler.h"
#include "srsran/mac/mac_pdu_handler.h"
#include "srsran/mac/mac_ue_configurator.h"
#include "srsran/mac/mac_ue_control_information_handler.h"
#include <memory>

namespace srsran {

class mac_interface
{
public:
  virtual ~mac_interface() = default;

  /// Returns handler of PRACHs.
  virtual mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index) = 0;

  /// Returns handler for UL and DL control information for a given cell.
  virtual mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) = 0;

  /// Returns handler for UE control information.
  virtual mac_ue_control_information_handler& get_ue_control_info_handler() = 0;

  /// \brief Returns handler of slot indications for a cell with provided cell_index.
  /// \param cell_index Index of cell currently activated in the DU.
  virtual mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) = 0;

  /// Returns MAC management handler.
  virtual mac_cell_manager& get_cell_manager() = 0;

  /// Returns MAC UE configuration handler.
  virtual mac_ue_configurator& get_ue_configurator() = 0;

  /// Returns PDU handler.
  virtual mac_pdu_handler& get_pdu_handler() = 0;

  /// Returns Paging handler.
  virtual mac_paging_information_handler& get_cell_paging_info_handler() = 0;
};

} // namespace srsran
