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

#include "../mac_ctrl/mac_scheduler_configurator.h"
#include "../mac_dl/mac_scheduler_cell_info_handler.h"
#include "../mac_ul/mac_scheduler_ce_info_handler.h"

namespace srsran {

/// \brief Interface from MAC to a scheduler implementation.
class mac_scheduler_adapter : public mac_scheduler_configurator,
                              public mac_scheduler_ce_info_handler,
                              public mac_scheduler_cell_info_handler,
                              public mac_paging_information_handler
{
public:
  virtual ~mac_scheduler_adapter() = default;

  /// \brief Gets the RACH handler for a given cell.
  ///
  /// \param cell_index DU-specific index of the cell for which a RACH handler is being retrieved.
  /// \return Cell-specific RACH handler.
  virtual mac_cell_rach_handler& get_cell_rach_handler(du_cell_index_t cell_index) = 0;

  /// \brief Gets the control info handler for a given cell.
  ///
  /// \param cell_index DU-specific index of the cell for which the control info handler is being retrieved.
  /// \return Cell-specific control info handler.
  virtual mac_cell_control_information_handler& get_cell_control_info_handler(du_cell_index_t cell_index) = 0;
};

} // namespace srsran
