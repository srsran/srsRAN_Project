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
  for (const ssb_information& ssb_info : result.dl.bc.ssb_info) {
    fmt::format_to(
        fmtbuf, "\n- SSB: ssbIdx={}, crbs={}, symbols={}", ssb_info.ssb_index, ssb_info.crbs, ssb_info.symbols);
  }
  for (const pdcch_dl_information& pdcch : result.dl.dl_pdcchs) {
    fmt::format_to(fmtbuf,
                   "\n- DL PDCCH: rnti={:#x} type={} format={} cce={} al={}",
                   pdcch.ctx.rnti,
                   dci_dl_rnti_config_rnti_type(pdcch.dci.type),
                   dci_dl_rnti_config_format(pdcch.dci.type),
                   pdcch.ctx.cces.ncce,
                   to_nof_cces(pdcch.ctx.cces.aggr_lvl));
    switch (pdcch.dci.type) {
      case dci_dl_rnti_config_type::c_rnti_f1_0: {
        auto& dci = pdcch.dci.c_rnti_f1_0;
        fmt::format_to(fmtbuf,
                       "dci: h_id={} ndi={} rv={} mcs={}",
                       dci.harq_process_number,
                       dci.new_data_indicator,
                       dci.redundancy_version,
                       dci.modulation_coding_scheme);
      } break;
      case dci_dl_rnti_config_type::tc_rnti_f1_0: {
        auto& dci = pdcch.dci.tc_rnti_f1_0;
        fmt::format_to(fmtbuf,
                       "dci: h_id={} ndi={} rv={} mcs={}",
                       dci.harq_process_number,
                       dci.new_data_indicator,
                       dci.redundancy_version,
                       dci.modulation_coding_scheme);
      } break;
      default:
        break;
    }
  }
  for (const pdcch_ul_information& pdcch : result.dl.ul_pdcchs) {
    fmt::format_to(fmtbuf,
                   "\n- UL PDCCH: rnti={:#x} rnti_type={} format={} cce={} al={}",
                   pdcch.ctx.rnti,
                   dci_ul_rnti_config_rnti_type(pdcch.dci.type),
                   dci_ul_rnti_config_format(pdcch.dci.type),
                   pdcch.ctx.cces.ncce,
                   to_nof_cces(pdcch.ctx.cces.aggr_lvl));
    switch (pdcch.dci.type) {
      case dci_ul_rnti_config_type::c_rnti_f0_0: {
        auto& dci = pdcch.dci.c_rnti_f0_0;
        fmt::format_to(fmtbuf,
                       "h_id={} ndi={} rv={} mcs={}",
                       dci.harq_process_number,
                       dci.new_data_indicator,
                       dci.redundancy_version,
                       dci.modulation_coding_scheme);
      } break;
      default:
        break;
    }
  }

  for (const sib_information& sib : result.dl.bc.sibs) {
    fmt::format_to(fmtbuf,
                   "\n- SI{} PDSCH: prbs={} symbols={} tbs={}bytes mcs={} rv={}",
                   sib.si_indicator == sib_information::sib1 ? "B1" : "",
                   sib.pdsch_cfg.prbs.prbs(),
                   sib.pdsch_cfg.symbols,
                   sib.pdsch_cfg.codewords[0].tb_size_bytes,
                   sib.pdsch_cfg.codewords[0].mcs_index,
                   sib.pdsch_cfg.codewords[0].rv_index);
  }
  for (const rar_information& rar : result.dl.rar_grants) {
    fmt::format_to(fmtbuf,
                   "\n- RAR PDSCH: ra-rnti={:#x} prbs={} symbols={} tbs={}bytes mcs={} rv={} grants ({}): ",
                   rar.pdsch_cfg.rnti,
                   rar.pdsch_cfg.prbs.prbs(),
                   rar.pdsch_cfg.symbols,
                   rar.pdsch_cfg.codewords[0].tb_size_bytes,
                   rar.pdsch_cfg.codewords[0].mcs_index,
                   rar.pdsch_cfg.codewords[0].rv_index,
                   rar.grants.size());
    for (const rar_ul_grant& grant : rar.grants) {
      fmt::format_to(fmtbuf,
                     "{}tc-rnti={:#x}: rapid={} ta={}",
                     (&grant == &rar.grants.front()) ? "" : ", ",
                     grant.temp_crnti,
                     grant.rapid,
                     grant.ta);
    }
  }
  for (const dl_msg_alloc& ue_dl_grant : result.dl.ue_grants) {
    fmt::format_to(fmtbuf,
                   "\n- UE PDSCH: c-rnti={:#x} prbs={} symbols={} tbs={}B mcs={} rv={} grants: ",
                   ue_dl_grant.pdsch_cfg.rnti,
                   ue_dl_grant.pdsch_cfg.prbs.prbs(),
                   ue_dl_grant.pdsch_cfg.symbols,
                   ue_dl_grant.pdsch_cfg.codewords[0].tb_size_bytes,
                   ue_dl_grant.pdsch_cfg.codewords[0].mcs_index,
                   ue_dl_grant.pdsch_cfg.codewords[0].rv_index);
    for (const dl_msg_lc_info& lc : ue_dl_grant.tb_list[0].lc_chs_to_sched) {
      fmt::format_to(fmtbuf, " lcid={}: size={}bytes", lc.lcid, lc.sched_bytes);
    }
  }
  for (const dl_paging_allocation& pg : result.dl.paging_grants) {
    fmt::format_to(
        fmtbuf,
        "\n- PCCH: pg{}, pg-id={:#x}, prbs={}, symbols={}, tbs={}B, mcs={}, rv={}",
        pg.paging_type_indicator == dl_paging_allocation::paging_identity_type::cn_ue_paging_identity ? "cn" : "ran",
        pg.paging_identity,
        pg.pdsch_cfg.prbs.prbs(),
        pg.pdsch_cfg.symbols,
        pg.pdsch_cfg.codewords[0].tb_size_bytes,
        pg.pdsch_cfg.codewords[0].mcs_index,
        pg.pdsch_cfg.codewords[0].rv_index);
  }

  for (const ul_sched_info& ul_info : result.ul.puschs) {
    fmt::format_to(fmtbuf,
                   "\n- PUSCH: c-rnti={:#x} h_id={} prbs={} symbols={} tbs={}B rv={}",
                   ul_info.pusch_cfg.rnti,
                   ul_info.pusch_cfg.harq_id,
                   ul_info.pusch_cfg.prbs.prbs(),
                   ul_info.pusch_cfg.symbols,
                   ul_info.pusch_cfg.tb_size_bytes,
                   ul_info.pusch_cfg.rv_index);
    if (ul_info.uci.has_value()) {
      fmt::format_to(fmtbuf,
                     " uci: harq_bits={} csi-1_bits={} csi-2_bits={}",
                     ul_info.uci.value().harq_ack_nof_bits,
                     ul_info.uci.value().csi_part1_nof_bits,
                     ul_info.uci.value().csi_part2_nof_bits);
    }
  }

  for (const pucch_info& pucch : result.ul.pucchs) {
    unsigned nof_harq_bits{0};
    unsigned nof_sr_bits{0};
    unsigned nof_csi_part1_bits{0};
    if (pucch.format == pucch_format::FORMAT_1) {
      nof_harq_bits = pucch.format_1.harq_ack_nof_bits;
      nof_sr_bits   = sr_nof_bits_to_uint(pucch.format_1.sr_bits);
    } else if (pucch.format == pucch_format::FORMAT_2) {
      nof_harq_bits      = pucch.format_2.harq_ack_nof_bits;
      nof_sr_bits        = sr_nof_bits_to_uint(pucch.format_2.sr_bits);
      nof_csi_part1_bits = pucch.format_2.csi_part1_bits;
    }
    if (pucch.resources.second_hop_prbs.empty()) {
      fmt::format_to(fmtbuf,
                     "\n- PUCCH: c-rnti={:#x}, format={}, prbs={}, symbols={}, uci: harq_bits={} sr={} csi-1_bits={}",
                     pucch.crnti,
                     pucch.format,
                     pucch.resources.prbs,
                     pucch.resources.symbols,
                     nof_harq_bits,
                     nof_sr_bits,
                     nof_csi_part1_bits);
    } else {
      fmt::format_to(fmtbuf,
                     "\n- PUCCH: c-rnti={:#x}, format={}, prbs={}, symbols={}, second_prbs={}, uci: harq_bits={} sr={} "
                     "csi-1_bits={}",
                     pucch.crnti,
                     pucch.format,
                     pucch.resources.prbs,
                     pucch.resources.symbols,
                     pucch.resources.second_hop_prbs,
                     nof_harq_bits,
                     nof_sr_bits,
                     nof_csi_part1_bits);
    }
  }

  if (fmtbuf.size() > 0) {
    logger.debug("SCHED output:{}", to_c_str(fmtbuf));
    fmtbuf.clear();
  }
}

