/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_result_logger.h"

using namespace srsgnb;

void scheduler_result_logger::log_debug(const sched_result& result)
{
  if (not result.dl.bc.ssb_info.empty()) {
    fmt::format_to(fmtbuf, "\nSSB ({} grants):", result.dl.bc.ssb_info.size());
    for (const ssb_information& ssb_info : result.dl.bc.ssb_info) {
      fmt::format_to(fmtbuf, "\n- ssbIdx={}, crbs={}, symbols={}", ssb_info.ssb_index, ssb_info.crbs, ssb_info.symbols);
    }
  }

  if (not result.dl.bc.sibs.empty() or not result.dl.rar_grants.empty() or not result.dl.ue_grants.empty()) {
    unsigned nof_pdschs = result.dl.bc.sibs.size() + result.dl.rar_grants.size() + result.dl.ue_grants.size();
    fmt::format_to(fmtbuf, "\nPDSCH ({} grants):", nof_pdschs);
    for (const sib_information& sib : result.dl.bc.sibs) {
      fmt::format_to(fmtbuf,
                     "\n- si{}, prbs={}, symbols={}, tbs={}B, mcs={}, rv={}",
                     sib.si_indicator == sib_information::sib1 ? "b1" : "",
                     sib.pdsch_cfg.prbs.prbs(),
                     sib.pdsch_cfg.symbols,
                     sib.pdsch_cfg.codewords[0].tb_size_bytes,
                     sib.pdsch_cfg.codewords[0].mcs_index,
                     sib.pdsch_cfg.codewords[0].rv_index);
    }
    for (const rar_information& rar : result.dl.rar_grants) {
      fmt::format_to(fmtbuf,
                     "\n- rar, ra-rnti={:#x}, prbs={}, symbols={}, tbs={}B, mcs={}, rv={}, grants ({}): [",
                     rar.pdsch_cfg.rnti,
                     rar.pdsch_cfg.prbs.prbs(),
                     rar.pdsch_cfg.symbols,
                     rar.pdsch_cfg.codewords[0].tb_size_bytes,
                     rar.pdsch_cfg.codewords[0].mcs_index,
                     rar.pdsch_cfg.codewords[0].rv_index,
                     rar.grants.size());
      for (const rar_ul_grant& grant : rar.grants) {
        fmt::format_to(fmtbuf,
                       "{{tc-rnti={:#x}, rapid={}, ta={}}}{}",
                       grant.temp_crnti,
                       grant.rapid,
                       grant.ta,
                       (&grant == &rar.grants.back()) ? "]" : ", ");
      }
    }
    for (const dl_msg_alloc& ue_dl_grant : result.dl.ue_grants) {
      fmt::format_to(fmtbuf,
                     "\n- ue, c-rnti={:#x}, prbs={}, symbols={}, tbs={}B, mcs={}, rv={}, sdus=[",
                     ue_dl_grant.pdsch_cfg.rnti,
                     ue_dl_grant.pdsch_cfg.prbs.prbs(),
                     ue_dl_grant.pdsch_cfg.symbols,
                     ue_dl_grant.pdsch_cfg.codewords[0].tb_size_bytes,
                     ue_dl_grant.pdsch_cfg.codewords[0].mcs_index,
                     ue_dl_grant.pdsch_cfg.codewords[0].rv_index);
      for (const dl_msg_lc_info& lc : ue_dl_grant.tb_list[0].subpdus) {
        fmt::format_to(fmtbuf,
                       "{{lcid={}, size={}B}}{}",
                       lc.lcid,
                       lc.sched_bytes,
                       &lc == &ue_dl_grant.tb_list[0].subpdus.back() ? "]" : ", ");
      }
    }
  }

  if (not result.ul.puschs.empty()) {
    fmt::format_to(fmtbuf, "\nPUSCH ({} grants):", result.ul.puschs.size());
    for (const ul_sched_info& ul_info : result.ul.puschs) {
      fmt::format_to(fmtbuf,
                     "\n- ue, rnti={:#x}, h_id={}, prbs={}, symbols={}, rv_idx={}",
                     ul_info.pusch_cfg.rnti,
                     ul_info.pusch_cfg.harq_id,
                     ul_info.pusch_cfg.prbs.prbs(),
                     ul_info.pusch_cfg.symbols,
                     ul_info.pusch_cfg.rv_index);
    }
  }

  if (fmtbuf.size() > 0) {
    logger.debug("SCHED grants:{}", to_c_str(fmtbuf));
    fmtbuf.clear();
  }
}

void scheduler_result_logger::log_info(const sched_result& result)
{
  for (const sib_information& sib_info : result.dl.bc.sibs) {
    fmt::format_to(fmtbuf,
                   "{}SI{}: prbs={}, tbs={}B",
                   fmtbuf.size() == 0 ? "" : ", ",
                   sib_info.si_indicator == sib_information::sib1 ? "B1" : "",
                   sib_info.pdsch_cfg.prbs.prbs(),
                   sib_info.pdsch_cfg.codewords[0].tb_size_bytes);
  }
  for (const rar_information& rar_info : result.dl.rar_grants) {
    fmt::format_to(fmtbuf,
                   "{}RAR: ra-rnti={:#x}, prbs={}, tbs={}B",
                   fmtbuf.size() == 0 ? "" : ", ",
                   rar_info.pdsch_cfg.rnti,
                   rar_info.pdsch_cfg.prbs.prbs(),
                   rar_info.pdsch_cfg.codewords[0].tb_size_bytes);
  }
  for (const dl_msg_alloc& ue_msg : result.dl.ue_grants) {
    fmt::format_to(fmtbuf,
                   "{}DL: c-rnti={:#x}, prbs={}, tbs={}B",
                   fmtbuf.size() == 0 ? "" : ", ",
                   ue_msg.pdsch_cfg.rnti,
                   ue_msg.pdsch_cfg.prbs.prbs(),
                   ue_msg.pdsch_cfg.codewords[0].tb_size_bytes);
  }
  for (const ul_sched_info& ue_msg : result.ul.puschs) {
    fmt::format_to(fmtbuf,
                   "{}UL: rnti={:#x}, prbs={}, tbs={}B",
                   fmtbuf.size() == 0 ? "" : ", ",
                   ue_msg.pusch_cfg.rnti,
                   ue_msg.pusch_cfg.prbs.prbs(),
                   ue_msg.pusch_cfg.tb_size_bytes);
  }

  if (fmtbuf.size() > 0) {
    logger.info("SCHED grants: {}", to_c_str(fmtbuf));
    fmtbuf.clear();
  }
}

void scheduler_result_logger::log(const sched_result& result)
{
  if (logger.debug.enabled()) {
    log_debug(result);
  } else if (logger.info.enabled()) {
    log_info(result);
  }
}
