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

#include "lib/scheduler/common_scheduling/csi_rs_scheduler.h"
#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/logging/scheduler_metrics_handler.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/support/csi_rs_helpers.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "lib/scheduler/ue_scheduling/ue_fallback_scheduler.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include "srsran/support/test_utils.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <utility>

using namespace srsran;

static cell_config_builder_params test_builder_params(duplex_mode duplx_mode)
{
  cell_config_builder_params builder_params{};
  if (duplx_mode == duplex_mode::TDD) {
    // Band 40.
    builder_params.dl_f_ref_arfcn = 474000;
    builder_params.scs_common     = srsran::subcarrier_spacing::kHz30;
    builder_params.band           = band_helper::get_band_from_dl_arfcn(builder_params.dl_f_ref_arfcn);
    builder_params.channel_bw_mhz = bs_channel_bandwidth::MHz20;

    const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
        builder_params.channel_bw_mhz,
        builder_params.scs_common,
        builder_params.band.has_value() ? band_helper::get_freq_range(builder_params.band.value())
                                        : frequency_range::FR1);

    std::optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
        band_helper::get_ssb_coreset0_freq_location(builder_params.dl_f_ref_arfcn,
                                                    *builder_params.band,
                                                    nof_crbs,
                                                    builder_params.scs_common,
                                                    builder_params.scs_common,
                                                    builder_params.search_space0_index,
                                                    builder_params.max_coreset0_duration);
    builder_params.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
    builder_params.k_ssb             = ssb_freq_loc->k_ssb;
    builder_params.coreset0_index    = ssb_freq_loc->coreset0_idx;
  } else {
    builder_params.band = band_helper::get_band_from_dl_arfcn(builder_params.dl_f_ref_arfcn);
  }

  return builder_params;
}

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  // Maximum number of slots to run per UE in order to validate the results of scheduler. Implementation defined.
  static constexpr unsigned max_test_run_slots_per_ue = 40;

  const scheduler_expert_config           sched_cfg;
  const scheduler_ue_expert_config&       expert_cfg{sched_cfg.ue};
  sched_cfg_dummy_notifier                dummy_notif;
  scheduler_ue_metrics_dummy_notifier     metrics_notif;
  scheduler_ue_metrics_dummy_configurator metrics_ue_handler;
  cell_config_builder_params              builder_params;
  scheduler_metrics_handler               metrics;

  sched_config_manager      cfg_mng{scheduler_config{sched_cfg, dummy_notif}, metrics};
  const cell_configuration& cell_cfg;

  cell_resource_allocator       res_grid{cell_cfg};
  cell_harq_manager             cell_harqs{MAX_NOF_DU_UES,
                               MAX_NOF_HARQS,
                               std::make_unique<scheduler_harq_timeout_dummy_notifier>()};
  pdcch_resource_allocator_impl pdcch_sch{cell_cfg};
  pucch_allocator_impl          pucch_alloc{cell_cfg, 31U, 32U};
  uci_allocator_impl            uci_alloc{pucch_alloc};
  ue_repository                 ue_db;
  ue_fallback_scheduler         fallback_sched;
  csi_rs_scheduler              csi_rs_sched;

  explicit test_bench(scheduler_expert_config                         sched_cfg_,
                      const cell_config_builder_params&               builder_params_,
                      const sched_cell_configuration_request_message& cell_req) :
    sched_cfg{std::move(sched_cfg_)},
    builder_params{builder_params_},
    cell_cfg{*[&]() { return cfg_mng.add_cell(cell_req); }()},
    fallback_sched(expert_cfg, cell_cfg, pdcch_sch, pucch_alloc, uci_alloc, ue_db),
    csi_rs_sched(cell_cfg)
  {
    srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
    srslog::init();
  }

  bool add_ue(const sched_ue_creation_request_message& create_req)
  {
    auto ev = cfg_mng.add_ue(create_req);
    if (not ev.valid()) {
      return false;
    }

    // Add UE to UE DB.
    auto u = std::make_unique<ue>(ue_creation_command{ev.next_config(), create_req.starts_in_fallback, cell_harqs});
    if (ue_db.contains(create_req.ue_index)) {
      // UE already exists.
      return false;
    }
    ue_db.add_ue(std::move(u));
    auto& ue = ue_db[create_req.ue_index];
    ue.get_pcell().set_fallback_state(true);
    ev.notify_completion();
    return true;
  }

  void set_conres_state(du_ue_index_t ue_index, bool state)
  {
    if (not ue_db.contains(ue_index)) {
      // UE already exists.
      return;
    }

    ue_db[ue_index].get_pcell().set_conres_state(state);
  }
};

class base_fallback_tester
{
protected:
  slot_point                 current_slot{0, 0};
  srslog::basic_logger&      mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger&      test_logger = srslog::fetch_basic_logger("TEST", true);
  scheduler_result_logger    result_logger{false, 0};
  std::optional<test_bench>  bench;
  duplex_mode                duplx_mode;
  bool                       enable_pusch_transform_precoding;
  cell_config_builder_params builder_params;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned max_k_value = 0;

  base_fallback_tester(duplex_mode duplx_mode_, bool enable_pusch_transform_precoding_) :
    duplx_mode(duplx_mode_),
    enable_pusch_transform_precoding(enable_pusch_transform_precoding_),
    builder_params(test_builder_params(duplx_mode))
  {
  }

  ~base_fallback_tester()
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void setup_sched(const scheduler_expert_config& sched_cfg, const sched_cell_configuration_request_message& msg)
  {
    current_slot = slot_point{to_numerology_value(msg.scs_common), 0};

    bench.emplace(sched_cfg, builder_params, msg);

    const auto& dl_lst = bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (const auto& pdsch : dl_lst) {
      max_k_value = std::max<unsigned int>(pdsch.k0, max_k_value);
    }
    const auto& ul_lst = bench->cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      max_k_value = std::max(pusch.k2, max_k_value);
    }

    mac_logger.set_context(current_slot.sfn(), current_slot.slot_index());
    test_logger.set_context(current_slot.sfn(), current_slot.slot_index());

