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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"

namespace srsran {
namespace srs_cu_cp {

/// Identifiers that associate a UE in the F1AP-CU.
struct f1ap_ue_ids {
  ue_index_t                         ue_index      = ue_index_t::invalid;
  gnb_cu_ue_f1ap_id_t                cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::invalid;
  std::optional<gnb_du_ue_f1ap_id_t> du_ue_f1ap_id;
};

} // namespace srs_cu_cp
} // namespace srsran
