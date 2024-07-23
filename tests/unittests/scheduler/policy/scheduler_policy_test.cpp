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

#include "../test_utils/config_generators.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/policy/scheduler_policy_factory.h"
#include "lib/scheduler/policy/scheduler_time_rr.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_scheduling/ue.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

/// \brief Type of policy scheduler.
///
/// The current types are:
/// - time_rr - Time based Round-Robin scheduler.
/// - time_pf - Time based Proportional Fair scheduler.
enum class policy_scheduler_type { time_rr, time_pf };

class base_scheduler_policy_test
{
protected:
  base_scheduler_policy_test(
      policy_scheduler_type   policy,
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config(),
      const sched_cell_configuration_request_message& msg =
          test_helpers::make_default_sched_cell_configuration_request()) :
    logger(srslog::fetch_basic_logger("SCHED", true)),
    res_logger(false, msg.pci),
    sched_cfg([&sched_cfg_, policy]() {
      if (policy == policy_scheduler_type::time_pf) {
        sched_cfg_.ue.strategy_cfg = time_pf_scheduler_expert_config{};
      }
      return sched_cfg_;
    }()),
    cell_cfg(*[this, &msg]() {
      return cell_cfg_list.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(sched_cfg, msg)).get();
    }()),
    slice_instance(ran_slice_id_t{0}, cell_cfg, slice_rrm_policy_config{})
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    grid_alloc.add_cell(to_du_cell_index(0), pdcch_alloc, uci_alloc, res_grid);
    ue_res_grid.add_cell(res_grid);

    sched = create_scheduler_strategy(sched_cfg.ue);
    if (sched == nullptr) {
      report_fatal_error("Failed to initialize slice scheduler");
    }
  }

  ~base_scheduler_policy_test() { srslog::flush(); }

  void run_slot()
  {
    logger.set_context(next_slot.sfn(), next_slot.slot_index());

    grid_alloc.slot_indication(next_slot);
    slice_instance.slot_indication(ues);

    res_grid.slot_indication(next_slot);
    pdcch_alloc.slot_indication(next_slot);
    pucch_alloc.slot_indication(next_slot);
    uci_alloc.slot_indication(next_slot);

    if (cell_cfg.is_dl_enabled(next_slot)) {
      sched->dl_sched(slice_pdsch_alloc, ue_res_grid, dl_slice_candidate);
      sched->ul_sched(slice_pusch_alloc, ue_res_grid, ul_slice_candidate);
    }

    // Log scheduler results.
    res_logger.on_scheduler_result(res_grid[0].result);

    ++next_slot;
  }

  bool run_until(unique_function<bool()> condition, unsigned max_slot_count = 1000)
  {
    for (unsigned count = 0; count != max_slot_count; ++count) {
      if (condition()) {
        return true;
      }
      run_slot();
    }
    return false;
  }

  ue& add_ue(du_ue_index_t ue_index, const std::initializer_list<lcid_t>& lcids_to_activate)
  {
    sched_ue_creation_request_message ue_creation_req =
        make_ue_create_req(ue_index, to_rnti(0x4601 + (unsigned)ue_index), lcids_to_activate, uint_to_lcg_id(0));
    return add_ue(ue_creation_req);
  }

  ue& add_ue(const sched_ue_creation_request_message& ue_req)
  {
    ue_ded_cell_cfg_list.push_back(
        std::make_unique<ue_configuration>(ue_req.ue_index, ue_req.crnti, cell_cfg_list, ue_req.cfg));
    ues.add_ue(std::make_unique<ue>(
        ue_creation_command{*ue_ded_cell_cfg_list.back(), ue_req.starts_in_fallback, harq_timeout_handler}));
    for (const auto& lc_cfg : *ue_req.cfg.lc_config_list) {
      slice_instance.add_logical_channel(&ues[ue_req.ue_index], lc_cfg.lcid);
    }
    return ues[ue_req.ue_index];
  }

  sched_ue_creation_request_message make_ue_create_req(du_ue_index_t                        ue_index,
                                                       rnti_t                               rnti,
                                                       const std::initializer_list<lcid_t>& lcids_to_activate,
                                                       lcg_id_t                             lcg_id)
  {
    sched_ue_creation_request_message req = test_helpers::create_default_sched_ue_creation_request();
    req.ue_index                          = ue_index;
    req.crnti                             = rnti;
    auto default_lc_cfg                   = config_helpers::create_default_logical_channel_config(uint_to_lcid(0));
    default_lc_cfg.lc_group               = lcg_id;
    req.cfg.lc_config_list.emplace();
    for (lcid_t lcid : lcids_to_activate) {
      default_lc_cfg.lcid = lcid;
      req.cfg.lc_config_list->push_back(default_lc_cfg);
    }
    return req;
  }

  void push_dl_bs(du_ue_index_t ue_index, lcid_t lcid, unsigned bytes)
  {
    dl_buffer_state_indication_message ind{};
    ind.ue_index = ue_index;
    ind.lcid     = lcid;
    ind.bs       = bytes;
    ues[ue_index].handle_dl_buffer_state_indication(ind);
  }

  void notify_ul_bsr(du_ue_index_t ue_index, lcg_id_t lcg_id, unsigned bytes)
  {
    // Assumptions:
    // - Only one LCG is assumed to have data to send.
    // - BSR is Short BSR.
    ul_bsr_indication_message msg{
        to_du_cell_index(0), ue_index, to_rnti(0x4601 + (unsigned)ue_index), bsr_format::SHORT_BSR, {}};
    msg.reported_lcgs.push_back(ul_bsr_lcg_report{lcg_id, bytes});

    ues[ue_index].handle_bsr_indication(msg);
  }

  srslog::basic_logger&                          logger;
  scheduler_result_logger                        res_logger;
  scheduler_expert_config                        sched_cfg;
  cell_common_configuration_list                 cell_cfg_list;
  std::vector<std::unique_ptr<ue_configuration>> ue_ded_cell_cfg_list;

  const cell_configuration&            cell_cfg;
  sched_cfg_dummy_notifier             dummy_mac_notif;
  scheduler_ue_metrics_dummy_notifier  metrics_notif;
  scheduler_harq_timeout_dummy_handler harq_timeout_handler;

  cell_resource_allocator       res_grid{cell_cfg};
  pdcch_resource_allocator_impl pdcch_alloc{cell_cfg};
  pucch_allocator_impl   pucch_alloc{cell_cfg, sched_cfg.ue.max_pucchs_per_slot, sched_cfg.ue.max_ul_grants_per_slot};
  uci_allocator_impl     uci_alloc{pucch_alloc};
  ue_resource_grid_view  ue_res_grid;
  ue_repository          ues;
  ue_cell_grid_allocator grid_alloc{sched_cfg.ue, ues, logger};
  std::unique_ptr<scheduler_policy> sched;

  // Default RAN slice instance.
  ran_slice_instance              slice_instance;
  dl_ran_slice_candidate          dl_slice_candidate{slice_instance};
  ul_ran_slice_candidate          ul_slice_candidate{slice_instance};
  dl_slice_ue_cell_grid_allocator slice_pdsch_alloc{grid_alloc, dl_slice_candidate};
  ul_slice_ue_cell_grid_allocator slice_pusch_alloc{grid_alloc, ul_slice_candidate};

  slot_point next_slot{0, test_rgen::uniform_int<unsigned>(0, 10239)};
};

