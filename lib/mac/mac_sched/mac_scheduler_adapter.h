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

#include "../mac_ctrl/mac_scheduler_configurator.h"
#include "../mac_dl/cell_sys_info_configurator.h"
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
