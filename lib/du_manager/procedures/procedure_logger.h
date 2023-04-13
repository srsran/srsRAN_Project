/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

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

  void log_proc_started() { log_impl(logger.debug, "started"); }

  void log_proc_completed() { log_impl(logger.debug, "finished successfully"); }

  template <typename... Args>
  void log_proc_failure(const char* reason_fmt, Args&&... args)
  {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, "failed. Cause: ");
    fmt::format_to(fmtbuf, reason_fmt, std::forward<Args>(args)...);
    log_impl(logger.warning, to_string(fmtbuf).c_str());
  }

private:
  void log_impl(srslog::log_channel& log_ch, const char* result_str)
  {
    if (rnti == INVALID_RNTI) {
      log_ch("ue={}: \"{}\" {}.", ue_index, proc_name, result_str);
    } else {
      log_ch("ue={} rnti={:#x}: \"{}\" {}.", ue_index, rnti, proc_name, result_str);
    }
  }

  srslog::basic_logger& logger;
  const char*           proc_name;
  du_ue_index_t         ue_index;
  rnti_t                rnti;
};

} // namespace srsran