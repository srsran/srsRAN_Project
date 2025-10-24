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

#include "../test_utils/config_generators.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/slicing/inter_slice_scheduler.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_context/ue.h"
#include "lib/scheduler/ue_scheduling/intra_slice_scheduler.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "srsran/adt/unique_function.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/qos/five_qi_qos_mapping.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

/// \brief Type of policy scheduler.
///
/// The current types are:
/// - time_rr - Time based Round-Robin scheduler.
/// - time_qos - Time based QoS-aware scheduler.
enum class policy_scheduler_type { time_rr, time_qos };

class base_scheduler_policy_test
{
protected:
  base_scheduler_policy_test(
      policy_scheduler_type      policy,
      scheduler_expert_config    sched_cfg_ = config_helpers::make_default_scheduler_expert_config(),
      cell_config_builder_params params_    = {}) :
    params(params_),
    cell_cfg_req(sched_config_helper::make_default_sched_cell_configuration_request(params)),
    logger(srslog::fetch_basic_logger("SCHED", true)),
    res_logger(false, cell_cfg_req.pci),
    sched_cfg([&sched_cfg_, policy]() {
      if (policy == policy_scheduler_type::time_qos) {
        sched_cfg_.ue.strategy_cfg = time_qos_scheduler_expert_config{};
      }
      return sched_cfg_;
    }()),
    cell_cfg(*[this]() {
      return cell_cfg_list.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(sched_cfg, cell_cfg_req))
          .get();
    }()),
    cell_ues(ues.add_cell(to_du_cell_index(0))),
    slice_sched(cell_cfg, ues),
    cell_metrics(cell_cfg, cell_cfg_req.metrics),
    intra_slice_sched(cell_cfg.expert_cfg.ue, ues, pdcch_alloc, uci_alloc, res_grid, cell_metrics, cell_harqs, logger)
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg_pool.add_cell(cell_cfg_req);
  }

  ~base_scheduler_policy_test() { srslog::flush(); }

  void run_slot()
  {
    logger.set_context(next_slot.sfn(), next_slot.slot_index());

    res_grid.slot_indication(next_slot);
    cell_harqs.slot_indication(next_slot);
    pdcch_alloc.slot_indication(next_slot);
    pucch_alloc.slot_indication(next_slot);
    uci_alloc.slot_indication(next_slot);
    intra_slice_sched.slot_indication(next_slot);
    slice_sched.slot_indication(next_slot, res_grid);

    if (cell_cfg.is_dl_enabled(next_slot)) {
      while (auto dl_slice_candidate = slice_sched.get_next_dl_candidate()) {
        intra_slice_sched.dl_sched(dl_slice_candidate.value(), slice_sched.get_policy(dl_slice_candidate->id()));
      }
      while (auto ul_slice_candidate = slice_sched.get_next_ul_candidate()) {
        intra_slice_sched.ul_sched(ul_slice_candidate.value(), slice_sched.get_policy(ul_slice_candidate->id()));
      }
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
        std::make_unique<ue_configuration>(ue_req.ue_index, ue_req.crnti, cell_cfg_list, cfg_pool.add_ue(ue_req)));
    ues.add_ue(
        std::make_unique<ue>(ue_creation_command{*ue_ded_cell_cfg_list.back(), ue_req.starts_in_fallback, cell_harqs}));
    slice_sched.add_ue(ue_req.ue_index);
    return ues[ue_req.ue_index];
  }

  sched_ue_creation_request_message make_ue_create_req(du_ue_index_t                        ue_index,
                                                       rnti_t                               rnti,
                                                       const std::initializer_list<lcid_t>& lcids_to_activate,
                                                       lcg_id_t                             lcg_id)
  {
    sched_ue_creation_request_message req = sched_config_helper::create_default_sched_ue_creation_request(params);
    req.ue_index                          = ue_index;
    req.crnti                             = rnti;
    // Set LCG ID for SRBs provided in the LCIDs to activate list.
    for (auto& lc_cfg : *req.cfg.lc_config_list) {
      if (lc_cfg.lcid < lcid_t::LCID_SRB2 and
          std::find(lcids_to_activate.begin(), lcids_to_activate.end(), lc_cfg.lcid) != lcids_to_activate.end()) {
        lc_cfg.lc_group = lcg_id;
      }
    }
    auto default_lc_cfg     = config_helpers::create_default_logical_channel_config(uint_to_lcid(0));
    default_lc_cfg.lc_group = lcg_id;
    // Add DRBs if any in the LCIDs to activate list.
    for (lcid_t lcid : lcids_to_activate) {
      if (lcid >= lcid_t::LCID_SRB2) {
        default_lc_cfg.lcid = lcid;
        req.cfg.lc_config_list->push_back(default_lc_cfg);
      }
    }
    return req;
  }

  void push_dl_bs(du_ue_index_t ue_index, lcid_t lcid, unsigned bytes)
  {
    ues[ue_index].handle_dl_buffer_state_indication(lcid, bytes);
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

  const cell_config_builder_params               params;
  const sched_cell_configuration_request_message cell_cfg_req;

  srslog::basic_logger&                          logger;
  scheduler_result_logger                        res_logger;
  scheduler_expert_config                        sched_cfg;
  cell_common_configuration_list                 cell_cfg_list;
  du_cell_group_config_pool                      cfg_pool;
  std::vector<std::unique_ptr<ue_configuration>> ue_ded_cell_cfg_list;

  const cell_configuration&           cell_cfg;
  sched_cfg_dummy_notifier            dummy_mac_notif;
  scheduler_ue_metrics_dummy_notifier metrics_notif;

  cell_resource_allocator       res_grid{cell_cfg};
  cell_harq_manager             cell_harqs{MAX_NOF_DU_UES,
                               MAX_NOF_HARQS,
                               std::make_unique<scheduler_harq_timeout_dummy_notifier>()};
  pdcch_resource_allocator_impl pdcch_alloc{cell_cfg};
  pucch_allocator_impl pucch_alloc{cell_cfg, sched_cfg.ue.max_pucchs_per_slot, sched_cfg.ue.max_ul_grants_per_slot};
  uci_allocator_impl   uci_alloc{pucch_alloc};
  ue_repository        ues;
  ue_cell_repository&  cell_ues;
  // NOTE: Policy scheduler is part of RAN slice instances created in slice scheduler.
  inter_slice_scheduler slice_sched;
  cell_metrics_handler  cell_metrics;
  intra_slice_scheduler intra_slice_sched;

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
      csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{15U}, true});
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
  const unsigned nof_ack_bits = pucch.uci_bits.harq_ack_nof_bits;
  du_ue_index_t  ue_with_retx;

  if (pucch.crnti == u1.crnti) {
    ue_with_retx = u1.ue_index;
    for (unsigned harq_bit_idx = 0; harq_bit_idx < nof_ack_bits; ++harq_bit_idx) {
      u1.get_pcell().handle_dl_ack_info(current_slot, mac_harq_ack_report_status::nack, harq_bit_idx, 100);
    }
  } else if (pucch.crnti == u2.crnti) {
    ue_with_retx = u2.ue_index;
    for (unsigned harq_bit_idx = 0; harq_bit_idx < nof_ack_bits; ++harq_bit_idx) {
      u2.get_pcell().handle_dl_ack_info(current_slot, mac_harq_ack_report_status::nack, harq_bit_idx, 100);
    }
  }

  run_slot();
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

