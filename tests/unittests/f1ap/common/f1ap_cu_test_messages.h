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

#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate a dummy F1AP Served Cell Item.
asn1::f1ap::gnb_du_served_cells_item_s generate_served_cells_item(unsigned nrcell_id, pci_t nrpci);

/// \brief Generates dummy F1AP SETUP REQUEST message.
f1ap_message generate_f1_setup_request(unsigned gnb_du_id = 0x11, unsigned nrcell_id = 6576, pci_t pci = 0);

/// \brief Generates dummy F1AP Initial UL RRC TRANSFER message.
f1ap_message generate_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                   rnti_t              crnti           = to_rnti(0x4601),
                                                   byte_buffer         du2cu_container = {});

/// \brief Generates dummy F1AP UL RRC TRANSFER message.
f1ap_message generate_ul_rrc_message_transfer(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                              gnb_du_ue_f1ap_id_t du_ue_id,
                                              srb_id_t            srb_id        = srb_id_t::srb1,
                                              byte_buffer         rrc_container = {0x1, 0x2, 0x3});

/// \brief Generates dummy F1AP UE CONTEXT RELEASE COMPLETE message.
f1ap_message generate_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP UE CONTEXT SETUP REQUEST message.
f1ap_message generate_ue_context_setup_request(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP UE CONTEXT SETUP RESPONSE message.
f1ap_message generate_ue_context_setup_response(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                gnb_du_ue_f1ap_id_t du_ue_id,
                                                rnti_t              crnti = to_rnti(0x4601));

/// \brief Generates dummy F1AP UE CONTEXT SETUP FAILURE message.
f1ap_message generate_ue_context_setup_failure(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy CU-CP UE CONTEXT MODIFICATION REQUEST message.
f1ap_ue_context_modification_request generate_ue_context_modification_request(ue_index_t ue_index);

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION RESPONSE message.
f1ap_message generate_ue_context_modification_response(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                       gnb_du_ue_f1ap_id_t du_ue_id,
                                                       rnti_t              crnti = to_rnti(0x4601));

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION FAILURE message.
f1ap_message generate_ue_context_modification_failure(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generate a dummy Paging message.
cu_cp_paging_message generate_paging_message();

/// \brief Generates dummy F1AP UE CONTEXT RELEASE REQUEST message.
f1ap_message generate_ue_context_release_request(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

} // namespace srs_cu_cp
} // namespace srsran
