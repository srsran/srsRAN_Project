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
