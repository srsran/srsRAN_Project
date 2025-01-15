/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// \brief Helper for logging Rx/Tx E1AP PDUs for the CU-CP and CU-UP
template <typename UeIndex>
void log_e1ap_pdu(srslog::basic_logger&         logger,
                  bool                          is_rx,
                  const std::optional<UeIndex>& ue_id,
                  const e1ap_message&           e1ap_msg,
                  bool                          json_enabled);

extern template void log_e1ap_pdu<srs_cu_cp::ue_index_t>(srslog::basic_logger&                       logger,
                                                         bool                                        is_rx,
                                                         const std::optional<srs_cu_cp::ue_index_t>& ue_id,
                                                         const e1ap_message&                         e1ap_msg,
                                                         bool                                        json_enabled);
extern template void log_e1ap_pdu<srs_cu_up::ue_index_t>(srslog::basic_logger&                       logger,
                                                         bool                                        is_rx,
                                                         const std::optional<srs_cu_up::ue_index_t>& ue_id,
                                                         const e1ap_message&                         e1ap_msg,
                                                         bool                                        json_enabled);

} // namespace srsran
