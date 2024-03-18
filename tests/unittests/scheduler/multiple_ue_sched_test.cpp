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

#include "lib/scheduler/scheduler_impl.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "lib/scheduler/ue_scheduling/ue_fallback_scheduler.h"
#include "test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/scheduler/scheduler_dci.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <unordered_map>

using namespace srsran;

struct sched_test_ue {
  rnti_t                                                         crnti;
  std::unordered_map<lcg_id_t, ul_bsr_lcg_report>                ul_bsr_list;
  std::unordered_map<lcid_t, dl_buffer_state_indication_message> dl_bsr_list;
  sched_ue_creation_request_message                              msg;
};

unsigned allocate_rnti()
{
  // Randomly chosen RNTI start.
  static unsigned rnti_start = 0x4601;
  return rnti_start++;
}

// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  // Maximum number of slots to run per UE in order to validate the results of scheduler. Implementation defined.
  static constexpr unsigned max_test_run_slots_per_ue = 80;

  scheduler_expert_config                          expert_cfg;
  cell_configuration                               cell_cfg;
  sched_cfg_dummy_notifier                         cfg_notif;
  scheduler_ue_metrics_dummy_notifier              metric_notif;
  scheduler_impl                                   sch;
  std::unordered_map<du_ue_index_t, sched_test_ue> ues;
  const sched_result*                              sched_res{nullptr};

  explicit test_bench(const scheduler_expert_config&                  expert_cfg_,
                      const sched_cell_configuration_request_message& cell_req) :
    expert_cfg{expert_cfg_}, cell_cfg{expert_cfg, cell_req}, sch{scheduler_config{expert_cfg, cfg_notif, metric_notif}}
  {
    sch.handle_cell_configuration_request(cell_req);
  }

  du_ue_index_t rnti_to_du_ue_index(rnti_t rnti)
  {
    for (const auto& u : ues) {
      if (u.second.crnti == rnti) {
        return u.first;
      }
    }
    return du_ue_index_t::INVALID_DU_UE_INDEX;
  }
};

class scheduler_impl_tester
{
protected:
  slot_point            current_slot{0, 0};
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST", true);
  optional<test_bench>  bench;

  unsigned last_csi_report_offset = 0;

  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned max_k_value = 0;

  ~scheduler_impl_tester()
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void setup_sched(const scheduler_expert_config& expert_cfg, const sched_cell_configuration_request_message& msg)
  {
    current_slot = slot_point{to_numerology_value(msg.scs_common), 0};

    const auto& dl_lst = msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (const auto& pdsch : dl_lst) {
      if (pdsch.k0 > max_k_value) {
        max_k_value = pdsch.k0;
      }
    }
    const auto& ul_lst = msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      if (pusch.k2 > max_k_value) {
        max_k_value = pusch.k2;
      }
    }

    bench.emplace(expert_cfg, msg);