    bench->res_grid.slot_indication(current_slot);
    bench->pdcch_sch.slot_indication(current_slot);
    bench->pucch_alloc.slot_indication(current_slot);
  }

  void run_slot()
  {
    ++current_slot;

    mac_logger.set_context(current_slot.sfn(), current_slot.slot_index());
    test_logger.set_context(current_slot.sfn(), current_slot.slot_index());

    bench->res_grid.slot_indication(current_slot);
    bench->pdcch_sch.slot_indication(current_slot);
    bench->pucch_alloc.slot_indication(current_slot);

    bench->csi_rs_sched.run_slot(bench->res_grid[0]);

    bench->ue_db.slot_indication(current_slot);

    bench->fallback_sched.run_slot(bench->res_grid);

    result_logger.on_scheduler_result(bench->res_grid[0].result);

    // Check sched result consistency.
    test_scheduler_result_consistency(bench->cell_cfg, bench->res_grid);
  }

  static scheduler_expert_config create_expert_config(sch_mcs_index max_msg4_mcs_index)
  {
    scheduler_expert_config     cfg   = config_helpers::make_default_scheduler_expert_config();
    scheduler_ue_expert_config& uecfg = cfg.ue;
    uecfg.dl_mcs                      = {10, 10};
    uecfg.ul_mcs                      = {10, 10};
    uecfg.max_msg4_mcs                = max_msg4_mcs_index;
    return cfg;
  }

  sched_cell_configuration_request_message
  create_custom_cell_config_request(unsigned                                      k0,
                                    const std::optional<tdd_ul_dl_config_common>& tdd_cfg                   = {},
                                    bool                                          add_extra_pdcch_candidate = false)
  {
    if (duplx_mode == duplex_mode::TDD and tdd_cfg.has_value()) {
      builder_params.tdd_ul_dl_cfg_common = *tdd_cfg;
    }
    sched_cell_configuration_request_message msg =
        sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0 = k0;

    if (add_extra_pdcch_candidate) {
      srsran_assert(msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces.size() > 1U,
                    "This test assumes that the cell configuration has at least 2 search spaces");
      msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].set_non_ss0_nof_candidates({0, 0, 2, 0, 0});
    }
    return msg;
  }

  bool ue_is_allocated_pdcch(const ue& u)
  {
    return std::any_of(bench->res_grid[0].result.dl.dl_pdcchs.begin(),
                       bench->res_grid[0].result.dl.dl_pdcchs.end(),
                       [&u](const pdcch_dl_information& pdcch) { return pdcch.ctx.rnti == u.crnti; });
  }

  const pdcch_dl_information* get_ue_allocated_pdcch(const ue& u)
  {
    const auto* it = std::find_if(bench->res_grid[0].result.dl.dl_pdcchs.begin(),
                                  bench->res_grid[0].result.dl.dl_pdcchs.end(),
                                  [&u](const pdcch_dl_information& pdcch) { return pdcch.ctx.rnti == u.crnti; });
    return it == bench->res_grid[0].result.dl.dl_pdcchs.end() ? nullptr : it;
  }

  bool ue_is_allocated_pdsch(const ue& u)
  {
    return std::any_of(bench->res_grid[0].result.dl.ue_grants.begin(),
                       bench->res_grid[0].result.dl.ue_grants.end(),
                       [&u](const dl_msg_alloc& grant) { return grant.pdsch_cfg.rnti == u.crnti; });
  }

  bool ue_is_allocated_pusch(const ue& u)
  {
    return std::any_of(bench->res_grid[0].result.ul.puschs.begin(),
                       bench->res_grid[0].result.ul.puschs.end(),
                       [&u](const ul_sched_info& grant) { return grant.pusch_cfg.rnti == u.crnti; });
  }

  const dl_msg_alloc* get_ue_allocated_pdsch(const ue& u)
  {
    const auto* it = std::find_if(bench->res_grid[0].result.dl.ue_grants.begin(),
                                  bench->res_grid[0].result.dl.ue_grants.end(),
                                  [&u](const dl_msg_alloc& grant) { return grant.pdsch_cfg.rnti == u.crnti; });

    return it == bench->res_grid[0].result.dl.ue_grants.end() ? nullptr : it;
  }

  bool ue_is_allocated_pucch(const ue& u)
  {
    return std::any_of(bench->res_grid[0].result.ul.pucchs.begin(),
                       bench->res_grid[0].result.ul.pucchs.end(),
                       [&u](const auto& pucch) { return pucch.crnti == u.crnti; });
  }

  bool tbs_scheduled_bytes_matches_given_size(const ue& u, unsigned exp_size)
  {
    unsigned total_cw_tb_size_bytes = 0;

    // Fetch PDSCH resource grid allocators.
    const cell_slot_resource_allocator& pdsch_alloc = bench->res_grid[0];
    // Search for PDSCH UE grant.
    for (const auto& grant : pdsch_alloc.result.dl.ue_grants) {
      if (grant.pdsch_cfg.rnti != u.crnti) {
        continue;
      }
      for (const auto& cw : grant.pdsch_cfg.codewords) {
        total_cw_tb_size_bytes += cw.tb_size_bytes;
      }
    }
    return total_cw_tb_size_bytes >= exp_size;
  }

  bool add_ue(rnti_t tc_rnti, du_ue_index_t ue_index, bool remove_ded_cfg = false)
  {
    // Add cell to UE cell grid allocator.
    auto ue_create_req               = remove_ded_cfg
                                           ? sched_config_helper::create_empty_spcell_cfg_sched_ue_creation_request()
                                           : sched_config_helper::create_default_sched_ue_creation_request(bench->builder_params);
    ue_create_req.crnti              = tc_rnti;
    ue_create_req.ue_index           = ue_index;
    ue_create_req.starts_in_fallback = true;
    if (enable_pusch_transform_precoding) {
      ue_create_req.cfg.cells.value()[0].serv_cell_cfg.ul_config.value().init_ul_bwp.pusch_cfg.value().trans_precoder =
          pusch_config::transform_precoder::enabled;
    }
    return bench->add_ue(ue_create_req);
  }

  void push_buffer_state_to_dl_ue(du_ue_index_t ue_idx,
                                  slot_point    sl,
                                  unsigned      buffer_size,
                                  bool          is_srb0   = true,
                                  bool          tx_conres = true)
  {
    // Notification from upper layers of DL buffer state.
    bench->ue_db[ue_idx].handle_dl_buffer_state_indication(is_srb0 ? LCID_SRB0 : LCID_SRB1, buffer_size);
    if (tx_conres) {
      bench->ue_db[ue_idx].handle_dl_mac_ce_indication(dl_mac_ce_indication{ue_idx, lcid_dl_sch_t::UE_CON_RES_ID});
      bench->fallback_sched.handle_conres_indication(ue_idx);
      bench->set_conres_state(ue_idx, false);
    }

    // Notify scheduler of DL buffer state.
    bench->fallback_sched.handle_dl_buffer_state_indication(ue_idx);
  }

  void push_buffer_state_to_ul_ue(du_ue_index_t ue_idx, slot_point sl, unsigned buffer_size)
  {
    // Notification from upper layers of UL buffer status report.
    ul_bsr_indication_message msg{.cell_index = to_du_cell_index(0U),
                                  .ue_index   = ue_idx,
                                  .crnti      = to_rnti(0x4601 + static_cast<uint16_t>(ue_idx)),
                                  .type       = srsran::bsr_format::SHORT_BSR};

    msg.reported_lcgs.emplace_back(ul_bsr_lcg_report{.lcg_id = uint_to_lcg_id(0U), .nof_bytes = buffer_size});

    bench->ue_db[ue_idx].handle_bsr_indication(msg);

    // Notify scheduler of UL buffer status report.
    bench->fallback_sched.handle_ul_bsr_indication(ue_idx, msg);
  }

  void generate_sr_for_ue(du_ue_index_t ue_idx, slot_point sl)
  {
    // Notification from upper layers of UL .
    bench->ue_db[ue_idx].handle_sr_indication();

    // Notify scheduler of SR.
    bench->fallback_sched.handle_sr_indication(ue_idx);
  }

  unsigned get_srb0_pending_bytes(du_ue_index_t ue_idx)
  {
    return bench->ue_db[ue_idx].pending_dl_newtx_bytes(LCID_SRB0);
  }

  unsigned get_srb0_and_ce_pending_bytes(du_ue_index_t ue_idx)
  {
    return bench->ue_db[ue_idx].pending_dl_newtx_bytes(LCID_SRB0) + bench->ue_db[ue_idx].pending_ce_bytes();
  }

  ue& get_ue(du_ue_index_t ue_idx) { return bench->ue_db[ue_idx]; }
};

// Parameters to be passed to test.
struct srb0_test_params {
  uint8_t     k0;
  duplex_mode duplx_mode;
};

// Parameters to be passed to test.
struct fallback_sched_test_params {
  bool        is_srb0;
  duplex_mode duplx_mode;
};

class fallback_scheduler_tester : public base_fallback_tester, public ::testing::TestWithParam<srb0_test_params>
{
protected:
  fallback_scheduler_tester() : base_fallback_tester(GetParam().duplx_mode, false), params{GetParam()} {}

  srb0_test_params params;
};

TEST_P(fallback_scheduler_tester, successfully_allocated_resources)
{
  setup_sched(create_expert_config(2), create_custom_cell_config_request(params.k0));
  // Add UE.
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(to_rnti(0x4601), ue_idx);
  // Notify about SRB0 message in DL of size 101 bytes.
  const unsigned mac_srb0_sdu_size = 101;
  push_buffer_state_to_dl_ue(ue_idx, current_slot, mac_srb0_sdu_size, true);

  const unsigned exp_size = get_srb0_and_ce_pending_bytes(ue_idx);

  // Test the following:
  // 1. Check for DCI_1_0 allocation for SRB0 on PDCCH.
  // 2. Check for PDSCH allocation.
  // 3. Check whether CW TB bytes matches with pending bytes to be sent.
  const auto& test_ue = get_ue(ue_idx);
  bool        is_ue_allocated_pdcch{false};
  bool        is_ue_allocated_pdsch{false};
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();
    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    if (pdcch_it != nullptr) {
      is_ue_allocated_pdcch = true;
    }
    if (is_ue_allocated_pdcch) {
      const dl_msg_alloc* pdsch_it = get_ue_allocated_pdsch(test_ue);
      if (pdsch_it != nullptr) {
        for (const auto& lc_info : pdsch_it->tb_list.back().lc_chs_to_sched) {
          if (lc_info.lcid.is_sdu()) {
            is_ue_allocated_pdsch = true;
            ASSERT_TRUE(tbs_scheduled_bytes_matches_given_size(test_ue, exp_size));
            break;
          }
        }
      }
    }
  }
  ASSERT_TRUE(is_ue_allocated_pdcch);
  ASSERT_TRUE(is_ue_allocated_pdsch);
  ASSERT_FALSE(test_ue.has_pending_dl_newtx_bytes(LCID_SRB0));
}

TEST_P(fallback_scheduler_tester, successfully_allocated_resources_for_srb1_pdu_even_if_cqi_is_zero)
{
  setup_sched(create_expert_config(3), create_custom_cell_config_request(params.k0));
  // Add UE.
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(to_rnti(0x4601), ue_idx);
  auto& test_ue = get_ue(ue_idx);
  // UE reports CQI 0.
  csi_report_data csi_report{};
  csi_report.first_tb_wideband_cqi.emplace(0);
  csi_report.valid = true;
  test_ue.get_pcell().handle_csi_report(csi_report);
  // Notify about SRB1 message in DL of size 320 bytes.
  const unsigned mac_srb1_sdu_size = 320;
  push_buffer_state_to_dl_ue(ue_idx, current_slot, mac_srb1_sdu_size, false);

  bool is_ue_allocated_pdcch{false};
  bool is_ue_allocated_pdsch{false};
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();
    // If the UE is allocated a PUCCH, then assume it has acked the ConRes.
    if (std::any_of(bench->res_grid[0].result.ul.pucchs.begin(),
                    bench->res_grid[0].result.ul.pucchs.end(),
                    [&test_ue](const pucch_info& pucch) {
                      return pucch.crnti == test_ue.crnti and pucch.uci_bits.harq_ack_nof_bits != 0 and
                             pucch.format() == srsran::pucch_format::FORMAT_1;
                    })) {
      bench->set_conres_state(ue_idx, true);
    }

    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    if (pdcch_it != nullptr) {
      is_ue_allocated_pdcch = true;
    }
    if (is_ue_allocated_pdcch) {
      const dl_msg_alloc* pdsch_it = get_ue_allocated_pdsch(test_ue);
      if (pdsch_it != nullptr) {
        for (const auto& lc_info : pdsch_it->tb_list.back().lc_chs_to_sched) {
          if (lc_info.lcid.is_sdu() and lc_info.lcid == LCID_SRB1) {
            is_ue_allocated_pdsch = true;
            break;
          }
        }
      }
    }
  }
  ASSERT_TRUE(is_ue_allocated_pdcch);
  ASSERT_TRUE(is_ue_allocated_pdsch);
  ASSERT_FALSE(test_ue.has_pending_dl_newtx_bytes(LCID_SRB1));
}

TEST_P(fallback_scheduler_tester, failed_allocating_resources)
{
  setup_sched(create_expert_config(3), create_custom_cell_config_request(params.k0));

  // Add UE 1.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 101 bytes.
  unsigned ue1_mac_srb0_sdu_size = 99;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, ue1_mac_srb0_sdu_size, true);

  // Add UE 2.
  add_ue(to_rnti(0x4602), to_du_ue_index(1));
  // Notify about SRB0 message in DL of size 450 bytes. i.e. big enough to not get allocated with the max. mcs chosen.
  unsigned ue2_mac_srb0_sdu_size = 450;
  push_buffer_state_to_dl_ue(to_du_ue_index(1), current_slot, ue2_mac_srb0_sdu_size, true);

  run_slot();

  // Allocation for UE2 should fail.
  const auto& test_ue = get_ue(to_du_ue_index(1));
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();
    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    if (pdcch_it != nullptr) {
      const dl_msg_alloc* pdsch_it = get_ue_allocated_pdsch(test_ue);
      ASSERT_FALSE(pdsch_it != nullptr and pdsch_it->tb_list.back().lc_chs_to_sched.back().lcid.is_sdu());
    }
  }
}

