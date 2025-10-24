/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/scheduler/result/sched_result.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/format/custom_formattable.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;

static auto make_dl_dci_log_entry(const dci_dl_info& dci)
{
  bool                   is_formattable = true;
  uint8_t                h_id           = 0;
  bool                   ndi            = false;
  uint8_t                rv             = 0;
  uint8_t                mcs            = 0;
  uint8_t                pucch_res_id   = 0;
  std::optional<int8_t>  tpc_cmd;
  std::optional<uint8_t> dai;
  std::optional<bool>    vrb_prb;

  switch (dci.type) {
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
      const auto& dci1_0 = dci.c_rnti_f1_0;
      h_id               = dci1_0.harq_process_number;
      ndi                = dci1_0.new_data_indicator;
      rv                 = dci1_0.redundancy_version;
      mcs                = dci1_0.modulation_coding_scheme;
      pucch_res_id       = dci1_0.pucch_resource_indicator;
      tpc_cmd            = dci1_0.tpc_command;
    } break;
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      const auto& dci1_0 = dci.tc_rnti_f1_0;
      h_id               = dci1_0.harq_process_number;
      ndi                = dci1_0.new_data_indicator;
      rv                 = dci1_0.redundancy_version;
      mcs                = dci1_0.modulation_coding_scheme;
      pucch_res_id       = dci1_0.pucch_resource_indicator;
    } break;
    case dci_dl_rnti_config_type::c_rnti_f1_1: {
      const auto& dci1_1 = dci.c_rnti_f1_1;
      h_id               = dci1_1.harq_process_number;
      ndi                = dci1_1.tb1_new_data_indicator;
      rv                 = dci1_1.tb1_redundancy_version;
      mcs                = dci1_1.tb1_modulation_coding_scheme;
      pucch_res_id       = dci1_1.pucch_resource_indicator;
      tpc_cmd            = dci1_1.tpc_command;
      vrb_prb            = dci1_1.vrb_prb_mapping;
      if (dci.c_rnti_f1_1.downlink_assignment_index.has_value()) {
        dai = dci.c_rnti_f1_1.downlink_assignment_index;
      }
    } break;
    default:
      is_formattable = false;
      break;
  }
  return make_formattable([is_formattable, h_id, ndi, rv, mcs, pucch_res_id, dai, tpc_cmd, vrb_prb](auto& ctx) {
    if (is_formattable) {
      fmt::format_to(ctx.out(), "dci: h_id={} ndi={} rv={} mcs={} res_ind={}", h_id, ndi, rv, mcs, pucch_res_id);
      if (tpc_cmd.has_value()) {
        fmt::format_to(ctx.out(), " tpc={}", *tpc_cmd);
      }
      if (dai.has_value()) {
        fmt::format_to(ctx.out(), " dai={}", dai.value());
      }
      if (vrb_prb.has_value()) {
        fmt::format_to(ctx.out(), " vrb_prb_map_used={}", vrb_prb.value() ? "yes" : "no");
      }
    }
    return ctx.out();
  });
}

static auto make_ul_dci_log_entry(const dci_ul_info& dci)
{
  uint8_t                h_id    = 0;
  bool                   ndi     = false;
  uint8_t                rv      = 0;
  uint8_t                mcs     = 0;
  int8_t                 tpc_cmd = 0;
  std::optional<uint8_t> dai;
  int8_t                 nof_layers = 1;
  int8_t                 ant        = 1;

  switch (dci.type) {
    case dci_ul_rnti_config_type::c_rnti_f0_0: {
      const auto& dci0_0 = dci.c_rnti_f0_0;
      h_id               = dci0_0.harq_process_number;
      ndi                = dci0_0.new_data_indicator;
      rv                 = dci0_0.redundancy_version;
      mcs                = dci0_0.modulation_coding_scheme;
      tpc_cmd            = dci0_0.tpc_command;
    } break;
    case dci_ul_rnti_config_type::tc_rnti_f0_0: {
      const auto& dci0_0 = dci.tc_rnti_f0_0;
      rv                 = dci0_0.redundancy_version;
      mcs                = dci0_0.modulation_coding_scheme;
      h_id               = 0;
      ndi                = true;
      tpc_cmd            = dci0_0.tpc_command;
    } break;
    case dci_ul_rnti_config_type::c_rnti_f0_1: {
      const auto& dci0_1 = dci.c_rnti_f0_1;
      h_id               = dci0_1.harq_process_number;
      ndi                = dci0_1.new_data_indicator;
      rv                 = dci0_1.redundancy_version;
      mcs                = dci0_1.modulation_coding_scheme;
      tpc_cmd            = dci0_1.tpc_command;
      dai                = dci0_1.first_dl_assignment_index;
      nof_layers         = dci0_1.precoding_info_nof_layers;
      ant                = dci0_1.antenna_ports;
    } break;
    default:
      report_fatal_error("Invalid UL DCI format");
  }

  return make_formattable([h_id, ndi, rv, mcs, tpc_cmd, dai, nof_layers, ant](auto& ctx) {
    fmt::format_to(ctx.out(), "dci: h_id={} ndi={} rv={} mcs={} tpc={}", h_id, ndi ? 1 : 0, rv, mcs, tpc_cmd);
    if (dai.has_value()) {
      fmt::format_to(ctx.out(), " dai={} mimo={} ant={}", *dai, nof_layers, ant);
    }
    return ctx.out();
  });
}

