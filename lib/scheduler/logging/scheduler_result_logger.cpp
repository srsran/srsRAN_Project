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

#include "scheduler_result_logger.h"
#include "srsran/ran/csi_report/csi_report_formatters.h"

using namespace srsran;

void scheduler_result_logger::log_debug(const sched_result& result)
{
  auto slot_stop_tp = std::chrono::high_resolution_clock::now();

  if (log_broadcast) {
    for (const ssb_information& ssb_info : result.dl.bc.ssb_info) {
      fmt::format_to(
          fmtbuf, "\n- SSB: ssbIdx={}, crbs={}, symb={}", ssb_info.ssb_index, ssb_info.crbs, ssb_info.symbols);
    }
  }
  for (const pdcch_dl_information& pdcch : result.dl.dl_pdcchs) {
    if (not log_broadcast and pdcch.ctx.rnti == SI_RNTI) {
      continue;
    }
    fmt::format_to(fmtbuf,
                   "\n- DL PDCCH: rnti={:#x} type={} cs_id={} ss_id={} format={} cce={} al={} ",
                   pdcch.ctx.rnti,
                   dci_dl_rnti_config_rnti_type(pdcch.dci.type),
                   pdcch.ctx.coreset_cfg->id,
                   pdcch.ctx.context.ss_id,
                   dci_dl_rnti_config_format(pdcch.dci.type),
                   pdcch.ctx.cces.ncce,
                   to_nof_cces(pdcch.ctx.cces.aggr_lvl));
    switch (pdcch.dci.type) {
      case dci_dl_rnti_config_type::c_rnti_f1_0: {
        const auto& dci = pdcch.dci.c_rnti_f1_0;
        fmt::format_to(fmtbuf,
                       " dci: h_id={} ndi={} rv={} mcs={}",
                       dci.harq_process_number,
                       dci.new_data_indicator ? 1 : 0,
                       dci.redundancy_version,
                       dci.modulation_coding_scheme);
      } break;
      case dci_dl_rnti_config_type::tc_rnti_f1_0: {
        const auto& dci = pdcch.dci.tc_rnti_f1_0;
        fmt::format_to(fmtbuf,
                       " dci: h_id={} ndi={} rv={} mcs={}",
                       dci.harq_process_number,
                       dci.new_data_indicator ? 1 : 0,
                       dci.redundancy_version,
                       dci.modulation_coding_scheme);
      } break;
      case dci_dl_rnti_config_type::c_rnti_f1_1: {
        const auto& dci = pdcch.dci.c_rnti_f1_1;
        fmt::format_to(fmtbuf,
                       " dci: h_id={} ndi={} rv={} mcs={}",
                       dci.harq_process_number,
                       dci.tb1_new_data_indicator ? 1 : 0,
                       dci.tb1_redundancy_version,
                       dci.tb1_modulation_coding_scheme);
        if (dci.downlink_assignment_index.has_value()) {
          fmt::format_to(fmtbuf, " dai={}", *dci.downlink_assignment_index);
        }
      } break;
      default:
        break;
    }
  }
  for (const pdcch_ul_information& pdcch : result.dl.ul_pdcchs) {
    fmt::format_to(fmtbuf,
                   "\n- UL PDCCH: rnti={:#x} type={} cs_id={} ss_id={} format={} cce={} al={} ",
                   pdcch.ctx.rnti,
                   dci_ul_rnti_config_rnti_type(pdcch.dci.type),
                   pdcch.ctx.coreset_cfg->id,
                   pdcch.ctx.context.ss_id,
                   dci_ul_rnti_config_format(pdcch.dci.type),
                   pdcch.ctx.cces.ncce,
                   to_nof_cces(pdcch.ctx.cces.aggr_lvl));
    switch (pdcch.dci.type) {
      case dci_ul_rnti_config_type::c_rnti_f0_0: {
        const auto& dci = pdcch.dci.c_rnti_f0_0;
        fmt::format_to(fmtbuf,
                       " h_id={} ndi={} rv={} mcs={}",
                       dci.harq_process_number,
                       dci.new_data_indicator ? 1 : 0,
                       dci.redundancy_version,
                       dci.modulation_coding_scheme);
      } break;
      case dci_ul_rnti_config_type::tc_rnti_f0_0: {
        const auto& dci = pdcch.dci.tc_rnti_f0_0;
        fmt::format_to(fmtbuf, "h_id=0 ndi=1 rv={} mcs={}", dci.redundancy_version, dci.modulation_coding_scheme);
      } break;
      case dci_ul_rnti_config_type::c_rnti_f0_1: {
        const auto& dci = pdcch.dci.c_rnti_f0_1;
        fmt::format_to(fmtbuf,
                       " h_id={} ndi={} rv={} mcs={} dai={}",
                       dci.harq_process_number,
                       dci.new_data_indicator ? 1 : 0,
                       dci.redundancy_version,
                       dci.modulation_coding_scheme,
                       dci.first_dl_assignment_index);
      } break;
      default:
        break;
    }
  }

  if (log_broadcast) {
    for (const csi_rs_info& csi_rs : result.dl.csi_rs) {
      fmt::format_to(fmtbuf,
                     "\n- CSI-RS: type={} crbs={} row={} freq={} symb0={} cdm_type={} freq_density={}",
                     csi_rs.type == csi_rs_type::CSI_RS_NZP ? "nzp" : "zp",
                     csi_rs.crbs,
                     csi_rs.row,
                     csi_rs.freq_domain,
                     csi_rs.symbol0,
                     to_string(csi_rs.cdm_type),
                     (unsigned)csi_rs.freq_density);
      if (csi_rs.type == csi_rs_type::CSI_RS_NZP) {
        fmt::format_to(fmtbuf, " scramb_id={}", csi_rs.scrambling_id);
      }
    }

    for (const sib_information& sib : result.dl.bc.sibs) {
      fmt::format_to(fmtbuf,
                     "\n- SI{} PDSCH: rb={} symb={} tbs={} mcs={} rv={}",
                     sib.si_indicator == sib_information::sib1 ? "B1" : "",
                     sib.pdsch_cfg.rbs,
                     sib.pdsch_cfg.symbols,
                     sib.pdsch_cfg.codewords[0].tb_size_bytes,
                     sib.pdsch_cfg.codewords[0].mcs_index,
                     sib.pdsch_cfg.codewords[0].rv_index);
    }
  }

  for (const rar_information& rar : result.dl.rar_grants) {
    fmt::format_to(fmtbuf,
                   "\n- RAR PDSCH: ra-rnti={:#x} rb={} symb={} tbs={} mcs={} rv={} grants ({}): ",
                   rar.pdsch_cfg.rnti,
                   rar.pdsch_cfg.rbs,
                   rar.pdsch_cfg.symbols,
                   rar.pdsch_cfg.codewords[0].tb_size_bytes,
                   rar.pdsch_cfg.codewords[0].mcs_index,
                   rar.pdsch_cfg.codewords[0].rv_index,
                   rar.grants.size());
    for (const rar_ul_grant& grant : rar.grants) {
      fmt::format_to(fmtbuf,
                     "{}tc-rnti={:#x}: rapid={} ta={} time_res={}",
                     (&grant == &rar.grants.front()) ? "" : ", ",
                     grant.temp_crnti,
                     grant.rapid,
                     grant.ta,
                     grant.time_resource_assignment);
    }
  }
  for (const dl_msg_alloc& ue_dl_grant : result.dl.ue_grants) {
    fmt::format_to(fmtbuf,
                   "\n- UE PDSCH: ue={} c-rnti={:#x} h_id={} rb={} symb={} tbs={} mcs={} rv={} nrtx={} k1={}",
                   ue_dl_grant.context.ue_index,
                   ue_dl_grant.pdsch_cfg.rnti,
                   ue_dl_grant.pdsch_cfg.harq_id,
                   ue_dl_grant.pdsch_cfg.rbs,
                   ue_dl_grant.pdsch_cfg.symbols,
                   ue_dl_grant.pdsch_cfg.codewords[0].tb_size_bytes,
                   ue_dl_grant.pdsch_cfg.codewords[0].mcs_index,
                   ue_dl_grant.pdsch_cfg.codewords[0].rv_index,
                   ue_dl_grant.context.nof_retxs,
                   ue_dl_grant.context.k1);
    if (ue_dl_grant.pdsch_cfg.precoding.has_value()) {
      const auto& prg_type = ue_dl_grant.pdsch_cfg.precoding->prg_infos[0].type;
      fmt::format_to(fmtbuf, " ri={} {}", ue_dl_grant.pdsch_cfg.nof_layers, csi_report_pmi{prg_type});
    }
    for (const dl_msg_lc_info& lc : ue_dl_grant.tb_list[0].lc_chs_to_sched) {
      fmt::format_to(fmtbuf,
                     "{}lcid={}: size={}",
                     (&lc == &ue_dl_grant.tb_list[0].lc_chs_to_sched.front()) ? " grants: " : ", ",
                     lc.lcid,
                     lc.sched_bytes);
    }
  }
  for (const dl_paging_allocation& pg : result.dl.paging_grants) {
    fmt::format_to(fmtbuf,
                   "\n- PCCH: rb={} symb={} tbs={} mcs={} rv={}",
                   pg.pdsch_cfg.rbs,
                   pg.pdsch_cfg.symbols,
                   pg.pdsch_cfg.codewords[0].tb_size_bytes,
                   pg.pdsch_cfg.codewords[0].mcs_index,
                   pg.pdsch_cfg.codewords[0].rv_index);

    for (const paging_ue_info& ue : pg.paging_ue_list) {
      fmt::format_to(fmtbuf,
                     "{}{}-pg-id={:#x}",
                     (&ue == &pg.paging_ue_list.front()) ? " ues: " : ", ",
                     ue.paging_type_indicator == paging_ue_info::paging_identity_type::cn_ue_paging_identity ? "cn"
                                                                                                             : "ran",
                     ue.paging_identity);
    }
  }

  for (const ul_sched_info& ul_info : result.ul.puschs) {
    fmt::format_to(fmtbuf, "\n- UE PUSCH: ");
    if (ul_info.context.ue_index != INVALID_DU_UE_INDEX) {
      fmt::format_to(fmtbuf, "ue={} c-rnti={:#x} ", ul_info.context.ue_index, ul_info.pusch_cfg.rnti);
    } else {
      fmt::format_to(fmtbuf, "ue={} tc-rnti={:#x} ", ul_info.context.ue_index, ul_info.pusch_cfg.rnti);
    }
    fmt::format_to(fmtbuf,
                   "h_id={} rb={} symb={} tbs={} rv={} nrtx={} ",
                   ul_info.pusch_cfg.harq_id,
                   ul_info.pusch_cfg.rbs,
                   ul_info.pusch_cfg.symbols,
                   ul_info.pusch_cfg.tb_size_bytes,
                   ul_info.pusch_cfg.rv_index,
                   ul_info.context.nof_retxs);
    if (ul_info.context.ue_index == INVALID_DU_UE_INDEX and ul_info.context.nof_retxs == 0 and
        ul_info.context.msg3_delay.has_value()) {
      fmt::format_to(fmtbuf, "msg3_delay={}", ul_info.context.msg3_delay.value());
    } else {
      fmt::format_to(fmtbuf, "k2={}", ul_info.context.k2);
    }

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
                     "\n- PUCCH: c-rnti={:#x}, format={}, prb={}, symb={}, uci: harq_bits={} sr={} csi-1_bits={}",
                     pucch.crnti,
                     pucch.format,
                     pucch.resources.prbs,
                     pucch.resources.symbols,
                     nof_harq_bits,
                     nof_sr_bits,
                     nof_csi_part1_bits);
    } else {
      fmt::format_to(fmtbuf,
                     "\n- PUCCH: c-rnti={:#x}, format={}, prb={}, symb={}, second_prbs={}, uci: harq_bits={} sr={} "
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
    auto decision_latency = std::chrono::duration_cast<std::chrono::microseconds>(slot_stop_tp - slot_start_tp);
    logger.debug("Slot decisions cell=0 t={}us:{}", decision_latency.count(), to_c_str(fmtbuf));
    fmtbuf.clear();
  }
}

