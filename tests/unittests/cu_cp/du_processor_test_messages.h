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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/cu_cp/du_setup_notifier.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate a valid dummy F1 Setup Request.
void generate_valid_f1_setup_request(
    du_setup_request& setup_request,
    gnb_du_id_t       gnb_du_id = int_to_gnb_du_id(0x11),
    nr_cell_identity  nrcell_id = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value(),
    pci_t             pci       = 0);

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