class scheduler_policy_test : public base_scheduler_policy_test, public ::testing::TestWithParam<policy_scheduler_type>
{
protected:
  scheduler_policy_test() : base_scheduler_policy_test(GetParam()) {}
};

TEST_P(scheduler_policy_test, when_coreset0_used_then_dl_grant_is_within_bounds_of_coreset0_rbs)
{
  auto  ue_req  = make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(4)}, uint_to_lcg_id(0));
  auto& ss_list = (*ue_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces;
  ss_list.clear();
  ss_list.push_back(config_helpers::make_default_common_search_space_config());
  ss_list.back().set_non_ss0_id(to_search_space_id(2));
  // Note: We use Aggregation Level 2 to avoid collisions with CORESET#0 PDCCH candidates.
  ss_list.back().set_non_ss0_nof_candidates({0, 2, 0, 0, 0});
  ue& u = add_ue(ue_req);
  // Note: set CQI=15 to use low aggregation level.
  u.get_pcell().handle_csi_report(
      csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{15U}});
  push_dl_bs(u.ue_index, uint_to_lcid(4), 100000000);

  run_slot();

  ASSERT_FALSE(this->res_grid[0].result.dl.ue_grants.empty());
  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants.back().context.ue_index, u.ue_index);
  crb_interval crbs         = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs();
  prb_interval alloced_prbs = prb_interval{this->res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().start(),
                                           this->res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().stop()};
  ASSERT_EQ(prb_to_crb(crbs, alloced_prbs), crbs);
}

