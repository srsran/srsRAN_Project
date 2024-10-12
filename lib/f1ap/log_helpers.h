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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// \brief Helper for logging Rx/Tx F1AP PDUs for the CU-CP and DU.
template <typename UeIndex>
void log_f1ap_pdu(srslog::basic_logger&         logger,
                  bool                          is_rx,
                  gnb_du_id_t                   du_id,
                  const std::optional<UeIndex>& ue_id,
                  const f1ap_message&           msg,
                  bool                          json_enabled);

extern template void log_f1ap_pdu<srs_cu_cp::ue_index_t>(srslog::basic_logger&                       logger,
                                                         bool                                        is_rx,
                                                         gnb_du_id_t                                 du_id,
                                                         const std::optional<srs_cu_cp::ue_index_t>& ue_id,
                                                         const f1ap_message&                         msg,
                                                         bool                                        json_enabled);
extern template void log_f1ap_pdu<du_ue_index_t>(srslog::basic_logger&               logger,
                                                 bool                                is_rx,
                                                 gnb_du_id_t                         du_id,
                                                 const std::optional<du_ue_index_t>& ue_id,
                                                 const f1ap_message&                 msg,
                                                 bool                                json_enabled);

} // namespace srsran