    // Initialize.
    mac_logger.set_context(current_slot.sfn(), current_slot.slot_index());
    test_logger.set_context(current_slot.sfn(), current_slot.slot_index());
    bench->sched_res = &bench->sch.slot_indication(current_slot, to_du_cell_index(0));
  }

  void run_slot()
  {
    current_slot++;

    mac_logger.set_context(current_slot.sfn(), current_slot.slot_index());
    test_logger.set_context(current_slot.sfn(), current_slot.slot_index());

    bench->sched_res = &bench->sch.slot_indication(current_slot, to_du_cell_index(0));

    // Check sched result consistency.
    test_scheduler_result_consistency(bench->cell_cfg, current_slot, *bench->sched_res);
  }

  static scheduler_expert_config create_expert_config(sch_mcs_index max_msg4_mcs_index)
  {
    auto cfg            = config_helpers::make_default_scheduler_expert_config();
    cfg.ue.max_msg4_mcs = max_msg4_mcs_index;
    return cfg;
  }

  sched_cell_configuration_request_message create_custom_cell_config_request(duplex_mode mode) const
  {
    return test_helpers::make_default_sched_cell_configuration_request(create_custom_cell_cfg_builder_params(mode));
  }

  unsigned pdsch_tbs_scheduled_bytes_per_lc(const sched_test_ue& u, lcid_t lcid)
  {
    unsigned total_cw_tb_size_bytes = 0;
    for (const auto& grant : bench->sched_res->dl.ue_grants) {
      if (grant.pdsch_cfg.rnti != u.crnti) {
        continue;
      }
      for (const auto& tb : grant.tb_list) {
        for (const auto& s_pdu : tb.lc_chs_to_sched) {
          if (s_pdu.lcid == lcid) {
            total_cw_tb_size_bytes += s_pdu.sched_bytes;
          }
        }
      }
    }
    return total_cw_tb_size_bytes;
  }

  unsigned pusch_tbs_scheduled_bytes(const sched_test_ue& u)
  {
    unsigned total_cw_tb_size_bytes = 0;
    for (const auto& grant : bench->sched_res->ul.puschs) {
      if (grant.pusch_cfg.rnti != u.crnti) {
        continue;
      }
      total_cw_tb_size_bytes += grant.pusch_cfg.tb_size_bytes;
    }
    return total_cw_tb_size_bytes;
  }

  cell_config_builder_params create_custom_cell_cfg_builder_params(duplex_mode mode) const
  {
    cell_config_builder_params cell_cfg{};
    if (mode == duplex_mode::TDD) {
      // Band 40.
      cell_cfg.dl_arfcn       = 474000;
      cell_cfg.scs_common     = srsran::subcarrier_spacing::kHz30;
      cell_cfg.band           = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
      cell_cfg.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;

      const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
          cell_cfg.channel_bw_mhz,
          cell_cfg.scs_common,
          cell_cfg.band.has_value() ? band_helper::get_freq_range(cell_cfg.band.value()) : frequency_range::FR1);

      optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
          band_helper::get_ssb_coreset0_freq_location(cell_cfg.dl_arfcn,
                                                      *cell_cfg.band,
                                                      nof_crbs,
                                                      cell_cfg.scs_common,
                                                      cell_cfg.scs_common,
                                                      cell_cfg.search_space0_index,
                                                      cell_cfg.max_coreset0_duration);
      cell_cfg.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
      cell_cfg.k_ssb             = ssb_freq_loc->k_ssb;
      cell_cfg.coreset0_index    = ssb_freq_loc->coreset0_idx;
    }
    cell_cfg.csi_rs_enabled = true;
    return cell_cfg;
  }

  void add_ue(du_ue_index_t ue_index, lcid_t lcid_, lcg_id_t lcgid_, duplex_mode mode)
  {
    const auto& cell_cfg_params = create_custom_cell_cfg_builder_params(mode);
    add_ue(ue_index, lcid_, lcgid_, cell_cfg_params);
  }

  void add_ue(du_ue_index_t ue_index, lcid_t lcid_, lcg_id_t lcgid_, const cell_config_builder_params& params)
  {
    auto ue_creation_req = test_helpers::create_default_sched_ue_creation_request(params);

    ue_creation_req.ue_index = ue_index;
    ue_creation_req.crnti    = to_rnti(allocate_rnti());

    auto it = std::find_if(ue_creation_req.cfg.lc_config_list->begin(),
                           ue_creation_req.cfg.lc_config_list->end(),
                           [lcid_](const auto& l) { return l.lcid == lcid_; });
    if (it == ue_creation_req.cfg.lc_config_list->end()) {
      ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid_));
      it = ue_creation_req.cfg.lc_config_list->end() - 1;
    }
    it->lc_group = lcgid_;

    const unsigned csi_report_period_slots = csi_report_periodicity_to_uint(
        variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
            ue_creation_req.cfg.cells.value()[0].serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list[0].report_cfg_type)
            .report_slot_period);
    if (bench->cell_cfg.tdd_cfg_common.has_value()) {
      optional<unsigned> slot_offset =
          find_next_tdd_full_ul_slot(bench->cell_cfg.tdd_cfg_common.value(), last_csi_report_offset + 1);
      srsran_assert(slot_offset.has_value(), "Unable to find a valid CSI report slot offset UE={}", ue_index);
      srsran_assert(slot_offset.value() < csi_report_period_slots,
                    "Unable to find a valid CSI report slot offset UE={}",
                    ue_index);
      variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
          ue_creation_req.cfg.cells.value()[0].serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list[0].report_cfg_type)
          .report_slot_offset = *slot_offset;
      last_csi_report_offset  = *slot_offset;
    } else {
      srsran_assert(
          variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
              ue_creation_req.cfg.cells.value()[0].serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list[0].report_cfg_type)
                      .report_slot_offset +
                  ue_index <
              csi_report_period_slots,
          "Unable to find a valid CSI report slot offset UE={}",
          ue_index);
      variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
          ue_creation_req.cfg.cells.value()[0].serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list[0].report_cfg_type)
          .report_slot_offset += ue_index;
    }

    bench->sch.handle_ue_creation_request(ue_creation_req);

    bench->ues[ue_index] = sched_test_ue{ue_creation_req.crnti, {}, {}, ue_creation_req};
  }

  void add_ue(const sched_ue_creation_request_message& ue_create_req)
  {
    bench->sch.handle_ue_creation_request(ue_create_req);

    bench->ues[ue_create_req.ue_index] = sched_test_ue{ue_create_req.crnti, {}, {}, ue_create_req};
  }

  sched_test_ue& get_ue(du_ue_index_t ue_idx) { return bench->ues.at(ue_idx); }

  void push_buffer_state_to_dl_ue(du_ue_index_t ue_index, unsigned buffer_size, lcid_t lcid)
  {
    auto& test_ue = get_ue(ue_index);
    // Notification from upper layers of DL buffer state.
    const dl_buffer_state_indication_message msg{ue_index, lcid, buffer_size};

    // Store to keep track of DL buffer status.
    test_ue.dl_bsr_list[lcid] = msg;

    bench->sch.handle_dl_buffer_state_indication(msg);
  }

  void notify_ul_bsr_from_ue(du_ue_index_t ue_index, unsigned buffer_size, lcg_id_t lcg_id)
  {
    auto& test_ue = get_ue(ue_index);
    // Assumptions:
    // - Only one LCG is assumed to have data to send.
    // - BSR is Short BSR.
    ul_bsr_indication_message msg{to_du_cell_index(0), ue_index, test_ue.crnti, bsr_format::SHORT_BSR, {}};
    msg.reported_lcgs.push_back(ul_bsr_lcg_report{lcg_id, buffer_size});

    // Store to keep track of current buffer status in UE.
    test_ue.ul_bsr_list[lcg_id] = ul_bsr_lcg_report{lcg_id, buffer_size};

    bench->sch.handle_ul_bsr_indication(msg);
  }

  const pdcch_dl_information* find_ue_dl_pdcch(const sched_test_ue& u) const
  {
    const pdcch_dl_information* it = std::find_if(bench->sched_res->dl.dl_pdcchs.begin(),
                                                  bench->sched_res->dl.dl_pdcchs.end(),
                                                  [&u](const auto& grant) { return grant.ctx.rnti == u.crnti; });
    return it == bench->sched_res->dl.dl_pdcchs.end() ? nullptr : &*it;
  }

  const pdcch_ul_information* find_ue_ul_pdcch(const sched_test_ue& u) const
  {
    const pdcch_ul_information* it = std::find_if(bench->sched_res->dl.ul_pdcchs.begin(),
                                                  bench->sched_res->dl.ul_pdcchs.end(),
                                                  [&u](const auto& grant) { return grant.ctx.rnti == u.crnti; });
    return it == bench->sched_res->dl.ul_pdcchs.end() ? nullptr : &*it;
  }

  const dl_msg_alloc* find_ue_pdsch(const sched_test_ue& u) const
  {
    const auto* it = std::find_if(bench->sched_res->dl.ue_grants.begin(),
                                  bench->sched_res->dl.ue_grants.end(),
                                  [&u](const auto& grant) { return grant.pdsch_cfg.rnti == u.crnti; });
    return it == bench->sched_res->dl.ue_grants.end() ? nullptr : &*it;
  }

  bool ue_is_allocated_pdsch(const sched_test_ue& u) const { return find_ue_pdsch(u) != nullptr; }

  const ul_sched_info* find_ue_pusch(const sched_test_ue& u) const
  {
    const auto* it = std::find_if(bench->sched_res->ul.puschs.begin(),
                                  bench->sched_res->ul.puschs.end(),
                                  [&u](const auto& grant) { return grant.pusch_cfg.rnti == u.crnti; });
    return it == bench->sched_res->ul.puschs.end() ? nullptr : &*it;
  }

  bool ue_is_allocated_pusch(const sched_test_ue& u) const { return find_ue_pusch(u) != nullptr; }

  optional<slot_point> get_pusch_scheduled_slot(const sched_test_ue& u) const
  {
    const pdcch_ul_information* it = std::find_if(bench->sched_res->dl.ul_pdcchs.begin(),
                                                  bench->sched_res->dl.ul_pdcchs.end(),
                                                  [&u](const auto& grant) { return grant.ctx.rnti == u.crnti; });
    const auto&                 ue_ul_lst =
        (*u.msg.cfg.cells)[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.value().pusch_td_alloc_list;
    const auto& cell_ul_lst = bench->cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    const auto& ul_lst      = ue_ul_lst.empty() ? cell_ul_lst : ue_ul_lst;

    if (it == bench->sched_res->dl.ul_pdcchs.end()) {
      return {};
    }

    switch (it->dci.type) {
      case dci_ul_rnti_config_type::tc_rnti_f0_0:
        return current_slot + ul_lst[it->dci.tc_rnti_f0_0.time_resource].k2;
      case dci_ul_rnti_config_type::c_rnti_f0_0:
        return current_slot + ul_lst[it->dci.c_rnti_f0_0.time_resource].k2;
      case dci_ul_rnti_config_type::c_rnti_f0_1:
        return current_slot + ul_lst[it->dci.c_rnti_f0_1.time_resource].k2;
    }

    return {};
  }

  optional<slot_point> get_pdsch_scheduled_slot(const sched_test_ue& u) const
  {
    const pdcch_dl_information* it = std::find_if(bench->sched_res->dl.dl_pdcchs.begin(),
                                                  bench->sched_res->dl.dl_pdcchs.end(),
                                                  [&u](const auto& grant) { return grant.ctx.rnti == u.crnti; });
    const auto& ue_dl_lst   = (*u.msg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg.value().pdsch_td_alloc_list;
    const auto& cell_dl_lst = bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    const auto& dl_lst      = ue_dl_lst.empty() ? cell_dl_lst : ue_dl_lst;

    if (it == bench->sched_res->dl.dl_pdcchs.end()) {
      return {};
    }

    switch (it->dci.type) {
      case dci_dl_rnti_config_type::c_rnti_f1_0:
        return current_slot + dl_lst[it->dci.c_rnti_f1_0.time_resource].k0;
      case dci_dl_rnti_config_type::tc_rnti_f1_0:
        return current_slot + dl_lst[it->dci.tc_rnti_f1_0.time_resource].k0;
      case dci_dl_rnti_config_type::c_rnti_f1_1:
        return current_slot + dl_lst[it->dci.c_rnti_f1_1.time_resource].k0;
      default:
        break;
    }

    return {};
  }

  optional<slot_point> get_pdsch_ack_nack_scheduled_slot(const sched_test_ue& u) const
  {
    const pdcch_dl_information* it = std::find_if(bench->sched_res->dl.dl_pdcchs.begin(),
                                                  bench->sched_res->dl.dl_pdcchs.end(),
                                                  [&u](const auto& grant) { return grant.ctx.rnti == u.crnti; });

    if (it == bench->sched_res->dl.dl_pdcchs.end()) {
      return {};
    }

    const auto& dl_to_ack_lst =
        (*u.msg.cfg.cells)[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value().dl_data_to_ul_ack;

    // TS38.213, 9.2.3 - For DCI f1_0, the PDSCH-to-HARQ-timing-indicator field values map to {1, 2, 3, 4, 5, 6, 7, 8}.
    // PDSCH-to-HARQ-timing-indicator provide the index in {1, 2, 3, 4, 5, 6, 7, 8} starting from 0 .. 7.
    switch (it->dci.type) {
      case dci_dl_rnti_config_type::c_rnti_f1_0:
        return current_slot + it->dci.c_rnti_f1_0.pdsch_harq_fb_timing_indicator + 1;
      case dci_dl_rnti_config_type::tc_rnti_f1_0:
        return current_slot + it->dci.tc_rnti_f1_0.pdsch_harq_fb_timing_indicator + 1;
      case dci_dl_rnti_config_type::c_rnti_f1_1:
        return current_slot + dl_to_ack_lst[it->dci.c_rnti_f1_1.pdsch_harq_fb_timing_indicator.value()];
      default:
        break;
    }

    return {};
  }

  uci_indication build_harq_ack_pucch_f0_f1_uci_ind(const du_ue_index_t ue_idx, const slot_point& sl_tx)
  {
    const sched_test_ue& u = get_ue(ue_idx);

    uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu pucch_pdu{};
    pucch_pdu.sr_detected = false;
    pucch_pdu.harqs.push_back(mac_harq_ack_report_status::ack);

    uci_indication::uci_pdu pdu{};
    pdu.crnti    = u.crnti;
    pdu.ue_index = ue_idx;
    pdu.pdu      = pucch_pdu;

    uci_indication uci_ind{};
    uci_ind.slot_rx    = sl_tx;
    uci_ind.cell_index = to_du_cell_index(0);
    uci_ind.ucis.push_back(pdu);

    return uci_ind;
  }

  uci_indication build_harq_nack_pucch_f0_f1_uci_ind(const du_ue_index_t ue_idx, const slot_point& sl_tx)
  {
    const sched_test_ue& u = get_ue(ue_idx);

    uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu pucch_pdu{};
    pucch_pdu.sr_detected = false;
    pucch_pdu.harqs.push_back(mac_harq_ack_report_status::nack);

    uci_indication::uci_pdu pdu{};
    pdu.crnti    = u.crnti;
    pdu.ue_index = ue_idx;
    pdu.pdu      = pucch_pdu;

    uci_indication uci_ind{};
    uci_ind.slot_rx    = sl_tx;
    uci_ind.cell_index = to_du_cell_index(0);
    uci_ind.ucis.push_back(pdu);

    return uci_ind;
  }

  uci_indication::uci_pdu build_pucch_uci_pdu(const pucch_info& pucch)
  {
    uci_indication::uci_pdu pdu{};
    pdu.crnti    = pucch.crnti;
    pdu.ue_index = bench->rnti_to_du_ue_index(pdu.crnti);

    switch (pucch.format) {
      case pucch_format::FORMAT_0:
      case pucch_format::FORMAT_1: {
        uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu pucch_pdu{};
        if (pucch.format == pucch_format::FORMAT_0) {
          pucch_pdu.sr_detected = sr_nof_bits_to_uint(pucch.format_0.sr_bits) > 0;
          // Auto ACK harqs.
          pucch_pdu.harqs.resize(pucch.format_0.harq_ack_nof_bits, mac_harq_ack_report_status::ack);
        } else {
          pucch_pdu.sr_detected = sr_nof_bits_to_uint(pucch.format_1.sr_bits) > 0;
          // Auto ACK harqs.
          pucch_pdu.harqs.resize(pucch.format_1.harq_ack_nof_bits, mac_harq_ack_report_status::ack);
        }
        pucch_pdu.ul_sinr = 55;
        pdu.pdu           = pucch_pdu;
        break;
      }
      case pucch_format::FORMAT_2: {
        uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu pucch_pdu{};
        pucch_pdu.sr_info.resize(sr_nof_bits_to_uint(pucch.format_2.sr_bits));
        pucch_pdu.sr_info.fill(0, sr_nof_bits_to_uint(pucch.format_2.sr_bits), true);
        // Auto ACK harqs.
        pucch_pdu.harqs.resize(pucch.format_2.harq_ack_nof_bits, mac_harq_ack_report_status::ack);
        if (pucch.csi_rep_cfg.has_value()) {
          pucch_pdu.csi.emplace();
          // Fill with dummy values.
          pucch_pdu.csi->ri                    = 1;
          pucch_pdu.csi->first_tb_wideband_cqi = 15;
        }
        pucch_pdu.ul_sinr = 55;
        pdu.pdu           = pucch_pdu;
        break;
      }
      default:
        srsran_assertion_failure("Unsupported PUCCH format");
    }
    return pdu;
  }

  uci_indication::uci_pdu build_pusch_uci_pdu(const ul_sched_info& pusch)
  {
    uci_indication::uci_pdu pdu{};
    pdu.crnti    = pusch.pusch_cfg.rnti;
    pdu.ue_index = bench->rnti_to_du_ue_index(pdu.crnti);

    uci_indication::uci_pdu::uci_pusch_pdu pusch_pdu{};
    // Auto ACK harqs.
    if (pusch.uci->harq.has_value()) {
      pusch_pdu.harqs.resize(pusch.uci->harq->harq_ack_nof_bits, mac_harq_ack_report_status::ack);
    }
    if (pusch.uci->csi.has_value()) {
      pusch_pdu.csi.emplace();
      // Fill with dummy values.
      pusch_pdu.csi->ri                    = 1;
      pusch_pdu.csi->first_tb_wideband_cqi = 15;
    }
    pdu.pdu = pusch_pdu;
    return pdu;
  }

  ul_crc_pdu_indication build_success_crc_pdu_indication(const du_ue_index_t ue_idx, const uint8_t harq_id)
  {
    const sched_test_ue& u = get_ue(ue_idx);

    ul_crc_pdu_indication pdu{};
    pdu.ue_index       = ue_idx;
    pdu.rnti           = u.crnti;
    pdu.harq_id        = (harq_id_t)harq_id;
    pdu.tb_crc_success = true;
    pdu.ul_sinr_metric = 55;

    return pdu;
  }

  optional<search_space_configuration> get_ss_cfg(const sched_test_ue& u, search_space_id ss_id)
  {
    auto it = std::find_if((*u.msg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg.value().search_spaces.begin(),
                           (*u.msg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg.value().search_spaces.end(),
                           [ss_id](const search_space_configuration& cfg) { return cfg.get_id() == ss_id; });
    if (it != (*u.msg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg.value().search_spaces.end()) {
      return *it;
    }

    it = std::find_if(bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces.begin(),
                      bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces.end(),
                      [ss_id](const search_space_configuration& cfg) { return cfg.get_id() == ss_id; });
    if (it != bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces.end()) {
      return *it;
    }

    return nullopt;
  }
};

// Parameters to be passed to test.
struct multiple_ue_test_params {
  uint16_t    nof_ues;
  uint16_t    min_buffer_size_in_bytes;
  uint16_t    max_buffer_size_in_bytes;
  duplex_mode duplx_mode;
};

class multiple_ue_sched_tester : public scheduler_impl_tester, public ::testing::TestWithParam<multiple_ue_test_params>
{
public:
  multiple_ue_sched_tester() : params{GetParam()} {}

protected:
  multiple_ue_test_params params;
};

TEST_P(multiple_ue_sched_tester, dl_buffer_state_indication_test)
{
  // Used to track BSR 0.
  std::map<unsigned, bool>                 is_bsr_zero_sent;
  std::map<unsigned, optional<slot_point>> pdsch_scheduled_slot_in_future;

  const lcid_t lcid = LCID_MIN_DRB;

  // Vector to keep track of ACKs to send.
  std::vector<uci_indication> uci_ind_to_send;

  setup_sched(create_expert_config(10), create_custom_cell_config_request(params.duplx_mode));
  // Add UE(s) and notify to each UE a DL buffer status indication of random size between min and max defined in params.
  // Assumption: LCID is DRB1.
  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    // Initialize.
    is_bsr_zero_sent[idx] = false;

    add_ue(to_du_ue_index(idx), LCID_MIN_DRB, static_cast<lcg_id_t>(0), params.duplx_mode);

    push_buffer_state_to_dl_ue(
        to_du_ue_index(idx),
        test_rgen::uniform_int<unsigned>(params.min_buffer_size_in_bytes, params.max_buffer_size_in_bytes),
        LCID_MIN_DRB);
  }

  for (unsigned i = 0; i != params.nof_ues * test_bench::max_test_run_slots_per_ue * (1U << current_slot.numerology());
       ++i) {
    run_slot();

    std::vector<uci_indication> uci_ind_not_for_current_slot;
    // Send ACKs if there are any to send.
    for (const auto& ind : uci_ind_to_send) {
      if (current_slot == ind.slot_rx) {
        bench->sch.handle_uci_indication(ind);
      } else {
        uci_ind_not_for_current_slot.push_back(ind);
      }
    }
    swap(uci_ind_to_send, uci_ind_not_for_current_slot);

    for (unsigned idx = 0; idx < params.nof_ues; idx++) {
      auto& test_ue = get_ue(to_du_ue_index(idx));
      // Update the PDSCH scheduled slot in the future.
      const auto& pdsch_slot = get_pdsch_scheduled_slot(test_ue);
      if (pdsch_slot.has_value()) {
        pdsch_scheduled_slot_in_future[idx] = pdsch_slot;
      }
      const auto& ack_nack_slot = get_pdsch_ack_nack_scheduled_slot(test_ue);
      if (ack_nack_slot.has_value()) {
        uci_ind_to_send.push_back(build_harq_ack_pucch_f0_f1_uci_ind(to_du_ue_index(idx), ack_nack_slot.value()));
      }

      const auto* grant = find_ue_pdsch(test_ue);
      if (is_bsr_zero_sent[idx] && pdsch_scheduled_slot_in_future[idx].has_value() &&
          current_slot > pdsch_scheduled_slot_in_future[idx].value()) {
        ASSERT_TRUE(grant == nullptr or not grant->pdsch_cfg.codewords[0].new_data)
            << fmt::format("Condition failed for UE with c-rnti={}", test_ue.crnti);
        continue;
      }

      const unsigned tbs_sched_bytes = pdsch_tbs_scheduled_bytes_per_lc(test_ue, lcid);
      if (tbs_sched_bytes == 0 && test_ue.dl_bsr_list.at(lcid).bs == 0 &&
          pdsch_scheduled_slot_in_future[idx].has_value() &&
          current_slot > pdsch_scheduled_slot_in_future[idx].value() && not is_bsr_zero_sent[idx]) {
        is_bsr_zero_sent[idx] = true;
        // Notify buffer status of 0 to ensure scheduler does not schedule further for this UE.
        push_buffer_state_to_dl_ue(to_du_ue_index(idx), 0, lcid);
      }

      if (grant != nullptr && grant->pdsch_cfg.codewords[0].new_data) {
        if (tbs_sched_bytes > test_ue.dl_bsr_list.at(lcid).bs) {
          // Accounting for MAC headers.
          test_ue.dl_bsr_list.at(lcid).bs = 0;
        } else {
          test_ue.dl_bsr_list.at(lcid).bs -= tbs_sched_bytes;
        }
      }
    }
  }

  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    const auto& test_ue = get_ue(to_du_ue_index(idx));
    ASSERT_EQ(test_ue.dl_bsr_list.at(lcid).bs, 0)
        << fmt::format("Condition failed for UE with c-rnti={}", test_ue.crnti);
  }
}

TEST_P(multiple_ue_sched_tester, ul_buffer_state_indication_test)
{
  // Used to track BSR 0.
  std::map<unsigned, bool>                 is_bsr_zero_sent;
  std::map<unsigned, optional<slot_point>> pusch_scheduled_slot_in_future;

  const lcg_id_t lcgid = uint_to_lcg_id(0);

  setup_sched(create_expert_config(10), create_custom_cell_config_request(params.duplx_mode));
  // Add UE(s) and notify UL BSR from UE of random size between min and max defined in params.
  // Assumption: LCID is DRB1.
  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    // Initialize.
    is_bsr_zero_sent[idx]               = false;
    pusch_scheduled_slot_in_future[idx] = {};

    add_ue(to_du_ue_index(idx), LCID_MIN_DRB, static_cast<lcg_id_t>(0), params.duplx_mode);

    notify_ul_bsr_from_ue(
        to_du_ue_index(idx),
        test_rgen::uniform_int<unsigned>(params.min_buffer_size_in_bytes, params.max_buffer_size_in_bytes),
        static_cast<lcg_id_t>(0));
  }

  for (unsigned i = 0; i != params.nof_ues * test_bench::max_test_run_slots_per_ue * (1U << current_slot.numerology());
       ++i) {
    run_slot();

    // CRC Indication to send if there PUSCH is scheduled in current slot.
    ul_crc_indication crc_ind{};
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.sl_rx      = current_slot;

    for (unsigned idx = 0; idx < params.nof_ues; idx++) {
      auto& test_ue = get_ue(to_du_ue_index(idx));
      // Update the PUSCH scheduled slot in the future.
      const auto& pusch_slot = get_pusch_scheduled_slot(test_ue);
      if (pusch_slot.has_value()) {
        pusch_scheduled_slot_in_future[idx] = pusch_slot;
      }

      const auto* pusch = find_ue_pusch(test_ue);
      if (is_bsr_zero_sent[idx] && pusch_scheduled_slot_in_future[idx].has_value() &&
          current_slot > pusch_scheduled_slot_in_future[idx].value()) {
        ASSERT_TRUE(pusch == nullptr or not pusch->pusch_cfg.new_data)
            << fmt::format("Condition failed for UE with CRNTI=0x{:x}", test_ue.crnti);
        continue;
      }

      const unsigned tbs_sched_bytes = pusch_tbs_scheduled_bytes(test_ue);
      if (tbs_sched_bytes == 0 && test_ue.ul_bsr_list.at(lcgid).nof_bytes == 0 &&
          pusch_scheduled_slot_in_future[idx].has_value() &&
          current_slot > pusch_scheduled_slot_in_future[idx].value() && not is_bsr_zero_sent[idx]) {
        is_bsr_zero_sent[idx] = true;
        // Notify BSR 0 to ensure scheduler does not schedule further for this UE.
        notify_ul_bsr_from_ue(to_du_ue_index(idx), 0, static_cast<lcg_id_t>(0));
      }

      if (pusch != nullptr && pusch->pusch_cfg.new_data) {
        crc_ind.crcs.push_back(build_success_crc_pdu_indication(to_du_ue_index(idx), pusch->pusch_cfg.harq_id));
        if (tbs_sched_bytes > test_ue.ul_bsr_list.at(lcgid).nof_bytes) {
          // Accounting for MAC headers.
          test_ue.ul_bsr_list.at(lcgid).nof_bytes = 0;
        } else {
          test_ue.ul_bsr_list.at(lcgid).nof_bytes -= tbs_sched_bytes;
        }
      }
    }

    if (not crc_ind.crcs.empty()) {
      bench->sch.handle_crc_indication(crc_ind);
    }
  }

  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    const auto& test_ue = get_ue(to_du_ue_index(idx));
    ASSERT_EQ(test_ue.ul_bsr_list.at(lcgid).nof_bytes, 0)
        << fmt::format("Condition failed for UE with c-rnti={}", test_ue.crnti);
  }
}

TEST_P(multiple_ue_sched_tester, when_scheduling_multiple_ue_in_small_bw_neither_pdsch_and_pusch_must_be_starved)
{
  const unsigned max_test_run_slots = 100;
  const lcid_t   lcid               = LCID_MIN_DRB;
  const lcg_id_t lcgid              = uint_to_lcg_id(0);

  // Make custom cell configuration for TDD and FDD i.e. 10 Mhz for TDD and 5Mhz for FDD.
  auto builder_params           = create_custom_cell_cfg_builder_params(params.duplx_mode);
  builder_params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz5;
  if (params.duplx_mode == duplex_mode::TDD) {
    builder_params.channel_bw_mhz = srsran::bs_channel_bandwidth_fr1::MHz10;
  }
  builder_params.band = band_helper::get_band_from_dl_arfcn(builder_params.dl_arfcn);

  const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
      builder_params.channel_bw_mhz,
      builder_params.scs_common,
      builder_params.band.has_value() ? band_helper::get_freq_range(builder_params.band.value())
                                      : frequency_range::FR1);

  optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
      band_helper::get_ssb_coreset0_freq_location(builder_params.dl_arfcn,
                                                  *builder_params.band,
                                                  nof_crbs,
                                                  builder_params.scs_common,
                                                  builder_params.scs_common,
                                                  builder_params.search_space0_index,
                                                  builder_params.max_coreset0_duration);
  builder_params.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
  builder_params.k_ssb             = ssb_freq_loc->k_ssb;
  builder_params.coreset0_index    = ssb_freq_loc->coreset0_idx;

  config_helpers::cell_config_builder_params_extended extended_params{builder_params};
  setup_sched(create_expert_config(10), test_helpers::make_default_sched_cell_configuration_request(extended_params));

  // NOTE: The buffer size must be high enough for the scheduler to keep allocating resources to the UE. In order to
  // avoid failing of test we ignore the min_buffer_size_in_bytes and max_buffer_size_in_bytes set in params.
  const unsigned dl_buffer_size = 10000;
  const unsigned ul_buffer_size = 10000;

  // Add UE(s) and notify to each UE a DL buffer status indication of random size between min and max defined in
  // params. Assumption: LCID is DRB1.
  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    add_ue(to_du_ue_index(idx), lcid, lcgid, extended_params);
    push_buffer_state_to_dl_ue(to_du_ue_index(idx), dl_buffer_size, lcid);
    notify_ul_bsr_from_ue(to_du_ue_index(idx), ul_buffer_size, lcgid);
  }

  bool     first_pdsch_scheduled = false;
  bool     first_pusch_scheduled = false;
  unsigned nof_cqi_reported      = 0;

  for (unsigned i = 0; i != max_test_run_slots; ++i) {
    run_slot();

    if (bench->sched_res == nullptr) {
      continue;
    }

    uci_indication uci_ind{};
    uci_ind.slot_rx    = current_slot;
    uci_ind.cell_index = to_du_cell_index(0);

    ul_crc_indication crc_ind{};
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.sl_rx      = current_slot;

    for (const auto& pucch : bench->sched_res->ul.pucchs) {
      uci_ind.ucis.push_back(build_pucch_uci_pdu(pucch));
      if (pucch.format == srsran::pucch_format::FORMAT_2 and pucch.csi_rep_cfg.has_value()) {
        // For now CSI report is only sent in PUCCH format 2.
        ++nof_cqi_reported;
      }
    }
    for (const auto& pusch : bench->sched_res->ul.puschs) {
      if (pusch.uci.has_value()) {
        uci_ind.ucis.push_back(build_pusch_uci_pdu(pusch));
        if (pusch.uci->csi.has_value()) {
          ++nof_cqi_reported;
        }
      }
      crc_ind.crcs.push_back(
          build_success_crc_pdu_indication(bench->rnti_to_du_ue_index(pusch.pusch_cfg.rnti), pusch.pusch_cfg.harq_id));
      first_pusch_scheduled = true;

      auto& test_ue = get_ue(bench->rnti_to_du_ue_index(pusch.pusch_cfg.rnti));
      // Keep track of nof. bytes scheduled and nof. bytes to be alloced.
      if (pusch.pusch_cfg.tb_size_bytes < test_ue.ul_bsr_list.at(lcgid).nof_bytes) {
        test_ue.ul_bsr_list.at(lcgid).nof_bytes -= pusch.pusch_cfg.tb_size_bytes;
      }
    }
    if (not uci_ind.ucis.empty()) {
      bench->sch.handle_uci_indication(uci_ind);
    }
    if (not crc_ind.crcs.empty()) {
      bench->sch.handle_crc_indication(crc_ind);
    }

    for (const auto& pdsch : bench->sched_res->dl.ue_grants) {
      first_pdsch_scheduled = true;
      // Keep track of nof. bytes scheduled and nof. bytes to be alloced.
      auto& test_ue = get_ue(bench->rnti_to_du_ue_index(pdsch.pdsch_cfg.rnti));
      if (pdsch.pdsch_cfg.codewords[0].tb_size_bytes < test_ue.dl_bsr_list.at(lcid).bs) {
        test_ue.dl_bsr_list.at(lcid).bs -= pdsch.pdsch_cfg.codewords[0].tb_size_bytes;
      }
    }

    for (unsigned idx = 0; idx < params.nof_ues; idx++) {
      auto& test_ue = get_ue(to_du_ue_index(idx));
      // Kep pushing buffer status and UL BSR to ensure scheduler keeps scheduling UE.
      push_buffer_state_to_dl_ue(
          to_du_ue_index(to_du_ue_index(idx)), dl_buffer_size + test_ue.dl_bsr_list.at(lcid).bs, lcid);
      notify_ul_bsr_from_ue(to_du_ue_index(idx), ul_buffer_size + test_ue.ul_bsr_list.at(lcgid).nof_bytes, lcgid);
    }

    // Ensure there is atleast one PDSCH scheduled in each DL slot and one PUSCH scheduled in each UL slot.
    if (first_pusch_scheduled and first_pdsch_scheduled and nof_cqi_reported >= params.nof_ues) {
      if (bench->cell_cfg.tdd_cfg_common.has_value()) {
        if (has_active_tdd_dl_symbols(bench->cell_cfg.tdd_cfg_common.value(), current_slot.slot_index())) {
          ASSERT_GE(bench->sched_res->dl.ue_grants.size(), 1);
        } else if (is_tdd_full_ul_slot(bench->cell_cfg.tdd_cfg_common.value(), current_slot.slot_index())) {
          ASSERT_GE(bench->sched_res->ul.puschs.size(), 1);
        }
      } else {
        // FDD.
        ASSERT_GE(bench->sched_res->dl.ue_grants.size(), 1);
        ASSERT_GE(bench->sched_res->ul.puschs.size(), 1);
      }
    }
  }
}

