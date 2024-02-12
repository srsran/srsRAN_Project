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

#include "../cu_cp_impl_interface.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between RRC DU and CU-CP
class rrc_du_cu_cp_adapter : public rrc_du_measurement_config_notifier
{
public:
  void connect_cu_cp(cu_cp_measurement_config_handler& meas_config_handler_)
  {
    meas_config_handler = &meas_config_handler_;
  }

  void on_cell_config_update_request(nr_cell_id_t                           nci,
                                     const serving_cell_meas_config&        serv_cell_cfg,
                                     std::vector<neighbor_cell_meas_config> ncells = {}) override
  {
    srsran_assert(meas_config_handler != nullptr, "Measurement config handler must not be nullptr");
    return meas_config_handler->handle_cell_config_update_request(nci, serv_cell_cfg, ncells);
  }

private:
  cu_cp_measurement_config_handler* meas_config_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
