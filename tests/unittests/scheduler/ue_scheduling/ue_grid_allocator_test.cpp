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

#include "../test_utils/dummy_test_components.h"
#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/logging/scheduler_metrics_handler.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/slicing/ran_slice_instance.h"
#include "lib/scheduler/support/bwp_helpers.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_context/ue.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/test_doubles/scheduler/scheduler_result_test.h"
#include "srsran/adt/unique_function.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace srsran;

class ue_grid_allocator_tester : public ::testing::TestWithParam<duplex_mode>
{
protected:
  ue_grid_allocator_tester(
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config()) :
    sched_cfg(std::move(sched_cfg_)),
    cell_cfg(*[this]() {
      cfg_builder_params.dl_f_ref_arfcn = GetParam() == duplex_mode::FDD ? 530000 : 520002;
      cfg_builder_params.scs_common =
          GetParam() == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30;
      cfg_builder_params.band           = band_helper::get_band_from_dl_arfcn(cfg_builder_params.dl_f_ref_arfcn);
      cfg_builder_params.channel_bw_mhz = bs_channel_bandwidth::MHz20;
      const auto* cfg =
          cfg_mng.add_cell(sched_config_helper::make_default_sched_cell_configuration_request(cfg_builder_params));
      srsran_assert(cfg != nullptr, "Cell configuration failed");
      return cfg;
    }()),
    cell_ues(ues.add_cell(to_du_cell_index(0))),
    slice_ues(ran_slice_id_t{0}, to_du_cell_index(0)),
    alloc(expert_cfg, ues, pdcch_alloc, uci_alloc, res_grid, logger),
    current_slot(cfg_builder_params.scs_common, 0)
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    // Initialize resource grid.
    slot_indication();
  }

  slot_point get_next_ul_slot(const slot_point starting_slot) const
  {
    slot_point next_slot = starting_slot + cfg_builder_params.min_k2;
    while (not cell_cfg.is_fully_ul_enabled(next_slot)) {
      ++next_slot;
    }
    return next_slot;
  }

  void slot_indication(std::function<void()> on_each_slot = []() {})
  {
    ++current_slot;
    logger.set_context(current_slot.sfn(), current_slot.slot_index());

    res_grid.slot_indication(current_slot);
    cell_harqs.slot_indication(current_slot);
    pdcch_alloc.slot_indication(current_slot);
    pucch_alloc.slot_indication(current_slot);
    uci_alloc.slot_indication(current_slot);
    ues.slot_indication(current_slot);

    // Prepare CRB bitmask that will be used to find available CRBs.
    const auto& init_dl_bwp = cell_cfg.dl_cfg_common.init_dl_bwp;
    const auto  prb_lims    = crb_to_prb(init_dl_bwp.generic_params.crbs,
                                     cell_cfg.expert_cfg.ue.pdsch_crb_limits & init_dl_bwp.generic_params.crbs);
    auto        used_prbs   = res_grid[0].dl_res_grid.used_prbs(init_dl_bwp.generic_params.scs,
                                                       init_dl_bwp.generic_params.crbs,
                                                       init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].symbols);
    if (not prb_lims.empty()) {
      used_prbs.fill(0, prb_lims.start());
      used_prbs.fill(prb_lims.stop(), used_prbs.size());
    }
    // Note: VRB-to-PRB interleaving is not supported in this test.
    used_dl_vrbs = used_prbs.convert_to<vrb_bitmap>();

    on_each_slot();

    alloc.post_process_results();

    // Log scheduler results.
    res_logger.on_scheduler_result(res_grid[0].result);
  }

  bool run_until(std::function<void()> to_run, unique_function<bool()> until, unsigned max_slot_count = 1000)
  {
    if (until()) {
      return true;
    }
    for (unsigned count = 0; count != max_slot_count; ++count) {
      slot_indication(to_run);
      if (until()) {
        return true;
      }
    }
    return false;
  }

  ue& add_ue(du_ue_index_t ue_index, const std::initializer_list<lcid_t>& lcids_to_activate)
  {
    sched_ue_creation_request_message ue_creation_req =
        sched_config_helper::create_default_sched_ue_creation_request(cfg_builder_params);
    ue_creation_req.ue_index = ue_index;
    ue_creation_req.crnti    = to_rnti(0x4601 + (unsigned)ue_index);
    for (lcid_t lcid : lcids_to_activate) {
      ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }

    return add_ue(ue_creation_req);
  }

  ue& add_ue(const sched_ue_creation_request_message& ue_creation_req)
  {
    auto ev = cfg_mng.add_ue(ue_creation_req);
    ues.add_ue(
        std::make_unique<ue>(ue_creation_command{ev.next_config(), ue_creation_req.starts_in_fallback, cell_harqs}));
    for (const auto& lc_cfg : *ue_creation_req.cfg.lc_config_list) {
      slice_ues.add_logical_channel(ues[ue_creation_req.ue_index], lc_cfg.lcid, lc_cfg.lc_group);
    }
    ev.notify_completion();
    return ues[ue_creation_req.ue_index];
  }

  void push_dl_bs(du_ue_index_t ue_index, lcid_t lcid, unsigned bytes)
  {
    ues[ue_index].handle_dl_buffer_state_indication(lcid, bytes);
  }

  void allocate_dl_newtx_grant(const slice_ue&         user,
                               unsigned                pending_bytes,
                               bool                    interleaving_enabled,
                               std::optional<unsigned> max_nof_rbs = std::nullopt)
  {
    const auto& init_dl_bwp = cell_cfg.dl_cfg_common.init_dl_bwp;
    auto        result =
        alloc.allocate_dl_grant(ue_newtx_dl_grant_request{user, current_slot, pending_bytes, interleaving_enabled});
    if (not result.has_value()) {
      return;
    }
    auto& builder = result.value();

    vrb_interval vrbs = builder.recommended_vrbs(used_dl_vrbs);

    // Compute the corresponding CRBs.
    // Note: VRB-to-PRB interleaving is not supported in this test.
    std::pair<crb_interval, crb_interval> crbs = {
        prb_to_crb(init_dl_bwp.generic_params.crbs, vrbs.convert_to<prb_interval>()), {}};

    builder.set_pdsch_params(vrbs, crbs, interleaving_enabled);
    used_dl_vrbs.fill(vrbs.start(), vrbs.stop());
  }

  void allocate_dl_retx_grant(const slice_ue& user, dl_harq_process_handle h_dl)
  {
    auto result = alloc.allocate_dl_grant(ue_retx_dl_grant_request{user, current_slot, h_dl, used_dl_vrbs});
    if (result.has_value()) {
      used_dl_vrbs.fill(result.value().start(), result.value().stop());
    }
  }

  alloc_status allocate_ul_newtx_grant(const slice_ue&         user,
                                       unsigned                pending_bytes,
                                       std::optional<unsigned> max_nof_rbs = std::nullopt)
  {
    return allocate_ul_newtx_grant(get_next_ul_slot(current_slot), user, pending_bytes, max_nof_rbs);
  }

  alloc_status allocate_ul_newtx_grant(slot_point              pusch_slot,
                                       const slice_ue&         user,
                                       unsigned                pending_bytes,
                                       std::optional<unsigned> max_nof_rbs = std::nullopt)
  {
    const auto& init_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp;
    auto        result      = alloc.allocate_ul_grant(ue_newtx_ul_grant_request{user, pusch_slot, pending_bytes});
    if (not result.has_value()) {
      return result.error();
    }
    auto& builder = result.value();

    // Note: VRB-to-PRB interleaving is not supported in this test.
    auto used_ul_vrbs = res_grid[pusch_slot]
                            .ul_res_grid
                            .used_prbs(init_ul_bwp.generic_params.scs,
                                       init_ul_bwp.generic_params.crbs,
                                       init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[0].symbols)
                            .convert_to<vrb_bitmap>();
    vrb_interval vrbs = builder.recommended_vrbs(used_ul_vrbs, max_nof_rbs.value_or(MAX_NOF_PRBS));
    builder.set_pusch_params(vrbs);
    used_ul_vrbs.fill(vrbs.start(), vrbs.stop());
    return alloc_status::success;
  }

  scheduler_expert_config                 sched_cfg;
  scheduler_ue_expert_config              expert_cfg{sched_cfg.ue};
  sched_cfg_dummy_notifier                mac_notif;
  scheduler_ue_metrics_dummy_notifier     metrics_notif;
  scheduler_ue_metrics_dummy_configurator metrics_ue_handler;
  scheduler_metrics_handler               metrics;

  cell_config_builder_params cfg_builder_params;
  sched_config_manager       cfg_mng{scheduler_config{sched_cfg, mac_notif}, metrics};
  const cell_configuration&  cell_cfg;

  cell_harq_manager       cell_harqs{MAX_NOF_DU_UES,
                               MAX_NOF_HARQS,
                               std::make_unique<scheduler_harq_timeout_dummy_notifier>()};
  cell_resource_allocator res_grid{cell_cfg};

  pdcch_resource_allocator_impl pdcch_alloc{cell_cfg};
  pucch_allocator_impl pucch_alloc{cell_cfg, expert_cfg.max_pucchs_per_slot, expert_cfg.max_ul_grants_per_slot};
  uci_allocator_impl   uci_alloc{pucch_alloc};

  srslog::basic_logger&   logger{srslog::fetch_basic_logger("SCHED")};
  scheduler_result_logger res_logger{false, cell_cfg.pci};

  ue_repository           ues;
  ue_cell_repository&     cell_ues;
  slice_ue_repository     slice_ues;
  slice_rrm_policy_config rrm_policy;
  ran_slice_instance      slice_inst{ran_slice_id_t{0}, cell_cfg, rrm_policy};
  ue_cell_grid_allocator  alloc;

  slot_point current_slot;
  vrb_bitmap used_dl_vrbs;
};

