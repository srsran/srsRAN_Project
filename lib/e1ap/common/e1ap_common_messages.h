/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/asn1/e1ap/e1ap_ies.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/common/e1ap_types.h"

namespace srsran {

/// Generate an E1AP Error Indication message.
e1ap_message generate_error_indication(uint8_t                                      transaction_id,
                                       const std::optional<gnb_cu_up_ue_e1ap_id_t>& cu_up_ue_id,
                                       const std::optional<gnb_cu_cp_ue_e1ap_id_t>& cu_cp_ue_id,
                                       const std::optional<asn1::e1ap::cause_c>&    cause);

} // namespace srsran
