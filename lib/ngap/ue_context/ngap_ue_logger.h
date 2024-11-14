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
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cu_types.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/format/prefixed_logger.h"
#include "fmt/format.h"
#include <string.h>

namespace srsran {
namespace srs_cu_cp {

class ngap_ue_log_prefix
{
public:
  ngap_ue_log_prefix(ue_index_t  ue_index,
                     ran_ue_id_t ran_ue_id = ran_ue_id_t::invalid,
                     amf_ue_id_t amf_ue_id = amf_ue_id_t::invalid)
  {
    fmt::memory_buffer buffer;
    fmt::format_to(buffer,
                   "ue={}{}{}: ",
                   ue_index,
                   ran_ue_id != ran_ue_id_t::invalid ? fmt::format(" ran_ue={}", ran_ue_id) : "",
                   amf_ue_id != amf_ue_id_t::invalid ? fmt::format(" amf_ue={}", amf_ue_id) : "");
    prefix = srsran::to_c_str(buffer);
  }
  const char* to_c_str() const { return prefix.c_str(); }

private:
  std::string prefix;
};

using ngap_ue_logger = prefixed_logger<ngap_ue_log_prefix>;

} // namespace srs_cu_cp
} // namespace srsran

namespace fmt {

// associated formatter
template <>
struct formatter<srsran::srs_cu_cp::ngap_ue_log_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::srs_cu_cp::ngap_ue_log_prefix o, FormatContext& ctx)

  {
    return format_to(ctx.out(), "{}", o.to_c_str());
  }
};
} // namespace fmt