TEST_P(ue_grid_allocator_tester,
       when_ue_dedicated_ss_is_css_then_allocation_is_within_coreset_start_crb_and_coreset0_end_crb)
{
  static const unsigned nof_bytes_to_schedule = 40U;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  // Change SS type to common.
  (*ue_creation_req.cfg.cells)[0]
      .serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]
      .set_non_ss0_monitored_dci_formats(search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true});
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);

  ue& u = add_ue(ue_creation_req);

  const crb_interval crbs =
      get_coreset_crbs((*ue_creation_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg.value().coresets.back());
  const crb_interval crb_lims = {
      crbs.start(), crbs.start() + cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length()};

  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  const auto& prb_alloc = res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().convert_to<prb_interval>();
  const auto  crb_alloc = prb_to_crb(crbs, prb_alloc);
  ASSERT_TRUE(crb_lims.contains(crb_alloc));
}

TEST_P(ue_grid_allocator_tester, when_using_non_fallback_dci_format_use_mcs_table_set_in_pdsch_cfg)
{
  static const unsigned nof_bytes_to_schedule = 40U;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  // Change PDSCH MCS table to be used when using non-fallback DCI format.
  (*ue_creation_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = srsran::pdsch_mcs_table::qam256;
  ue_creation_req.ue_index                                                       = to_du_ue_index(0);
  ue_creation_req.crnti                                                          = to_rnti(0x4601);

  const ue& u = add_ue(ue_creation_req);

  // SearchSpace#2 uses non-fallback DCI format hence the MCS table set in dedicated PDSCH configuration must be used.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.codewords.back().mcs_table,
            srsran::pdsch_mcs_table::qam256);
}

