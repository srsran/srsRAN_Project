/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/cu_cp/mobility_manager_config.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/logical_channel/lcid.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/qos/qos_flow_id.h"
#include "srsran/support/async/async_task.h"
#include <map>

namespace srsran {

struct cu_handover_control_config {
  nr_cell_global_id_t                                    target_cell_id;
  std::map<pdu_session_id_t, std::vector<qos_flow_id_t>> pdu_sessions_for_ho;
  std::map<drb_id_t, std::vector<qos_flow_id_t>>         drbs_for_ho;
  std::vector<nr_cell_global_id_t>                       secondary_cells_to_be_added;
};

class cu_configurator
{
public:
  cu_configurator(srs_cu_cp::mobility_manager_cu_cp_notifier& mobility_notif_) : mobility_notif(mobility_notif_) {}
  virtual ~cu_configurator() = default;
  srs_cu_cp::mobility_manager_cu_cp_notifier& get_mobility_notifier() { return mobility_notif; }

private:
  srs_cu_cp::mobility_manager_cu_cp_notifier& mobility_notif;
};

} // namespace srsran
