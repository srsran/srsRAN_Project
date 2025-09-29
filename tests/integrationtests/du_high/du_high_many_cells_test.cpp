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

#include "test_utils/du_high_env_simulator.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/mac/mac_test_messages.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "tests/test_doubles/scheduler/scheduler_result_test.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

namespace {

struct test_params {
  unsigned nof_cells = 1;
};

/// Formatter for test params.
void PrintTo(const test_params& value, ::std::ostream* os)
{
  *os << fmt::format("nof_cells={}", value.nof_cells);
}

} // namespace

class du_high_many_cells_tester : public du_high_env_simulator, public testing::TestWithParam<test_params>
{
public:
  du_high_many_cells_tester() : du_high_env_simulator(du_high_env_sim_params{GetParam().nof_cells}) {}

  bool run_until_csi(du_cell_index_t cell_idx, rnti_t rnti)
  {
    return run_until([this, cell_idx, rnti]() {
      if (phy.cells[cell_idx].last_ul_res.has_value()) {
        return find_ue_uci_with_csi(rnti, *phy.cells[cell_idx].last_ul_res.value().ul_res) != nullptr;
      }
      return false;
    });
  }
};

TEST_P(du_high_many_cells_tester, when_du_high_initiated_then_f1_setup_is_sent_with_correct_number_of_cells)
{
  // Starting the DU-high initiates the F1 Setup procedure and sets the number of serving cells correctly.
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.type().value,
            asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
  asn1::f1ap::f1_setup_request_s& f1_setup =
      this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.f1_setup_request();

  // Ensure all cells are set.
  ASSERT_EQ(f1_setup->gnb_du_served_cells_list.size(), GetParam().nof_cells);
  std::set<pci_t>    unique_pcis;
  std::set<unsigned> unique_cgis;
  for (unsigned i = 0; i != f1_setup->gnb_du_served_cells_list.size(); ++i) {
    auto& serv_cell = f1_setup->gnb_du_served_cells_list[i]->gnb_du_served_cells_item();
    ASSERT_TRUE(unique_pcis.insert(serv_cell.served_cell_info.nr_pci).second);
    ASSERT_TRUE(unique_cgis.insert(serv_cell.served_cell_info.nr_cgi.nr_cell_id.to_number()).second)
        << fmt::format("CGI={:#x} repeated", serv_cell.served_cell_info.nr_cgi.nr_cell_id.to_number());
  }
}

TEST_P(du_high_many_cells_tester,
       when_ccch_msg_received_in_a_cell_then_init_ul_rrc_message_with_matching_cgi_is_sent_to_cu)
{
  // Add one UE per cell
  for (unsigned i = 0; i != GetParam().nof_cells; ++i) {
    // Reset previous F1 message.
    cu_notifier.f1ap_ul_msgs.clear();

    // Inject UL-CCCH with RRC Setup.
    rnti_t rnti = to_rnti(0x4601 + i);
    du_hi->get_pdu_handler().handle_rx_data_indication(
        test_helpers::create_ccch_message(next_slot, rnti, to_du_cell_index(i)));

    // Wait for F1AP message to be propagated to the CU-CP.
    this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });

    // Ensure the F1AP Initial UL RRC message is correct.
    ASSERT_EQ(cu_notifier.f1ap_ul_msgs.size(), 1);
    ASSERT_TRUE(test_helpers::is_init_ul_rrc_msg_transfer_valid(
        cu_notifier.f1ap_ul_msgs.rbegin()->second, rnti, du_high_cfg.ran.cells[i].nr_cgi));
  }
}