TEST_P(ue_grid_allocator_tester, allocates_pdsch_restricted_to_recommended_max_nof_rbs)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  static const unsigned sched_bytes             = 2000U;
  const unsigned        max_nof_rbs_to_schedule = 10U;

  ASSERT_TRUE(
      run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, max_nof_rbs_to_schedule); },
                [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH corresponding to the grant.
  ASSERT_GE(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            max_nof_rbs_to_schedule);
}

TEST_P(ue_grid_allocator_tester, allocates_pusch_restricted_to_recommended_max_nof_rbs)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  const unsigned recommended_nof_bytes_to_schedule = 2000U;
  const unsigned max_nof_rbs_to_schedule           = 10U;

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u1.ue_index], recommended_nof_bytes_to_schedule, max_nof_rbs_to_schedule);
      },
      [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH corresponding to the grant.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(), max_nof_rbs_to_schedule);
}

TEST_P(ue_grid_allocator_tester, does_not_allocate_pusch_with_all_remaining_rbs_if_its_a_sr_indication)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  ue& u1                   = add_ue(ue_creation_req);
  // Trigger a SR indication.
  u1.handle_sr_indication();

  const crb_interval cell_crbs = {cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start(),
                                  cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.stop()};

  ASSERT_TRUE(run_until([&]() { allocate_ul_newtx_grant(slice_ues[u1.ue_index], u1.pending_ul_newtx_bytes()); },
                        [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH corresponding to the grant.
  ASSERT_LT(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(), cell_crbs.length());
}

TEST_P(ue_grid_allocator_tester, no_two_pdschs_are_allocated_in_same_slot_for_a_ue)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false);
        allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false);
      },
      [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));

  // Only one PDSCH per slot per UE.
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.size(), 1);
}