void scheduler_result_logger::log_info(const sched_result& result)
{
  auto slot_stop_tp = std::chrono::high_resolution_clock::now();

  if (log_broadcast) {
    for (const sib_information& sib_info : result.dl.bc.sibs) {
      fmt::format_to(fmtbuf,
                     "{}SI{}: rb={} tbs={}",
                     fmtbuf.size() == 0 ? "" : ", ",
                     sib_info.si_indicator == sib_information::sib1 ? "B1" : "",
                     sib_info.pdsch_cfg.rbs,
                     sib_info.pdsch_cfg.codewords[0].tb_size_bytes);
    }
  }
  for (const rar_information& rar_info : result.dl.rar_grants) {
    fmt::format_to(fmtbuf,
                   "{}RAR: ra-rnti={:#x} rb={} tbs={}",
                   fmtbuf.size() == 0 ? "" : ", ",
                   rar_info.pdsch_cfg.rnti,
                   rar_info.pdsch_cfg.rbs,
                   rar_info.pdsch_cfg.codewords[0].tb_size_bytes);
  }
  for (const dl_msg_alloc& ue_msg : result.dl.ue_grants) {
    fmt::format_to(fmtbuf,
                   "{}DL: ue={} c-rnti={:#x} rb={} h_id={} ss_id={} k1={} rv={} tbs={}",
                   fmtbuf.size() == 0 ? "" : ", ",
                   ue_msg.context.ue_index,
                   ue_msg.pdsch_cfg.rnti,
                   ue_msg.pdsch_cfg.rbs,
                   ue_msg.pdsch_cfg.harq_id,
                   ue_msg.context.ss_id,
                   ue_msg.context.k1,
                   ue_msg.pdsch_cfg.codewords[0].rv_index,
                   ue_msg.pdsch_cfg.codewords[0].tb_size_bytes);
  }
  for (const ul_sched_info& ue_msg : result.ul.puschs) {
    fmt::format_to(fmtbuf,
                   "{}UL: ue={} rnti={:#x} h_id={} ss_id={} rb={} rv={} tbs={} ",
                   fmtbuf.size() == 0 ? "" : ", ",
                   ue_msg.context.ue_index,
                   ue_msg.pusch_cfg.rnti,
                   ue_msg.pusch_cfg.harq_id,
                   ue_msg.context.ss_id,
                   ue_msg.pusch_cfg.rbs,
                   ue_msg.pusch_cfg.rv_index,
                   ue_msg.pusch_cfg.tb_size_bytes);
    if (ue_msg.context.ue_index == INVALID_DU_UE_INDEX and ue_msg.context.nof_retxs == 0 and
        ue_msg.context.msg3_delay.has_value()) {
      fmt::format_to(fmtbuf, "msg3_delay={}", ue_msg.context.msg3_delay.value());
    } else {
      fmt::format_to(fmtbuf, "k2={}", ue_msg.context.k2);
    }
  }
  for (const dl_paging_allocation& pg_info : result.dl.paging_grants) {
    fmt::format_to(fmtbuf,
                   "{}PG: rb={} tbs={}",
                   fmtbuf.size() == 0 ? "" : ", ",
                   pg_info.pdsch_cfg.rbs,
                   pg_info.pdsch_cfg.codewords[0].tb_size_bytes);
    for (const paging_ue_info& ue : pg_info.paging_ue_list) {
      fmt::format_to(fmtbuf,
                     "{}{}-pg-id={:#x}",
                     (&ue == &pg_info.paging_ue_list.front()) ? " ues: " : ", ",
                     ue.paging_type_indicator == paging_ue_info::paging_identity_type::cn_ue_paging_identity ? "cn"
                                                                                                             : "ran",
                     ue.paging_identity);
    }
  }

  if (fmtbuf.size() > 0) {
    auto decision_latency = std::chrono::duration_cast<std::chrono::microseconds>(slot_stop_tp - slot_start_tp);
    logger.info("Slot decisions cell=0 t={}us: {}", decision_latency.count(), to_c_str(fmtbuf));
    fmtbuf.clear();
  }
}

void scheduler_result_logger::on_scheduler_result(const sched_result& result)
{
  if (not enabled) {
    return;
  }
  if (logger.debug.enabled()) {
    log_debug(result);
  } else {
    log_info(result);
  }
}