TEST_P(multiple_ue_sched_tester, not_scheduled_when_buffer_status_zero)
{
  setup_sched(create_expert_config(10), create_custom_cell_config_request(params.duplx_mode));
  // Add UE(s) and notify UL BSR + DL Buffer status with zero value.
  // Assumption: LCID is DRB1.
  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    add_ue(to_du_ue_index(idx), LCID_MIN_DRB, static_cast<lcg_id_t>(0), params.duplx_mode);

    notify_ul_bsr_from_ue(to_du_ue_index(idx), 0, static_cast<lcg_id_t>(0));
    push_buffer_state_to_dl_ue(to_du_ue_index(idx), 0, LCID_MIN_DRB);
  }

  for (unsigned i = 0; i != params.nof_ues * test_bench::max_test_run_slots_per_ue * (1U << current_slot.numerology());
       ++i) {
    run_slot();
    for (unsigned idx = 0; idx < params.nof_ues; idx++) {
      auto& test_ue = get_ue(to_du_ue_index(idx));
      ASSERT_FALSE(ue_is_allocated_pusch(test_ue));
      ASSERT_FALSE(ue_is_allocated_pdsch(test_ue));
    }
  }
}

TEST_P(multiple_ue_sched_tester, dl_dci_format_1_1_test)
{
  // Used to track BSR 0.
  std::map<unsigned, bool>                 is_bsr_zero_sent;
  std::map<unsigned, optional<slot_point>> pdsch_scheduled_slot_in_future;

  const lcid_t lcid = LCID_MIN_DRB;

  // Vector to keep track of ACKs to send.
  std::vector<uci_indication> uci_ind_to_send;

  // Pre-populate common UE creation request parameters.
  const auto& cell_cfg_params = create_custom_cell_cfg_builder_params(params.duplx_mode);
  auto        ue_creation_req = test_helpers::create_default_sched_ue_creation_request(cell_cfg_params);

  auto it = std::find_if(ue_creation_req.cfg.lc_config_list->begin(),
                         ue_creation_req.cfg.lc_config_list->end(),
                         [](const auto& l) { return l.lcid == lcid; });
  if (it == ue_creation_req.cfg.lc_config_list->end()) {
    ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    it = ue_creation_req.cfg.lc_config_list->end() - 1;
  }
  it->lc_group = static_cast<lcg_id_t>(0);

  // Set DCI format to 1_1 in SearchSpace#2.
  ue_creation_req.cfg.cells->begin()
      ->serv_cell_cfg.init_dl_bwp.pdcch_cfg.value()
      .search_spaces.back()
      .set_non_ss0_monitored_dci_formats(srsran::search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);

  // Setup scheduler and add UEs.
  setup_sched(create_expert_config(10), create_custom_cell_config_request(params.duplx_mode));
  // Add UE(s) and notify to each UE a DL buffer status indication of random size between min and max defined in params.
  // Assumption: LCID is DRB1.
  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    // Initialize.
    is_bsr_zero_sent[idx] = false;

    ue_creation_req.ue_index = to_du_ue_index(idx);
    ue_creation_req.crnti    = to_rnti(allocate_rnti());

    add_ue(ue_creation_req);

    push_buffer_state_to_dl_ue(
        to_du_ue_index(idx),
        test_rgen::uniform_int<unsigned>(params.min_buffer_size_in_bytes, params.max_buffer_size_in_bytes),
        LCID_MIN_DRB);
  }

  for (unsigned i = 0; i != params.nof_ues * test_bench::max_test_run_slots_per_ue * (1U << current_slot.numerology());
       ++i) {
    run_slot();

    std::vector<uci_indication> uci_ind_not_for_current_slot;
    // Send ACKs if there are any to send.
    for (const auto& ind : uci_ind_to_send) {
      if (current_slot == ind.slot_rx) {
        bench->sch.handle_uci_indication(ind);
      } else {
        uci_ind_not_for_current_slot.push_back(ind);
      }
    }
    swap(uci_ind_to_send, uci_ind_not_for_current_slot);

    for (unsigned idx = 0; idx < params.nof_ues; idx++) {
      sched_test_ue& test_ue = get_ue(to_du_ue_index(idx));

      // Check whether DCI formats match the SearchSpace configuration and uses DCI format 1_1 for SS#2.
      const auto* pdcch_grant = find_ue_dl_pdcch(test_ue);
      if (pdcch_grant != nullptr) {
        const auto& ss_cfg = get_ss_cfg(test_ue, pdcch_grant->ctx.context.ss_id);
        ASSERT_TRUE(ss_cfg.has_value()) << fmt::format(
            "Condition failed for UE with c-rnti={} and SS id={}", test_ue.crnti, pdcch_grant->ctx.context.ss_id);
        if (ss_cfg->is_common_search_space()) {
          // Checking for only TC-RNTI and C-RNTI F1_0.
          ASSERT_TRUE(pdcch_grant->dci.type == srsran::dci_dl_rnti_config_type::c_rnti_f1_0 or
                      pdcch_grant->dci.type == srsran::dci_dl_rnti_config_type::tc_rnti_f1_0)
              << fmt::format("Condition failed for UE with c-rnti={} and SS id={}",
                             test_ue.crnti,
                             pdcch_grant->ctx.context.ss_id);
        } else {
          const auto dci_fmt =
              variant_get<search_space_configuration::ue_specific_dci_format>(ss_cfg->get_monitored_dci_formats());
          if (dci_fmt == srsran::search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0) {
            ASSERT_TRUE(pdcch_grant->dci.type == srsran::dci_dl_rnti_config_type::c_rnti_f1_0) << fmt::format(
                "Condition failed for UE with c-rnti={} and SS id={}", test_ue.crnti, pdcch_grant->ctx.context.ss_id);
          } else {
            ASSERT_TRUE(pdcch_grant->dci.type == srsran::dci_dl_rnti_config_type::c_rnti_f1_1) << fmt::format(
                "Condition failed for UE with c-rnti={} and SS id={}", test_ue.crnti, pdcch_grant->ctx.context.ss_id);
          }
        }
      }

      // Perform task related to sending ACK back for scheduled PDSCH.
      const auto& pdsch_slot = get_pdsch_scheduled_slot(test_ue);
      if (pdsch_slot.has_value()) {
        pdsch_scheduled_slot_in_future[idx] = pdsch_slot;
      }
      const auto& ack_nack_slot = get_pdsch_ack_nack_scheduled_slot(test_ue);
      if (ack_nack_slot.has_value()) {
        uci_ind_to_send.push_back(build_harq_ack_pucch_f0_f1_uci_ind(to_du_ue_index(idx), ack_nack_slot.value()));
      }

      // Wait until last PDSCH scheduled slot has passed to send Buffer status 0.
      const auto* grant = find_ue_pdsch(test_ue);
      if (is_bsr_zero_sent[idx] && pdsch_scheduled_slot_in_future[idx].has_value() &&
          current_slot > pdsch_scheduled_slot_in_future[idx].value()) {
        ASSERT_TRUE(grant == nullptr or not grant->pdsch_cfg.codewords[0].new_data)
            << fmt::format("Condition failed for UE with c-rnti={}", test_ue.crnti);
        continue;
      }

      // Send Buffer status 0 upon receiving grants for all requested bytes.
      const unsigned tbs_sched_bytes = pdsch_tbs_scheduled_bytes_per_lc(test_ue, lcid);
      if (tbs_sched_bytes == 0 && test_ue.dl_bsr_list.at(lcid).bs == 0 &&
          pdsch_scheduled_slot_in_future[idx].has_value() &&
          current_slot > pdsch_scheduled_slot_in_future[idx].value() && not is_bsr_zero_sent[idx]) {
        is_bsr_zero_sent[idx] = true;
        // Notify buffer status of 0 to ensure scheduler does not schedule further for this UE.
        push_buffer_state_to_dl_ue(to_du_ue_index(idx), 0, lcid);
      }

      // Update Buffer status maintained in test UE.
      if (grant != nullptr && grant->pdsch_cfg.codewords[0].new_data) {
        if (tbs_sched_bytes > test_ue.dl_bsr_list.at(lcid).bs) {
          // Accounting for MAC headers.
          test_ue.dl_bsr_list.at(lcid).bs = 0;
        } else {
          test_ue.dl_bsr_list.at(lcid).bs -= tbs_sched_bytes;
        }
      }
    }
  }
}