TEST_P(scheduler_policy_test, scheduler_uses_only_specific_searchspaces_defined_in_dedicated_configuration)
{
  sched_ue_creation_request_message ue_req =
      make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(4)}, uint_to_lcg_id(1));

  const ue& u = add_ue(ue_req);
  push_dl_bs(u.ue_index, uint_to_lcid(4), 1053);
  notify_ul_bsr(u.ue_index, uint_to_lcg_id(1), 1053);

  run_slot();

  ASSERT_EQ(this->res_grid[0].result.dl.dl_pdcchs.size(), 1);
  ASSERT_EQ(this->res_grid[0].result.dl.ul_pdcchs.size(), 1);
  // SearchSpace#2 is the only SearchSpace defined in UE dedicated configuration.
  ASSERT_EQ(this->res_grid[0].result.dl.dl_pdcchs.back().ctx.context.ss_id, to_search_space_id(2));
  ASSERT_EQ(this->res_grid[0].result.dl.ul_pdcchs.back().ctx.context.ss_id, to_search_space_id(2));
}

TEST_P(scheduler_policy_test, scheduler_allocates_more_than_one_ue_in_case_their_bsr_is_low)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  const ue& u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));

  notify_ul_bsr(u1.ue_index, lcg_id, 10);
  notify_ul_bsr(u2.ue_index, lcg_id, 10);

  run_slot();

  ASSERT_EQ(this->res_grid[0].result.dl.ul_pdcchs.size(), 2);
  ASSERT_NE(this->res_grid[0].result.dl.ul_pdcchs[0].ctx.rnti, this->res_grid[0].result.dl.ul_pdcchs[1].ctx.rnti);
  // NOTE: Both UEs have same PUSCH time domain resources configured.
  span<const pusch_time_domain_resource_allocation> pusch_td_res_list =
      u1.get_pcell().cfg().cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
  // In default UE dedicated configuration, SearchSpace#2 is configured to use DCI format 1_1/0_1.
  if (this->res_grid[0].result.dl.ul_pdcchs[0].dci.c_rnti_f0_1.time_resource ==
      this->res_grid[0].result.dl.ul_pdcchs[1].dci.c_rnti_f0_1.time_resource) {
    const unsigned k2 = pusch_td_res_list[this->res_grid[0].result.dl.ul_pdcchs[0].dci.c_rnti_f0_1.time_resource].k2;
    const auto&    pusch_res_grid = this->res_grid[k2];
    ASSERT_EQ(pusch_res_grid.result.ul.puschs.size(), 2);
    ASSERT_FALSE(pusch_res_grid.result.ul.puschs[0].pusch_cfg.rbs.type1().overlaps(
        pusch_res_grid.result.ul.puschs[1].pusch_cfg.rbs.type1()));
  }
}

TEST_P(scheduler_policy_test, scheduler_allocates_more_than_one_ue_in_case_their_dl_buffer_state_is_low)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  const ue& u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));

  push_dl_bs(u1.ue_index, uint_to_lcid(5), 10);
  push_dl_bs(u2.ue_index, uint_to_lcid(5), 10);

  run_slot();

  ASSERT_EQ(this->res_grid[0].result.dl.dl_pdcchs.size(), 2);
  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants.size(), 2);
  ASSERT_NE(this->res_grid[0].result.dl.dl_pdcchs[0].ctx.rnti, this->res_grid[0].result.dl.dl_pdcchs[1].ctx.rnti);
  ASSERT_FALSE(this->res_grid[0].result.dl.ue_grants[0].pdsch_cfg.rbs.type1().overlaps(
      this->res_grid[0].result.dl.ue_grants[1].pdsch_cfg.rbs.type1()));
}

TEST_P(scheduler_policy_test, scheduler_allocates_ues_with_sr_opportunity_first_than_ues_with_only_ul_data)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  ue&       u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));

  notify_ul_bsr(u1.ue_index, lcg_id, 200);
  u2.handle_sr_indication();

  run_slot();

  ASSERT_GE(this->res_grid[0].result.dl.ul_pdcchs.size(), 1);
  ASSERT_EQ(this->res_grid[0].result.dl.ul_pdcchs.front().ctx.rnti, to_rnti(0x4602))
      << fmt::format("UE with SR opportunity should have been scheduled first.");
}