static auto make_dl_pdcch_log_entry(const pdcch_dl_information& pdcch)
{
  return make_formattable([rnti     = pdcch.ctx.rnti,
                           dci_type = pdcch.dci.type,
                           cs_id    = pdcch.ctx.coreset_cfg->id,
                           ss_id    = pdcch.ctx.context.ss_id,
                           cces     = pdcch.ctx.cces,
                           dci_log  = make_dl_dci_log_entry(pdcch.dci)](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- DL PDCCH: rnti={} type={} cs_id={} ss_id={} format={} cce={} al={} {}",
                   rnti,
                   dci_dl_rnti_config_rnti_type(dci_type),
                   fmt::underlying(cs_id),
                   fmt::underlying(ss_id),
                   dci_dl_rnti_config_format(dci_type),
                   cces.ncce,
                   to_nof_cces(cces.aggr_lvl),
                   dci_log);
    return ctx.out();
  });
}

static auto make_dl_pdcch_log_list(const pdcch_dl_info_list& pdcchs, bool log_broadcast)
{
  using pdcch_entry_type = decltype(make_dl_pdcch_log_entry(std::declval<pdcch_dl_information>()));
  static_vector<pdcch_entry_type, MAX_DL_PDCCH_PDUS_PER_SLOT> list;
  for (const pdcch_dl_information& pdcch : pdcchs) {
    if (not log_broadcast and pdcch.ctx.rnti == rnti_t::SI_RNTI) {
      continue;
    }
    list.push_back(make_dl_pdcch_log_entry(pdcch));
  }
  return list;
}

static auto make_ul_pdcch_log_entry(const pdcch_ul_information& pdcch)
{
  return make_formattable([rnti     = pdcch.ctx.rnti,
                           dci_type = pdcch.dci.type,
                           cid      = pdcch.ctx.coreset_cfg->id,
                           ssid     = pdcch.ctx.context.ss_id,
                           cces     = pdcch.ctx.cces,
                           dci_log  = make_ul_dci_log_entry(pdcch.dci)](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- UL PDCCH: rnti={} type={} cs_id={} ss_id={} format={} cce={} al={} {}",
                   rnti,
                   dci_ul_rnti_config_rnti_type(dci_type),
                   fmt::underlying(cid),
                   fmt::underlying(ssid),
                   dci_ul_rnti_config_format(dci_type),
                   cces.ncce,
                   to_nof_cces(cces.aggr_lvl),
                   dci_log);
    return ctx.out();
  });
}

static auto make_sib_info_log_entry(const sib_information& sib_info)
{
  return make_formattable([si_ind = sib_info.si_indicator,
                           rbs    = sib_info.pdsch_cfg.rbs,
                           tbs    = sib_info.pdsch_cfg.codewords[0].tb_size_bytes](auto& ctx) {
    return fmt::format_to(ctx.out(), "SI{}: rb={} tbs={}", si_ind == sib_information::sib1 ? "B1" : "", rbs, tbs);
  });
}

static auto make_sib_info_log_list(const static_vector<sib_information, MAX_SI_PDUS_PER_SLOT>& sibs, bool log_broadcast)
{
  using sib_entry_type = decltype(make_sib_info_log_entry(std::declval<sib_information>()));

  static_vector<sib_entry_type, MAX_SI_PDUS_PER_SLOT> list;
  if (log_broadcast) {
    for (const sib_information& sib : sibs) {
      list.push_back(make_sib_info_log_entry(sib));
    }
  }

  return list;
}

static auto make_rar_info_log_entry(const rar_information& rar_info)
{
  return make_formattable([rnti = rar_info.pdsch_cfg.rnti,
                           rb   = rar_info.pdsch_cfg.rbs,
                           tbs  = rar_info.pdsch_cfg.codewords[0].tb_size_bytes](auto& ctx) {
    return fmt::format_to(ctx.out(), "RAR: ra-rnti={} rb={} tbs={}", rnti, rb, tbs);
  });
}