TEST_P(multiple_ue_sched_tester, ul_dci_format_0_1_test)
{
  // Used to track BSR 0.
  std::map<unsigned, bool>                 is_bsr_zero_sent;
  std::map<unsigned, optional<slot_point>> pusch_scheduled_slot_in_future;

  const lcg_id_t lcgid = uint_to_lcg_id(0);

  // Pre-populate common UE creation request parameters.
  const auto& cell_cfg_params = create_custom_cell_cfg_builder_params(params.duplx_mode);
  auto        ue_creation_req = test_helpers::create_default_sched_ue_creation_request(cell_cfg_params);

  auto it = std::find_if(ue_creation_req.cfg.lc_config_list->begin(),
                         ue_creation_req.cfg.lc_config_list->end(),
                         [](const auto& l) { return l.lcid == LCID_MIN_DRB; });
  if (it == ue_creation_req.cfg.lc_config_list->end()) {
    ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(LCID_MIN_DRB));
    it = ue_creation_req.cfg.lc_config_list->end() - 1;
  }
  it->lc_group = static_cast<lcg_id_t>(0);

  // Set DCI format to 1_1 in SearchSpace#2.
  ue_creation_req.cfg.cells->begin()
      ->serv_cell_cfg.init_dl_bwp.pdcch_cfg.value()
      .search_spaces.back()
      .set_non_ss0_monitored_dci_formats(srsran::search_space_configuration::ue_specific_dci_format::f0_1_and_1_1);

  // Setup scheduler and add UEs.
  setup_sched(create_expert_config(10), create_custom_cell_config_request(params.duplx_mode));
  // Add UE(s) and notify UL BSR from UE of random size between min and max defined in params.
  // Assumption: LCID is DRB1.
  for (unsigned idx = 0; idx < params.nof_ues; idx++) {
    // Initialize.
    is_bsr_zero_sent[idx]               = false;
    pusch_scheduled_slot_in_future[idx] = {};

    ue_creation_req.ue_index = to_du_ue_index(idx);
    ue_creation_req.crnti    = to_rnti(allocate_rnti());

    add_ue(ue_creation_req);

    notify_ul_bsr_from_ue(
        to_du_ue_index(idx),
        test_rgen::uniform_int<unsigned>(params.min_buffer_size_in_bytes, params.max_buffer_size_in_bytes),
        static_cast<lcg_id_t>(0));
  }

  for (unsigned i = 0; i != params.nof_ues * test_bench::max_test_run_slots_per_ue * (1U << current_slot.numerology());
       ++i) {
    run_slot();

    // CRC Indication to send if there PUSCH is scheduled in current slot.
    ul_crc_indication crc_ind{};
    crc_ind.cell_index = to_du_cell_index(0);
    crc_ind.sl_rx      = current_slot;

    for (unsigned idx = 0; idx < params.nof_ues; idx++) {
      auto& test_ue = get_ue(to_du_ue_index(idx));

      // Check whether DCI formats match the SearchSpace configuration and uses DCI format 1_1 for SS#2.
      const auto* pdcch_grant = find_ue_ul_pdcch(test_ue);
      if (pdcch_grant != nullptr) {
        const auto& ss_cfg = get_ss_cfg(test_ue, pdcch_grant->ctx.context.ss_id);
        ASSERT_TRUE(ss_cfg.has_value()) << fmt::format(
            "Condition failed for UE with c-rnti={} and SS id={}", test_ue.crnti, pdcch_grant->ctx.context.ss_id);
        if (ss_cfg->is_common_search_space()) {
          // Checking for only TC-RNTI and C-RNTI F1_0.
          ASSERT_TRUE(pdcch_grant->dci.type == srsran::dci_ul_rnti_config_type::c_rnti_f0_0 or
                      pdcch_grant->dci.type == srsran::dci_ul_rnti_config_type::tc_rnti_f0_0)
              << fmt::format("Condition failed for UE with c-rnti={} and SS id={}",
                             test_ue.crnti,
                             pdcch_grant->ctx.context.ss_id);
        } else {
          const auto dci_fmt =
              variant_get<search_space_configuration::ue_specific_dci_format>(ss_cfg->get_monitored_dci_formats());
          if (dci_fmt == srsran::search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0) {
            ASSERT_TRUE(pdcch_grant->dci.type == srsran::dci_ul_rnti_config_type::c_rnti_f0_0) << fmt::format(
                "Condition failed for UE with c-rnti={} and SS id={}", test_ue.crnti, pdcch_grant->ctx.context.ss_id);
          } else {
            ASSERT_TRUE(pdcch_grant->dci.type == srsran::dci_ul_rnti_config_type::c_rnti_f0_1) << fmt::format(
                "Condition failed for UE with c-rnti={} and SS id={}", test_ue.crnti, pdcch_grant->ctx.context.ss_id);
          }
        }
      }

      // Update the PUSCH scheduled slot scheduled in the future i.e. based on info got from PDCCH.
      const auto& pusch_slot = get_pusch_scheduled_slot(test_ue);
      if (pusch_slot.has_value()) {
        pusch_scheduled_slot_in_future[idx] = pusch_slot;
      }

      // Wait until last PUSCH scheduled slot has passed to send BSR 0.
      const auto* pusch = find_ue_pusch(test_ue);
      if (is_bsr_zero_sent[idx] && pusch_scheduled_slot_in_future[idx].has_value() &&
          current_slot > pusch_scheduled_slot_in_future[idx].value()) {
        ASSERT_TRUE(pusch == nullptr or not pusch->pusch_cfg.new_data)
            << fmt::format("Condition failed for UE with CRNTI=0x{:x}", test_ue.crnti);
        continue;
      }

      // Send BSR 0 upon receiving grants for all requested bytes.
      const unsigned tbs_sched_bytes = pusch_tbs_scheduled_bytes(test_ue);
      if (tbs_sched_bytes == 0 && test_ue.ul_bsr_list.at(lcgid).nof_bytes == 0 &&
          pusch_scheduled_slot_in_future[idx].has_value() &&
          current_slot > pusch_scheduled_slot_in_future[idx].value() && not is_bsr_zero_sent[idx]) {
        is_bsr_zero_sent[idx] = true;
        // Notify BSR 0 to ensure scheduler does not schedule further for this UE.
        notify_ul_bsr_from_ue(to_du_ue_index(idx), 0, static_cast<lcg_id_t>(0));
      }

      // Update Buffer status maintained in test UE.
      if (pusch != nullptr && pusch->pusch_cfg.new_data) {
        crc_ind.crcs.push_back(build_success_crc_pdu_indication(to_du_ue_index(idx), pusch->pusch_cfg.harq_id));
        if (tbs_sched_bytes > test_ue.ul_bsr_list.at(lcgid).nof_bytes) {
          // Accounting for MAC headers.
          test_ue.ul_bsr_list.at(lcgid).nof_bytes = 0;
        } else {
          test_ue.ul_bsr_list.at(lcgid).nof_bytes -= tbs_sched_bytes;
        }
      }
    }

    if (not crc_ind.crcs.empty()) {
      bench->sch.handle_crc_indication(crc_ind);
    }
  }
}

