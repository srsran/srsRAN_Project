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

#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/ran/cu_types.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/format/prefixed_logger.h"
#include "fmt/format.h"
#include <string.h>

namespace srsran {
namespace srs_cu_up {

class e1ap_ue_log_prefix
{
public:
  e1ap_ue_log_prefix(ue_index_t             ue_index,
                     gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid,
                     gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(
        buffer,
        "ue={}{}{}: ",
        ue_index,
        cu_up_ue_e1ap_id != gnb_cu_up_ue_e1ap_id_t::invalid ? fmt::format(" cu_up_ue_e1ap_id={}", cu_up_ue_e1ap_id)
                                                            : "",
        cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid ? fmt::format(" cu_cp_ue_e1ap_id={}", cu_cp_ue_e1ap_id)
                                                            : "");
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using e1ap_ue_logger = prefixed_logger<e1ap_ue_log_prefix>;

} // namespace srs_cu_up
} // namespace srsran

namespace fmt {

// associated formatter
template <>
struct formatter<srsran::srs_cu_up::e1ap_ue_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_up::e1ap_ue_log_prefix o, FormatContext& ctx)

  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};
} // namespace fmt