static auto make_rar_info_log_list(const static_vector<rar_information, MAX_RAR_PDUS_PER_SLOT>& rars)
{
  using rar_entry_type = decltype(make_rar_info_log_entry(std::declval<rar_information>()));

  static_vector<rar_entry_type, MAX_RAR_PDUS_PER_SLOT> list;
  for (const rar_information& rar : rars) {
    list.push_back(make_rar_info_log_entry(rar));
  }

  return list;
}

static auto make_ue_dl_msg_info_log_entry(const dl_msg_alloc& ue_msg)
{
  return make_formattable([ue_idx   = ue_msg.context.ue_index,
                           rnti     = ue_msg.pdsch_cfg.rnti,
                           rb       = ue_msg.pdsch_cfg.rbs,
                           h_id     = ue_msg.pdsch_cfg.harq_id,
                           ss_id    = ue_msg.context.ss_id,
                           k1       = ue_msg.context.k1,
                           new_data = ue_msg.pdsch_cfg.codewords[0].new_data,
                           rv       = ue_msg.pdsch_cfg.codewords[0].rv_index,
                           tbs      = ue_msg.pdsch_cfg.codewords[0].tb_size_bytes,
                           ri       = ue_msg.pdsch_cfg.nof_layers,
                           dl_bo    = ue_msg.context.buffer_occupancy](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "DL: ue={} c-rnti={} h_id={} ss_id={} rb={} k1={} newtx={} rv={} tbs={}",
                   fmt::underlying(ue_idx),
                   rnti,
                   fmt::underlying(h_id),
                   fmt::underlying(ss_id),
                   rb,
                   k1,
                   new_data,
                   rv,
                   tbs);
    if (new_data) {
      fmt::format_to(ctx.out(), " ri={} dl_bo={}", ri, dl_bo);
    }
    return ctx.out();
  });
}

static auto make_ue_dl_msg_info_log_list(const static_vector<dl_msg_alloc, MAX_UE_PDUS_PER_SLOT>& ue_msgs)
{
  using entry_type = decltype(make_ue_dl_msg_info_log_entry(std::declval<dl_msg_alloc>()));

  static_vector<entry_type, MAX_UE_PDUS_PER_SLOT> list;
  for (const dl_msg_alloc& ue_msg : ue_msgs) {
    list.push_back(make_ue_dl_msg_info_log_entry(ue_msg));
  }

  return list;
}

static auto make_ue_ul_msg_info_log_entry(const ul_sched_info& ue_msg)
{
  return make_formattable([ue_idx     = ue_msg.context.ue_index,
                           rnti       = ue_msg.pusch_cfg.rnti,
                           rb         = ue_msg.pusch_cfg.rbs,
                           h_id       = ue_msg.pusch_cfg.harq_id,
                           ss_id      = ue_msg.context.ss_id,
                           k2         = ue_msg.context.k2,
                           new_data   = ue_msg.pusch_cfg.new_data,
                           rv         = ue_msg.pusch_cfg.rv_index,
                           tbs        = ue_msg.pusch_cfg.tb_size_bytes,
                           nof_retx   = ue_msg.context.nof_retxs,
                           msg3_delay = ue_msg.context.msg3_delay](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "UL: ue={} rnti={} h_id={} ss_id={} rb={} newtx={} rv={} tbs={}",
                   fmt::underlying(ue_idx),
                   rnti,
                   fmt::underlying(h_id),
                   fmt::underlying(ss_id),
                   rb,
                   new_data,
                   rv,
                   tbs);
    if (ue_idx == INVALID_DU_UE_INDEX and nof_retx == 0 and msg3_delay.has_value()) {
      fmt::format_to(ctx.out(), " msg3_delay={}", msg3_delay.value());
    } else {
      fmt::format_to(ctx.out(), " k2={}", k2);
    }
    return ctx.out();
  });
}

static auto make_ue_ul_msg_info_log_list(const static_vector<ul_sched_info, MAX_UE_PDUS_PER_SLOT>& ue_msgs)
{
  using entry_type = decltype(make_ue_ul_msg_info_log_entry(std::declval<ul_sched_info>()));

  static_vector<entry_type, MAX_UE_PDUS_PER_SLOT> list;
  for (const ul_sched_info& ue_msg : ue_msgs) {
    list.push_back(make_ue_ul_msg_info_log_entry(ue_msg));
  }

  return list;
}