TEST_P(fallback_scheduler_tester, when_conres_and_msg4_scheduled_separately_msg4_not_scheduled_until_conres_acked)
{
  setup_sched(create_expert_config(1), create_custom_cell_config_request(params.k0));

  // Add UE 1.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 99 bytes (this size is set empirically to force the scheduler to schedule
  // Conres and MSG4 separately).
  unsigned ue1_mac_srb0_sdu_size = 99;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, ue1_mac_srb0_sdu_size, true);

  // ConRes and Msg4 are scheduled separately.
  const auto&               test_ue = get_ue(to_du_ue_index(0));
  std::optional<slot_point> conres_pdcch;
  std::optional<slot_point> msg4_pdcch;
  unsigned                  sl_idx             = 0;
  const unsigned            max_test_run_slots = 10U * (1U << current_slot.numerology());
  for (; sl_idx != max_test_run_slots; ++sl_idx) {
    run_slot();

    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    if (pdcch_it != nullptr) {
      if (pdcch_it->dci.type == dci_dl_rnti_config_type::tc_rnti_f1_0) {
        conres_pdcch = current_slot;
      } else if (pdcch_it->dci.type == dci_dl_rnti_config_type::c_rnti_f1_0) {
        msg4_pdcch = current_slot;
      }
    }
  }

  ASSERT_TRUE(conres_pdcch.has_value());
  ASSERT_FALSE(msg4_pdcch.has_value());

  // Ack the ConRes to set the Contention Resolution complete.
  bench->set_conres_state(test_ue.ue_index, true);

  for (; sl_idx != max_test_run_slots * 2; ++sl_idx) {
    run_slot();

    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    if (pdcch_it != nullptr and pdcch_it->dci.type == dci_dl_rnti_config_type::c_rnti_f1_0) {
      msg4_pdcch = current_slot;
    }
  }
  ASSERT_TRUE(msg4_pdcch.has_value());
}

TEST_P(fallback_scheduler_tester, conres_and_msg4_scheduled_scheduled_over_different_slots_if_they_dont_fit_together)
{
  setup_sched(create_expert_config(1), create_custom_cell_config_request(params.k0));

  // Add UE 1.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 99 bytes (this size is set empirically to force the scheduler to schedule
  // Conres and MSG4 separately).
  unsigned ue1_mac_srb0_sdu_size = 99;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, ue1_mac_srb0_sdu_size, true);

  // ConRes and Msg4 are scheduled separately.
  const auto&               test_ue = get_ue(to_du_ue_index(0));
  std::optional<slot_point> conres_pdcch;
  std::optional<slot_point> msg4_pdcch;
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();

    // If PUCCH is allocated, set the Contention Resolution complete for the UE.
    bool con_res_pucch_allocated =
        std::any_of(bench->res_grid[0].result.ul.pucchs.begin(),
                    bench->res_grid[0].result.ul.pucchs.end(),
                    [&test_ue](const pucch_info& pucch) {
                      return pucch.crnti == test_ue.crnti and pucch.uci_bits.harq_ack_nof_bits == 1U;
                    });
    if (con_res_pucch_allocated) {
      // Set ConRes complete for the UE.
      bench->set_conres_state(test_ue.ue_index, true);
    }

    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    if (pdcch_it != nullptr) {
      if (pdcch_it->dci.type == dci_dl_rnti_config_type::tc_rnti_f1_0) {
        conres_pdcch = current_slot;
      } else if (pdcch_it->dci.type == dci_dl_rnti_config_type::c_rnti_f1_0) {
        msg4_pdcch = current_slot;
      }
    }
  }
  ASSERT_TRUE(conres_pdcch.has_value());
  ASSERT_TRUE(msg4_pdcch.has_value());
  ASSERT_TRUE(conres_pdcch != msg4_pdcch);
}

TEST_P(fallback_scheduler_tester, when_conres_and_msg4_srb1_scheduled_separately_msg4_not_scheduled_until_conres_acked)
{
  setup_sched(create_expert_config(1), create_custom_cell_config_request(params.k0));

  // Add UE 1.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 101 bytes.
  unsigned ue1_mac_srb1_sdu_size = 99;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, ue1_mac_srb1_sdu_size, false);

  // ConRes and Msg4 are scheduled separately.
  const auto&               test_ue = get_ue(to_du_ue_index(0));
  std::optional<slot_point> conres_pdcch;
  std::optional<slot_point> msg4_srb1_pdcch;
  unsigned                  sl_idx             = 0;
  const unsigned            max_test_run_slots = 10U * (1U << current_slot.numerology());
  for (; sl_idx != max_test_run_slots; ++sl_idx) {
    // Set DL grid at slot 1 (the first slot, where the PDSCH will be allocated) busy from RB 3 until the end of the bw;
    // this will force the scheduler to allocate Conres in isolation. Avoid the first symbols of teh slot to allow the
    // PDCCH to be allocated.
    static constexpr unsigned first_dl_allocable_slot = 1U;
    bench->res_grid[first_dl_allocable_slot].dl_res_grid.fill(
        grant_info(bench->cell_cfg.scs_common,
                   ofdm_symbol_range{3, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP},
                   crb_interval{3, bench->cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.stop()}));
    run_slot();

    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    if (pdcch_it != nullptr) {
      if (pdcch_it->dci.type == dci_dl_rnti_config_type::tc_rnti_f1_0) {
        conres_pdcch = current_slot;
      } else if (pdcch_it->dci.type == dci_dl_rnti_config_type::c_rnti_f1_0) {
        msg4_srb1_pdcch = current_slot;
      }
    }
  }

  ASSERT_TRUE(conres_pdcch.has_value());
  ASSERT_FALSE(msg4_srb1_pdcch.has_value());

  // Ack the ConRes to set the Contention Resolution complete.
  bench->set_conres_state(test_ue.ue_index, true);

  for (; sl_idx != max_test_run_slots * 2; ++sl_idx) {
    run_slot();

    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    if (pdcch_it != nullptr and pdcch_it->dci.type == dci_dl_rnti_config_type::c_rnti_f1_0) {
      msg4_srb1_pdcch = current_slot;
    }
  }
  ASSERT_TRUE(msg4_srb1_pdcch.has_value());
}

TEST_P(fallback_scheduler_tester, in_ul_and_dl_allocation_pucch_cannot_collide_with_pusch_for_same_ue)
{
  // Simulate the case in the fallback scheduler first schedules a PUSCH grant for a given slot, and then there is an
  // opportunity to schedule a PDSCH whose corresponding PUCCH falls in the same slot as the PUSCH. Verify that in this
  // case the PUCCH is allocated in the next available slot.

  // Add an extra PDCCH candidate for SS#1 to enable 2 PDCCHs (1 UL and 1 DL) to be allocated in the same slot.
  const bool add_extra_pdcch_candidate = true;
  setup_sched(create_expert_config(1), create_custom_cell_config_request(params.k0, {}, add_extra_pdcch_candidate));

  // Add UE 1.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));

  // Advance the slot count, so that the UL slot (corresponding to the PDCCH) for PUSCH (with k2=4) and PUCCH
  // (with k2=4) are on an UL slot (e.g. slot with idx 8) in both FDD and TDD.
  while (current_slot.slot_index() != 3U) {
    run_slot();
  }

  // Push both DL and UL buffer state for the UE.
  unsigned ue1_mac_srb1_sdu_size = 99;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, ue1_mac_srb1_sdu_size, false);
  push_buffer_state_to_ul_ue(to_du_ue_index(0), current_slot, ue1_mac_srb1_sdu_size);

  const auto&    test_ue            = get_ue(to_du_ue_index(0));
  bool           pdsch_allocated    = false;
  bool           pusch_allocated    = false;
  unsigned       sl_idx             = 0;
  const unsigned max_test_run_slots = 10U * (1U << current_slot.numerology());
  for (; sl_idx != max_test_run_slots; ++sl_idx) {
    run_slot();

    ASSERT_FALSE(ue_is_allocated_pusch(test_ue) and ue_is_allocated_pucch(test_ue))
        << fmt::format("PUSCH and PUCCH allocated at slot {} ", current_slot);

    pdsch_allocated |= ue_is_allocated_pdsch(test_ue);
    pusch_allocated |= ue_is_allocated_pusch(test_ue);
  }

  ASSERT_TRUE(pdsch_allocated);
  ASSERT_TRUE(pusch_allocated);
}

TEST_P(fallback_scheduler_tester, test_large_srb0_buffer_size)
{
  setup_sched(create_expert_config(27), create_custom_cell_config_request(params.k0));
  // Add UE.
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(to_rnti(0x4601), ue_idx);
  // Notify about SRB0 message in DL of size 458 bytes.
  const unsigned mac_srb0_sdu_size = 458;
  push_buffer_state_to_dl_ue(ue_idx, current_slot, mac_srb0_sdu_size, true);

  const unsigned exp_size = get_srb0_pending_bytes(ue_idx);

  const auto& test_ue = get_ue(ue_idx);
  bool        is_ue_allocated_pdcch{false};
  bool        is_ue_allocated_pdsch{false};
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();
    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    if (pdcch_it != nullptr) {
      is_ue_allocated_pdcch = true;
    }
    if (is_ue_allocated_pdcch) {
      const dl_msg_alloc* pdsch_it = get_ue_allocated_pdsch(test_ue);
      if (pdsch_it != nullptr) {
        for (const auto& lc_info : pdsch_it->tb_list.back().lc_chs_to_sched) {
          if (lc_info.lcid.is_sdu()) {
            is_ue_allocated_pdsch = true;
            ASSERT_TRUE(tbs_scheduled_bytes_matches_given_size(test_ue, exp_size));
            break;
          }
        }
      }
    }
  }
  ASSERT_TRUE(is_ue_allocated_pdcch);
  ASSERT_TRUE(is_ue_allocated_pdsch);

  ASSERT_FALSE(test_ue.has_pending_dl_newtx_bytes(LCID_SRB0));
}

