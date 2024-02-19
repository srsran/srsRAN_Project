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

#include "../../common/proc_logger.h"
#include "f1ap_ue_ids.h"
#include "srsran/support/prefixed_logger.h"

namespace srsran {
namespace srs_cu_cp {

/// Log prefix for UEs in F1AP-CU.
struct f1ap_ue_log_prefix : public srsran::f1ap_common_log_prefix {
  using srsran::f1ap_common_log_prefix::f1ap_common_log_prefix;

  f1ap_ue_log_prefix(ue_index_t ue_index_, gnb_cu_ue_f1ap_id_t cu_ue_id_, gnb_du_ue_f1ap_id_t du_ue_id_) :
    srsran::f1ap_common_log_prefix(du_ue_id_, cu_ue_id_), ue_index(ue_index_)
  {
  }
  f1ap_ue_log_prefix(const f1ap_ue_ids& context_, const char* proc_name_ = nullptr) :
    srsran::f1ap_common_log_prefix(context_.du_ue_f1ap_id, context_.cu_ue_f1ap_id, proc_name_),
    ue_index(context_.ue_index)
  {
  }

  ue_index_t ue_index = ue_index_t::invalid;
};

/// Logger that automatically prefixes log messages with the UE IDs.
using f1ap_ue_logger = prefixed_logger<f1ap_ue_log_prefix>;

} // namespace srs_cu_cp
} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::srs_cu_cp::f1ap_ue_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::srs_cu_cp::f1ap_ue_log_prefix& prefix, FormatContext& ctx)
  {
    bool needs_sep = prefix.ue_index != srsran::srs_cu_cp::ue_index_t::invalid;
    if (prefix.ue_index != srsran::srs_cu_cp::ue_index_t::invalid) {
      format_to(ctx.out(), "ue={}", prefix.ue_index);
    }
    return format_to(
        ctx.out(), "{}{}", needs_sep ? " " : "", static_cast<const srsran::f1ap_common_log_prefix&>(prefix));
  }
};

} // namespace fmt
