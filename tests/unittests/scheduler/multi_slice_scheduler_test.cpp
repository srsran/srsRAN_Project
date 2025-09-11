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

#include "tests/test_doubles/scheduler/cell_config_builder_profiles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_simulator.h"
#include "srsran/adt/ranges/transform.h"
#include <gtest/gtest.h>

using namespace srsran;

struct multi_slice_test_params {
  std::vector<slice_rrm_policy_config> slices;
};

static s_nssai_t get_nssai(unsigned sst, unsigned sd)
{
  return s_nssai_t{slice_service_type{(uint8_t)sst}, slice_differentiator::create(sd).value()};
}

static rrm_policy_member get_rrm_policy(unsigned sst, unsigned sd)
{
  return rrm_policy_member{plmn_identity::test_value(), get_nssai(sst, sd)};
}

class base_multi_slice_scheduler_tester : public scheduler_test_simulator
{
protected:
  base_multi_slice_scheduler_tester(const multi_slice_test_params& params_) :
    scheduler_test_simulator(4, subcarrier_spacing::kHz30), params(params_)
  {
    // Add Cell.
    auto sched_cell_cfg_req = sched_config_helper::make_default_sched_cell_configuration_request(builder_params);
    sched_cell_cfg_req.rrm_policy_members = params.slices;
    this->add_cell(sched_cell_cfg_req);
  }

  rnti_t add_ue(const std::vector<std::pair<lcid_t, s_nssai_t>>& lcid_to_cfg)
  {
    auto                get_lcid  = [](const auto& e) { return e.first; };
    auto                only_lcid = views::transform(lcid_to_cfg, get_lcid);
    std::vector<lcid_t> lcid_list(only_lcid.begin(), only_lcid.end());
    auto ue_cfg = sched_config_helper::create_default_sched_ue_creation_request(builder_params, lcid_list);
    for (unsigned i = 0; i < lcid_list.size(); i++) {
      auto it                = std::find_if(ue_cfg.cfg.lc_config_list->begin(),
                             ue_cfg.cfg.lc_config_list->end(),
                             [lcid = lcid_to_cfg[i].first](const auto& l) { return l.lcid == lcid; });
      it->rrm_policy.s_nssai = lcid_to_cfg[i].second;
    }
    ue_cfg.crnti    = to_rnti(0x4601 + ue_count);
    ue_cfg.ue_index = to_du_ue_index(ue_count);
    scheduler_test_simulator::add_ue(ue_cfg);

    // Set high CQI.
    uci_indication uci;
    uci.slot_rx    = next_slot_rx();
    uci.cell_index = to_du_cell_index(0);
    uci.ucis.resize(1);
    uci.ucis[0].crnti    = ue_cfg.crnti;
    uci.ucis[0].ue_index = ue_cfg.ue_index;
    uci.ucis[0].pdu      = uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu{
             .csi = csi_report_data{.first_tb_wideband_cqi = 15, .valid = true}};
    this->sched->handle_uci_indication(uci);

    ue_count++;
    return ue_cfg.crnti;
  }

  multi_slice_test_params    params;
  cell_config_builder_params builder_params = cell_config_builder_profiles::tdd();

  unsigned ue_count = 0;
};

class single_slice_limited_max_rbs_scheduler_test : public base_multi_slice_scheduler_tester, public ::testing::Test
{
protected:
  static constexpr unsigned max_slice_rbs = 10;

  single_slice_limited_max_rbs_scheduler_test() :
    base_multi_slice_scheduler_tester(
        multi_slice_test_params{{slice_rrm_policy_config{get_rrm_policy(1, 1), 0, max_slice_rbs}}})
  {
  }
};