class single_ue_sched_tester : public scheduler_impl_tester, public ::testing::Test
{};

TEST_F(single_ue_sched_tester, successfully_schedule_srb0_retransmission_fdd)
{
  setup_sched(create_expert_config(6), create_custom_cell_config_request(duplex_mode::FDD));

  // Keep track of ACKs to send.
  optional<uci_indication> uci_ind_to_send;

  // Add UE(s) and notify UL BSR + DL Buffer status with 110 value.
  // Assumption: LCID is SRB0.
  add_ue(to_du_ue_index(0), LCID_SRB0, static_cast<lcg_id_t>(0), duplex_mode::FDD);

  // Enqueue ConRes CE.
  bench->sch.handle_dl_mac_ce_indication(dl_mac_ce_indication{to_du_ue_index(0), lcid_dl_sch_t::UE_CON_RES_ID});

  // Notify about SRB0 message in DL of size 101 bytes.
  const unsigned mac_srb0_sdu_size = 101;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size, LCID_SRB0);

  bool successfully_schedule_srb0_retransmission = false;

  for (unsigned i = 0; i != test_bench::max_test_run_slots_per_ue * get_nof_slots_per_subframe(current_slot.scs());
       ++i) {
    run_slot();

    auto&       test_ue = get_ue(to_du_ue_index(0));
    const auto* grant   = find_ue_pdsch(test_ue);
    // Re-transmission scenario.
    if (grant != nullptr && grant->context.nof_retxs > 0) {
      // Must be Typ1-PDCCH CSS.
      // See 3GPP TS 38.213, clause 10.1,
      // A UE monitors PDCCH candidates in one or more of the following search spaces sets
      //  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
      //    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
      ASSERT_EQ(grant->pdsch_cfg.ss_set_type, search_space_set_type::type1);
      successfully_schedule_srb0_retransmission = true;
      break;
    }

    if (uci_ind_to_send.has_value() and current_slot == uci_ind_to_send.value().slot_rx) {
      bench->sch.handle_uci_indication(uci_ind_to_send.value());
      uci_ind_to_send.reset();
    }

    const auto& ack_nack_slot = get_pdsch_ack_nack_scheduled_slot(test_ue);
    if (ack_nack_slot.has_value()) {
      uci_ind_to_send.emplace(build_harq_nack_pucch_f0_f1_uci_ind(to_du_ue_index(0), ack_nack_slot.value()));
    }
  }
  ASSERT_TRUE(successfully_schedule_srb0_retransmission) << fmt::format("SRB0 retransmission is not scheduled");
}

