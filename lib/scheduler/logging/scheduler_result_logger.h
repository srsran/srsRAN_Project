/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/scheduler/scheduler_slot_handler.h"
#include "srsgnb/support/format_utils.h"

namespace srsgnb {

class scheduler_result_logger
{
public:
  scheduler_result_logger() : logger(srslog::fetch_basic_logger("MAC")) {}

  void log(const sched_result& result)
  {
    if (logger.debug.enabled()) {
      log_debug(result);
    } else if (logger.info.enabled()) {
      log_info(result);
    }
  }

private:
  void log_debug(const sched_result& result)
  {
    fmtbuf.clear();

    if (not result.dl.bc.ssb_info.empty()) {
      fmt::format_to(fmtbuf, "\nSSB ({} grants):", result.dl.bc.ssb_info.size());
      for (const ssb_information& ssb_info : result.dl.bc.ssb_info) {
        fmt::format_to(
            fmtbuf, "\n- ssbIdx={}, crbs={}, symbols={}", ssb_info.ssb_index, ssb_info.crbs, ssb_info.symbols);
      }
    }

    if (not result.ul.puschs.empty()) {
      fmt::format_to(fmtbuf, "\nPUSCH ({} grants):", result.ul.puschs.size());
      for (const ul_sched_info& ul_info : result.ul.puschs) {
        fmt::format_to(fmtbuf,
                       "\n- rnti={:#x}, h_id={}, prbs={}, symbols={}, rv_idx={}",
                       ul_info.pusch_cfg.rnti,
                       ul_info.pusch_cfg.harq_id,
                       ul_info.pusch_cfg.prbs.prbs(),
                       ul_info.pusch_cfg.symbols,
                       ul_info.pusch_cfg.rv_index);
      }
    }

    if (fmtbuf.size() > 0) {
      logger.debug("SCHED decisions:{}", to_c_str(fmtbuf));
    }
  }

  void log_info(const sched_result& result) { fmtbuf.clear(); }

  srslog::basic_logger& logger;
  fmt::memory_buffer    fmtbuf;
};

} // namespace srsgnb