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

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_paging_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_paging_test() : cu_cp_test_environment(cu_cp_test_env_params{})
  {
    // Run NG setup to completion.
    run_ng_setup();
  }

  [[nodiscard]] unsigned connect_du()
  {
    // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure).
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    return ret.value();
  }

  [[nodiscard]] unsigned setup_du(const f1ap_message& f1_setup_request)
  {
    // Setup DU.
    unsigned tmp_du_idx = connect_du();

    get_du(tmp_du_idx).push_ul_pdu(f1_setup_request);
    EXPECT_TRUE(this->wait_for_f1ap_tx_pdu(tmp_du_idx, f1ap_pdu));
    return tmp_du_idx;
  }

  [[nodiscard]] bool send_ngap_paging(unsigned du_idx_, const ngap_message& paging_msg)
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx_).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");

    get_amf().push_tx_pdu(paging_msg);
    return true;
  }

  [[nodiscard]] bool send_minimal_ngap_paging_and_await_f1ap_paging(unsigned du_idx_)
  {
    // Inject NGAP Paging and wait for F1AP Paging.
    if (!send_ngap_paging(du_idx_, generate_valid_minimal_paging_message())) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx_, f1ap_pdu), "Failed to receive F1AP Paging");
    report_fatal_error_if_not(test_helpers::is_valid_paging(f1ap_pdu), "Invalid F1AP Paging");
    report_fatal_error_if_not(is_valid_minimal_paging_result(f1ap_pdu), "Invalid minimal F1AP Paging");
    return true;
  }

  [[nodiscard]] bool send_ngap_paging_and_await_f1ap_paging(unsigned du_idx_)
  {
    // Inject NGAP Paging and wait for F1AP Paging.
    if (!send_ngap_paging(du_idx_, generate_valid_paging_message())) {
      return false;
    }
    report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx_, f1ap_pdu), "Failed to receive F1AP Paging");
    report_fatal_error_if_not(test_helpers::is_valid_paging(f1ap_pdu), "Invalid F1AP Paging");
    report_fatal_error_if_not(is_valid_paging_result(f1ap_pdu), "Invalid F1AP Paging");
    return true;
  }

  [[nodiscard]] bool is_valid_minimal_paging_result(const f1ap_message& msg)
  {
    const auto& paging_msg = msg.pdu.init_msg().value.paging();

    // Check UE ID idx value.
    if (paging_msg->ue_id_idx_value.idx_len10().to_number() != (279089024671 % 1024)) {
      test_logger.error("UE ID idx value mismatch {} != {}",
                        paging_msg->ue_id_idx_value.idx_len10().to_number(),
                        (279089024671 % 1024));
      return false;
    }

    // Check paging ID.
    if (paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number() != 279089024671) {
      test_logger.error("Paging ID mismatch {} != {}",
                        paging_msg->paging_id.cn_ue_paging_id().five_g_s_tmsi().to_number(),
                        279089024671);
      return false;
    }

    // Check paging cell list.
    if (paging_msg->paging_cell_list.size() != 1) {
      test_logger.error("Paging cell list size mismatch {} != {}", paging_msg->paging_cell_list.size(), 1);
      return false;
    }

    const auto&      paging_cell_item = paging_msg->paging_cell_list[0].value().paging_cell_item();
    nr_cell_identity nci              = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
    if (paging_cell_item.nr_cgi.nr_cell_id.to_number() != nci.value()) {
      test_logger.error("NR CGI NCI mismatch {} != {}", paging_cell_item.nr_cgi.nr_cell_id.to_number(), nci);
      return false;
    }
    if (paging_cell_item.nr_cgi.plmn_id.to_string() != "00f110") {
      test_logger.error("NR CGI PLMN mismatch {} != 00f110", paging_cell_item.nr_cgi.plmn_id.to_string());
      return false;
    }

    return true;
  }

  [[nodiscard]] bool is_valid_paging_result(const f1ap_message& msg)
  {
    if (!is_valid_minimal_paging_result(msg)) {
      return false;
    }

    const auto& paging_msg = msg.pdu.init_msg().value.paging();

    // Check paging DRX.
    if (!paging_msg->paging_drx_present) {
      return false;
    }
    if (paging_msg->paging_drx.to_number() != 64) {
      test_logger.error("Paging DRX mismatch {} != {}", paging_msg->paging_drx.to_number(), 64);
      return false;
    }

    // Check paging prio.
    if (!paging_msg->paging_prio_present) {
      return false;
    }
    if (paging_msg->paging_prio.to_number() != 5) {
      test_logger.error("Paging prio mismatch {} != {}", paging_msg->paging_prio.to_number(), 5);
      return false;
    }

    // Check paging origin.
    if (!paging_msg->paging_origin_present) {
      return false;
    }
    if ((std::string)paging_msg->paging_origin.to_string() != "non-3gpp") {
      test_logger.error("Paging origin mismatch {} != non-3gpp", paging_msg->paging_origin.to_string());
      return false;
    }

    return true;
  }

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
};

