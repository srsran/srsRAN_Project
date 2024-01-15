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

#include "ue_bearer_manager.h"
#include "../procedures/f1ap_du_event_manager.h"
#include "f1c_du_bearer_impl.h"
#include "srsran/f1u/du/f1u_bearer_factory.h"

using namespace srsran;
using namespace srs_du;

void ue_bearer_manager::add_srb0_f1c_bearer(f1c_rx_sdu_notifier&       f1c_rx_sdu_notif,
                                            const nr_cell_global_id_t& pcell_cgi,
                                            const byte_buffer&         du_cu_rrc_container,
                                            f1ap_event_manager&        ev_mng)
{
  f1c_bearers.emplace(
      0,
      std::make_unique<f1c_srb0_du_bearer>(
          ue_ctx, pcell_cgi, du_cu_rrc_container, f1ap_notifier, f1c_rx_sdu_notif, ev_mng, ctrl_exec, ue_exec));
}

void ue_bearer_manager::add_f1c_bearer(srb_id_t srb_id, f1c_rx_sdu_notifier& rx_sdu_notif)
{
  srsran_assert(srb_id != srb_id_t::srb0, "This function is only for SRB1, SRB2 or SRB3");

  f1c_bearers.emplace(
      srb_id_to_uint(srb_id),
      std::make_unique<f1c_other_srb_du_bearer>(ue_ctx, srb_id, f1ap_notifier, rx_sdu_notif, ctrl_exec, ue_exec));
}
