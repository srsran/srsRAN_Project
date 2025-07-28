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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/du_setup_notifier.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/f1ap/f1ap_message.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate a valid dummy F1 Setup Request.
void generate_valid_f1_setup_request(du_setup_request& setup_request,
                                     gnb_du_id_t       gnb_du_id = int_to_gnb_du_id(0x11),
                                     nr_cell_identity  nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value(),
                                     pci_t             pci = 0,
                                     unsigned          tac = 7);

/// \brief Generate a dummy F1 Setup Request base to extend.
void generate_f1_setup_request_base(du_setup_request& setup_request);

/// \brief Generate a dummy F1 Setup Request with unsupported number of cells.
f1ap_message create_f1_setup_request_with_too_many_cells(const f1ap_message& base = {});
void         generate_f1_setup_request_with_too_many_cells(du_setup_request& setup_request);

/// \brief Generate a dummy UE Creation Message.
/// \param[in] ue_index The UE index to use.
/// \param[in] c_rnti The C-RNTI to use.
/// \param[in] nrcell_id The NR Cell Id to use.
/// \return The dummy UE Creation Message.
ue_rrc_context_creation_request generate_ue_rrc_context_creation_request(
    ue_index_t       ue_index,
    rnti_t           c_rnti,
    nr_cell_identity nrcell_id = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value());

} // namespace srs_cu_cp
} // namespace srsran
