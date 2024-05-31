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

#include "srsran/adt/optional.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_message.h"
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