TEST_P(scheduler_policy_test, scheduler_allocates_ues_with_ul_retx_first_than_ues_with_newtx)
{
  const lcg_id_t lcg_id = uint_to_lcg_id(2);
  ue&            u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  ue&            u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));

  // Push high enough UL BSR such that grant occupies entire BWP CRBs.
  notify_ul_bsr(u1.ue_index, lcg_id, 20000);
  notify_ul_bsr(u2.ue_index, lcg_id, 20000);

  bool             pusch_scheduled = run_until([this]() { return not this->res_grid[0].result.ul.puschs.empty(); });
  const slot_point current_slot    = next_slot - 1;
  ASSERT_TRUE(pusch_scheduled);
  const ul_sched_info& pusch = this->res_grid[0].result.ul.puschs[0];
  // Send CRC=KO to trigger retransmission.
  const ul_crc_pdu_indication crc_pdu{
      pusch.pusch_cfg.rnti, pusch.context.ue_index, to_harq_id(pusch.pusch_cfg.harq_id), false};
  if (pusch.context.ue_index == u1.ue_index) {
    u1.get_pcell().handle_crc_pdu(current_slot, crc_pdu);
  } else if (pusch.context.ue_index == u2.ue_index) {
    u2.get_pcell().handle_crc_pdu(current_slot, crc_pdu);
  }
  const du_ue_index_t ue_with_retx = pusch.context.ue_index;

  pusch_scheduled = run_until([this]() { return not this->res_grid[0].result.ul.puschs.empty(); });
  ASSERT_TRUE(pusch_scheduled);
  ASSERT_EQ(this->res_grid[0].result.ul.puschs[0].context.ue_index, ue_with_retx);
}

TEST_P(scheduler_policy_test, scheduler_allocates_ues_with_ul_srb_newtx_first_than_ues_with_ul_drb_newtx)
{
  const lcg_id_t drb_lcg_id = uint_to_lcg_id(2);
  const lcg_id_t srb_lcg_id = uint_to_lcg_id(0);
  ue&            u1 = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {LCID_MIN_DRB}, drb_lcg_id));
  ue&            u2 = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {LCID_SRB1}, srb_lcg_id));

  notify_ul_bsr(u1.ue_index, drb_lcg_id, 1000);
  notify_ul_bsr(u2.ue_index, srb_lcg_id, 1000);

  bool pusch_scheduled = run_until([this]() { return not this->res_grid[0].result.ul.puschs.empty(); });
  ASSERT_TRUE(pusch_scheduled);
  // UE2 with SRB newTx data is scheduled first.
  ASSERT_EQ(this->res_grid[0].result.ul.puschs[0].context.ue_index, u2.ue_index);
}

TEST_P(scheduler_policy_test, scheduler_allocates_ues_with_dl_retx_first_than_ues_with_newtx)
{
  const lcg_id_t lcg_id = uint_to_lcg_id(2);
  ue&            u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  ue&            u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));

  // Push high enough DL buffer status such that grant occupies entire BWP CRBs.
  push_dl_bs(u1.ue_index, uint_to_lcid(5), 100000);
  push_dl_bs(u2.ue_index, uint_to_lcid(5), 100000);

  bool pdsch_scheduled = run_until([this]() { return not this->res_grid[0].result.dl.ue_grants.empty(); });
  ASSERT_TRUE(pdsch_scheduled);
  const bool pdsch_ack_scheduled = run_until([this]() { return not this->res_grid[0].result.ul.pucchs.empty(); });
  ASSERT_TRUE(pdsch_ack_scheduled);
  const slot_point  current_slot = next_slot - 1;
  const pucch_info& pucch        = this->res_grid[0].result.ul.pucchs[0];
  // Auto NACK HARQ.
  unsigned      nof_ack_bits = 0;
  du_ue_index_t ue_with_retx;
  switch (pucch.format) {
    case pucch_format::FORMAT_0:
      nof_ack_bits = pucch.format_0.harq_ack_nof_bits;
      break;
    case pucch_format::FORMAT_1:
      nof_ack_bits = pucch.format_1.harq_ack_nof_bits;
      break;
    case pucch_format::FORMAT_2:
      nof_ack_bits = pucch.format_2.harq_ack_nof_bits;
      break;
    default:
      break;
  }
  if (pucch.crnti == u1.crnti) {
    ue_with_retx = u1.ue_index;
    for (unsigned harq_bit_idx = 0; harq_bit_idx < nof_ack_bits; ++harq_bit_idx) {
      u1.get_pcell().handle_dl_ack_info(current_slot, mac_harq_ack_report_status::ack, harq_bit_idx, 100);
    }
  } else if (pucch.crnti == u2.crnti) {
    ue_with_retx = u2.ue_index;
    for (unsigned harq_bit_idx = 0; harq_bit_idx < nof_ack_bits; ++harq_bit_idx) {
      u2.get_pcell().handle_dl_ack_info(current_slot, mac_harq_ack_report_status::ack, harq_bit_idx, 100);
    }
  }

  pdsch_scheduled = run_until([this]() { return not this->res_grid[0].result.dl.ue_grants.empty(); });
  ASSERT_TRUE(pdsch_scheduled);
  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants[0].context.ue_index, ue_with_retx);
}

