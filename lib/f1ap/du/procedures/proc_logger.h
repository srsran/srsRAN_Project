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

#include "../../proc_logger.h"
#include "../ue_context/f1ap_ue_context.h"

namespace srsran {
namespace srs_du {

struct f1ap_log_prefix : public srsran::f1ap_common_log_prefix {
  using srsran::f1ap_common_log_prefix::f1ap_common_log_prefix;

  f1ap_log_prefix(const f1ap_ue_context& context_, const char* proc_name_) :
    srsran::f1ap_common_log_prefix(context_.gnb_du_ue_f1ap_id, context_.gnb_cu_ue_f1ap_id, proc_name_),
    ue_index(context_.ue_index)
  {
  }

  du_ue_index_t ue_index = INVALID_DU_UE_INDEX;
};

} // namespace srs_du
} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::srs_du::f1ap_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_du::f1ap_log_prefix& prefix, FormatContext& ctx) const
  {
    bool needs_sep = prefix.ue_index != srsran::INVALID_DU_UE_INDEX;
    if (prefix.ue_index != srsran::INVALID_DU_UE_INDEX) {
      format_to(ctx.out(), "ue={}", fmt::underlying(prefix.ue_index));
    }
    return format_to(
        ctx.out(), "{}{}", needs_sep ? " " : "", static_cast<const srsran::f1ap_common_log_prefix&>(prefix));
  }
};

} // namespace fmt