static auto make_paging_info_log_entry(const dl_paging_allocation& pg_info)
{
  return make_formattable([rb             = pg_info.pdsch_cfg.rbs,
                           tbs            = pg_info.pdsch_cfg.codewords[0].tb_size_bytes,
                           paging_ue_list = pg_info.paging_ue_list](auto& ctx) {
    fmt::format_to(ctx.out(), "PG: rb={} tbs={}", rb, tbs);
    for (const paging_ue_info& ue : paging_ue_list) {
      fmt::format_to(ctx.out(),
                     "{}{}-pg-id={:#x}",
                     (&ue == &paging_ue_list.front()) ? " ues: " : ", ",
                     ue.paging_type_indicator == paging_ue_info::paging_identity_type::cn_ue_paging_identity ? "cn"
                                                                                                             : "ran",
                     ue.paging_identity);
    }
    return ctx.out();
  });
}

static auto make_paging_info_log_list(const static_vector<dl_paging_allocation, MAX_PAGING_PDUS_PER_SLOT>& pg_list)
{
  using entry_type = decltype(make_paging_info_log_entry(std::declval<dl_paging_allocation>()));

  static_vector<entry_type, MAX_UE_PDUS_PER_SLOT> list;
  for (const dl_paging_allocation& pg_alloc : pg_list) {
    list.push_back(make_paging_info_log_entry(pg_alloc));
  }

  return list;
}

static auto make_info_log_entry(const sched_result& result, bool log_broadcast)
{
  return make_formattable([sibs    = make_sib_info_log_list(result.dl.bc.sibs, log_broadcast),
                           rars    = make_rar_info_log_list(result.dl.rar_grants),
                           ue_msgs = make_ue_dl_msg_info_log_list(result.dl.ue_grants),
                           puschs  = make_ue_ul_msg_info_log_list(result.ul.puschs),
                           pgs     = make_paging_info_log_list(result.dl.paging_grants)](auto& ctx) {
    fmt::format_to(ctx.out(), "{}", fmt::join(sibs.begin(), sibs.end(), ", "));
    const char* sep = sibs.empty() ? "" : ", ";
    fmt::format_to(ctx.out(), "{}{}", rars.empty() ? "" : sep, fmt::join(rars.begin(), rars.end(), ", "));
    sep = rars.empty() ? sep : ", ";
    fmt::format_to(ctx.out(), "{}{}", ue_msgs.empty() ? "" : sep, fmt::join(ue_msgs.begin(), ue_msgs.end(), ", "));
    sep = ue_msgs.empty() ? sep : ", ";
    fmt::format_to(ctx.out(), "{}{}", puschs.empty() ? "" : sep, fmt::join(puschs.begin(), puschs.end(), ", "));
    sep = puschs.empty() ? sep : ", ";
    fmt::format_to(ctx.out(), "{}{}", pgs.empty() ? "" : sep, fmt::join(pgs.begin(), pgs.end(), ", "));
    return ctx.out();
  });
}

static auto make_ssb_debug_log_entry(const ssb_information& ssb_info)
{
  return make_formattable([ssb_idx = ssb_info.ssb_index, crbs = ssb_info.crbs, symbs = ssb_info.symbols](auto& ctx) {
    return fmt::format_to(ctx.out(), "\n- SSB: ssbIdx={} crbs={} symb={}", ssb_idx, crbs, symbs);
  });
}

template <typename ItemType, typename ListItemFormatter, std::size_t N>
static auto
format_each(const static_vector<ItemType, N>& list, const ListItemFormatter& item_formatter, bool enabled = true)
{
  static_vector<decltype(item_formatter(std::declval<ItemType>())), N> out;
  if (enabled) {
    for (const ItemType& item : list) {
      out.push_back(item_formatter(item));
    }
  }
  return out;
}

static auto make_csi_rs_log_entry(const csi_rs_info& csi_rs)
{
  return make_formattable([type          = csi_rs.type,
                           crbs          = csi_rs.crbs,
                           row           = csi_rs.row,
                           freq_domain   = csi_rs.freq_domain,
                           symbol0       = csi_rs.symbol0,
                           cdm_type      = csi_rs.cdm_type,
                           freq_density  = csi_rs.freq_density,
                           scrambling_id = csi_rs.scrambling_id](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- CSI-RS: type={} crbs={} row={} freq={} symb0={} cdm_type={} freq_density={}",
                   type == csi_rs_type::CSI_RS_NZP ? "nzp" : "zp",
                   crbs,
                   row,
                   freq_domain,
                   symbol0,
                   to_string(cdm_type),
                   to_string(freq_density));
    if (type == csi_rs_type::CSI_RS_NZP) {
      fmt::format_to(ctx.out(), " scramb_id={}", scrambling_id);
    }
    return ctx.out();
  });
}

