/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/ran/rnti.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/format_utils.h"

namespace srsran {

class du_procedure_logger
{
public:
  du_procedure_logger(srslog::basic_logger& logger_, const char* proc_name_) : logger(logger_), proc_name(proc_name_) {}

  void log_proc_started() { log_impl(logger.debug, "Procedure started"); }

  template <typename... Args>
  void log_progress(const char* fmt_str, Args&&... args)
  {
    if (not logger.debug.enabled()) {
      return;
    }
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, fmt_str, std::forward<Args>(args)...);
    log_impl(logger.debug, to_c_str(fmtbuf));
  }

  void log_proc_completed() { log_impl(logger.debug, "Procedure finished successfully"); }

  template <typename... Args>
  void log_proc_failure(const char* reason_fmt, Args&&... args)
  {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, "Procedure failed. Cause: ");
    fmt::format_to(fmtbuf, reason_fmt, std::forward<Args>(args)...);
    log_impl(logger.warning, to_c_str(fmtbuf));
  }

private:
  void log_impl(srslog::log_channel& log_ch, const char* result_str)
  {
    log_ch("proc=\"{}\": {}.", proc_name, result_str);
  }

  srslog::basic_logger& logger;
  const char*           proc_name;
};

class ue_procedure_logger
{
public:
  ue_procedure_logger(srslog::basic_logger& logger_,
                      const char*           proc_name_,
                      du_ue_index_t         ue_index_,
                      rnti_t                rnti_ = INVALID_RNTI) :
    logger(logger_), proc_name(proc_name_), ue_index(ue_index_), rnti(rnti_)
  {
  }

  void log_proc_started() { log_impl(logger.info, "Procedure started..."); }

  void log_proc_completed() { log_impl(logger.info, "Procedure finished successfully"); }

  template <typename... Args>
  void log_proc_failure(const char* reason_fmt, Args&&... args)
  {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, "Procedure failed. Cause: ");
    fmt::format_to(fmtbuf, reason_fmt, std::forward<Args>(args)...);
    log_impl(logger.warning, to_c_str(fmtbuf));
  }

  template <typename... Args>
  void log_proc_warning(const char* event_fmt, Args&&... args)
  {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, event_fmt, std::forward<Args>(args)...);
    log_impl(logger.warning, to_c_str(fmtbuf));
  }

private:
  void log_impl(srslog::log_channel& log_ch, const char* result_str)
  {
    if (rnti == INVALID_RNTI) {
      log_ch("ue={} proc=\"{}\": {}.", ue_index, proc_name, result_str);
    } else {
      log_ch("ue={} rnti={:#x} proc=\"{}\": {}.", ue_index, rnti, proc_name, result_str);
    }
  }

  srslog::basic_logger& logger;
  const char*           proc_name;
  du_ue_index_t         ue_index;
  rnti_t                rnti;
};

} // namespace srsran