TEST_P(du_high_many_cells_tester, when_ue_created_in_multiple_cells_then_traffic_flows_in_separate_cells)
{
  // Create one UE per cell.
  for (unsigned i = 0; i != GetParam().nof_cells; ++i) {
    cu_up_sim.last_drb_id.reset();

    rnti_t rnti = to_rnti(0x4601 + i);
    ASSERT_TRUE(add_ue(rnti, to_du_cell_index(i)));
    ASSERT_TRUE(run_rrc_setup(rnti));
    ASSERT_TRUE(run_ue_context_setup(rnti));
    // Wait for a CSI to be sent, otherwise the TBs will be small.
    ASSERT_TRUE(run_until_csi(to_du_cell_index(i), rnti));

    // Ensure DU<->CU-UP tunnel was created.
    ASSERT_EQ(cu_up_sim.last_drb_id.value(), drb_id_t::drb1);
  }

  // Forward several DRB PDUs to all UEs.
  const unsigned nof_pdcp_pdus = 100;
  const unsigned pdcp_pdu_size = 128;
  for (unsigned i = 0; i < nof_pdcp_pdus; ++i) {
    for (auto& bearer : cu_up_sim.bearers) {
      nru_dl_message f1u_pdu{
          .t_pdu = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, i, pdcp_pdu_size, i)};
      bearer.second.rx_notifier->on_new_pdu(f1u_pdu);
    }
  }

  // synchronization point - Wait for PDUs to reach RLC SDU queues.
  this->workers.flush_pending_dl_pdus();

  // Ensure DRB is active by verifying that the DRB PDUs are scheduled.
  std::vector<unsigned> bytes_sched_per_cell(GetParam().nof_cells, 0);
  for (unsigned i = 0; i != GetParam().nof_cells; ++i) {
    phy.cells[i].last_dl_data.reset();
  }

  // condition to stop experiment.
  auto stop_condition = [this, &bytes_sched_per_cell]() {
    for (unsigned c = 0; c != du_high_cfg.ran.cells.size(); ++c) {
      if (bytes_sched_per_cell[c] < nof_pdcp_pdus * pdcp_pdu_size) {
        // one of the cells still didn't send all the pending traffic.
        return false;
      }
    }
    return true;
  };
  // check if a PDSCH was scheduled.
  auto pdsch_grant_scheduled = [this]() {
    for (unsigned i = 0; i != du_high_cfg.ran.cells.size(); ++i) {
      if (phy.cells[i].last_dl_data.has_value() and not phy.cells[i].last_dl_data.value().ue_pdus.empty()) {
        return true;
      }
    }
    return false;
  };

  std::vector<unsigned> largest_pdu_per_cell(GetParam().nof_cells, 0);
  while (not stop_condition() and this->run_until([pdsch_grant_scheduled]() { return pdsch_grant_scheduled(); })) {
    for (unsigned c = 0; c != du_high_cfg.ran.cells.size(); ++c) {
      auto& phy_cell = phy.cells[c];
      if (not phy_cell.last_dl_data.has_value()) {
        continue;
      }
      for (unsigned i = 0; i != phy_cell.last_dl_data.value().ue_pdus.size(); ++i) {
        const auto& ue_grant = phy_cell.last_dl_res.value().dl_res->ue_grants[i];
        ASSERT_EQ(ue_grant.pdsch_cfg.rnti, to_rnti(0x4601 + c))
            << fmt::format("c-rnti={} scheduled in the wrong cell={}", ue_grant.pdsch_cfg.rnti, c);

        // Update the total number of bytes scheduled.
        if (ue_grant.pdsch_cfg.codewords[0].new_data and
            std::any_of(ue_grant.tb_list[0].lc_chs_to_sched.begin(),
                        ue_grant.tb_list[0].lc_chs_to_sched.end(),
                        // is DRB data
                        [](const dl_msg_lc_info& lc) { return lc.lcid.is_sdu() and lc.lcid.to_lcid() > LCID_SRB3; })) {
          unsigned pdu_size = phy_cell.last_dl_data.value().ue_pdus[i].pdu.get_buffer().size();
          bytes_sched_per_cell[c] += pdu_size;
          largest_pdu_per_cell[c] = std::max(largest_pdu_per_cell[c], pdu_size);
        }
      }
      phy.cells[c].last_dl_data.reset();
    }
  }

  ASSERT_TRUE(stop_condition()) << "Experiment did not finish when all cells transmitted pending traffic";

  // Cells should schedule equally large MAC PDUs
  for (unsigned c = 1; c != du_high_cfg.ran.cells.size(); ++c) {
    ASSERT_EQ(largest_pdu_per_cell[c], largest_pdu_per_cell[0])
        << fmt::format("cells {} and {} cannot schedule equally large PDUs", 0, c);
  }
}