TEST_P(fallback_scheduler_tester, test_srb0_buffer_size_exceeding_max_msg4_mcs_index)
{
  setup_sched(create_expert_config(3), create_custom_cell_config_request(params.k0));
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 360 bytes which requires MCS index > 3.
  const unsigned mac_srb0_sdu_size = 360;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, mac_srb0_sdu_size, true);

  // Allocation for UE should fail.
  const auto& test_ue = get_ue(to_du_ue_index(0));
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();
    const pdcch_dl_information* pdcch_it = get_ue_allocated_pdcch(test_ue);
    // ConRes CE is sent beforehand hence DCI 1_0 scrambled with C-RNTI is used to send Msg4.
    ASSERT_FALSE(pdcch_it != nullptr and pdcch_it->dci.type == srsran::dci_dl_rnti_config_type::c_rnti_f1_0);
    if (pdcch_it != nullptr) {
      const dl_msg_alloc* pdsch_it = get_ue_allocated_pdsch(test_ue);
      // ConRes CE is sent beforehand hence Msg4 should consist only SDU with no ConRes MAC CE.
      ASSERT_FALSE(pdsch_it != nullptr and pdsch_it->tb_list.back().lc_chs_to_sched.back().lcid.is_sdu());
    }
  }
}

TEST_P(fallback_scheduler_tester, sanity_check_with_random_max_mcs_and_payload_size)
{
  const sch_mcs_index max_msg4_mcs = test_rgen::uniform_int<unsigned>(0, 27);
  setup_sched(create_expert_config(max_msg4_mcs), create_custom_cell_config_request(params.k0));
  // Add UE.
  const du_ue_index_t ue_idx = to_du_ue_index(0);
  add_ue(to_rnti(0x4601), ue_idx);
  // Random payload size.
  const auto mac_srb0_sdu_size = test_rgen::uniform_int<unsigned>(1, 458);
  push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, mac_srb0_sdu_size, true);

  srslog::basic_logger& logger(srslog::fetch_basic_logger("TEST"));
  logger.info("SRB0 scheduler sanity test params PDU size ({}), max msg4 mcs ({}).", mac_srb0_sdu_size, max_msg4_mcs);

  run_slot();
}

class fallback_scheduler_tdd_tester : public base_fallback_tester, public ::testing::Test
{
protected:
  fallback_scheduler_tdd_tester() : base_fallback_tester(srsran::duplex_mode::TDD, false) {}
};

TEST_F(fallback_scheduler_tdd_tester, test_allocation_in_appropriate_slots_in_tdd)
{
  const unsigned      k0                 = 0;
  const sch_mcs_index max_msg4_mcs_index = 5;
  auto                cell_cfg           = create_custom_cell_config_request(k0);
  setup_sched(create_expert_config(max_msg4_mcs_index), cell_cfg);

  const unsigned MAX_UES            = 4;
  const unsigned MAX_TEST_RUN_SLOTS = 40;
  const unsigned MAC_SRB0_SDU_SIZE  = 129;

  // Add UEs.
  for (unsigned idx = 0; idx < MAX_UES; idx++) {
    add_ue(to_rnti(0x4601 + idx), to_du_ue_index(idx));
    // Notify about SRB0 message in DL.
    push_buffer_state_to_dl_ue(to_du_ue_index(idx), current_slot, MAC_SRB0_SDU_SIZE, true);
  }

  for (unsigned idx = 0; idx < MAX_UES * MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();
    if (not bench->cell_cfg.is_dl_enabled(current_slot)) {
      // Check whether PDCCH/PDSCH is not scheduled in UL slots for any of the UEs.
      for (unsigned ue_idx = 0; ue_idx < MAX_UES; ue_idx++) {
        const auto& test_ue = get_ue(to_du_ue_index(ue_idx));
        ASSERT_FALSE(ue_is_allocated_pdcch(test_ue));
        ASSERT_FALSE(ue_is_allocated_pdsch(test_ue));
      }
    }
    if (not bench->cell_cfg.is_ul_enabled(current_slot)) {
      // Check whether PUCCH HARQ is not scheduled in DL slots for any of the UEs.
      for (unsigned ue_idx = 0; ue_idx < MAX_UES; ue_idx++) {
        const auto& test_ue = get_ue(to_du_ue_index(ue_idx));
        ASSERT_FALSE(ue_is_allocated_pucch(test_ue));
      }
    }
  }

  for (unsigned ue_idx = 0; ue_idx < MAX_UES; ue_idx++) {
    const auto& test_ue = get_ue(to_du_ue_index(ue_idx));
    ASSERT_FALSE(test_ue.has_pending_dl_newtx_bytes(LCID_SRB0)) << "UE " << ue_idx << " has still pending DL bytes";
  }
}

TEST_F(fallback_scheduler_tdd_tester, test_allocation_in_partial_slots_tdd)
{
  const unsigned                k0                 = 0;
  const sch_mcs_index           max_msg4_mcs_index = 8;
  const tdd_ul_dl_config_common tdd_cfg{.ref_scs  = subcarrier_spacing::kHz30,
                                        .pattern1 = {.dl_ul_tx_period_nof_slots = 10,
                                                     .nof_dl_slots              = 7,
                                                     .nof_dl_symbols            = 8,
                                                     .nof_ul_slots              = 2,
                                                     .nof_ul_symbols            = 0}};
  // Disabled CSI-RS resources, as this test uses a TDD configuration that is not compatible with CSI-RS scheduling.
  this->builder_params.csi_rs_enabled               = false;
  sched_cell_configuration_request_message cell_cfg = create_custom_cell_config_request(k0, tdd_cfg);
  // Generate PDSCH Time domain allocation based on the partial slot TDD configuration.
  cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list =
      config_helpers::make_pdsch_time_domain_resource(cell_cfg.searchspace0,
                                                      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common,
                                                      std::nullopt,
                                                      cell_cfg.tdd_ul_dl_cfg_common);
  // Set minimum k1 according to TDD pattern.
  scheduler_expert_config expert_cfg = create_expert_config(max_msg4_mcs_index);
  expert_cfg.ue.min_k1               = 2;
  setup_sched(expert_cfg, cell_cfg);

  const unsigned MAX_TEST_RUN_SLOTS = 40;
  const unsigned MAC_SRB0_SDU_SIZE  = 129;

  // Add a single UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));

  for (unsigned idx = 0; idx < MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();
    // Notify about SRB0 message in DL 1 slot before partial slot in order for it to be scheduled in the next
    // (partial) slot.
    if (bench->cell_cfg.is_dl_enabled(current_slot + 1) and
        (not bench->cell_cfg.is_fully_dl_enabled(current_slot + 1))) {
      push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, MAC_SRB0_SDU_SIZE, true);
    }
    // Check SRB0 allocation in partial slot.
    if (bench->cell_cfg.is_dl_enabled(current_slot) and (not bench->cell_cfg.is_fully_dl_enabled(current_slot))) {
      const auto& test_ue = get_ue(to_du_ue_index(0));
      ASSERT_TRUE(ue_is_allocated_pdcch(test_ue));
      ASSERT_TRUE(ue_is_allocated_pdsch(test_ue));
      break;
    }
  }
}

INSTANTIATE_TEST_SUITE_P(fallback_scheduler,
                         fallback_scheduler_tester,
                         testing::Values(srb0_test_params{.k0 = 0, .duplx_mode = duplex_mode::FDD},
                                         srb0_test_params{.k0 = 0, .duplx_mode = duplex_mode::TDD}));

class fallback_scheduler_head_scheduling : public base_fallback_tester,
                                           public ::testing::TestWithParam<fallback_sched_test_params>
{
protected:
  const unsigned MAX_NOF_SLOTS_GRID_IS_BUSY = 4;
  const unsigned MAX_TEST_RUN_SLOTS         = 2100;
  // NOTE: Ensure that the SDU size is small enough so that there is no segmentation when tested for SRB1.
  const unsigned MAC_SRB_SDU_SIZE = 101;

  fallback_scheduler_head_scheduling() : base_fallback_tester(GetParam().duplx_mode, false)
  {
    const unsigned      k0                 = 0;
    const sch_mcs_index max_msg4_mcs_index = 8;
    auto                cell_cfg           = create_custom_cell_config_request(k0);
    setup_sched(create_expert_config(max_msg4_mcs_index), cell_cfg);
  }

  // Helper that generates the slot for the SRB0 buffer update.
  static unsigned generate_srb0_traffic_slot() { return test_rgen::uniform_int(20U, 30U); }

  // Helper that generates the number of slot during the scheduler res grid is fully used.
  unsigned generate_nof_slot_grid_occupancy() const
  {
    return test_rgen::uniform_int(1U, MAX_NOF_SLOTS_GRID_IS_BUSY + 1);
  }

  // Returns the next DL slot starting from the input slot.
  slot_point get_next_dl_slot(slot_point sl) const
  {
    while (not bench->cell_cfg.is_dl_enabled(sl)) {
      sl++;
    }
    return sl;
  }

  // Returns the next candidate slot at which the SRB0 scheduler is expected to allocate a grant.
  slot_point get_next_candidate_alloc_slot(slot_point sched_slot, unsigned nof_slot_grid_occupancy) const
  {
    if (nof_slot_grid_occupancy == 0) {
      return sched_slot;
    }

    unsigned occupy_grid_slot_cnt = 0;

    // The allocation must be on a DL slot.
    do {
      sched_slot++;
      if (bench->cell_cfg.is_dl_enabled(sched_slot)) {
        occupy_grid_slot_cnt++;
      }
    } while (occupy_grid_slot_cnt != nof_slot_grid_occupancy);

    auto k1_falls_on_ul = [&cfg = bench->cell_cfg](slot_point pdsch_slot) {
      static const std::array<uint8_t, 5> dci_1_0_k1_values = {4, 5, 6, 7};
      return std::any_of(dci_1_0_k1_values.begin(), dci_1_0_k1_values.end(), [&cfg, pdsch_slot](uint8_t k1) {
        return cfg.is_ul_enabled(pdsch_slot + k1);
      });
    };

    // Make sure the final slot for the SRB0/SRB1 PDSCH is such that the corresponding PUCCH falls on a UL slot.
    while ((not k1_falls_on_ul(sched_slot)) or (not bench->cell_cfg.is_dl_enabled(sched_slot)) or
           csi_helper::is_csi_rs_slot(bench->cell_cfg, sched_slot)) {
      sched_slot++;
    }

    return sched_slot;
  }

  void fill_resource_grid(slot_point sl, unsigned nof_slots, crb_interval crbs)
  {
    for (unsigned sl_inc = 0; sl_inc < nof_slots; ++sl_inc) {
      if (bench->cell_cfg.is_dl_enabled(sl + sl_inc)) {
        unsigned   nof_dl_symbols = bench->cell_cfg.get_nof_dl_symbol_per_slot(sl + sl_inc);
        grant_info grant{bench->cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs, {0, nof_dl_symbols}, crbs};
        bench->res_grid[sl + sl_inc].dl_res_grid.fill(grant);
      }
    }
  }
};

