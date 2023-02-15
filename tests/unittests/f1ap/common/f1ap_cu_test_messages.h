/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1ap/common/f1ap_types.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Generate a dummy F1AP Served Cell Item.
asn1::f1ap::gnb_du_served_cells_item_s generate_served_cells_item(unsigned nrcell_id, unsigned nrpci);

/// \brief Generates dummy F1AP SETUP REQUEST message.
f1ap_message generate_f1_setup_request();

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
cu_cp_ue_context_modification_request
generate_ue_context_modification_request(ue_index_t ue_index, const std::initializer_list<drb_id_t>& drbs_to_add);

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION RESPONSE message.
f1ap_message generate_ue_context_modification_response(gnb_cu_ue_f1ap_id_t                    cu_ue_id,
                                                       gnb_du_ue_f1ap_id_t                    du_ue_id,
                                                       rnti_t                                 crnti = to_rnti(0x4601),
                                                       const std::initializer_list<drb_id_t>& drbs_added = {});

/// \brief Generates dummy F1AP UE CONTEXT MODIFICATION FAILURE message.
f1ap_message generate_ue_context_modification_failure(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id);

} // namespace srs_cu_cp
} // namespace srsgnb
