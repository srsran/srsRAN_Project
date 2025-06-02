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

#include "../../../test_doubles/f1ap/f1ap_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include <optional>

namespace srsran {
namespace srs_cu_cp {

/// \brief Generates dummy F1AP Initial UL RRC Transfer message without DU to CU container.
f1ap_message generate_init_ul_rrc_message_transfer_without_du_to_cu_container(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                              rnti_t              crnti);

/// \brief Generates dummy F1AP Initial UL RRC TRANSFER message.
f1ap_message generate_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                   rnti_t              crnti           = to_rnti(0x4601),
                                                   byte_buffer         du2cu_container = {});

/// \brief Generates dummy F1AP UL RRC TRANSFER message.
f1ap_message generate_ul_rrc_message_transfer(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                              gnb_du_ue_f1ap_id_t du_ue_id,
                                              srb_id_t            srb_id = srb_id_t::srb1,
                                              byte_buffer rrc_container = byte_buffer::create({0x1, 0x2, 0x3}).value());

/// \brief Generates dummy F1AP UE CONTEXT RELEASE COMPLETE message.
f1ap_message generate_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP UE CONTEXT SETUP REQUEST message.
f1ap_message generate_ue_context_setup_request(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy F1AP UE CONTEXT SETUP RESPONSE message.
f1ap_message generate_ue_context_setup_response(
    gnb_cu_ue_f1ap_id_t   cu_ue_id,
    gnb_du_ue_f1ap_id_t   du_ue_id,
    std::optional<rnti_t> crnti = std::nullopt,
    byte_buffer           cell_group_config =
        make_byte_buffer("5c02b091117aec701061e000b1c03544cde4a20c7c080408c008241000100000").value());

/// \brief Generates dummy F1AP UE CONTEXT SETUP FAILURE message.
f1ap_message generate_ue_context_setup_failure(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generates dummy CU-CP UE CONTEXT MODIFICATION REQUEST message.
f1ap_ue_context_modification_request generate_ue_context_modification_request(ue_index_t ue_index);

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION RESPONSE message.
f1ap_message
generate_ue_context_modification_response(gnb_cu_ue_f1ap_id_t          cu_ue_id,
                                          gnb_du_ue_f1ap_id_t          du_ue_id,
                                          rnti_t                       crnti               = to_rnti(0x4601),
                                          const std::vector<drb_id_t>& drbs_setup_mod_list = {drb_id_t::drb1},
                                          const std::vector<drb_id_t>& drbs_modified_list  = {});

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION FAILURE message.
f1ap_message generate_ue_context_modification_failure(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

/// \brief Generate a dummy PAGING message.
cu_cp_paging_message generate_paging_message();

/// \brief Generate a dummy GNB-CU CONFIGURATION FAILURE message.
f1ap_message generate_gnb_cu_configuration_update_failure();

/// \brief Generate a dummy GNB-CU CONFIGURATION ACKNOWLEDGEMENT message.
f1ap_message generate_gnb_cu_configuration_update_acknowledgement(
    const std::vector<f1ap_cell_failed_to_activate>& cells_failed_to_activate);

} // namespace srs_cu_cp
} // namespace srsran