TEST_P(scheduler_policy_test, scheduler_allocates_ues_with_dl_srb_newtx_first_than_ues_with_dl_drb_newtx)
{
  const lcg_id_t drb_lcg_id = uint_to_lcg_id(2);
  const lcg_id_t srb_lcg_id = uint_to_lcg_id(0);
  ue&            u1 = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {LCID_MIN_DRB}, drb_lcg_id));
  ue&            u2 = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {LCID_SRB1}, srb_lcg_id));

  push_dl_bs(u1.ue_index, LCID_MIN_DRB, 1000);
  push_dl_bs(u2.ue_index, LCID_SRB1, 1000);

  bool pdsch_scheduled = run_until([this]() { return not this->res_grid[0].result.dl.ue_grants.empty(); });
  ASSERT_TRUE(pdsch_scheduled);
  // UE2 with SRB newTx data is scheduled first.
  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants[0].context.ue_index, u2.ue_index);
}

class scheduler_policy_partial_slot_tdd_test : public base_scheduler_policy_test,
                                               public ::testing::TestWithParam<policy_scheduler_type>
{
protected:
  scheduler_policy_partial_slot_tdd_test() :
    base_scheduler_policy_test(GetParam(), config_helpers::make_default_scheduler_expert_config(), []() {
      cell_config_builder_params builder_params{};
      // Band 40.
      builder_params.dl_arfcn       = 465000;
      builder_params.scs_common     = subcarrier_spacing::kHz30;
      builder_params.band           = band_helper::get_band_from_dl_arfcn(builder_params.dl_arfcn);
      builder_params.channel_bw_mhz = srsran::bs_channel_bandwidth_fr1::MHz20;

      const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
          builder_params.channel_bw_mhz,
          builder_params.scs_common,
          builder_params.band.has_value() ? band_helper::get_freq_range(builder_params.band.value())
                                          : frequency_range::FR1);

      std::optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
          band_helper::get_ssb_coreset0_freq_location(builder_params.dl_arfcn,
                                                      *builder_params.band,
                                                      nof_crbs,
                                                      builder_params.scs_common,
                                                      builder_params.scs_common,
                                                      builder_params.search_space0_index,
                                                      builder_params.max_coreset0_duration);
      srsran_assert(ssb_freq_loc.has_value(), "Invalid cell config parameters");
      builder_params.offset_to_point_a    = ssb_freq_loc->offset_to_point_A;
      builder_params.k_ssb                = ssb_freq_loc->k_ssb;
      builder_params.coreset0_index       = ssb_freq_loc->coreset0_idx;
      builder_params.tdd_ul_dl_cfg_common = tdd_ul_dl_config_common{.ref_scs  = subcarrier_spacing::kHz30,
                                                                    .pattern1 = {.dl_ul_tx_period_nof_slots = 10,
                                                                                 .nof_dl_slots              = 5,
                                                                                 .nof_dl_symbols            = 5,
                                                                                 .nof_ul_slots              = 4,
                                                                                 .nof_ul_symbols            = 0}};
      return test_helpers::make_default_sched_cell_configuration_request(builder_params);
    }())
  {
    next_slot = {to_numerology_value(subcarrier_spacing::kHz30), 0};
  }
};

TEST_P(scheduler_policy_partial_slot_tdd_test, scheduler_allocates_in_partial_slot_tdd)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));

  // Buffer has to be large enough so that the allocation is extended over partial slot.
  push_dl_bs(u1.ue_index, uint_to_lcid(5), 100000);

  // Run until one slot before partial slot.
  while (cell_cfg.is_fully_dl_enabled(next_slot) or cell_cfg.is_fully_ul_enabled(next_slot)) {
    run_slot();
  }
  // Run for partial slot.
  run_slot();

  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants.size(), 1);
}

class scheduler_round_robin_test : public base_scheduler_policy_test, public ::testing::Test
{
protected:
  scheduler_round_robin_test() : base_scheduler_policy_test(policy_scheduler_type::time_rr) {}
};