TEST_P(fallback_scheduler_head_scheduling, test_ahead_scheduling_for_srb_allocation_1_ue)
{
  // In this test, we check if the SRB0/SRB1 allocation can schedule ahead with respect to the reference slot, which is
  // given by the slot_indication in the scheduler. Every time we generate SRB0 buffer bytes, we set the resource grid
  // as occupied for a random number of slots. This forces the scheduler to try the allocation in next  slot(s).
  // The test assumes every HARQ process is acked positively.

  unsigned du_idx = 0;
  add_ue(to_rnti(0x4601 + du_idx), to_du_ue_index(du_idx));

  auto& test_ue = get_ue(to_du_ue_index(du_idx));

  // The slots at which we generate traffic and the number of slots the grid is occupied are generated randomly.
  slot_point slot_update_srb_traffic{current_slot.numerology(), generate_srb0_traffic_slot()};
  unsigned   nof_slots_grid_is_busy = generate_nof_slot_grid_occupancy();
  slot_point candidate_srb_slot     = get_next_dl_slot(slot_update_srb_traffic);
  slot_point check_alloc_slot       = get_next_candidate_alloc_slot(candidate_srb_slot, nof_slots_grid_is_busy);

  for (unsigned idx = 1; idx < MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();
    if (current_slot != check_alloc_slot) {
      ASSERT_FALSE(ue_is_allocated_pdcch(test_ue));
      ASSERT_FALSE(ue_is_allocated_pdsch(test_ue));
    } else {
      ASSERT_TRUE(ue_is_allocated_pdcch(test_ue))
          << fmt::format("Current slot={}, slot_update_srb_traffic={}, nof_slots_grid_is_busy={}, "
                         "candidate_srb_slot={}, check_alloc_slot={}",
                         current_slot,
                         slot_update_srb_traffic,
                         nof_slots_grid_is_busy,
                         candidate_srb_slot,
                         check_alloc_slot);
      ASSERT_TRUE(ue_is_allocated_pdsch(test_ue));

      slot_update_srb_traffic = current_slot + generate_srb0_traffic_slot();
      nof_slots_grid_is_busy  = generate_nof_slot_grid_occupancy();
      candidate_srb_slot      = get_next_dl_slot(slot_update_srb_traffic);
      check_alloc_slot        = get_next_candidate_alloc_slot(candidate_srb_slot, nof_slots_grid_is_busy);
    }

    // Allocate buffer and occupy the grid to test the scheduler in advance scheduling.
    if (current_slot == slot_update_srb_traffic) {
      push_buffer_state_to_dl_ue(to_du_ue_index(du_idx), current_slot, MAC_SRB_SDU_SIZE, GetParam().is_srb0);

      // Mark resource grid as occupied.
      fill_resource_grid(current_slot,
                         check_alloc_slot.to_uint() - current_slot.to_uint(),
                         bench->cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs);
    }

    // Ack the HARQ processes that are waiting for ACK, otherwise the scheduler runs out of empty HARQs.
    const unsigned                        bit_index_1_harq_only = 0U;
    std::optional<dl_harq_process_handle> dl_harq =
        test_ue.get_pcell().harqs.find_dl_harq_waiting_ack(current_slot, bit_index_1_harq_only);
    if (dl_harq.has_value()) {
      dl_harq->dl_ack_info(mac_harq_ack_report_status::ack, std::nullopt);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(fallback_scheduler,
                         fallback_scheduler_head_scheduling,
                         testing::Values(fallback_sched_test_params{.is_srb0 = true, .duplx_mode = duplex_mode::FDD},
                                         fallback_sched_test_params{.is_srb0 = true, .duplx_mode = duplex_mode::TDD},
                                         fallback_sched_test_params{.is_srb0 = false, .duplx_mode = duplex_mode::FDD},
                                         fallback_sched_test_params{.is_srb0 = false, .duplx_mode = duplex_mode::TDD}));

class fallback_scheduler_retx : public base_fallback_tester, public ::testing::TestWithParam<fallback_sched_test_params>
{
protected:
  fallback_scheduler_retx() : base_fallback_tester(GetParam().duplx_mode, false)
  {
    const unsigned      k0                 = 0;
    const sch_mcs_index max_msg4_mcs_index = 8;
    auto                cell_cfg           = create_custom_cell_config_request(k0);
    setup_sched(create_expert_config(max_msg4_mcs_index), cell_cfg);
    ues_testers.reserve(MAX_UES);
  }

  // Class that implements a state-machine for the UE, to keep track of acks and retransmissions.
  class ue_retx_tester
  {
  public:
    enum class ue_state { idle, waiting_for_tx, waiting_for_ack, waiting_for_retx, reset_harq };

    explicit ue_retx_tester(const cell_configuration& cell_cfg_, ue& test_ue_, fallback_scheduler_retx* parent_) :
      cell_cfg(cell_cfg_), test_ue(test_ue_), parent(parent_)
    {
      slot_update_srb_traffic = parent->current_slot + generate_srb1_next_update_delay();
      nof_packet_to_tx        = parent->SRB_PACKETS_TOT_TX;
    }

    void slot_indication(slot_point sl)
    {
      switch (state) {
          // Wait until the slot to update the SRB0/SRB1 traffic.
        case ue_state::idle: {
          if (sl == slot_update_srb_traffic and nof_packet_to_tx > 0) {
            // Notify about SRB0/SRB1 message in DL.
            parent->push_buffer_state_to_dl_ue(test_ue.ue_index, sl, parent->MAC_SRB0_SDU_SIZE, GetParam().is_srb0);
            state = ue_state::waiting_for_tx;
          }
          break;
        }
          // Wait until the UE transmits the PDSCH with SRB0/SRB1.
        case ue_state::waiting_for_tx: {
          for (harq_id_t h_id = to_harq_id(0); h_id != MAX_HARQ_ID;
               h_id           = to_harq_id(std::underlying_type_t<harq_id_t>(h_id) + 1)) {
            std::optional<dl_harq_process_handle> h_dl = test_ue.get_pcell().harqs.dl_harq(h_id);
            if (h_dl.has_value() and h_dl->is_waiting_ack()) {
              ongoing_h_id = h_id;
              break;
            }
          }

          if (ongoing_h_id != INVALID_HARQ_ID) {
            --nof_packet_to_tx;
            state = ue_state::waiting_for_ack;
          }
          break;
        }
          // Wait until the slot at which the ACK is expected and update successful and unsuccessful TX counters.
        case ue_state::waiting_for_ack: {
          std::optional<dl_harq_process_handle> h_dl = test_ue.get_pcell().harqs.dl_harq(ongoing_h_id);

          if (h_dl.has_value() and h_dl->uci_slot() == sl) {
            static constexpr double ack_probability = 0.5f;
            ack_outcome                             = test_rgen::bernoulli(ack_probability);
            if (ack_outcome) {
              ++successful_tx_cnt;
              state = ue_state::reset_harq;
            } else {
              if (h_dl->nof_retxs() != h_dl->max_nof_retxs()) {
                state = ue_state::waiting_for_retx;
              } else {
                ++unsuccessful_tx_cnt;
                state = ue_state::reset_harq;
              }
            }
          }
          break;
        }
          // Wait for UE to re-transmit the SRBO / SRB1 related PDSCH
        case ue_state::waiting_for_retx: {
          std::optional<dl_harq_process_handle> h_dl = test_ue.get_pcell().harqs.dl_harq(ongoing_h_id);
          if (not h_dl.has_value() or h_dl->empty()) {
            ++missed_srb_cnt;
            state = ue_state::reset_harq;
          } else if (h_dl->is_waiting_ack()) {
            state = ue_state::waiting_for_ack;
          }
          break;
        }
          // This state in necessary to set a delay in the reset of the HARQ-ID, which is needed by the
          // ack_harq_process() function.
        case ue_state::reset_harq: {
          // Notify about SRB0 message in DL.
          slot_update_srb_traffic = sl + generate_srb1_next_update_delay();
          ongoing_h_id            = INVALID_HARQ_ID;
          state                   = ue_state::idle;

          break;
        }
      }
    }

    void ack_harq_process(slot_point sl)
    {
      // Ack the HARQ processes that are waiting for ACK, otherwise the scheduler runs out of empty HARQs.
      const unsigned                        bit_index_1_harq_only = 0U;
      std::optional<dl_harq_process_handle> dl_harq =
          test_ue.get_pcell().harqs.find_dl_harq_waiting_ack(sl, bit_index_1_harq_only);
      if (dl_harq.has_value()) {
        srsran_assert(dl_harq->id() == ongoing_h_id, "HARQ process mismatch");
        dl_harq->dl_ack_info(ack_outcome ? mac_harq_ack_report_status::ack : mac_harq_ack_report_status::nack, {});
      }
    }

    static unsigned generate_srb1_next_update_delay()
    {
      // Generate a random number of slots to wait until the next SRB1 buffer update.
      return test_rgen::uniform_int(20U, 40U);
    }

    const cell_configuration& cell_cfg;
    ue&                       test_ue;
    ue_state                  state = ue_state::idle;
    slot_point                slot_update_srb_traffic;
    unsigned                  nof_packet_to_tx;
    harq_id_t                 ongoing_h_id = INVALID_HARQ_ID;
    fallback_scheduler_retx*  parent;
    bool                      ack_outcome = false;
    // Counter of TXs that terminate with ACK before reaching max_nof_harq_retxs.
    unsigned successful_tx_cnt = 0;
    // Counter of a TX that terminates with all NACKs until max_nof_harq_retxs is reached.
    unsigned unsuccessful_tx_cnt = 0;
    // Counter of a TX that require HARQ re-tx, but are not served by the scheduler.
    unsigned missed_srb_cnt = 0;
  };

  const unsigned SRB_PACKETS_TOT_TX = 20;
  const unsigned MAX_UES            = 1;
  const unsigned MAX_TEST_RUN_SLOTS = 2100;
  // NOTE: Ensure that the SDU size is small enough so that there is no segmentation when tested for SRB1.
  const unsigned MAC_SRB0_SDU_SIZE = 101;

  std::vector<ue_retx_tester> ues_testers;
};

TEST_P(fallback_scheduler_retx, test_scheduling_for_srb_retransmissions_multi_ue)
{
  // In this test, we check if the SRB0/SRB1 scheduler handles re-transmissions. Each time a SRB0/SRB1 buffer is updated
  // for a given UE, the GNB is expected to schedule a PDSCH grant, or multiple grants (for retransmissions) in case of
  // NACKs. Every time the SRB0/SRB1 buffer is updated, the scheduler is expected serve this SRB, which can result in a
  // successful transmission (at least 1 ACK before reaching max_nof_harq_retxs) or an unsuccessful tx (all NACKs until
  // max_nof_harq_retxs).
  // The test fails if: (i) the tot. number of successful transmissions plus unsuccessful transmissions is less
  // than the SRB0/SRB1 buffer updates; (ii) if any NACKed re-transmissions are left unserved (unless it reaches the
  // max_nof_harq_retxs).

  for (unsigned du_idx = 0; du_idx < MAX_UES; du_idx++) {
    add_ue(to_rnti(0x4601 + du_idx), to_du_ue_index(du_idx));
    ues_testers.emplace_back(bench->cell_cfg, get_ue(to_du_ue_index(du_idx)), this);
  }

  for (unsigned idx = 1; idx < MAX_UES * MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();

    for (auto& tester : ues_testers) {
      tester.slot_indication(current_slot);
      tester.ack_harq_process(current_slot);
    }
  }

  for (auto& tester : ues_testers) {
    ASSERT_EQ(0U, tester.missed_srb_cnt);
    ASSERT_EQ(SRB_PACKETS_TOT_TX, tester.successful_tx_cnt + tester.unsuccessful_tx_cnt);
  }
}

INSTANTIATE_TEST_SUITE_P(fallback_scheduler,
                         fallback_scheduler_retx,
                         testing::Values(fallback_sched_test_params{.is_srb0 = true, .duplx_mode = duplex_mode::FDD},
                                         fallback_sched_test_params{.is_srb0 = true, .duplx_mode = duplex_mode::TDD},
                                         fallback_sched_test_params{.is_srb0 = false, .duplx_mode = duplex_mode::FDD},
                                         fallback_sched_test_params{.is_srb0 = false, .duplx_mode = duplex_mode::TDD}));

class fallback_scheduler_srb1_segmentation : public base_fallback_tester, public ::testing::TestWithParam<duplex_mode>
{
protected:
  fallback_scheduler_srb1_segmentation() : base_fallback_tester(GetParam(), false)
  {
    const unsigned      k0                 = 0;
    const sch_mcs_index max_msg4_mcs_index = 8;
    auto                cell_cfg           = create_custom_cell_config_request(k0);
    setup_sched(create_expert_config(max_msg4_mcs_index), cell_cfg);
    ues_testers.reserve(MAX_UES);
  }

  // Class that implements a state-machine for the UE, to keep track of acks and retransmissions.
  class ue_retx_tester
  {
  public:
    explicit ue_retx_tester(const cell_configuration&             cell_cfg_,
                            ue&                                   test_ue_,
                            fallback_scheduler_srb1_segmentation* parent_) :
      cell_cfg(cell_cfg_), test_ue(test_ue_), parent(parent_)
    {
      slot_update_srb_traffic = parent->current_slot + generate_srb1_next_update_delay();
      nof_packet_to_tx        = parent->SRB_PACKETS_TOT_TX;
      test_logger.set_level(srslog::basic_levels::debug);

      latest_harq_states.reserve(MAX_NOF_HARQS);
      for (uint8_t h_id_idx = 0; h_id_idx != std::underlying_type_t<harq_id_t>(MAX_HARQ_ID); ++h_id_idx) {
        latest_harq_states.emplace_back(h_state::empty);
      }
    }

    void rlc_buffer_state_emulator(slot_point sl)
    {
      // Generate new traffic when at the proper slot.
      if (sl == slot_update_srb_traffic and nof_packet_to_tx > 0) {
        // Notify about SRB1 message in DL.
        pending_srb1_bytes = generate_srb1_buffer_size();
        parent->push_buffer_state_to_dl_ue(test_ue.ue_index, sl, pending_srb1_bytes, false, false);
        latest_rlc_update_slot.emplace(sl);
        --nof_packet_to_tx;
        test_logger.debug("rnti={}, slot={}: pushing SRB1 traffic of {} bytes", test_ue.crnti, sl, pending_srb1_bytes);
      }

      // When all pending bytes have been transmitted, generate the slot for the next traffic generation.
      if (sl > slot_update_srb_traffic and pending_srb1_bytes == 0 and nof_packet_to_tx > 0) {
        slot_update_srb_traffic = sl + generate_srb1_next_update_delay();
      }

      // Generate an RLC buffer update every time a HARQ process gets transmitted for the first time.
      for (uint8_t h_id_idx = 0; h_id_idx != std::underlying_type_t<harq_id_t>(MAX_HARQ_ID); ++h_id_idx) {
        harq_id_t h_id = to_harq_id(h_id_idx);

        std::optional<dl_harq_process_handle> h_dl = test_ue.get_pcell().harqs.dl_harq(h_id);
        if (h_dl.has_value() and h_dl->is_waiting_ack() and h_dl->nof_retxs() == 0 and h_dl->pdsch_slot() == sl) {
          const unsigned tx_bytes = h_dl->get_grant_params().tbs_bytes > MAX_MAC_SDU_SUBHEADER_SIZE
                                        ? h_dl->get_grant_params().tbs_bytes - MAX_MAC_SDU_SUBHEADER_SIZE
                                        : 0U;
          if (pending_srb1_bytes > tx_bytes) {
            pending_srb1_bytes -= tx_bytes;
          } else {
            pending_srb1_bytes = 0U;
          }
          test_logger.debug("rnti={}, slot={}: RLC buffer state update for h_id={} with {} bytes",
                            test_ue.crnti,
                            sl,
                            fmt::underlying(to_harq_id(h_dl->id())),
                            pending_srb1_bytes);
          parent->push_buffer_state_to_dl_ue(test_ue.ue_index, sl, pending_srb1_bytes, false, false);
          latest_rlc_update_slot.emplace(sl);
        }
      }

      // Resend a new RLC buffer update if more than max_rlc_update_delay have passed since the latest RLC update. This
      // is because the RLC buffer emulator computes an estimate and might not be keeping correct track of the bytes
      // sent; it can happen that the scheduler completes the scheduling for SRB1, but the RLC buffer emulator still
      // thinks there are remaining bytes to be transmitted, and makes the test fail.
      // Sending a new RLC buffer update ensures that the RLC buffer emulator reaches the 0 byte count.
      if (latest_rlc_update_slot.has_value() and sl - latest_rlc_update_slot.value() > max_rlc_update_delay) {
        test_logger.debug("rnti={}, slot={}: refreshing RLC buffer state update with {} bytes",
                          test_ue.crnti,
                          sl,
                          pending_srb1_bytes);
        parent->push_buffer_state_to_dl_ue(test_ue.ue_index, sl, pending_srb1_bytes, false, false);
        latest_rlc_update_slot.emplace(sl);
      }
    }

    void slot_indication(slot_point sl)
    {
      for (uint8_t h_id_idx = 0; h_id_idx != std::underlying_type_t<harq_id_t>(MAX_HARQ_ID); ++h_id_idx) {
        harq_id_t h_id = to_harq_id(h_id_idx);

        std::optional<dl_harq_process_handle> h_dl = test_ue.get_pcell().harqs.dl_harq(h_id);
        if (h_dl.has_value() and h_dl->is_waiting_ack() and h_dl->nof_retxs() == 0 and h_dl->pdsch_slot() == sl) {
          const unsigned tx_bytes = h_dl->get_grant_params().tbs_bytes > MAX_MAC_SDU_SUBHEADER_SIZE
                                        ? h_dl->get_grant_params().tbs_bytes - MAX_MAC_SDU_SUBHEADER_SIZE
                                        : 0U;
          pending_srb1_bytes > tx_bytes ? pending_srb1_bytes -= tx_bytes : pending_srb1_bytes = 0U;
          test_logger.debug("rnti={}, slot={}: RLC buffer state update for h_id={} with {} bytes",
                            test_ue.crnti,
                            sl,
                            fmt::underlying(to_harq_id(h_dl->id())),
                            pending_srb1_bytes);
          parent->push_buffer_state_to_dl_ue(test_ue.ue_index, sl, pending_srb1_bytes, false, false);
        }

        // Check if any HARQ process with pending transmissions is re-set by the scheduler.
        if (latest_harq_states[h_id_idx] == h_state::pending_retx and
            not test_ue.get_pcell().harqs.dl_harq(h_id).has_value()) {
          ++missing_retx;
        }

        // Save HARQ process latest.
        if (not test_ue.get_pcell().harqs.dl_harq(h_id).has_value()) {
          latest_harq_states[h_id_idx] = h_state::empty;
        } else if (test_ue.get_pcell().harqs.dl_harq(h_id)->is_waiting_ack()) {
          latest_harq_states[h_id_idx] = h_state::waiting_ack;
        } else {
          latest_harq_states[h_id_idx] = h_state::pending_retx;
        }
      }

      // Update HARQ process.
      ack_harq_process(sl);
    }

    void ack_harq_process(slot_point sl)
    {
      // Ack the HARQ processes that are waiting for ACK, otherwise the scheduler runs out of empty HARQs.
      const unsigned                        bit_index_1_harq_only = 0U;
      std::optional<dl_harq_process_handle> dl_harq =
          test_ue.get_pcell().harqs.find_dl_harq_waiting_ack(sl, bit_index_1_harq_only);
      if (dl_harq.has_value()) {
        static constexpr double ack_probability = 0.5f;
        const bool              ack             = test_rgen::bernoulli(ack_probability);
        dl_harq->dl_ack_info(ack ? mac_harq_ack_report_status::ack : mac_harq_ack_report_status::nack, {});
        test_logger.debug("Slot={}, rnti={}: acking process h_id={} with {}",
                          sl,
                          test_ue.crnti,
                          fmt::underlying(to_harq_id(dl_harq->id())),
                          ack ? "ACK" : "NACK");
      }
    }

    unsigned generate_srb1_buffer_size() const { return test_rgen::uniform_int(128U, parent->MAX_MAC_SRB0_SDU_SIZE); }
    static unsigned generate_srb1_next_update_delay()
    {
      // Generate a random number of slots to wait until the next SRB1 buffer update.
      return test_rgen::uniform_int(20U, 40U);
    }

    const cell_configuration&             cell_cfg;
    ue&                                   test_ue;
    slot_point                            slot_update_srb_traffic;
    unsigned                              nof_packet_to_tx;
    fallback_scheduler_srb1_segmentation* parent;
    srslog::basic_logger&                 test_logger        = srslog::fetch_basic_logger("TEST");
    unsigned                              missing_retx       = 0;
    unsigned                              pending_srb1_bytes = 0;
    std::optional<slot_point>             latest_rlc_update_slot;
    const int                             max_rlc_update_delay = 40;

    using h_state = harq_utils::harq_state_t;
    std::vector<h_state> latest_harq_states;
  };

  const unsigned SRB_PACKETS_TOT_TX    = 10;
  const unsigned MAX_UES               = 10;
  const unsigned MAX_TEST_RUN_SLOTS    = 200;
  const unsigned MAX_MAC_SRB0_SDU_SIZE = 1600;

  std::vector<ue_retx_tester> ues_testers;
};

TEST_P(fallback_scheduler_srb1_segmentation, test_scheduling_srb1_segmentation)
{
  // In this test, we check if the SRB0/SRB1 scheduler handles segmentation for the SRB1. Each time a SRB1 buffer is
  // updated, the GNB is to sends one or more grants, until the UE's SRB1 buffer is emptied.
  // The test fails if: (i) at the end of the test, the SRB1 buffer is not empty; (ii) if any NACKed re-transmissions
  // are left unserved (unless it reaches the max_nof_harq_retxs).

  for (unsigned du_idx = 0; du_idx < MAX_UES; du_idx++) {
    add_ue(to_rnti(0x4601 + du_idx), to_du_ue_index(du_idx));
    // For this test, assumes the ConRes has been transmitted and acked.
    //    bench->set_conres_complete(to_du_ue_index(du_idx));
    ues_testers.emplace_back(bench->cell_cfg, get_ue(to_du_ue_index(du_idx)), this);
  }

  for (unsigned idx = 1; idx < MAX_UES * MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();

    for (auto& tester : ues_testers) {
      tester.rlc_buffer_state_emulator(current_slot);
      tester.slot_indication(current_slot);
    }
  }

  for (auto& tester : ues_testers) {
    ASSERT_EQ(0, tester.missing_retx);
    ASSERT_FALSE(tester.test_ue.has_pending_dl_newtx_bytes())
        << fmt::format("UE {} has still pending DL bytes", fmt::underlying(tester.test_ue.ue_index));
  }
}

INSTANTIATE_TEST_SUITE_P(fallback_scheduler,
                         fallback_scheduler_srb1_segmentation,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));

/////// UL Scheduling tests ///////

// Parameters to be passed to test.
struct ul_fallback_sched_test_params {
  duplex_mode duplx_mode;
  bool        enable_pusch_transform_precoding;
};

class ul_fallback_scheduler_tester : public base_fallback_tester,
                                     public ::testing::TestWithParam<ul_fallback_sched_test_params>
{
protected:
  ul_fallback_scheduler_tester() :
    base_fallback_tester(GetParam().duplx_mode, GetParam().enable_pusch_transform_precoding)
  {
    auto msg = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    msg.ul_cfg_common.init_ul_bwp.rach_cfg_common->msg3_transform_precoder = enable_pusch_transform_precoding;
    setup_sched(config_helpers::make_default_scheduler_expert_config(), msg);
  }

  class ue_ul_tester
  {
  public:
    explicit ue_ul_tester(const cell_configuration& cell_cfg_, ue& test_ue_, ul_fallback_scheduler_tester* parent_) :
      cell_cfg(cell_cfg_), test_ue(test_ue_), parent(parent_)
    {
      slot_generate_srb_traffic = slot_point{to_numerology_value(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs),
                                             test_rgen::uniform_int(20U, 40U)};
    }

    void slot_indication(slot_point sl)
    {
      if (sl == slot_generate_srb_traffic) {
        const unsigned srb_buffer = test_rgen::uniform_int(128U, parent->MAX_MAC_UL_SRB1_SDU_SIZE);
        parent->push_buffer_state_to_ul_ue(test_ue.ue_index, sl, srb_buffer);
        buffer_bytes            = srb_buffer;
        initied_with_ul_traffic = true;
        test_logger.info("rnti={}, slot={}: generating initial BSR indication {}", test_ue.crnti, sl, srb_buffer);
      }

      for (uint8_t h_id_idx = 0; h_id_idx != std::underlying_type_t<harq_id_t>(MAX_HARQ_ID); ++h_id_idx) {
        harq_id_t h_id = to_harq_id(h_id_idx);

        std::optional<ul_harq_process_handle> h_ul = test_ue.get_pcell().harqs.ul_harq(h_id);
        if (h_ul.has_value() and h_ul->is_waiting_ack() and h_ul->pusch_slot() == sl) {
          bool           ack             = ack_harq_process(sl, *h_ul);
          const unsigned delivered_bytes = ack ? h_ul->get_grant_params().tbs_bytes - 10U : 0U;
          buffer_bytes > delivered_bytes ? buffer_bytes -= delivered_bytes : buffer_bytes = 0U;
          test_logger.info(
              "rnti={}, slot={}: generating BSR indication with {} bytes", test_ue.crnti, sl, buffer_bytes);
          parent->push_buffer_state_to_ul_ue(test_ue.ue_index, sl, buffer_bytes);
        }
      }
    }

    bool ack_harq_process(slot_point sl, ul_harq_process_handle h_ul)
    {
      static constexpr double ack_probability = 0.5f;

      // NOTE: to simply the generation of SRB1 buffer, we only allow max 3 NACKs.
      const bool ack = h_ul.nof_retxs() <= 3U ? test_rgen::bernoulli(ack_probability) : true;
      h_ul.ul_crc_info(ack);
      test_logger.info("Slot={}, rnti={}: ACKing process h_id={} with {}",
                       sl,
                       test_ue.crnti,
                       fmt::underlying(to_harq_id(h_ul.id())),
                       ack ? "ACK" : "NACK");
      return ack;
    }

    const cell_configuration&     cell_cfg;
    ue&                           test_ue;
    ul_fallback_scheduler_tester* parent;
    unsigned                      buffer_bytes = 0;
    srslog::basic_logger&         test_logger  = srslog::fetch_basic_logger("TEST");
    slot_point                    slot_generate_srb_traffic;
    bool                          initied_with_ul_traffic = false;
  };

  ul_fallback_sched_test_params params;
  const unsigned                MAX_UES                  = 10;
  const unsigned                MAX_TEST_RUN_SLOTS       = 100;
  const unsigned                MAX_MAC_UL_SRB1_SDU_SIZE = 3000;

  std::vector<ue_ul_tester> ues_testers;
};

TEST_P(ul_fallback_scheduler_tester, all_ul_ue_are_served_and_buffer_gets_emptied)
{
  // This test verifies that all UEs get a BSR for UL SRB traffic, and by the end of the test, all UEs will get served
  // and their buffer will be left with 0 bytes.

  for (unsigned du_idx = 0; du_idx < MAX_UES; du_idx++) {
    add_ue(to_rnti(0x4601 + du_idx), to_du_ue_index(du_idx));
    ues_testers.emplace_back(bench->cell_cfg, get_ue(to_du_ue_index(du_idx)), this);
  }

  for (unsigned idx = 1, idx_end = MAX_UES * MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx != idx_end;
       ++idx) {
    run_slot();

    for (auto& tester : ues_testers) {
      tester.slot_indication(current_slot);
    }
  }

  for (auto& tester : ues_testers) {
    ASSERT_TRUE(tester.initied_with_ul_traffic)
        << fmt::format("No UL traffic generated for UE {}", fmt::underlying(tester.test_ue.ue_index));
    ASSERT_FALSE(tester.buffer_bytes > 0)
        << fmt::format("UE {} has still pending UL bytes", fmt::underlying(tester.test_ue.ue_index));
  }
}

INSTANTIATE_TEST_SUITE_P(
    test_fdd_and_tdd,
    ul_fallback_scheduler_tester,
    testing::Values(
        ul_fallback_sched_test_params{.duplx_mode = duplex_mode::FDD, .enable_pusch_transform_precoding = false},
        ul_fallback_sched_test_params{.duplx_mode = duplex_mode::TDD, .enable_pusch_transform_precoding = false},
        ul_fallback_sched_test_params{.duplx_mode = duplex_mode::FDD, .enable_pusch_transform_precoding = true},
        ul_fallback_sched_test_params{.duplx_mode = duplex_mode::TDD, .enable_pusch_transform_precoding = true}));

class ul_fallback_sched_tester_sr_indication : public base_fallback_tester,
                                               public ::testing::TestWithParam<ul_fallback_sched_test_params>
{
protected:
  ul_fallback_sched_tester_sr_indication() :
    base_fallback_tester(GetParam().duplx_mode, GetParam().enable_pusch_transform_precoding)
  {
    auto msg = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    msg.ul_cfg_common.init_ul_bwp.rach_cfg_common->msg3_transform_precoder = enable_pusch_transform_precoding;
    setup_sched(config_helpers::make_default_scheduler_expert_config(), msg);
    slot_generate_srb_traffic =
        slot_point{to_numerology_value(bench->cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs),
                   test_rgen::uniform_int(20U, 40U)};
  }

  ul_fallback_sched_test_params params;
  const unsigned                MAX_TEST_RUN_SLOTS = 100;
  slot_point                    slot_generate_srb_traffic;
};

TEST_P(ul_fallback_sched_tester_sr_indication, when_gnb_receives_sr_ind_ue_gets_scheduled)
{
  unsigned du_idx = 0;
  add_ue(to_rnti(0x4601 + du_idx), to_du_ue_index(du_idx));

  auto& ue = bench->ue_db[to_du_ue_index(du_idx)];

  // Generate SR for this UE.
  generate_sr_for_ue(to_du_ue_index(du_idx), slot_generate_srb_traffic);

  bool pusch_allocated = false;
  for (unsigned sl = 1; sl < MAX_TEST_RUN_SLOTS; ++sl) {
    run_slot();

    auto& puschs   = bench->res_grid[0].result.ul.puschs;
    auto* pusch_it = std::find_if(puschs.begin(), puschs.end(), [rnti = ue.crnti](const ul_sched_info& pusch) {
      return pusch.pusch_cfg.rnti == rnti;
    });
    if (pusch_it != puschs.end()) {
      pusch_allocated = true;
      break;
    }
  }

  ASSERT_TRUE(pusch_allocated);
}

INSTANTIATE_TEST_SUITE_P(
    test_fdd_and_tdd,
    ul_fallback_sched_tester_sr_indication,
    testing::Values(
        ul_fallback_sched_test_params{.duplx_mode = duplex_mode::FDD, .enable_pusch_transform_precoding = false},
        ul_fallback_sched_test_params{.duplx_mode = duplex_mode::TDD, .enable_pusch_transform_precoding = false},
        ul_fallback_sched_test_params{.duplx_mode = duplex_mode::FDD, .enable_pusch_transform_precoding = true},
        ul_fallback_sched_test_params{.duplx_mode = duplex_mode::TDD, .enable_pusch_transform_precoding = true}));

class fallback_sched_ue_w_out_pucch_cfg : public base_fallback_tester, public ::testing::Test
{
protected:
  fallback_sched_ue_w_out_pucch_cfg() : base_fallback_tester(duplex_mode::TDD, false)
  {
    const unsigned      k0                 = 0;
    const sch_mcs_index max_msg4_mcs_index = 8;
    auto                cell_cfg           = create_custom_cell_config_request(k0);
    setup_sched(create_expert_config(max_msg4_mcs_index), cell_cfg);
  }

  // Helper that generates the slot for the SRB0 buffer update.
  static unsigned generate_srb_traffic_slot() { return test_rgen::uniform_int(20U, 30U); }

  const unsigned MAC_SRB_SDU_SIZE   = 101;
  const unsigned MAX_TEST_RUN_SLOTS = 50;
};

TEST_F(fallback_sched_ue_w_out_pucch_cfg, when_srb0_is_retx_ed_only_pucch_common_is_scheduled)
{
  add_ue(to_rnti(0x4601), to_du_ue_index(0), true);
  auto& u = bench->ue_db[to_du_ue_index(0)];

  ASSERT_FALSE(u.get_pcell().cfg().init_bwp().ul_ded.has_value());

  slot_point slot_update_srb_traffic{current_slot.numerology(), generate_srb_traffic_slot()};

  // Check if the SRB0 gets transmitted at least once.
  bool srb_transmitted = false;
  for (unsigned idx = 1; idx < MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();

    // Allocate buffer for SRB0.
    if (current_slot == slot_update_srb_traffic) {
      push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, MAC_SRB_SDU_SIZE, true);
    }

    // If PUCCH is detected, then it must be 1 grant only (PUCCH common).
    auto& pucchs = bench->res_grid[0].result.ul.pucchs;
    if (not pucchs.empty()) {
      srb_transmitted        = true;
      const auto* pucch_srb0 = std::find_if(
          pucchs.begin(), pucchs.end(), [rnti = u.crnti](const pucch_info& pucch) { return pucch.crnti == rnti; });
      ASSERT_TRUE(pucch_srb0 != pucchs.end());
      ASSERT_TRUE(pucch_srb0->pdu_context.is_common);
      ASSERT_EQ(1, std::count_if(pucchs.begin(), pucchs.end(), [rnti = u.crnti](const pucch_info& pucch) {
                  return pucch.crnti == rnti;
                }));
    }

    // NACK the HARQ processes that are waiting for ACK to trigger a retransmissions.
    const unsigned                        bit_index_1_harq_only = 0U;
    std::optional<dl_harq_process_handle> dl_harq =
        u.get_pcell().harqs.find_dl_harq_waiting_ack(current_slot, bit_index_1_harq_only);
    if (dl_harq.has_value()) {
      dl_harq->dl_ack_info(mac_harq_ack_report_status::nack, {});
    }
  }

  ASSERT_TRUE(srb_transmitted);
}

TEST_F(fallback_sched_ue_w_out_pucch_cfg, when_reconf_is_after_reest_both_common_and_ded_pucch_are_scheduled)
{
  const auto rnti        = to_rnti(0x4601);
  const auto du_ue_index = to_du_ue_index(0);
  ASSERT_TRUE(add_ue(rnti, du_ue_index));
  auto& u = bench->ue_db[to_du_ue_index(0)];
  ASSERT_TRUE(u.get_pcell().cfg().init_bwp().ul_ded.has_value());

  // Signal a UE reconfiguration that happens after re-establishment.
  auto ue_cfg = sched_config_helper::create_default_sched_ue_creation_request(bench->builder_params);
  sched_ue_reconfiguration_message reconf_msg{
      .ue_index = du_ue_index, .crnti = rnti, .cfg = ue_cfg.cfg, .reestablished = true};
  auto ev = bench->cfg_mng.update_ue(reconf_msg);
  u.handle_reconfiguration_request(ue_reconf_command{.cfg = ev.next_config()}, true);

  slot_point slot_update_srb_traffic{current_slot.numerology(), generate_srb_traffic_slot()};

  // Check if the SRB1 gets transmitted at least once.
  bool srb_transmitted = false;
  for (unsigned idx = 1; idx < MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();

    // Allocate buffer for SRB1.
    if (current_slot == slot_update_srb_traffic) {
      push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, MAC_SRB_SDU_SIZE, false);
    }

    // If PUCCH is detected, then it must be 1 grant only (PUCCH common).
    auto& pucchs = bench->res_grid[0].result.ul.pucchs;
    if (not pucchs.empty()) {
      srb_transmitted = true;
      ASSERT_EQ(2, std::count_if(pucchs.begin(), pucchs.end(), [rnti = u.crnti](const pucch_info& pucch) {
                  return pucch.crnti == rnti;
                }));

      const auto* pucch_common = std::find_if(pucchs.begin(), pucchs.end(), [rnti = u.crnti](const pucch_info& pucch) {
        return pucch.crnti == rnti and pucch.pdu_context.is_common;
      });
      ASSERT_TRUE(pucch_common != pucchs.end());

      const auto* pucch_ded = std::find_if(pucchs.begin(), pucchs.end(), [rnti = u.crnti](const pucch_info& pucch) {
        return pucch.crnti == rnti and not pucch.pdu_context.is_common;
      });
      ASSERT_TRUE(pucch_ded != pucchs.end());
    }

    // NACK the HARQ processes that are waiting for ACK to trigger a retransmissions.
    const unsigned                        bit_index_1_harq_only = 0U;
    std::optional<dl_harq_process_handle> dl_harq =
        u.get_pcell().harqs.find_dl_harq_waiting_ack(current_slot, bit_index_1_harq_only);
    if (dl_harq.has_value()) {
      dl_harq->dl_ack_info(mac_harq_ack_report_status::nack, {});
    }
  }

  ASSERT_TRUE(srb_transmitted);
}