static auto make_sib_debug_log_entry(const sib_information& sib_info)
{
  return make_formattable([si_ind  = sib_info.si_indicator,
                           rbs     = sib_info.pdsch_cfg.rbs,
                           symbols = sib_info.pdsch_cfg.symbols,
                           tbs     = sib_info.pdsch_cfg.codewords[0].tb_size_bytes,
                           mcs     = sib_info.pdsch_cfg.codewords[0].mcs_index,
                           rv      = sib_info.pdsch_cfg.codewords[0].rv_index](auto& ctx) {
    return fmt::format_to(ctx.out(),
                          "\n- SI{} PDSCH: rb={} symb={} tbs={} mcs={} rv={}",
                          si_ind == sib_information::sib1 ? "B1" : "",
                          rbs,
                          symbols,
                          tbs,
                          mcs,
                          rv);
  });
}

static auto make_rar_debug_log_entry(const rar_information& rar_info)
{
  auto make_rar_grant_debug_entry = [](const rar_ul_grant& grant) {
    return make_formattable(
        [tcrnti = grant.temp_crnti, rapid = grant.rapid, ta = grant.ta, td = grant.time_resource_assignment](
            auto& ctx) {
          return fmt::format_to(ctx.out(), "tc-rnti={}: rapid={} ta={} time_res={}", tcrnti, rapid, ta, td);
        });
  };

  return make_formattable([rnti       = rar_info.pdsch_cfg.rnti,
                           rb         = rar_info.pdsch_cfg.rbs,
                           symbols    = rar_info.pdsch_cfg.symbols,
                           tbs        = rar_info.pdsch_cfg.codewords[0].tb_size_bytes,
                           mcs        = rar_info.pdsch_cfg.codewords[0].mcs_index,
                           rv         = rar_info.pdsch_cfg.codewords[0].rv_index,
                           nof_grants = rar_info.grants.size(),
                           grants     = format_each(rar_info.grants, make_rar_grant_debug_entry)](auto& ctx) {
    return fmt::format_to(ctx.out(),
                          "\n- RAR PDSCH: ra-rnti={} rb={} symb={} tbs={} mcs={} rv={} grants ({}): {}",
                          rnti,
                          rb,
                          symbols,
                          tbs,
                          mcs,
                          rv,
                          nof_grants,
                          grants);
  });
}

static auto make_ue_dl_msg_debug_log_entry(const dl_msg_alloc& ue_grant)
{
  return make_formattable([ue_grant](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- UE PDSCH: ue={} c-rnti={} h_id={} rb={} symb={} tbs={} mcs={} rv={} nrtx={} k1={}",
                   fmt::underlying(ue_grant.context.ue_index),
                   ue_grant.pdsch_cfg.rnti,
                   fmt::underlying(ue_grant.pdsch_cfg.harq_id),
                   ue_grant.pdsch_cfg.rbs,
                   ue_grant.pdsch_cfg.symbols,
                   ue_grant.pdsch_cfg.codewords[0].tb_size_bytes,
                   ue_grant.pdsch_cfg.codewords[0].mcs_index,
                   ue_grant.pdsch_cfg.codewords[0].rv_index,
                   ue_grant.context.nof_retxs,
                   ue_grant.context.k1);
    if (ue_grant.pdsch_cfg.precoding.has_value() and not ue_grant.pdsch_cfg.precoding.value().prg_infos.empty()) {
      const auto& prg_type = ue_grant.pdsch_cfg.precoding->prg_infos[0].type;
      fmt::format_to(ctx.out(), " ri={} {}", ue_grant.pdsch_cfg.nof_layers, csi_report_pmi{prg_type});
    }
    if (ue_grant.pdsch_cfg.codewords[0].new_data) {
      fmt::format_to(ctx.out(), " dl_bo={}", ue_grant.context.buffer_occupancy);
    }
    if (ue_grant.context.olla_offset.has_value()) {
      fmt::format_to(ctx.out(), " olla={:.3}", *ue_grant.context.olla_offset);
    }
    if (not ue_grant.tb_list.empty()) {
      for (const dl_msg_lc_info& lc : ue_grant.tb_list[0].lc_chs_to_sched) {
        fmt::format_to(ctx.out(),
                       "{}lcid={}: size={}",
                       (&lc == &ue_grant.tb_list[0].lc_chs_to_sched.front()) ? " grants: " : ", ",
                       lc.lcid,
                       lc.sched_bytes);
      }
    }
    return ctx.out();
  });
}