void scheduler_result_logger::log_info(const sched_result& result)
{
  for (const sib_information& sib_info : result.dl.bc.sibs) {
    fmt::format_to(fmtbuf,
                   "{}SI{}: prbs={} tbs={}B",
                   fmtbuf.size() == 0 ? "" : ", ",
                   sib_info.si_indicator == sib_information::sib1 ? "B1" : "",
                   sib_info.pdsch_cfg.prbs.prbs(),
                   sib_info.pdsch_cfg.codewords[0].tb_size_bytes);
  }
  for (const rar_information& rar_info : result.dl.rar_grants) {
    fmt::format_to(fmtbuf,
                   "{}RAR: ra-rnti={:#x} prbs={} tbs={}B",
                   fmtbuf.size() == 0 ? "" : ", ",
                   rar_info.pdsch_cfg.rnti,
                   rar_info.pdsch_cfg.prbs.prbs(),
                   rar_info.pdsch_cfg.codewords[0].tb_size_bytes);
  }
  for (const dl_msg_alloc& ue_msg : result.dl.ue_grants) {
    fmt::format_to(fmtbuf,
                   "{}DL: c-rnti={:#x} prbs={} tbs={}B",
                   fmtbuf.size() == 0 ? "" : ", ",
                   ue_msg.pdsch_cfg.rnti,
                   ue_msg.pdsch_cfg.prbs.prbs(),
                   ue_msg.pdsch_cfg.codewords[0].tb_size_bytes);
  }
  for (const ul_sched_info& ue_msg : result.ul.puschs) {
    fmt::format_to(fmtbuf,
                   "{}UL: rnti={:#x} h_id={} prbs={} tbs={}B",
                   fmtbuf.size() == 0 ? "" : ", ",
                   ue_msg.pusch_cfg.rnti,
                   ue_msg.pusch_cfg.harq_id,
                   ue_msg.pusch_cfg.prbs.prbs(),
                   ue_msg.pusch_cfg.tb_size_bytes);
  }
  for (const dl_paging_allocation& pg_info : result.dl.paging_grants) {
    fmt::format_to(fmtbuf,
                   "{}PG{}: pg-id={:#x} prbs={} tbs={}B",
                   fmtbuf.size() == 0 ? "" : ", ",
                   pg_info.paging_type_indicator == dl_paging_allocation::paging_identity_type::cn_ue_paging_identity
                       ? "cn"
                       : "ran",
                   pg_info.paging_identity,
                   pg_info.pdsch_cfg.prbs.prbs(),
                   pg_info.pdsch_cfg.codewords[0].tb_size_bytes);
  }

  if (fmtbuf.size() > 0) {
    logger.info("SCHED output: {}", to_c_str(fmtbuf));
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
