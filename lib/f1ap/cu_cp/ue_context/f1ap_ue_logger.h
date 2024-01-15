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
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ran/cu_types.h"
#include "srsran/support/prefixed_logger.h"
#include "fmt/format.h"
#include <string.h>

namespace srsran {
namespace srs_cu_cp {

class f1ap_ue_log_prefix
{
public:
  f1ap_ue_log_prefix(ue_index_t          ue_index,
                     gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::invalid,
                     gnb_du_ue_f1ap_id_t du_ue_f1ap_id = gnb_du_ue_f1ap_id_t::invalid)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer,
                   "ue={}{}{}: ",
                   ue_index,
                   cu_ue_f1ap_id != gnb_cu_ue_f1ap_id_t::invalid ? fmt::format(" cu_ue_f1ap_id={}", cu_ue_f1ap_id) : "",
                   du_ue_f1ap_id != gnb_du_ue_f1ap_id_t::invalid ? fmt::format(" du_ue_f1ap_id={}", du_ue_f1ap_id)
                                                                 : "");
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using f1ap_ue_logger = prefixed_logger<f1ap_ue_log_prefix>;

} // namespace srs_cu_cp
} // namespace srsran

namespace fmt {

// associated formatter
template <>
struct formatter<srsran::srs_cu_cp::f1ap_ue_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_cp::f1ap_ue_log_prefix o, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};
} // namespace fmt