TEST_F(single_ue_sched_tester, srb0_retransmission_not_scheduled_if_csi_rs_is_present_fdd)
{
  // Keep track of ACKs to send.
  optional<uci_indication> uci_ind_to_send;

  setup_sched(create_expert_config(10), create_custom_cell_config_request(srsran::duplex_mode::FDD));
  // Add UE.
  add_ue(to_du_ue_index(0), LCID_SRB0, static_cast<lcg_id_t>(0), srsran::duplex_mode::FDD);

  if (not bench->cell_cfg.nzp_csi_rs_list.empty()) {
    const unsigned csi_rs_periodicity =
        bench->cell_cfg.nzp_csi_rs_list[0].csi_res_period.has_value()
            ? csi_resource_periodicity_to_uint(*bench->cell_cfg.nzp_csi_rs_list[0].csi_res_period)
            : 0;
    const unsigned csi_rs_slot_offset = bench->cell_cfg.nzp_csi_rs_list[0].csi_res_offset.has_value()
                                            ? *bench->cell_cfg.nzp_csi_rs_list[0].csi_res_offset
                                            : 0;
    const unsigned min_k1             = (*get_ue(to_du_ue_index(0)).msg.cfg.cells)[0]
                                .serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->dl_data_to_ul_ack[0];
    auto& test_ue = get_ue(to_du_ue_index(0));
    // Flag to keep track of multiplexing status of SRB0 retransmission PDSCH and CSI-RS.
    bool is_csi_muplxed_with_srb0_retx_pdsch = false;
    // CSI-RS periodicity in slot + CSI-RS slot offset - Min. K1 in list - 1 (slot in which to schedule SRB0 new tx) - 1
    // (UL HARQ processing delay).
    const unsigned srb0_new_tx_to_csi_rs_slot_delay = csi_rs_periodicity + csi_rs_slot_offset - min_k1 - 2;

    for (unsigned i = 0; i != csi_rs_periodicity * 3; ++i) {
      // Run until the next CSI-RS period minus min_k1 slot.
      if (i < srb0_new_tx_to_csi_rs_slot_delay) {
        run_slot();
        continue;
      }
      // Push DL buffer status indication of 100 bytes srb0_new_tx_to_csi_rs_slot_delay slots before CSI-RS so that SRB0
      // retransmission falls in slot with CSI-RS.
      if (i == srb0_new_tx_to_csi_rs_slot_delay) {
        push_buffer_state_to_dl_ue(to_du_ue_index(0), 100, LCID_SRB0);
      }
      run_slot();

      const auto* grant = find_ue_pdsch(test_ue);
      // Re-transmission scenario.
      if (grant != nullptr and grant->context.nof_retxs > 0 and (not bench->sched_res->dl.csi_rs.empty())) {
        is_csi_muplxed_with_srb0_retx_pdsch = true;
      }

      if (uci_ind_to_send.has_value() and current_slot == uci_ind_to_send.value().slot_rx) {
        bench->sch.handle_uci_indication(uci_ind_to_send.value());
        uci_ind_to_send.reset();
      }

      const auto& ack_nack_slot = get_pdsch_ack_nack_scheduled_slot(test_ue);
      if (ack_nack_slot.has_value()) {
        uci_ind_to_send.emplace(build_harq_nack_pucch_f0_f1_uci_ind(to_du_ue_index(0), ack_nack_slot.value()));
      }
    }

    ASSERT_FALSE(is_csi_muplxed_with_srb0_retx_pdsch)
        << fmt::format("CSI-RS being multiplexed with SRB0 retransmission");
  }
}

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
// See https://github.com/google/googletest/issues/3805.
void PrintTo(const multiple_ue_test_params&, ::std::ostream*) {}