TEST_F(single_slice_limited_max_rbs_scheduler_test, single_ue_limited_to_max_rbs)
{
  // Create UE and fill its buffer.
  rnti_t rnti = this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(0), LCID_MIN_DRB, 500});

  ASSERT_TRUE(this->run_slot_until(
      [this, rnti]() { return find_ue_pdsch(rnti, this->last_sched_res_list[0]->dl.ue_grants) != nullptr; }));
  const dl_msg_alloc* msg = find_ue_pdsch(rnti, this->last_sched_res_list[0]->dl.ue_grants);

  ASSERT_TRUE(msg->pdsch_cfg.rbs.type1().length() <= max_slice_rbs);
}

TEST_F(single_slice_limited_max_rbs_scheduler_test, multi_ue_limited_to_max_rbs)
{
  // Create UE and fill its buffer.
  unsigned            nof_ues = test_rgen::uniform_int<unsigned>(2, 10);
  unsigned            dl_bo   = test_rgen::uniform_int<unsigned>(1, 50);
  std::vector<rnti_t> rntis;
  for (unsigned i = 0; i < nof_ues; i++) {
    rntis.push_back(this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))}));
    this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(i), LCID_MIN_DRB, dl_bo});
  }

  ASSERT_TRUE(this->run_slot_until(
      [&]() { return find_ue_pdsch(rntis.front(), this->last_sched_res_list[0]->dl.ue_grants) != nullptr; }));
  unsigned nof_rbs = 0;
  for (const dl_msg_alloc& msg : this->last_sched_res_list[0]->dl.ue_grants) {
    nof_rbs += msg.pdsch_cfg.rbs.type1().length();
  }

  ASSERT_TRUE(nof_rbs <= max_slice_rbs);
}

class multi_slice_with_prio_slice_scheduler_test : public base_multi_slice_scheduler_tester, public ::testing::Test
{
protected:
  static constexpr unsigned slice1_min_rbs = 10;
  static constexpr unsigned slice1_max_rbs = 20;

  multi_slice_with_prio_slice_scheduler_test() :
    base_multi_slice_scheduler_tester(
        multi_slice_test_params{{slice_rrm_policy_config{get_rrm_policy(1, 1), slice1_min_rbs, slice1_max_rbs},
                                 slice_rrm_policy_config{get_rrm_policy(1, 2), 0, MAX_NOF_PRBS}}})
  {
  }
};

TEST_F(multi_slice_with_prio_slice_scheduler_test, multi_ue_limited_to_max_rbs)
{
  // Create 3 UEs and fill their buffers.
  this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(0), LCID_MIN_DRB, 100000});
  this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 1))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(1), LCID_MIN_DRB, 100000});
  this->add_ue({std::make_pair(LCID_MIN_DRB, get_nssai(1, 2))});
  this->push_dl_buffer_state(dl_buffer_state_indication_message{to_du_ue_index(2), LCID_MIN_DRB, 100000});

  unsigned                nof_checks = 4;
  std::array<unsigned, 3> rnti_sum_rbs{0};
  for (unsigned i = 0; i < nof_checks; i++) {
    ASSERT_TRUE(this->run_slot_until([&]() { return not this->last_sched_res_list[0]->dl.ue_grants.empty(); }));

    std::array<unsigned, 3> rnti_rbs{0};
    for (const dl_msg_alloc& msg : this->last_sched_res_list[0]->dl.ue_grants) {
      unsigned idx = static_cast<unsigned>(msg.pdsch_cfg.rnti) - 0x4601;
      rnti_rbs[idx] += msg.pdsch_cfg.rbs.type1().length();
    }
    ASSERT_GE(rnti_rbs[0] + rnti_rbs[1], slice1_min_rbs);
    ASSERT_LE(rnti_rbs[0] + rnti_rbs[1], slice1_max_rbs);
    ASSERT_GT(rnti_rbs[2], 0);

    for (unsigned j = 0; j != rnti_rbs.size(); j++) {
      rnti_sum_rbs[j] += rnti_rbs[j];
    }
  }

  ASSERT_TRUE(std::all_of(rnti_sum_rbs.begin(), rnti_sum_rbs.end(), [](unsigned n) { return n > 0; }));
}