static auto make_paging_debug_log_entry(const dl_paging_allocation& pg)
{
  return make_formattable([pg](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- PCCH: rb={} symb={} tbs={} mcs={} rv={}",
                   pg.pdsch_cfg.rbs,
                   pg.pdsch_cfg.symbols,
                   pg.pdsch_cfg.codewords[0].tb_size_bytes,
                   pg.pdsch_cfg.codewords[0].mcs_index,
                   pg.pdsch_cfg.codewords[0].rv_index);

    for (const paging_ue_info& ue : pg.paging_ue_list) {
      fmt::format_to(ctx.out(),
                     "{}{}-pg-id={:#x}",
                     (&ue == &pg.paging_ue_list.front()) ? " ues: " : ", ",
                     ue.paging_type_indicator == paging_ue_info::paging_identity_type::cn_ue_paging_identity ? "cn"
                                                                                                             : "ran",
                     ue.paging_identity);
    }
    return ctx.out();
  });
}

static auto make_pusch_debug_log_entry(const ul_sched_info& ul_info)
{
  return make_formattable([ul_info](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- UE PUSCH: ue={} {}c-rnti={} h_id={} rb={} symb={} tbs={} rv={} nrtx={} nof_layers={}",
                   fmt::underlying(ul_info.context.ue_index),
                   ul_info.context.ue_index == INVALID_DU_UE_INDEX ? "t" : "",
                   ul_info.pusch_cfg.rnti,
                   fmt::underlying(ul_info.pusch_cfg.harq_id),
                   ul_info.pusch_cfg.rbs,
                   ul_info.pusch_cfg.symbols,
                   ul_info.pusch_cfg.tb_size_bytes,
                   ul_info.pusch_cfg.rv_index,
                   ul_info.context.nof_retxs,
                   ul_info.pusch_cfg.nof_layers);
    if (ul_info.context.olla_offset.has_value()) {
      fmt::format_to(ctx.out(), " olla={:.3}", ul_info.context.olla_offset.value());
    }
    if (ul_info.context.ue_index == INVALID_DU_UE_INDEX and ul_info.context.nof_retxs == 0 and
        ul_info.context.msg3_delay.has_value()) {
      fmt::format_to(ctx.out(), " msg3_delay={}", ul_info.context.msg3_delay.value());
    } else {
      fmt::format_to(ctx.out(), " k2={}", ul_info.context.k2);
    }

    if (ul_info.uci.has_value()) {
      fmt::format_to(
          ctx.out(),
          " uci: harq_bits={} csi-1_bits={} csi-2_present={}",
          ul_info.uci.value().harq.has_value() ? ul_info.uci.value().harq.value().harq_ack_nof_bits : 0U,
          ul_info.uci.value().csi.has_value() ? ul_info.uci.value().csi.value().csi_part1_nof_bits : 0U,
          ul_info.uci.value().csi.has_value() && ul_info.uci.value().csi.value().beta_offset_csi_2.has_value() ? "Yes"
                                                                                                               : "No");
    }

    return ctx.out();
  });
}