TEST_P(du_high_many_cells_tester, when_cell_stopped_then_ues_are_released)
{
  // Create one UE per cell.
  for (unsigned i = 0; i != GetParam().nof_cells; ++i) {
    rnti_t rnti = to_rnti(0x4601 + i);
    ASSERT_TRUE(add_ue(rnti, to_du_cell_index(i)));
    ASSERT_TRUE(run_rrc_setup(rnti));
    ASSERT_TRUE(run_ue_context_setup(rnti));
    ASSERT_TRUE(run_until_csi(to_du_cell_index(i), rnti));
  }

  // Stop one cell via F1AP gNB-CU Configuration Update.
  const unsigned rem_cell_idx = test_rgen::uniform_int<unsigned>(0, GetParam().nof_cells - 1);
  this->cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message req = test_helpers::generate_gnb_cu_configuration_update_request(
      0, {}, {{nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(rem_cell_idx).value()}}});
  this->du_hi->get_f1ap_du().handle_message(req);

  // Expect release request for UE connected to the cell being stopped.
  const rnti_t crnti = to_rnti(0x4601 + rem_cell_idx);
  auto&        u     = ues.at(crnti);
  ASSERT_TRUE(this->run_until([this]() { return not this->cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_request(this->cu_notifier.f1ap_ul_msgs.rbegin()->second,
                                                                u.du_ue_id.value()));

  // CU releases UE.
  this->cu_notifier.f1ap_ul_msgs.clear();
  auto cmd_rel = test_helpers::generate_ue_context_release_command(
      u.cu_ue_id.value(), u.du_ue_id.value(), srb_id_t::srb1, byte_buffer::create({0x1, 0x2, 0x3}).value());
  this->du_hi->get_f1ap_du().handle_message(cmd_rel);
  ASSERT_TRUE(this->run_until([this]() { return not this->cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_TRUE(
      test_helpers::is_valid_ue_context_release_complete(this->cu_notifier.f1ap_ul_msgs.begin()->second, cmd_rel));
  this->cu_notifier.f1ap_ul_msgs.erase(this->cu_notifier.f1ap_ul_msgs.begin());

  // gNB-CU Configuration Update Complete.
  ASSERT_TRUE(this->run_until([this]() { return not this->cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_TRUE(
      test_helpers::is_gnb_cu_config_update_acknowledge_valid(this->cu_notifier.f1ap_ul_msgs.rbegin()->second, req));
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.size(), 1);

  // Ensure the cell is indeed stopped.
  const unsigned nof_test_slots = 100;
  for (unsigned i = 0; i != nof_test_slots; ++i) {
    this->run_slot();

    auto& cell_res = this->phy.cells[rem_cell_idx];
    ASSERT_FALSE(cell_res.last_dl_data.has_value());
    ASSERT_TRUE(not cell_res.last_dl_res.has_value() or (cell_res.last_dl_res.value().dl_pdcch_pdus.empty() and
                                                         cell_res.last_dl_res.value().ul_pdcch_pdus.empty()));
  }
}

TEST_P(du_high_many_cells_tester, when_cell_restarted_then_ues_can_be_created)
{
  // Create one UE per cell.
  for (unsigned i = 0; i != GetParam().nof_cells; ++i) {
    rnti_t rnti = to_rnti(0x4601 + i);
    ASSERT_TRUE(add_ue(rnti, to_du_cell_index(i)));
    ASSERT_TRUE(run_rrc_setup(rnti));
    ASSERT_TRUE(run_ue_context_setup(rnti));
    ASSERT_TRUE(run_until_csi(to_du_cell_index(i), rnti));
  }

  // Stop one cell via F1AP gNB-CU Configuration Update.
  const unsigned      rem_cell_idx = test_rgen::uniform_int<unsigned>(0, GetParam().nof_cells - 1);
  nr_cell_global_id_t rem_cgi{plmn_identity::test_value(), nr_cell_identity::create(rem_cell_idx).value()};
  f1ap_message        req = test_helpers::generate_gnb_cu_configuration_update_request(0, {}, {{rem_cgi}});
  this->du_hi->get_f1ap_du().handle_message(req);
  const rnti_t crnti   = to_rnti(0x4601 + rem_cell_idx);
  auto&        u       = ues.at(crnti);
  auto         cmd_rel = test_helpers::generate_ue_context_release_command(
      u.cu_ue_id.value(), u.du_ue_id.value(), srb_id_t::srb1, byte_buffer::create({0x1, 0x2, 0x3}).value());
  this->du_hi->get_f1ap_du().handle_message(cmd_rel);
  ASSERT_TRUE(this->run_until([this, &req]() {
    return not this->cu_notifier.f1ap_ul_msgs.empty() and test_helpers::is_gnb_cu_config_update_acknowledge_valid(
                                                              this->cu_notifier.f1ap_ul_msgs.rbegin()->second, req);
  }));

  // Random number of slots elapsed.
  const unsigned nof_slots_before_reactivation = test_rgen::uniform_int<unsigned>(0, 500);
  for (unsigned i = 0; i != nof_slots_before_reactivation; ++i) {
    this->run_slot();
  }

  // Restart the cell.
  this->cu_notifier.f1ap_ul_msgs.clear();
  f1ap_message req_restart = test_helpers::generate_gnb_cu_configuration_update_request(0, {{rem_cgi}}, {});
  this->du_hi->get_f1ap_du().handle_message(req_restart);
  ASSERT_TRUE(this->run_until([this, &req_restart]() {
    return not this->cu_notifier.f1ap_ul_msgs.empty() and
           test_helpers::is_gnb_cu_config_update_acknowledge_valid(this->cu_notifier.f1ap_ul_msgs.rbegin()->second,
                                                                   req_restart);
  }));

  // Create UE in the restarted cell.
  rnti_t new_crnti = to_rnti(0x4601 + GetParam().nof_cells);
  ASSERT_TRUE(add_ue(new_crnti, to_du_cell_index(rem_cell_idx)));
  ASSERT_TRUE(run_rrc_setup(new_crnti));
  ASSERT_TRUE(run_ue_context_setup(new_crnti));
  ASSERT_TRUE(run_until_csi(to_du_cell_index(rem_cell_idx), new_crnti));
}

INSTANTIATE_TEST_SUITE_P(du_high_many_cells_test_suite,
                         du_high_many_cells_tester,
                         ::testing::Values(test_params{2}, test_params{4}));

class du_high_many_cells_deferred_activation_test : public du_high_env_simulator, public testing::Test
{
protected:
  static constexpr unsigned nof_cells = 4;

  du_high_many_cells_deferred_activation_test() :
    du_high_env_simulator(
        du_high_env_sim_params{nof_cells, std::nullopt, std::nullopt, std::nullopt, std::nullopt, false})
  {
  }
};

TEST_F(du_high_many_cells_deferred_activation_test, when_cell_starts_deactivated_then_no_allocation_is_performed)
{
  const unsigned test_nof_slots = 256;

  // While the cells remain deactivated, there should not be any allocation.
  for (unsigned i = 0; i != test_nof_slots; ++i) {
    this->run_slot();

    for (unsigned c = 0; c != this->nof_cells; ++c) {
      if (phy.cells[c].last_dl_res.has_value()) {
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().dl_res->csi_rs.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().dl_res->bc.sibs.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().dl_res->bc.ssb_info.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().dl_res->dl_pdcchs.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().dl_res->ul_pdcchs.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().dl_res->ue_grants.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().dl_res->rar_grants.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().dl_res->paging_grants.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().dl_pdcch_pdus.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().ul_pdcch_pdus.empty());
        ASSERT_TRUE(phy.cells[c].last_dl_res.value().ssb_pdus.empty());
      }
      if (phy.cells[c].last_ul_res.has_value()) {
        ASSERT_TRUE(phy.cells[c].last_ul_res.value().ul_res->prachs.empty());
        ASSERT_TRUE(phy.cells[c].last_ul_res.value().ul_res->pucchs.empty());
        ASSERT_TRUE(phy.cells[c].last_ul_res.value().ul_res->puschs.empty());
        ASSERT_TRUE(phy.cells[c].last_ul_res.value().ul_res->srss.empty());
      }
    }
  }

  // CU sends gNB-CU-ConfigurationUpdateRequest to activate cells.
  this->cu_notifier.f1ap_ul_msgs.clear();
  std::vector<nr_cell_global_id_t> cgis;
  for (unsigned i = 0; i != nof_cells; ++i) {
    cgis.push_back(nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(i).value()});
  }
  auto req_msg = test_helpers::generate_gnb_cu_configuration_update_request(0, cgis, {});
  this->du_hi->get_f1ap_du().handle_message(req_msg);
  ASSERT_TRUE(this->run_until([this, req_msg]() {
    return not this->cu_notifier.f1ap_ul_msgs.empty() and test_helpers::is_gnb_cu_config_update_acknowledge_valid(
                                                              this->cu_notifier.f1ap_ul_msgs.rbegin()->second, req_msg);
  }));

  static_vector<bool, MAX_NOF_DU_CELLS> csi_rs_alloc(nof_cells, false);
  static_vector<bool, MAX_NOF_DU_CELLS> ssb_alloc(nof_cells, false);
  static_vector<bool, MAX_NOF_DU_CELLS> sib1_alloc(nof_cells, false);
  static_vector<bool, MAX_NOF_DU_CELLS> prach_alloc(nof_cells, false);
  for (unsigned i = 0; i != test_nof_slots; ++i) {
    this->run_slot();

    for (unsigned c = 0; c != nof_cells; ++c) {
      if (phy.cells[c].last_dl_res.has_value()) {
        csi_rs_alloc[c] |= not phy.cells[c].last_dl_res.value().dl_res->csi_rs.empty();
        ssb_alloc[c] |= not phy.cells[c].last_dl_res.value().dl_res->bc.ssb_info.empty();
        sib1_alloc[c] |= not phy.cells[c].last_dl_res.value().dl_res->bc.sibs.empty();
      }
      if (phy.cells[c].last_ul_res.has_value()) {
        prach_alloc[c] |= not phy.cells[c].last_ul_res.value().ul_res->prachs.empty();
      }
    }
  }
  // Check that all cells have been activated.
  for (unsigned c = 0; c != nof_cells; ++c) {
    ASSERT_TRUE(csi_rs_alloc[c]);
    ASSERT_TRUE(ssb_alloc[c]);
    ASSERT_TRUE(sib1_alloc[c]);
    ASSERT_TRUE(prach_alloc[c]);
  }
}

class du_high_many_cells_metrics_test : public du_high_env_simulator, public testing::Test
{
protected:
  static constexpr std::chrono::milliseconds METRICS_PERIOD{100};
  static constexpr unsigned                  nof_cells = 4;

  du_high_many_cells_metrics_test() :
    du_high_env_simulator([]() {
      du_high_env_sim_params params;
      params.nof_cells   = nof_cells;
      auto cfg           = create_du_high_configuration(params);
      cfg.metrics.period = METRICS_PERIOD;
      return cfg;
    }())
  {
  }
};

TEST_F(du_high_many_cells_metrics_test, when_du_metrics_are_configured_then_metrics_are_collected_periodically)
{
  // Create UEs.
  rnti_t rnti1 = to_rnti(0x4601);
  ASSERT_TRUE(add_ue(rnti1));
  ASSERT_TRUE(run_rrc_setup(rnti1));
  ASSERT_TRUE(run_ue_context_setup(rnti1));
  rnti_t rnti2 = to_rnti(0x4602);
  ASSERT_TRUE(add_ue(rnti2, to_du_cell_index(1)));
  ASSERT_TRUE(run_rrc_setup(rnti2));
  ASSERT_TRUE(run_ue_context_setup(rnti2));

  // Drop the first metrics report as it may not contain all slots.
  const unsigned metrics_period_slots =
      METRICS_PERIOD.count() * get_nof_slots_per_subframe(du_high_cfg.ran.cells[0].scs_common);
  const unsigned nof_test_slots = metrics_period_slots + 20;
  if (not run_until([this]() { return du_metrics.last_report.has_value(); }, nof_test_slots)) {
    this->workers.flush_pending_control_tasks();
    ASSERT_TRUE(du_metrics.last_report.has_value());
  }
  du_metrics.last_report.reset();

  // Wait for the next report.
  if (not run_until([this]() { return du_metrics.last_report.has_value(); }, nof_test_slots)) {
    this->workers.flush_pending_control_tasks();
    ASSERT_TRUE(du_metrics.last_report.has_value());
  }
  // Metrics received.
  auto& sched_metrics = du_metrics.last_report.value().mac.value().sched;
  ASSERT_EQ(sched_metrics.cells.size(), nof_cells);
  for (unsigned i = 0; i != nof_cells; ++i) {
    ASSERT_EQ(sched_metrics.cells[i].nof_slots, metrics_period_slots);
  }
  ASSERT_EQ(sched_metrics.cells[0].ue_metrics.size(), 1);
  ASSERT_EQ(sched_metrics.cells[1].ue_metrics.size(), 1);
  ASSERT_EQ(sched_metrics.cells[2].ue_metrics.size(), 0);
  ASSERT_EQ(sched_metrics.cells[3].ue_metrics.size(), 0);
  ASSERT_EQ(sched_metrics.cells[0].ue_metrics[0].rnti, rnti1);
  ASSERT_EQ(sched_metrics.cells[1].ue_metrics[0].rnti, rnti2);
  auto& mac_metrics = du_metrics.last_report.value().mac.value();
  ASSERT_EQ(mac_metrics.dl.cells.size(), nof_cells);
  for (unsigned i = 0; i != nof_cells; ++i) {
    ASSERT_EQ(mac_metrics.dl.cells[i].nof_slots, metrics_period_slots);
  }
  ASSERT_EQ(du_metrics.last_report.value().f1ap.value().ues.size(), 2);

  // After one metric period elapses, we should receive a new report.
  du_metrics.last_report.reset();
  if (not this->run_until([this]() { return du_metrics.last_report.has_value(); }, nof_test_slots)) {
    this->workers.flush_pending_control_tasks();
    ASSERT_TRUE(du_metrics.last_report.has_value());
  }
  // Metrics received.
  ASSERT_EQ(du_metrics.last_report.value().mac.value().sched.cells.size(), nof_cells);
  ASSERT_EQ(du_metrics.last_report.value().mac.value().dl.cells.size(), nof_cells);
  ASSERT_EQ(du_metrics.last_report.value().f1ap.value().ues.size(), 2);
}