TEST_F(cu_cp_paging_test, when_du_connection_not_finished_then_paging_is_not_sent_to_du)
{
  // Connect DU (note that this creates a DU processor, but the DU is only connected after the F1Setup procedure).
  unsigned du_idx = connect_du();

  // Inject NGAP Paging with only mandatory values.
  ASSERT_TRUE(send_ngap_paging(du_idx, generate_valid_minimal_paging_message()));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the DU.
  ASSERT_FALSE(this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_no_du_for_tac_exists_then_paging_is_not_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject NGAP Paging with unknown TAC.
  ngap_message paging_msg = generate_valid_minimal_paging_message();
  paging_msg.pdu.init_msg().value.paging()->tai_list_for_paging[0].tai.tac.from_number(8);
  ASSERT_TRUE(send_ngap_paging(du_idx, paging_msg));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the DU.
  ASSERT_FALSE(this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_assist_data_for_paging_for_unknown_tac_is_included_then_paging_is_not_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject NGAP Paging with unknown TAC but assist data for paging.
  ngap_message paging_msg = generate_valid_paging_message();
  paging_msg.pdu.init_msg().value.paging()->tai_list_for_paging[0].tai.tac.from_number(8);
  ASSERT_TRUE(send_ngap_paging(du_idx, paging_msg));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the DU.
  ASSERT_FALSE(this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_invalid_paging_message_received_then_paging_is_not_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject invalid NGAP Paging.
  ASSERT_TRUE(send_ngap_paging(du_idx, generate_invalid_paging_message()));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the DU.
  ASSERT_FALSE(this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_valid_paging_message_received_then_paging_is_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject NGAP Paging with only mandatory values and await F1AP Paging.
  ASSERT_TRUE(send_minimal_ngap_paging_and_await_f1ap_paging(du_idx));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";
}

TEST_F(cu_cp_paging_test, when_valid_paging_message_received_then_paging_is_only_sent_to_du_with_matching_tac)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Connect second DU and run F1Setup.
  unsigned du_idx2 = setup_du(test_helpers::generate_f1_setup_request(
      int_to_gnb_du_id(0x12), {{nr_cell_identity::create(6577).value(), 1, 8}}));

  // Inject NGAP Paging with only mandatory values and await F1AP Paging.
  ASSERT_TRUE(send_minimal_ngap_paging_and_await_f1ap_paging(du_idx));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the second DU.
  ASSERT_FALSE(this->get_du(du_idx2).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_valid_paging_message_received_then_paging_is_only_sent_to_du_with_matching_nci)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Connect second DU and run F1Setup.
  unsigned du_idx2 = setup_du(test_helpers::generate_f1_setup_request(
      int_to_gnb_du_id(0x12), {{nr_cell_identity::create(6577).value(), 1, 7}}));

  // Connect third DU and run F1Setup.
  unsigned du_idx3 = setup_du(test_helpers::generate_f1_setup_request(
      int_to_gnb_du_id(0x13), {{nr_cell_identity::create(6578).value(), 2, 7}}));

  // Drop the second DU connection to test that paging is not sent to disconnected DUs.
  ASSERT_TRUE(drop_du_connection(du_idx2));

  // Inject NGAP Paging with only mandatory values and await F1AP Paging.
  ASSERT_TRUE(send_minimal_ngap_paging_and_await_f1ap_paging(du_idx));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";

  // Make sure that no paging was sent to the third DU.
  ASSERT_FALSE(this->get_du(du_idx3).try_pop_dl_pdu(f1ap_pdu));
}

TEST_F(cu_cp_paging_test, when_valid_paging_message_with_optional_values_received_then_paging_is_sent_to_du)
{
  // Connect DU and run F1Setup.
  unsigned du_idx = setup_du(test_helpers::generate_f1_setup_request());

  // Inject NGAP Paging with optional values and await F1AP Paging.
  ASSERT_TRUE(send_ngap_paging_and_await_f1ap_paging(du_idx));

  // Make sure he paging request is in the metrics.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ngaps[0].metrics.nof_cn_initiated_paging_requests, 1) << "Paging request should be in the metrics";
}
