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

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_common.h"

namespace srsran {
namespace srs_cu_cp {

class f1ap_du_management_notifier;

/// \brief Handle the F1 Removal Procedure as per TS 38.473 section 8.2.8.
void handle_f1_removal_procedure(const asn1::f1ap::f1_removal_request_s& request,
                                 du_index_t                              du_index,
                                 f1ap_message_notifier&                  pdu_notifier,
                                 f1ap_du_management_notifier&            du_management_notifier,
                                 srslog::basic_logger&                   logger);

} // namespace srs_cu_cp
} // namespace srsran