TEST_P(scheduler_policy_test, scheduler_allocates_pdsch_even_when_only_pending_ces_are_enqueued)
{
  const lcg_id_t drb_lcg_id = uint_to_lcg_id(2);
  ue&            u1 = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {LCID_MIN_DRB}, drb_lcg_id));

  // Enqueue TA CMD MAC CE.
  dl_mac_ce_indication ind{};
  ind.ue_index = u1.ue_index;
  ind.ce_lcid  = lcid_dl_sch_t::TA_CMD;
  u1.handle_dl_mac_ce_indication(ind);

  bool pdsch_scheduled = run_until([this]() { return not this->res_grid[0].result.dl.ue_grants.empty(); });
  ASSERT_TRUE(pdsch_scheduled);
  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants[0].context.ue_index, u1.ue_index);
  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants[0].tb_list.back().lc_chs_to_sched.back().lcid, lcid_dl_sch_t::TA_CMD);
}

class scheduler_policy_partial_slot_tdd_test : public base_scheduler_policy_test,
                                               public ::testing::TestWithParam<policy_scheduler_type>
{
protected:
  scheduler_policy_partial_slot_tdd_test() :
    base_scheduler_policy_test(GetParam(), config_helpers::make_default_scheduler_expert_config(), []() {
      cell_config_builder_params builder_params{};
      // Band 40.
      builder_params.dl_f_ref_arfcn = 465000;
      builder_params.scs_common     = subcarrier_spacing::kHz30;
      builder_params.band           = band_helper::get_band_from_dl_arfcn(builder_params.dl_f_ref_arfcn);
      builder_params.channel_bw_mhz = srsran::bs_channel_bandwidth::MHz20;

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
      return builder_params;
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
  scheduler_pf_test() : base_scheduler_policy_test(policy_scheduler_type::time_qos) {}
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
        csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{15U}, true});
    // Worst channel condition.
    u2.get_pcell().handle_csi_report(
        csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{10U}, true});
    u3.get_pcell().handle_csi_report(
        csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{12U}, true});

    run_slot();
    const slot_point current_slot = next_slot - 1;
    for (const auto& grant : this->res_grid[0].result.dl.ue_grants) {
      ++ue_pdsch_scheduled_count[grant.context.ue_index];
    }

    // Auto ACK HARQs.
    for (const pucch_info& pucch : this->res_grid[0].result.ul.pucchs) {
      const unsigned nof_ack_bits = pucch.uci_bits.harq_ack_nof_bits;
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
  ASSERT_GT(ue_pdsch_scheduled_count[u2.ue_index], ue_pdsch_scheduled_count[u1.ue_index]);
  ASSERT_GT(ue_pdsch_scheduled_count[u2.ue_index], ue_pdsch_scheduled_count[u3.ue_index]);
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

// Expressed in bits per second.
using gbr_bitrate_bps = uint64_t;

class scheduler_pf_qos_test : public base_scheduler_policy_test, public ::testing::TestWithParam<gbr_bitrate_bps>
{
protected:
  scheduler_pf_qos_test() : base_scheduler_policy_test(policy_scheduler_type::time_qos) {}

  static double to_bytes_per_slot(uint64_t bitrate_bps, subcarrier_spacing bwp_scs)
  {
    // Deduce the number of slots per subframe.
    const unsigned nof_slots_per_subframe   = get_nof_slots_per_subframe(bwp_scs);
    const unsigned nof_subframes_per_second = 1000;
    // Deduce the number of slots per second.
    const unsigned nof_slots_per_second = nof_slots_per_subframe * nof_subframes_per_second;

    return (static_cast<double>(bitrate_bps) / static_cast<double>(nof_slots_per_second * 8U));
  }
};

TEST_P(scheduler_pf_qos_test, pf_upholds_qos_in_dl_gbr_flows)
{
  const lcg_id_t lcg_id              = uint_to_lcg_id(2);
  const lcid_t   gbr_bearer_lcid     = uint_to_lcid(6);
  const lcid_t   non_gbr_bearer_lcid = uint_to_lcid(5);

  const gbr_bitrate_bps brate = GetParam();

  ue& ue_with_gbr =
      add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {non_gbr_bearer_lcid, gbr_bearer_lcid}, lcg_id));
  // Prepare request with GBR bearer information.
  sched_ue_reconfiguration_message recfg_req{};
  sched_ue_config_request&         cfg_req = recfg_req.cfg;
  cfg_req.lc_config_list.emplace();
  cfg_req.lc_config_list->resize(4);
  (*cfg_req.lc_config_list)[0]          = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB0);
  (*cfg_req.lc_config_list)[1]          = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1);
  (*cfg_req.lc_config_list)[2]          = config_helpers::create_default_logical_channel_config(non_gbr_bearer_lcid);
  (*cfg_req.lc_config_list)[2].lc_group = lcg_id;
  (*cfg_req.lc_config_list)[2].qos.emplace();
  (*cfg_req.lc_config_list)[2].qos->qos          = *get_5qi_to_qos_characteristics_mapping(uint_to_five_qi(9));
  (*cfg_req.lc_config_list)[2].qos->arp_priority = arp_prio_level_t::max();
  (*cfg_req.lc_config_list)[3] = config_helpers::create_default_logical_channel_config(gbr_bearer_lcid);
  // Put GBR bearer in a different LCG than non-GBR bearer.
  (*cfg_req.lc_config_list)[3].lc_group = uint_to_lcg_id(lcg_id - 1);
  (*cfg_req.lc_config_list)[3].qos.emplace();
  (*cfg_req.lc_config_list)[3].qos->qos          = *get_5qi_to_qos_characteristics_mapping(uint_to_five_qi(1));
  (*cfg_req.lc_config_list)[3].qos->arp_priority = arp_prio_level_t::max();
  (*cfg_req.lc_config_list)[3].qos->gbr_qos_info = gbr_qos_flow_information{brate, brate, brate, brate};
  ue_ded_cell_cfg_list[0]->update(cell_cfg_list, cfg_pool.reconf_ue(recfg_req));
  // Add UE with no GBR bearer.
  ue& ue_with_no_gbr = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {non_gbr_bearer_lcid}, lcg_id));

  const double expected_rate = to_bytes_per_slot(brate, cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs);

  push_dl_bs(ue_with_gbr.ue_index, gbr_bearer_lcid, expected_rate);
  push_dl_bs(ue_with_no_gbr.ue_index, non_gbr_bearer_lcid, 1000000);

  // Report best channel condition for different UEs.
  ue_with_gbr.get_pcell().handle_csi_report(
      csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{15U}, true});
  ue_with_no_gbr.get_pcell().handle_csi_report(
      csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{15U}, true});

  unsigned              scheduled_bytes_for_gbr_bearer = 0;
  static const unsigned max_nof_slots                  = 50;

  for (unsigned i = 0; i != max_nof_slots; ++i) {
    run_slot();
    // Keep pushing buffer status update for UE with GBR bearer so that there is steady flow of pending bytes.
    push_dl_bs(ue_with_gbr.ue_index, gbr_bearer_lcid, expected_rate);
    for (const auto& grant : this->res_grid[0].result.dl.ue_grants) {
      if (grant.context.ue_index == ue_with_gbr.ue_index) {
        for (const auto& tb_info : grant.tb_list) {
          const auto* it = std::find_if(tb_info.lc_chs_to_sched.begin(),
                                        tb_info.lc_chs_to_sched.end(),
                                        [](const dl_msg_lc_info& lc_info) { return lc_info.lcid == gbr_bearer_lcid; });
          if (it == tb_info.lc_chs_to_sched.end()) {
            continue;
          }
          scheduled_bytes_for_gbr_bearer += it->sched_bytes;
        }
      }
    }

    // Auto ACK HARQs.
    const slot_point current_slot = next_slot - 1;
    for (const pucch_info& pucch : this->res_grid[0].result.ul.pucchs) {
      const unsigned nof_ack_bits = pucch.uci_bits.harq_ack_nof_bits;
      if (pucch.crnti == ue_with_gbr.crnti) {
        for (unsigned harq_bit_idx = 0; harq_bit_idx < nof_ack_bits; ++harq_bit_idx) {
          ue_with_gbr.get_pcell().handle_dl_ack_info(current_slot, mac_harq_ack_report_status::ack, harq_bit_idx, 100);
        }
      } else if (pucch.crnti == ue_with_no_gbr.crnti) {
        for (unsigned harq_bit_idx = 0; harq_bit_idx < nof_ack_bits; ++harq_bit_idx) {
          ue_with_no_gbr.get_pcell().handle_dl_ack_info(
              current_slot, mac_harq_ack_report_status::ack, harq_bit_idx, 100);
        }
      }
    }
  }
  const double observed_rate = static_cast<double>(scheduled_bytes_for_gbr_bearer) / static_cast<double>(max_nof_slots);
  ASSERT_GE(observed_rate, expected_rate);
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
  prb_interval expected_prb_interval =
      crb_to_prb(u1.get_pcell().cfg().bwp(u1.get_pcell().active_bwp_id()).dl_common->value().generic_params.crbs,
                 expected_crb_allocation);
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
  prb_interval expected_prb_interval =
      crb_to_prb(u1.get_pcell().cfg().bwp(u1.get_pcell().active_bwp_id()).ul_common->value().generic_params.crbs,
                 expected_crb_allocation);
  vrb_interval expected_vrb_interval{expected_prb_interval.start(), expected_prb_interval.stop()};

  ASSERT_TRUE(this->res_grid[0].result.ul.puschs.back().pusch_cfg.rbs.type1() == expected_vrb_interval);
}

INSTANTIATE_TEST_SUITE_P(scheduler_policy,
                         scheduler_policy_test,
                         testing::Values(policy_scheduler_type::time_rr, policy_scheduler_type::time_qos));
INSTANTIATE_TEST_SUITE_P(scheduler_policy,
                         scheduler_policy_partial_slot_tdd_test,
                         testing::Values(policy_scheduler_type::time_rr, policy_scheduler_type::time_qos));
INSTANTIATE_TEST_SUITE_P(scheduler_policy,
                         scheduler_policy_alloc_bounds_test,
                         testing::Values(policy_scheduler_type::time_rr, policy_scheduler_type::time_qos));
INSTANTIATE_TEST_SUITE_P(scheduler_policy, scheduler_pf_qos_test, testing::Values(128000, 256000));
