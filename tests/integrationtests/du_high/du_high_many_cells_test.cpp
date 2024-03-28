/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "test_utils/du_high_env_simulator.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
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

INSTANTIATE_TEST_SUITE_P(du_high_many_cells_test_suite,
                         du_high_many_cells_tester,
                         ::testing::Values(test_params{2}, test_params{4}));
