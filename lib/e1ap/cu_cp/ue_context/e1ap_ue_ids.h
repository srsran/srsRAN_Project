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
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/support/format_utils.h"

namespace srsran {
namespace srs_cu_cp {

/// IDs that identify an E1AP UE context.
struct e1ap_ue_ids {
  /// UE identifier that is internal to the CU-CP.
  ue_index_t ue_index = ue_index_t::invalid;
  /// CU-CP-UE-E1AP-ID.
  const gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid;
  /// CU-UP-UE-E1AP-ID.
  gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid;
};

} // namespace srs_cu_cp
} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::srs_cu_cp::e1ap_ue_ids> : public basic_fmt_parser {
  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::e1ap_ue_ids& p, FormatContext& ctx)
  {
    using namespace srsran;
    using namespace srsran::srs_cu_cp;
    const char* sep = "";

    if (p.ue_index != ue_index_t::invalid) {
      fmt::format_to(ctx.out(), "ue={}", p.ue_index);
      sep = " ";
    }
    if (p.cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid) {
      fmt::format_to(ctx.out(), "{}cu_cp_ue={}", sep, p.cu_cp_ue_e1ap_id);
      sep = " ";
    }
    if (p.cu_up_ue_e1ap_id != gnb_cu_up_ue_e1ap_id_t::invalid) {
      fmt::format_to(ctx.out(), "{}cu_up_ue={}", sep, p.cu_up_ue_e1ap_id);
    }
    return ctx.out();
  }
};

} // namespace fmt