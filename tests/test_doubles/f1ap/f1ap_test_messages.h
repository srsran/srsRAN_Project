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

///\file
///\brief f1ap_test_messages: Contains helper functions to generate dummy F1AP messages for testing purposes.

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/nr_cell_identity.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"

namespace srsran {

struct f1ap_message;

namespace test_helpers {

/// \brief Generate a dummy F1AP Served Cell Item.
asn1::f1ap::gnb_du_served_cells_item_s generate_served_cells_item(nr_cell_identity nci, pci_t nrpci, unsigned tac = 7);

/// \brief Generates dummy F1AP SETUP REQUEST message.
f1ap_message generate_f1_setup_request(gnb_du_id_t      gnb_du_id = int_to_gnb_du_id(0x11),
                                       nr_cell_identity nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0U).value(),
                                       pci_t            pci = 0,
                                       unsigned         tac = 7);

/// \brief Generates dummy F1 SETUP RESPONSE message based on the request.
f1ap_message generate_f1_setup_response(const f1ap_message& f1_setup_request);

/// \brief Generates dummy F1 REMOVAL REQUEST message.
f1ap_message generate_f1_removal_request(unsigned transaction_id);

/// \brief Generates dummy F1 REMOVAL RESPONSE message based on the request.
f1ap_message generate_f1_removal_response(const f1ap_message& f1_removal_request);

/// \brief Generates dummy F1AP UE CONTEXT SETUP REQUEST message.
f1ap_message create_ue_context_setup_request(gnb_cu_ue_f1ap_id_t                cu_ue_id,
                                             std::optional<gnb_du_ue_f1ap_id_t> du_ue_id,
                                             uint32_t                           rrc_container_pdcp_sn,
                                             const std::vector<drb_id_t>&       drbs_to_setup);

/// \brief Generates F1AP Initial UL RRC TRANSFER message.
f1ap_message create_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                 rnti_t              crnti,
                                                 byte_buffer         cell_group_cfg = {},
                                                 byte_buffer         rrc_container  = {});

/// \brief Generates F1AP DL RRC TRANSFER message with provided RRC container.
f1ap_message create_dl_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                            gnb_cu_ue_f1ap_id_t cu_ue_id,
                                            srb_id_t            srb_id,
                                            byte_buffer         rrc_container);

/// \brief Generates F1AP UL RRC TRANSFER message with provided RRC container.
f1ap_message create_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                            gnb_cu_ue_f1ap_id_t cu_ue_id,
                                            srb_id_t            srb_id,
                                            byte_buffer         rrc_container);

/// \brief Generates F1AP UL RRC TRANSFER message with provided RRC UL DCCH message and PDCP header with provided SN.
f1ap_message create_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                            gnb_cu_ue_f1ap_id_t cu_ue_id,
                                            srb_id_t            srb_id,
                                            uint32_t            pdcp_sn,
                                            byte_buffer         ul_dcch_msg);

/// \brief Generates dummy F1AP UE CONTEXT RELEASE REQUEST message.
f1ap_message generate_ue_context_release_request(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP UE CONTEXT RELEASE COMPLETE message.
f1ap_message generate_ue_context_release_complete(const f1ap_message& ue_ctxt_release_cmd);
f1ap_message generate_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION RESPONSE message.
f1ap_message
generate_ue_context_modification_response(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id, rnti_t rnti);

/// \brief Remove PDCP header from DL-DCCH message.
byte_buffer extract_dl_dcch_msg(const byte_buffer& rrc_container);

} // namespace test_helpers
} // namespace srsran
