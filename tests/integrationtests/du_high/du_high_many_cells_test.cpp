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

#include "test_utils/du_high_env_simulator.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/pdcp/pdcp_pdu_generator.h"
#include "tests/test_doubles/scheduler/scheduler_result_test.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/common/f1ap_message.h"
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
  ASSERT_EQ(this->cu_notifier.last_f1ap_msgs.size(), 1);
  ASSERT_EQ(this->cu_notifier.last_f1ap_msgs.back().pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.last_f1ap_msgs.back().pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
  asn1::f1ap::f1_setup_request_s& f1_setup =
      this->cu_notifier.last_f1ap_msgs.back().pdu.init_msg().value.f1_setup_request();

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
    cu_notifier.last_f1ap_msgs.clear();

    // Inject UL-CCCH with RRC Setup.
    rnti_t rnti = to_rnti(0x4601 + i);
    du_hi->get_pdu_handler().handle_rx_data_indication(
        test_helpers::create_ccch_message(next_slot, rnti, to_du_cell_index(i)));

    // Wait for F1AP message to be propagated to the CU-CP.
    this->run_until([this]() { return not cu_notifier.last_f1ap_msgs.empty(); });

    // Ensure the F1AP Initial UL RRC message is correct.
    ASSERT_EQ(cu_notifier.last_f1ap_msgs.size(), 1);
    ASSERT_TRUE(test_helpers::is_init_ul_rrc_msg_transfer_valid(
        cu_notifier.last_f1ap_msgs.back(), rnti, du_high_cfg.cells[i].nr_cgi));
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
  const unsigned nof_pdcp_pdus = 100, pdcp_pdu_size = 128;
  for (unsigned i = 0; i < nof_pdcp_pdus; ++i) {
    for (unsigned c = 0; c != GetParam().nof_cells; ++c) {
      nru_dl_message f1u_pdu{
          .t_pdu = test_helpers::create_pdcp_pdu(pdcp_sn_size::size12bits, /* is_srb = */ false, i, pdcp_pdu_size, i)};
      cu_up_sim.created_du_notifs[c]->on_new_pdu(f1u_pdu);
    }
  }

  // Ensure DRB is active by verifying that the DRB PDUs are scheduled.
  std::vector<unsigned> bytes_sched_per_cell(GetParam().nof_cells, 0);
  for (unsigned i = 0; i != GetParam().nof_cells; ++i) {
    phy.cells[i].last_dl_data.reset();
  }

  // condition to stop experiment.
  auto stop_condition = [this, &bytes_sched_per_cell]() {
    for (unsigned c = 0; c != du_high_cfg.cells.size(); ++c) {
      if (bytes_sched_per_cell[c] < nof_pdcp_pdus * pdcp_pdu_size) {
        // one of the cells still didn't send all the pending traffic.
        return false;
      }
    }
    return true;
  };
  // check if a PDSCH was scheduled.
  auto pdsch_grant_scheduled = [this]() {
    for (unsigned i = 0; i != du_high_cfg.cells.size(); ++i) {
      if (phy.cells[i].last_dl_data.has_value() and not phy.cells[i].last_dl_data.value().ue_pdus.empty()) {
        return true;
      }
    }
    return false;
  };

  std::vector<unsigned> largest_pdu_per_cell(GetParam().nof_cells, 0);
  while (not stop_condition() and this->run_until([pdsch_grant_scheduled]() { return pdsch_grant_scheduled(); })) {
    for (unsigned c = 0; c != du_high_cfg.cells.size(); ++c) {
      auto& phy_cell = phy.cells[c];
      if (not phy_cell.last_dl_data.has_value()) {
        continue;
      }
      for (unsigned i = 0; i != phy_cell.last_dl_data.value().ue_pdus.size(); ++i) {
        auto& ue_grant = phy_cell.last_dl_res.value().dl_res->ue_grants[i];
        ASSERT_EQ(ue_grant.pdsch_cfg.rnti, to_rnti(0x4601 + c))
            << fmt::format("c-rnti={} scheduled in the wrong cell={}", ue_grant.pdsch_cfg.rnti, c);

        // Update the total number of bytes scheduled.
        if (ue_grant.pdsch_cfg.codewords[0].new_data and
            std::any_of(ue_grant.tb_list[0].lc_chs_to_sched.begin(),
                        ue_grant.tb_list[0].lc_chs_to_sched.end(),
                        // is DRB data
                        [](const dl_msg_lc_info& lc) { return lc.lcid.is_sdu() and lc.lcid.to_lcid() >= LCID_SRB3; })) {
          unsigned pdu_size = phy_cell.last_dl_data.value().ue_pdus[i].pdu.size();
          bytes_sched_per_cell[c] += pdu_size;
          largest_pdu_per_cell[c] = std::max(largest_pdu_per_cell[c], pdu_size);
        }
      }
      phy.cells[c].last_dl_data.reset();
    }
  }

  ASSERT_TRUE(stop_condition()) << "Experiment did not finish when all cells transmitted pending traffic";

  // Cells should schedule equally large MAC PDUs
  for (unsigned c = 1; c != du_high_cfg.cells.size(); ++c) {
    ASSERT_EQ(largest_pdu_per_cell[c], largest_pdu_per_cell[0])
        << fmt::format("cells {} and {} cannot schedule equally large PDUs", 0, c);
  }
}

INSTANTIATE_TEST_SUITE_P(du_high_many_cells_test_suite,
                         du_high_many_cells_tester,
                         ::testing::Values(test_params{2}, test_params{4}));
