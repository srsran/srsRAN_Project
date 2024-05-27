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

#include "srsran/adt/optional.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"

namespace srsran {

/// Generate an F1AP Error Indication message.
f1ap_message generate_error_indication(uint8_t                                   transaction_id,
                                       const std::optional<gnb_du_ue_f1ap_id_t>& du_ue_id,
                                       const std::optional<gnb_cu_ue_f1ap_id_t>& cu_ue_id,
                                       const std::optional<asn1::f1ap::cause_c>& cause);

} // namespace srsran
