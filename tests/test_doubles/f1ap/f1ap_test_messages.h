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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/rnti.h"

namespace srsran {

struct f1ap_message;

namespace test_helpers {

f1ap_message create_f1_setup_response(const f1ap_message& f1_setup_request);

/// \brief Generates dummy F1AP UE CONTEXT SETUP REQUEST message.
f1ap_message create_ue_context_setup_request(gnb_cu_ue_f1ap_id_t           cu_ue_id,
                                             optional<gnb_du_ue_f1ap_id_t> du_ue_id,
                                             const std::vector<drb_id_t>&  drbs_to_setup);

/// \brief Generates F1AP Initial UL RRC TRANSFER message.
f1ap_message create_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                 rnti_t              crnti,
                                                 byte_buffer         cell_group_cfg = {},
                                                 byte_buffer         rrc_container  = {});

} // namespace test_helpers
} // namespace srsran