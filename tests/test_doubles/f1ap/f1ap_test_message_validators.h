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

#include "srsran/f1ap/common/f1ap_ue_id.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/rnti.h"

namespace srsran {

struct f1ap_message;

namespace test_helpers {

bool is_init_ul_rrc_msg_transfer_valid(const f1ap_message& msg, rnti_t rnti);

bool is_ul_rrc_msg_transfer_valid(const f1ap_message& msg, srb_id_t srb_id);

bool is_ue_context_setup_response_valid(const f1ap_message& msg);

} // namespace test_helpers
} // namespace srsran