TEST_F(scheduler_round_robin_test, round_robin_does_not_account_ues_with_empty_buffers)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  const ue& u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));
  const ue& u3     = add_ue(make_ue_create_req(to_du_ue_index(2), to_rnti(0x4603), {uint_to_lcid(5)}, lcg_id));

  push_dl_bs(u1.ue_index, uint_to_lcid(5), 1000000);
  push_dl_bs(u3.ue_index, uint_to_lcid(5), 1000000);

  for (unsigned i = 0; i != 10; ++i) {
    const bool pdsch_scheduled = run_until([this]() { return not this->res_grid[0].result.dl.ue_grants.empty(); });
    ASSERT_TRUE(pdsch_scheduled);
    for (const auto& grant : this->res_grid[0].result.dl.ue_grants) {
      ASSERT_NE(grant.context.ue_index, u2.ue_index);
    }
    // Run once to move to next slot. If not /c (not this->res_grid[0].result.dl.ue_grants.empty()) will keep returning
    // true.
    run_slot();
  }
}

TEST_F(scheduler_round_robin_test, round_robin_must_not_attempt_to_allocate_twice_for_same_ue_in_one_slot)
{
  const lcg_id_t lcg_id = uint_to_lcg_id(2);
  ue&            u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));

  auto get_pdsch_grant_count_for_ue = [&](const rnti_t crnti) {
    return std::count_if(this->res_grid[0].result.dl.ue_grants.begin(),
                         this->res_grid[0].result.dl.ue_grants.end(),
                         [&](const dl_msg_alloc& grant) { return crnti == grant.pdsch_cfg.rnti; });
  };
  auto get_pusch_grant_count_for_ue = [&](const rnti_t crnti) {
    return std::count_if(this->res_grid[0].result.ul.puschs.begin(),
                         this->res_grid[0].result.ul.puschs.end(),
                         [&](const ul_sched_info& grant) { return crnti == grant.pusch_cfg.rnti; });
  };

  // Action: Push buffer status notification for DL + Ul and a SR indication.
  push_dl_bs(u1.ue_index, uint_to_lcid(5), 20000000);
  notify_ul_bsr(u1.ue_index, lcg_id, 2000000);
  u1.handle_sr_indication();

  // Action: Run for at least 256 slots or more so that there are some HARQs with pending reTx.
  // Status: Policy scheduler should not allocate reTx and new Tx for the same UE at the same time.
  for (unsigned i = 0; i != 512; ++i) {
    ASSERT_LE(get_pdsch_grant_count_for_ue(u1.crnti), 1) << fmt::format("At slot={}", next_slot);
    ASSERT_LE(get_pusch_grant_count_for_ue(u1.crnti), 1) << fmt::format("At slot={}", next_slot);
    run_slot();
  }
}

class scheduler_pf_test : public base_scheduler_policy_test, public ::testing::Test
{
protected:
  scheduler_pf_test() : base_scheduler_policy_test(policy_scheduler_type::time_pf) {}
};

TEST_F(scheduler_pf_test, pf_does_not_account_ues_with_empty_buffers)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  const ue& u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));
  const ue& u3     = add_ue(make_ue_create_req(to_du_ue_index(2), to_rnti(0x4603), {uint_to_lcid(5)}, lcg_id));

  push_dl_bs(u1.ue_index, uint_to_lcid(5), 1000000);
  notify_ul_bsr(u1.ue_index, lcg_id, 1000000);
  push_dl_bs(u3.ue_index, uint_to_lcid(5), 1000000);
  notify_ul_bsr(u3.ue_index, lcg_id, 1000000);

  std::set<du_ue_index_t> dl_scheduled_ues;
  std::set<du_ue_index_t> ul_scheduled_ues;
  for (unsigned i = 0; i != 10; ++i) {
    run_slot();
    for (const auto& grant : this->res_grid[0].result.dl.ue_grants) {
      dl_scheduled_ues.emplace(grant.context.ue_index);
      ASSERT_NE(grant.context.ue_index, u2.ue_index);
    }
    for (const auto& grant : this->res_grid[0].result.ul.puschs) {
      ul_scheduled_ues.emplace(grant.context.ue_index);
      ASSERT_NE(grant.context.ue_index, u2.ue_index);
    }
  }
  ASSERT_EQ(dl_scheduled_ues.size(), 2);
  ASSERT_EQ(ul_scheduled_ues.size(), 2);
}

