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

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/format_utils.h"

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
