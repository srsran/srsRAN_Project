/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/f1ap/common/f1ap_types.h"
#include "srsran/ran/lcid.h"

namespace srsran {

struct f1ap_message;

namespace test_helpers {

/// \brief Generates dummy F1AP UE CONTEXT SETUP REQUEST message.
f1ap_message create_ue_context_setup_request(gnb_cu_ue_f1ap_id_t           cu_ue_id,
                                             optional<gnb_du_ue_f1ap_id_t> du_ue_id,
                                             const std::vector<drb_id_t>&  drbs_to_setup);

} // namespace test_helpers
} // namespace srsran