static auto make_pucch_debug_log_entry(const pucch_info& pucch)
{
  return make_formattable([pucch](auto& ctx) {
    switch (pucch.format()) {
      case pucch_format::FORMAT_0: {
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=0 prb={}", pucch.crnti, pucch.resources.prbs);
        if (not pucch.resources.second_hop_prbs.empty()) {
          format_to(ctx.out(), " second_prbs={}", pucch.resources.second_hop_prbs);
        }
        format_to(ctx.out(),
                  " symb={} uci: harq_bits={} sr={}",
                  pucch.resources.symbols,
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits));
      } break;
      case pucch_format::FORMAT_1: {
        const auto& format_1 = std::get<pucch_format_1>(pucch.format_params);
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=1 prb={}", pucch.crnti, pucch.resources.prbs);
        if (not pucch.resources.second_hop_prbs.empty()) {
          format_to(ctx.out(), " second_prbs={}", pucch.resources.second_hop_prbs);
        }
        format_to(ctx.out(),
                  " symb={} cs={} occ={} uci: harq_bits={} sr={}",
                  pucch.resources.symbols,
                  format_1.initial_cyclic_shift,
                  format_1.time_domain_occ,
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits));

      } break;
      case pucch_format::FORMAT_2: {
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=2 prb={}", pucch.crnti, pucch.resources.prbs);
        if (not pucch.resources.second_hop_prbs.empty()) {
          format_to(ctx.out(), " second_prbs={}", pucch.resources.second_hop_prbs);
        }
        format_to(ctx.out(),
                  " symb={} uci: harq_bits={} sr={} csi-1_bits={}",
                  pucch.resources.symbols,
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits),
                  pucch.uci_bits.csi_part1_nof_bits);

      } break;
      case pucch_format::FORMAT_3: {
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=3 prb={}", pucch.crnti, pucch.resources.prbs);
        if (not pucch.resources.second_hop_prbs.empty()) {
          format_to(ctx.out(), " second_prbs={}", pucch.resources.second_hop_prbs);
        }
        format_to(ctx.out(),
                  " symb={} uci: harq_bits={} sr={} csi-1_bits={}",
                  pucch.resources.symbols,
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits),
                  pucch.uci_bits.csi_part1_nof_bits);
      } break;
      case pucch_format::FORMAT_4: {
        const auto& format_4 = std::get<pucch_format_4>(pucch.format_params);
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=4 prb={}", pucch.crnti, pucch.resources.prbs);
        if (not pucch.resources.second_hop_prbs.empty()) {
          format_to(ctx.out(), " second_prbs={}", pucch.resources.second_hop_prbs);
        }
        format_to(ctx.out(),
                  " symb={} occ={}/{} uci: harq_bits={} sr={} csi-1_bits={}",
                  pucch.resources.symbols,
                  format_4.orthog_seq_idx,
                  fmt::underlying(format_4.n_sf_pucch_f4),
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits),
                  pucch.uci_bits.csi_part1_nof_bits);
      } break;
      default:
        srsran_assertion_failure("Invalid PUCCH format");
    }
    return ctx.out();
  });
}

static auto make_srs_debug_log_entry(const srs_info& srs)
{
  return make_formattable([srs](auto& ctx) {
    fmt::format_to(
        ctx.out(),
        "\n- SRS: c-rnti={} symb={} tx-comb=(n{} o={} cs={}) c_srs={} f_sh={} seq_id={} requests=[ch_mtx={} pos={}]",
        srs.crnti,
        srs.symbols,
        static_cast<unsigned>(srs.tx_comb),
        srs.comb_offset,
        srs.cyclic_shift,
        srs.config_index,
        srs.freq_shift,
        srs.sequence_id,
        srs.normalized_channel_iq_matrix_requested ? "yes" : "no",
        srs.positioning_report_requested ? "yes" : "no");
    return ctx.out();
  });
}

static auto make_prach_debug_log_entry(const prach_occasion_info& prach)
{
  return make_formattable([prach](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- PRACH: pci={} format={} nof_occasions={} nof_preambles={}",
                   prach.pci,
                   to_string(prach.format),
                   prach.nof_prach_occasions,
                   prach.nof_preamble_indexes);
    return ctx.out();
  });
}