TEST_P(ue_grid_allocator_tester, no_two_puschs_are_allocated_in_same_slot_for_a_ue)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule);
        allocate_ul_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule);
      },
      [&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul) != nullptr; }));

  // Only one PUSCH per slot per UE.
  ASSERT_EQ(res_grid[0].result.ul.puschs.size(), 1);
}

TEST_P(ue_grid_allocator_tester, consecutive_puschs_for_a_ue_are_allocated_in_increasing_order_of_time)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PUSCH grant for the UE.
  slot_point pusch_slot;
  ASSERT_TRUE(run_until(
      [&]() {
        pusch_slot = get_next_ul_slot(current_slot);
        allocate_ul_newtx_grant(pusch_slot, slice_ues[u.ue_index], nof_bytes_to_schedule);
      },
      [&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul) != nullptr; }));

  // Second PUSCH grant for the UE trying to allocate PUSCH in a slot previous to grant1.
  alloc_status result = alloc_status::invalid_params;
  ASSERT_FALSE(run_until(
      [&]() { result = allocate_ul_newtx_grant(pusch_slot - 1, slice_ues[u.ue_index], nof_bytes_to_schedule); },
      [&]() { return result == alloc_status::success; },
      1));
}

TEST_P(ue_grid_allocator_tester, consecutive_pdschs_for_a_ue_are_allocated_in_increasing_order_of_time)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PDSCH grant for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  slot_point last_pdsch_slot = current_slot;

  // Second PDSCH grant in the same slot for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_GE(current_slot, last_pdsch_slot);
}

TEST_P(ue_grid_allocator_tester,
       ack_slot_of_consecutive_pdschs_for_a_ue_must_be_greater_than_or_equal_to_last_ack_slot_allocated)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PDSCH grant for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  slot_point last_pdsch_ack_slot = current_slot + find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants)->context.k1;

  // Second PDSCH grant in the same slot for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_GE(current_slot + find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants)->context.k1, last_pdsch_ack_slot);
}

TEST_P(ue_grid_allocator_tester, successfully_allocated_pdsch_even_with_large_gap_to_last_pdsch_slot_allocated)
{
  static const unsigned nof_bytes_to_schedule                       = 8U;
  const unsigned        nof_slot_until_pdsch_is_allocated_threshold = SCHEDULER_MAX_K0;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // Ensure current slot is the middle of 1024 SFNs. i.e. current slot=511.0
  while (current_slot.sfn() != NOF_SFNS / 2) {
    slot_indication();
  }

  // First PDSCH grant for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));

  // Ensure next PDSCH to be allocated slot is after wrap around of 1024 SFNs (large gap to last allocated PDSCH slot)
  // and current slot value is less than last allocated PDSCH slot. e.g. next PDSCH to be allocated slot=SFN 2, slot 2
  // after wrap around of 1024 SFNs.
  for (unsigned i = 0; i < current_slot.nof_slots_per_hyper_system_frame() / 2 + current_slot.nof_slots_per_frame();
       ++i) {
    slot_indication();
  }

  // Next PDSCH grant to be allocated.
  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule, false); },
                        [&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; },
                        nof_slot_until_pdsch_is_allocated_threshold));
}