TEST_F(scheduler_pf_test, pf_ensures_fairness_in_dl_when_ues_have_different_channel_conditions)
{
  const lcg_id_t lcg_id = uint_to_lcg_id(2);
  ue&            u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  ue&            u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));
  ue&            u3     = add_ue(make_ue_create_req(to_du_ue_index(2), to_rnti(0x4603), {uint_to_lcid(5)}, lcg_id));

  // Push high enough DL buffer status to ensure UEs occupy entire BWP CRBs when scheduled.
  push_dl_bs(u1.ue_index, uint_to_lcid(5), 1000000);
  push_dl_bs(u2.ue_index, uint_to_lcid(5), 1000000);
  push_dl_bs(u3.ue_index, uint_to_lcid(5), 1000000);

  std::unordered_map<du_ue_index_t, unsigned> ue_pdsch_scheduled_count;
  ue_pdsch_scheduled_count[u1.ue_index] = 0;
  ue_pdsch_scheduled_count[u2.ue_index] = 0;
  ue_pdsch_scheduled_count[u3.ue_index] = 0;

  for (unsigned i = 0; i != 100; ++i) {
    // Report different CQIs for different UEs.
    // Best channel condition.
    u1.get_pcell().handle_csi_report(
        csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{15U}});
    // Worst channel condition.
    u2.get_pcell().handle_csi_report(
        csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{10U}});
    u3.get_pcell().handle_csi_report(
        csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{12U}});

    run_slot();
    const slot_point current_slot = next_slot - 1;
    for (const auto& grant : this->res_grid[0].result.dl.ue_grants) {
      ++ue_pdsch_scheduled_count[grant.context.ue_index];
    }

    // Auto ACK HARQs.
    for (const pucch_info& pucch : this->res_grid[0].result.ul.pucchs) {
      unsigned nof_ack_bits = 0;
      switch (pucch.format) {
        case pucch_format::FORMAT_0:
          nof_ack_bits = pucch.format_0.harq_ack_nof_bits;
          break;
        case pucch_format::FORMAT_1:
          nof_ack_bits = pucch.format_1.harq_ack_nof_bits;
          break;
        case pucch_format::FORMAT_2:
          nof_ack_bits = pucch.format_2.harq_ack_nof_bits;
          break;
        default:
          break;
      }
      if (pucch.crnti == u1.crnti) {
        for (unsigned harq_bit_idx = 0; harq_bit_idx < nof_ack_bits; ++harq_bit_idx) {
          u1.get_pcell().handle_dl_ack_info(current_slot, mac_harq_ack_report_status::ack, harq_bit_idx, 100);
        }
      } else if (pucch.crnti == u2.crnti) {
        for (unsigned harq_bit_idx = 0; harq_bit_idx < nof_ack_bits; ++harq_bit_idx) {
          u2.get_pcell().handle_dl_ack_info(current_slot, mac_harq_ack_report_status::ack, harq_bit_idx, 100);
        }
      } else if (pucch.crnti == u3.crnti) {
        for (unsigned harq_bit_idx = 0; harq_bit_idx < nof_ack_bits; ++harq_bit_idx) {
          u3.get_pcell().handle_dl_ack_info(current_slot, mac_harq_ack_report_status::ack, harq_bit_idx, 100);
        }
      }
    }
  }

  // PF scheduler ensures fairness by scheduling UE2 more than UE1 and UE3 due to having worse channel conditions when
  // compared to that of UE1 and UE3.
  ASSERT_TRUE(ue_pdsch_scheduled_count[u2.ue_index] > ue_pdsch_scheduled_count[u1.ue_index] and
              ue_pdsch_scheduled_count[u2.ue_index] > ue_pdsch_scheduled_count[u3.ue_index]);
}