INSTANTIATE_TEST_SUITE_P(multiple_ue_sched_tester,
                         multiple_ue_sched_tester,
                         testing::Values(multiple_ue_test_params{.nof_ues                  = 3,
                                                                 .min_buffer_size_in_bytes = 1000,
                                                                 .max_buffer_size_in_bytes = 3000,
                                                                 .duplx_mode               = duplex_mode::FDD},
                                         multiple_ue_test_params{.nof_ues                  = 3,
                                                                 .min_buffer_size_in_bytes = 2000,
                                                                 .max_buffer_size_in_bytes = 3000,
                                                                 .duplx_mode               = duplex_mode::TDD},
                                         multiple_ue_test_params{.nof_ues                  = 2,
                                                                 .min_buffer_size_in_bytes = 1000,
                                                                 .max_buffer_size_in_bytes = 3000,
                                                                 .duplx_mode               = duplex_mode::TDD}),
                         [](const testing::TestParamInfo<multiple_ue_sched_tester::ParamType>& params) -> std::string {
                           const auto& p = params.param;
                           return fmt::format("nof_ues_{}_buffer_size_{}_{}_mode_{}",
                                              p.nof_ues,
                                              p.min_buffer_size_in_bytes,
                                              p.max_buffer_size_in_bytes,
                                              to_string(p.duplx_mode));
                         });

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