TEST_F(fallback_sched_ue_w_out_pucch_cfg, when_srb1_is_scheduled_with_crnti_both_common_and_ded_pucch_are_scheduled)
{
  const auto rnti        = to_rnti(0x4601);
  const auto du_ue_index = to_du_ue_index(0);
  ASSERT_TRUE(add_ue(rnti, du_ue_index));
  auto& u = bench->ue_db[to_du_ue_index(0)];
  ASSERT_TRUE(u.get_pcell().cfg().init_bwp().ul_ded.has_value());

  slot_point slot_update_srb_traffic{current_slot.numerology(), generate_srb_traffic_slot()};

  // Check if the SRB1 gets transmitted at least once.
  bool srb_transmitted = false;
  for (unsigned idx = 1; idx < MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();

    // Allocate buffer for SRB1.
    if (current_slot == slot_update_srb_traffic) {
      // Do not transmit CON_RES so that dci type is c_rnti_f1_0.
      push_buffer_state_to_dl_ue(to_du_ue_index(0), current_slot, MAC_SRB_SDU_SIZE, false, false);
    }

    // If PUCCH is detected, then it must be 1 grant only (PUCCH common).
    auto& pucchs = bench->res_grid[0].result.ul.pucchs;
    if (not pucchs.empty()) {
      srb_transmitted = true;
      ASSERT_EQ(2, std::count_if(pucchs.begin(), pucchs.end(), [rnti = u.crnti](const pucch_info& pucch) {
                  return pucch.crnti == rnti;
                }));

      const auto* pucch_common = std::find_if(pucchs.begin(), pucchs.end(), [rnti = u.crnti](const pucch_info& pucch) {
        return pucch.crnti == rnti and pucch.pdu_context.is_common;
      });
      ASSERT_TRUE(pucch_common != pucchs.end());

      const auto* pucch_ded = std::find_if(pucchs.begin(), pucchs.end(), [rnti = u.crnti](const pucch_info& pucch) {
        return pucch.crnti == rnti and not pucch.pdu_context.is_common;
      });
      ASSERT_TRUE(pucch_ded != pucchs.end());
    }

    // NACK the HARQ processes that are waiting for ACK to trigger a retransmissions.
    const unsigned                        bit_index_1_harq_only = 0U;
    std::optional<dl_harq_process_handle> dl_harq =
        u.get_pcell().harqs.find_dl_harq_waiting_ack(current_slot, bit_index_1_harq_only);
    if (dl_harq.has_value()) {
      dl_harq->dl_ack_info(mac_harq_ack_report_status::nack, {});
    }
  }

  ASSERT_TRUE(srb_transmitted);
}
