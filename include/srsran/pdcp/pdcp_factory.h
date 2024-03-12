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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_entity.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/ran/lcid.h"
#include "srsran/support/timers.h"
#include <memory>

/// This factory header file depends on the PDCP entity interfaces (see above include list). It is kept separate as
/// clients of the PDCP entity interfaces dont need to call factory methods.

namespace srsran {

struct pdcp_entity_creation_message {
  uint32_t                        ue_index;
  rb_id_t                         rb_id;
  pdcp_config                     config;
  pdcp_tx_lower_notifier*         tx_lower;
  pdcp_tx_upper_control_notifier* tx_upper_cn;
  pdcp_rx_upper_data_notifier*    rx_upper_dn;
  pdcp_rx_upper_control_notifier* rx_upper_cn;
  timer_factory                   ue_dl_timer_factory;
  timer_factory                   ue_ul_timer_factory;
  timer_factory                   ue_ctrl_timer_factory;
};

/// Creates an instance of a PDCP entity.
std::unique_ptr<pdcp_entity> create_pdcp_entity(pdcp_entity_creation_message& msg);

} // namespace srsran
