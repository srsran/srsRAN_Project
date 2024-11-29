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

#include "fmt/format.h"
#include <string>

namespace srsran {

/// RLC NR modes
enum class rlc_mode { tm, um_bidir, um_unidir_ul, um_unidir_dl, am };

inline bool from_string(rlc_mode& mode, const std::string& str)
{
  if (str == "am") {
    mode = rlc_mode::am;
    return true;
  }
  if (str == "um-bidir") {
    mode = rlc_mode::um_bidir;
    return true;
  }
  if (str == "um-unidir-ul") {
    mode = rlc_mode::um_unidir_ul;
    return true;
  }
  if (str == "um-unidir-dl") {
    mode = rlc_mode::um_unidir_dl;
    return true;
  }
  if (str == "tm") {
    mode = rlc_mode::tm;
    return true;
  }
  return false;
}

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::rlc_mode> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rlc_mode mode, FormatContext& ctx) const
  {
    static constexpr const char* options[] = {"TM", "UM Bi-dir", "UM Uni-dir-UL", "UM Uni-dir-DL", "AM"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(mode)]);
  }
};

} // namespace fmt
