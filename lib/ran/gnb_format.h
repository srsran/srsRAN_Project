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

#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/rnti.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/format_utils.h"

namespace srsran {

struct ue_event_prefix {
  const char*     direction;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
  du_cell_index_t cell_index;
  const char*     channel = nullptr;
  lcid_t          lcid;

  ue_event_prefix(const char*     dir_      = "CTRL",
                  du_ue_index_t   ue_index_ = MAX_NOF_DU_UES,
                  rnti_t          rnti_     = rnti_t::INVALID_RNTI,
                  du_cell_index_t cell_idx_ = MAX_NOF_DU_CELLS,
                  const char*     channel_  = nullptr,
                  lcid_t          lcid_     = INVALID_LCID) :
    direction(dir_), ue_index(ue_index_), rnti(rnti_), cell_index(cell_idx_), channel(channel_), lcid(lcid_)
  {
  }

  ue_event_prefix& set_type(const char* t)
  {
    direction = t;
    return *this;
  }
  ue_event_prefix& set_channel(const char* ch_)
  {
    channel = ch_;
    return *this;
  }

  ue_event_prefix& operator|(rnti_t rnti_)
  {
    rnti = rnti_;
    return *this;
  }

  ue_event_prefix& operator|(du_ue_index_t ue_index_)
  {
    ue_index = ue_index_;
    return *this;
  }

  ue_event_prefix& operator|(du_cell_index_t cell_index_)
  {
    cell_index = cell_index_;
    return *this;
  }

  ue_event_prefix& operator|(lcid_t lcid_)
  {
    lcid = lcid_;
    return *this;
  }
};

inline void log_proc_started(srslog::basic_logger& logger, du_ue_index_t ue_index, rnti_t rnti, const char* proc_name)
{
  logger.debug("{}: \"{}\" started.", ue_event_prefix{"CTRL", ue_index, rnti}, proc_name);
}

inline void log_proc_started(srslog::basic_logger& logger, du_ue_index_t ue_index, const char* proc_name)
{
  logger.debug("{}: \"{}\" started.", ue_event_prefix{"CTRL", ue_index}, proc_name);
}

inline void log_proc_completed(srslog::basic_logger& logger, du_ue_index_t ue_index, rnti_t rnti, const char* proc_name)
{
  logger.debug("{}: \"{}\" completed.", ue_event_prefix{"CTRL", ue_index, rnti}, proc_name);
}

template <typename... Args>
void log_proc_failure(srslog::basic_logger& logger,
                      du_ue_index_t         ue_index,
                      const char*           proc_name,
                      const char*           cause_fmt = "",
                      Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  if (strcmp(cause_fmt, "") != 0) {
    fmt::format_to(fmtbuf, "Cause: ");
    fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  }
  logger.warning("{}: \"{}\" failed. {}", ue_event_prefix{"CTRL", ue_index}, proc_name, to_c_str(fmtbuf));
}

template <typename... Args>
void log_proc_failure(srslog::basic_logger& logger,
                      du_ue_index_t         ue_index,
                      rnti_t                rnti,
                      const char*           proc_name,
                      const char*           cause_fmt = "",
                      Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  if (strcmp(cause_fmt, "") != 0) {
    fmt::format_to(fmtbuf, "Cause: ");
    fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  }
  logger.warning("{}: \"{}\" failed. {}", ue_event_prefix{"CTRL", ue_index, rnti}, proc_name, to_c_str(fmtbuf));
}

template <typename... Args>
void log_proc_event(srslog::basic_logger& logger,
                    du_ue_index_t         ue_index,
                    const char*           proc_name,
                    const char*           cause_fmt,
                    Args&&... args)
{
  if (not logger.debug.enabled()) {
    return;
  }
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  logger.debug("{}: {}", ue_event_prefix{"CTRL", ue_index}, to_c_str(fmtbuf));
}

template <typename... Args>
void log_ue_event(srslog::basic_logger& logger, const ue_event_prefix& ue_prefix, const char* cause_fmt, Args&&... args)
{
  if (not logger.debug.enabled()) {
    return;
  }
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  logger.debug("{}: {}", ue_prefix, to_c_str(fmtbuf));
}

template <typename... Args>
void log_ue_proc_event(srslog::log_channel&   log_ch,
                       const ue_event_prefix& ue_prefix,
                       const char*            proc_name,
                       const char*            cause_fmt,
                       Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "\"{}\" ", proc_name);
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  log_ch("{}: {}", ue_prefix, to_c_str(fmtbuf));
}

template <typename... Args>
void log_ul_pdu(srslog::basic_logger& logger,
                du_ue_index_t         ue_index,
                rnti_t                rnti,
                du_cell_index_t       cell_index,
                const char*           ch,
                const char*           cause_fmt,
                Args&&... args)
{
  log_ue_event(logger, ue_event_prefix{"UL", ue_index, rnti, cell_index, ch}, cause_fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_ul_pdu(srslog::basic_logger& logger, rnti_t rnti, du_cell_index_t cc, const char* cause_fmt, Args&&... args)
{
  log_ue_event(logger, ue_event_prefix{"UL", MAX_NOF_DU_UES, rnti, cc}, cause_fmt, std::forward<Args>(args)...);
}

} // namespace srsran

namespace fmt {

/// FMT formatter of slot_point type.
template <>
struct formatter<srsran::ue_event_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(const srsran::ue_event_prefix& ue_prefix, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    using namespace srsran;
    auto ret = format_to(ctx.out(), "{:<4}", ue_prefix.direction);
    if (ue_prefix.ue_index != srsran::MAX_NOF_DU_UES) {
      ret = format_to(ctx.out(), " ueId={}", ue_prefix.ue_index);
    } else {
      ret = format_to(ctx.out(), "{: <7}", "");
    }
    if (ue_prefix.rnti != srsran::rnti_t::INVALID_RNTI) {
      ret = format_to(ctx.out(), " {}", ue_prefix.rnti);
    } else {
      ret = format_to(ctx.out(), " {: <6}", "");
    }
    if (ue_prefix.cell_index != srsran::MAX_NOF_DU_CELLS) {
      ret = format_to(ctx.out(), " cell={}", ue_prefix.cell_index);
    }
    if (ue_prefix.channel != nullptr) {
      ret = format_to(ctx.out(), " {}", ue_prefix.channel);
    }
    if (ue_prefix.lcid <= MAX_LCID) {
      ret = format_to(ctx.out(), " {}", ue_prefix.lcid);
    }
    return ret;
  }
};

} // namespace fmt