TEST_P(ue_grid_allocator_tester, successfully_allocates_pdsch_with_gbr_lc_prioritized_over_non_gbr_lc)
{
  const lcg_id_t lcg_id              = uint_to_lcg_id(2);
  const lcid_t   gbr_bearer_lcid     = uint_to_lcid(6);
  const lcid_t   non_gbr_bearer_lcid = uint_to_lcid(5);

  // Add UE.
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  ue& u1                   = add_ue(ue_creation_req);

  // Reconfigure UE to include non-GBR bearer and GBR bearer.
  sched_ue_reconfiguration_message reconf_msg{
      .ue_index = ue_creation_req.ue_index, .crnti = ue_creation_req.crnti, .cfg = ue_creation_req.cfg};
  sched_ue_config_request& cfg_req = reconf_msg.cfg;
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
  (*cfg_req.lc_config_list)[3].qos->gbr_qos_info = gbr_qos_flow_information{128000, 128000, 128000, 128000};
  ue_config_update_event ev                      = cfg_mng.update_ue(reconf_msg);
  u1.handle_reconfiguration_request({ev.next_config()}, false);
  u1.handle_config_applied();

  // Add LCID to the bearers of the UE belonging to this slice.
  for (const auto& lc_cfg : *cfg_req.lc_config_list) {
    slice_ues.add_logical_channel(u1, lc_cfg.lcid, lc_cfg.lc_group);
  }

  // Push buffer state update to both bearers.
  push_dl_bs(u1.ue_index, gbr_bearer_lcid, 200);
  push_dl_bs(u1.ue_index, non_gbr_bearer_lcid, 1500);

  static const unsigned sched_bytes = 2000U;

  ASSERT_TRUE(run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, false); },
                        [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));

  const auto* ue_pdsch = find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants);
  ASSERT_TRUE(not ue_pdsch->tb_list.empty());
  ASSERT_TRUE(not ue_pdsch->tb_list.back().lc_chs_to_sched.empty());
  // TB info contains GBR LC channel first and then non-GBR LC channel.
  ASSERT_EQ(ue_pdsch->tb_list.back().lc_chs_to_sched.front().lcid, gbr_bearer_lcid);
}

TEST_P(ue_grid_allocator_tester, successfully_allocated_pusch_even_with_large_gap_to_last_pusch_slot_allocated)
{
  static const unsigned nof_bytes_to_schedule                       = 400U;
  const unsigned        nof_slot_until_pusch_is_allocated_threshold = SCHEDULER_MAX_K2;

  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // Ensure current slot is the middle of 1024 SFNs. i.e. current slot=511.0
  while (current_slot.sfn() != NOF_SFNS / 2) {
    slot_indication();
  }

  // First PUSCH grant for the UE.
  ASSERT_TRUE(run_until([&]() { allocate_ul_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule); },
                        [&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul.puschs) != nullptr; }));

  // Ensure next PUSCH to be allocated slot is after wrap around of 1024 SFNs (large gap to last allocated PUSCH slot)
  // and current slot value is less than last allocated PUSCH slot. e.g. next PUSCH to be allocated slot=SFN 2, slot 2
  // after wrap around of 1024 SFNs.
  for (unsigned i = 0; i < current_slot.nof_slots_per_hyper_system_frame() / 2 + current_slot.nof_slots_per_frame();
       ++i) {
    slot_indication();
  }

  // Second PUSCH grant for the UE.
  ASSERT_TRUE(run_until(
      [&]() { return allocate_ul_newtx_grant(slice_ues[u.ue_index], nof_bytes_to_schedule) == alloc_status::success; },
      [&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul.puschs) != nullptr; },
      nof_slot_until_pusch_is_allocated_threshold));
}

class ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester : public ue_grid_allocator_tester
{
public:
  ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester() :
    ue_grid_allocator_tester(([]() {
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config();
      sched_cfg_.ue.pdsch_nof_rbs        = {20, 40};
      sched_cfg_.ue.pusch_nof_rbs        = {20, 40};
      return sched_cfg_;
    }()))
  {
  }
};

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pdsch_with_expert_cfg_min_nof_rbs_even_if_rbs_required_to_schedule_is_low)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is low enough such that < 20 RBs (configured in constructor) are required to schedule.
  static const unsigned sched_bytes             = 20U;
  const unsigned        max_nof_rbs_to_schedule = 10U;

  ASSERT_TRUE(
      run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, max_nof_rbs_to_schedule); },
                [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH.
  ASSERT_EQ(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            std::max(expert_cfg.pdsch_nof_rbs.start(), max_nof_rbs_to_schedule));
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pdsch_with_expert_cfg_max_nof_rbs_even_if_rbs_required_to_schedule_is_high)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 40 RBs (configured in constructor) are required to schedule.
  static const unsigned sched_bytes             = 20000U;
  const unsigned        max_nof_rbs_to_schedule = 273U;

  ASSERT_TRUE(
      run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, max_nof_rbs_to_schedule); },
                [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH.
  ASSERT_EQ(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            std::min(expert_cfg.pdsch_nof_rbs.stop(), max_nof_rbs_to_schedule));
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pusch_with_expert_cfg_min_nof_rbs_even_if_rbs_required_to_schedule_is_low)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is low enough such that < 20 RBs (configured in constructor) are required to schedule.
  const unsigned recommended_nof_bytes_to_schedule = 20U;
  const unsigned max_nof_rbs_to_schedule           = 10U;

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u1.ue_index], recommended_nof_bytes_to_schedule, max_nof_rbs_to_schedule);
      },
      [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(),
            std::max(expert_cfg.pdsch_nof_rbs.start(), max_nof_rbs_to_schedule));
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pusch_with_expert_cfg_max_nof_rbs_even_if_rbs_required_to_schedule_is_high)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 40 RBs (configured in constructor) are required to schedule.
  const unsigned recommended_nof_bytes_to_schedule = 200000U;
  const unsigned max_nof_rbs_to_schedule           = 273U;

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u1.ue_index], recommended_nof_bytes_to_schedule, max_nof_rbs_to_schedule);
      },
      [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(),
            std::min(expert_cfg.pdsch_nof_rbs.stop(), max_nof_rbs_to_schedule));
}

class ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester : public ue_grid_allocator_tester
{
public:
  ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester() :
    ue_grid_allocator_tester(([]() {
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config();
      sched_cfg_.ue.pdsch_crb_limits     = {20, 40};
      sched_cfg_.ue.pusch_crb_limits     = {20, 40};
      return sched_cfg_;
    }()))
  {
    // Assume SS#2 is USS configured with DCI format 1_1/0_1 and is the only SS used for UE PDSCH/PUSCH scheduling.
    const prb_interval pdsch_prbs =
        crb_to_prb(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs, sched_cfg.ue.pdsch_crb_limits);
    pdsch_vrb_limits = vrb_interval{pdsch_prbs.start(), pdsch_prbs.stop()};
    pusch_vrb_limits = rb_helper::crb_to_vrb_ul_non_interleaved(
        sched_cfg.ue.pusch_crb_limits, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start());
  }

protected:
  vrb_interval pdsch_vrb_limits;
  vrb_interval pusch_vrb_limits;
};

TEST_P(ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester, allocates_pdsch_within_expert_cfg_pdsch_rb_limits)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 20 RBs (configured in constructor) are required to schedule.
  static const unsigned sched_bytes             = 20000U;
  const unsigned        max_nof_rbs_to_schedule = 273U;

  ASSERT_TRUE(
      run_until([&]() { allocate_dl_newtx_grant(slice_ues[u1.ue_index], sched_bytes, max_nof_rbs_to_schedule); },
                [&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH within RB limits.
  ASSERT_EQ(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1(), pdsch_vrb_limits);
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester, allocates_pusch_within_expert_cfg_pusch_rb_limits)
{
  sched_ue_creation_request_message ue_creation_req =
      sched_config_helper::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 20 RBs (configured in constructor) are required to schedule.
  const unsigned recommended_nof_bytes_to_schedule = 200000U;
  const unsigned max_nof_rbs_to_schedule           = 273U;

  ASSERT_TRUE(run_until(
      [&]() {
        allocate_ul_newtx_grant(slice_ues[u1.ue_index], recommended_nof_bytes_to_schedule, max_nof_rbs_to_schedule);
      },
      [&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH within RB limits.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1(), pusch_vrb_limits);
}

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_tester,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));