static auto make_debug_log_entry(const sched_result& result, bool log_broadcast)
{
  return make_formattable(
      [ssbs      = format_each(result.dl.bc.ssb_info, make_ssb_debug_log_entry, log_broadcast),
       dl_pdcchs = make_dl_pdcch_log_list(result.dl.dl_pdcchs, log_broadcast),
       ul_pdcchs = format_each(result.dl.ul_pdcchs, make_ul_pdcch_log_entry),
       csi_rs    = format_each(result.dl.csi_rs, make_csi_rs_log_entry, log_broadcast),
       sibs      = format_each(result.dl.bc.sibs, make_sib_debug_log_entry, log_broadcast),
       rars      = format_each(result.dl.rar_grants, make_rar_debug_log_entry),
       ue_grants = format_each(result.dl.ue_grants, make_ue_dl_msg_debug_log_entry),
       paging    = format_each(result.dl.paging_grants, make_paging_debug_log_entry),
       puschs    = format_each(result.ul.puschs, make_pusch_debug_log_entry),
       pucchs    = format_each(result.ul.pucchs, make_pucch_debug_log_entry),
       srss      = format_each(result.ul.srss, make_srs_debug_log_entry),
       prachs    = format_each(result.ul.prachs, make_prach_debug_log_entry, log_broadcast)](auto& ctx) {
        fmt::format_to(ctx.out(), "{}", fmt::join(ssbs.begin(), ssbs.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(dl_pdcchs.begin(), dl_pdcchs.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(ul_pdcchs.begin(), ul_pdcchs.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(csi_rs.begin(), csi_rs.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(sibs.begin(), sibs.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(rars.begin(), rars.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(ue_grants.begin(), ue_grants.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(paging.begin(), paging.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(puschs.begin(), puschs.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(pucchs.begin(), pucchs.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(srss.begin(), srss.end(), ""));
        fmt::format_to(ctx.out(), "{}", fmt::join(prachs.begin(), prachs.end(), ""));
        return ctx.out();
      });
}

scheduler_result_logger::scheduler_result_logger(bool log_broadcast_, pci_t pci_) :
  logger(srslog::fetch_basic_logger("SCHED")), log_broadcast(log_broadcast_), enabled(logger.info.enabled()), pci(pci_)
{
}

void scheduler_result_logger::log_debug(const sched_result& result, std::chrono::microseconds decision_latency)
{
  const bool broadcast_is_empty = result.dl.bc.ssb_info.empty() and result.dl.bc.sibs.empty() and
                                  result.dl.csi_rs.empty() and result.ul.prachs.empty();

  const bool non_broadcast_is_empty =
      std::none_of(result.dl.dl_pdcchs.begin(),
                   result.dl.dl_pdcchs.end(),
                   [](const pdcch_dl_information& pdcch) { return pdcch.ctx.rnti != rnti_t::SI_RNTI; }) and
      result.dl.ul_pdcchs.empty() and result.dl.paging_grants.empty() and result.dl.rar_grants.empty() and
      result.dl.ue_grants.empty() and result.ul.puschs.empty() and result.ul.pucchs.empty() and result.ul.srss.empty();

  const bool failed_attempts = result.failed_attempts.pdcch + result.failed_attempts.uci > 0;
  const bool slot_is_logged =
      (log_broadcast and not broadcast_is_empty) or not non_broadcast_is_empty or failed_attempts;

  if (slot_is_logged) {
    unsigned nof_pdschs = result.dl.paging_grants.size() + result.dl.rar_grants.size() + result.dl.ue_grants.size();
    if (log_broadcast) {
      nof_pdschs += result.dl.bc.sibs.size();
    }
    const unsigned nof_puschs       = result.ul.puschs.size();
    const unsigned nof_pucchs       = result.ul.pucchs.size();
    const unsigned nof_failed_pdcch = result.failed_attempts.pdcch;
    const unsigned nof_failed_uci   = result.failed_attempts.uci;
    logger.debug("Slot decisions pci={} t={}us ({} PDSCH{}, {} PUSCH{}, {} PUCCH{}, {} attempted PDCCH{}, {} attempted "
                 "UCI{}):{}",
                 pci,
                 decision_latency.count(),
                 nof_pdschs,
                 nof_pdschs == 1 ? "" : "s",
                 nof_puschs,
                 nof_puschs == 1 ? "" : "s",
                 nof_pucchs,
                 nof_pucchs == 1 ? "" : "s",
                 nof_failed_pdcch,
                 nof_failed_pdcch == 1 ? "" : "s",
                 nof_failed_uci,
                 nof_failed_uci == 1 ? "" : "s",
                 make_debug_log_entry(result, log_broadcast));
  }
}

void scheduler_result_logger::log_info(const sched_result& result, std::chrono::microseconds decision_latency)
{
  const bool failed_attempts = result.failed_attempts.pdcch + result.failed_attempts.uci > 0;
  bool       slot_is_logged  = (log_broadcast and not result.dl.bc.sibs.empty()) or not result.dl.rar_grants.empty() or
                        not result.dl.ue_grants.empty() or not result.ul.puschs.empty() or
                        not result.dl.paging_grants.empty() or failed_attempts;

  if (slot_is_logged) {
    unsigned nof_pdschs = result.dl.paging_grants.size() + result.dl.rar_grants.size() + result.dl.ue_grants.size();
    if (log_broadcast) {
      nof_pdschs += result.dl.bc.sibs.size();
    }
    const unsigned nof_puschs = result.ul.puschs.size();
    const unsigned nof_pucchs = result.ul.pucchs.size();
    logger.info("Slot decisions pci={} t={}us ({} PDSCH{}, {} PUSCH{}, {} PUCCH{}): {}",
                pci,
                decision_latency.count(),
                nof_pdschs,
                nof_pdschs == 1 ? "" : "s",
                nof_puschs,
                nof_puschs == 1 ? "" : "s",
                nof_pucchs,
                nof_pucchs == 1 ? "" : "s",
                make_info_log_entry(result, log_broadcast));
  }
}

void scheduler_result_logger::on_scheduler_result(const sched_result&       result,
                                                  std::chrono::microseconds decision_latency)
{
  if (not enabled) {
    return;
  }
  if (logger.debug.enabled()) {
    log_debug(result, decision_latency);
  } else {
    log_info(result, decision_latency);
  }
}