TEST_F(scheduler_pf_test, pf_ensures_fairness_in_ul_when_ues_have_different_channel_conditions)
{
  const lcg_id_t lcg_id = uint_to_lcg_id(2);
  ue&            u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  ue&            u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));
  ue&            u3     = add_ue(make_ue_create_req(to_du_ue_index(2), to_rnti(0x4603), {uint_to_lcid(5)}, lcg_id));

  // Push high enough UL BSR to ensure UEs occupy entire BWP CRBs when scheduled.
  notify_ul_bsr(u1.ue_index, lcg_id, 1000000);
  notify_ul_bsr(u2.ue_index, lcg_id, 1000000);
  notify_ul_bsr(u3.ue_index, lcg_id, 1000000);

  std::unordered_map<du_ue_index_t, unsigned> ue_pusch_scheduled_count;
  ue_pusch_scheduled_count[u1.ue_index] = 0;
  ue_pusch_scheduled_count[u2.ue_index] = 0;
  ue_pusch_scheduled_count[u3.ue_index] = 0;

  for (unsigned i = 0; i != 100; ++i) {
    run_slot();
    const slot_point current_slot = next_slot - 1;
    for (const auto& grant : this->res_grid[0].result.ul.puschs) {
      ++ue_pusch_scheduled_count[grant.context.ue_index];
      // Auto send CRC=OK.
      ul_crc_pdu_indication crc_pdu{
          grant.pusch_cfg.rnti, grant.context.ue_index, to_harq_id(grant.pusch_cfg.harq_id), true};
      if (grant.context.ue_index == u1.ue_index) {
        // Worst channel condition.
        crc_pdu.ul_sinr_dB = 10;
        u1.get_pcell().handle_crc_pdu(current_slot, crc_pdu);
      } else if (grant.context.ue_index == u2.ue_index) {
        crc_pdu.ul_sinr_dB = 15;
        u2.get_pcell().handle_crc_pdu(current_slot, crc_pdu);
      } else if (grant.context.ue_index == u3.ue_index) {
        // Best channel condition.
        crc_pdu.ul_sinr_dB = 30;
        u3.get_pcell().handle_crc_pdu(current_slot, crc_pdu);
      }
    }
  }

  // PF scheduler ensures fairness by scheduling UE1 more than UE2 and UE3 due to having worse channel conditions when
  // compared to that of UE2 and UE3.
  ASSERT_TRUE(ue_pusch_scheduled_count[u1.ue_index] > ue_pusch_scheduled_count[u2.ue_index] and
              ue_pusch_scheduled_count[u1.ue_index] > ue_pusch_scheduled_count[u3.ue_index]);
}

class scheduler_policy_alloc_bounds_test : public base_scheduler_policy_test,
                                           public ::testing::TestWithParam<policy_scheduler_type>
{
protected:
  scheduler_policy_alloc_bounds_test() :
    base_scheduler_policy_test(GetParam(), []() {
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config();

      // Modify boundaries within which PDSCH and PUSCH needs to be allocated to UEs.
      sched_cfg_.ue.pdsch_crb_limits = {10, 15};
      sched_cfg_.ue.pusch_crb_limits = {10, 15};

      return sched_cfg_;
    }())
  {
    run_slot();
  }
};

TEST_P(scheduler_policy_alloc_bounds_test, scheduler_allocates_pdsch_within_configured_boundaries)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));

  // Buffer has to be large enough so that the allocation does not stop.
  push_dl_bs(u1.ue_index, uint_to_lcid(5), 100000);

  // Run for partial slot.
  run_slot();

  // Expected CRBs is based on the expert configuration passed to scheduler during class initialization.
  crb_interval expected_crb_allocation{10, 15};
  prb_interval expected_prb_interval = crb_to_prb(
      u1.get_pcell().cfg().bwp(u1.get_pcell().active_bwp_id()).dl_common->generic_params.crbs, expected_crb_allocation);
  vrb_interval expected_vrb_interval{expected_prb_interval.start(), expected_prb_interval.stop()};

  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants.size(), 1);
  ASSERT_TRUE(this->res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1() == expected_vrb_interval);
}

TEST_P(scheduler_policy_alloc_bounds_test, scheduler_allocates_pusch_within_configured_boundaries)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));

  // Buffer has to be large enough so that the allocation does not stop.
  notify_ul_bsr(u1.ue_index, lcg_id, 2000000);

  // Run until one slot before partial slot.
  while (this->res_grid[0].result.ul.puschs.empty()) {
    run_slot();
  }

  // Expected CRBs is based on the expert configuration passed to scheduler during class initialization.
  crb_interval expected_crb_allocation{10, 15};
  prb_interval expected_prb_interval = crb_to_prb(
      u1.get_pcell().cfg().bwp(u1.get_pcell().active_bwp_id()).ul_common->generic_params.crbs, expected_crb_allocation);
  vrb_interval expected_vrb_interval{expected_prb_interval.start(), expected_prb_interval.stop()};

  ASSERT_TRUE(this->res_grid[0].result.ul.puschs.back().pusch_cfg.rbs.type1() == expected_vrb_interval);
}

INSTANTIATE_TEST_SUITE_P(scheduler_policy,
                         scheduler_policy_test,
                         testing::Values(policy_scheduler_type::time_rr, policy_scheduler_type::time_pf));
INSTANTIATE_TEST_SUITE_P(scheduler_policy,
                         scheduler_policy_partial_slot_tdd_test,
                         testing::Values(policy_scheduler_type::time_rr, policy_scheduler_type::time_pf));
INSTANTIATE_TEST_SUITE_P(scheduler_policy,
                         scheduler_policy_alloc_bounds_test,
                         testing::Values(policy_scheduler_type::time_rr, policy_scheduler_type::time_pf));
