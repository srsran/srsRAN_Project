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
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/support/format/fmt_basic_parser.h"

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
struct formatter<srsran::srs_cu_cp::e1ap_ue_ids> : public basic_parser {
  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::e1ap_ue_ids& p, FormatContext& ctx) const
  {
    using namespace srsran;
    using namespace srsran::srs_cu_cp;
    const char* sep = "";

    if (p.ue_index != ue_index_t::invalid) {
      fmt::format_to(ctx.out(), "ue={}", p.ue_index);
      sep = " ";
    }
    if (p.cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid) {
      fmt::format_to(ctx.out(), "{}cu_cp_ue={}", sep, fmt::underlying(p.cu_cp_ue_e1ap_id));
      sep = " ";
    }
    if (p.cu_up_ue_e1ap_id != gnb_cu_up_ue_e1ap_id_t::invalid) {
      fmt::format_to(ctx.out(), "{}cu_up_ue={}", sep, fmt::underlying(p.cu_up_ue_e1ap_id));
    }
    return ctx.out();
  }
};

} // namespace fmt
