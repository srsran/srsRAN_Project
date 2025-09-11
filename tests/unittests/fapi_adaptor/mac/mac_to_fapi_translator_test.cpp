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

#include "../../../lib/fapi_adaptor/mac/mac_to_fapi_translator.h"
#include "messages/helpers.h"
#include "srsran/fapi/messages/dl_tti_request.h"
#include "srsran/fapi/messages/tx_data_request.h"
#include "srsran/fapi/messages/ul_dci_request.h"
#include "srsran/fapi/messages/ul_tti_request.h"
#include "srsran/fapi/slot_last_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "srsran/mac/mac_cell_timing_context.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;

namespace {

/// Spy implementation of a slot message gateway.
class slot_message_gateway_spy : public fapi::slot_message_gateway
{
  bool                          has_dl_tti_request_method_been_called = false;
  fapi::dl_tti_request_message  dl_tti_msg;
  bool                          has_ul_tti_request_method_been_called = false;
  fapi::ul_tti_request_message  ul_tti_msg;
  bool                          has_tx_data_request_method_been_called = false;
  fapi::tx_data_request_message tx_data_msg;
  bool                          has_ul_dci_request_method_been_called = false;
  fapi::ul_dci_request_message  ul_dci_msg;

public:
  bool has_dl_tti_request_method_called() const { return has_dl_tti_request_method_been_called; }
  bool has_ul_tti_request_method_called() const { return has_ul_tti_request_method_been_called; }
  bool has_tx_data_request_method_called() const { return has_tx_data_request_method_been_called; }
  bool has_ul_dci_request_method_called() const { return has_ul_dci_request_method_been_called; }
  const fapi::dl_tti_request_message&  dl_tti_request_msg() const { return dl_tti_msg; }
  const fapi::ul_tti_request_message&  ul_tti_request_msg() const { return ul_tti_msg; }
  const fapi::tx_data_request_message& tx_data_request_msg() const { return tx_data_msg; }
  const fapi::ul_dci_request_message&  ul_dci_request_msg() const { return ul_dci_msg; }

  void dl_tti_request(const fapi::dl_tti_request_message& msg) override
  {
    has_dl_tti_request_method_been_called = true;
    dl_tti_msg                            = msg;
  }

  void ul_tti_request(const fapi::ul_tti_request_message& msg) override
  {
    has_ul_tti_request_method_been_called = true;
    ul_tti_msg                            = msg;
  }

  void ul_dci_request(const fapi::ul_dci_request_message& msg) override
  {
    has_ul_dci_request_method_been_called = true;
    ul_dci_msg                            = msg;
  }

  void tx_data_request(const fapi::tx_data_request_message& msg) override
  {
    has_tx_data_request_method_been_called = true;
    tx_data_msg                            = msg;
  }
};

class slot_last_message_notifier_spy : public fapi::slot_last_message_notifier
{
  bool       has_been_notified = false;
  slot_point notif_slot;

public:
  void on_last_message(slot_point slot) override
  {
    has_been_notified = true;
    notif_slot        = slot;
  }

  bool       has_on_last_message_method_called() const { return has_been_notified; }
  slot_point slot() const { return notif_slot; }
};

class mac_cell_slot_handler_spy : public mac_cell_slot_handler
{
  error_event error;
  bool        has_been_notified = false;

public:
  void handle_slot_indication(const mac_cell_timing_context& context) override {}
  void handle_error_indication(slot_point sl_tx, error_event event) override
  {
    error             = event;
    has_been_notified = true;
  }
  void handle_stop_indication() override {}

  bool               has_error_been_notified() const { return has_been_notified; }
  const error_event& get_error() const { return error; }
};

class mac_to_fapi_translator_fixture : public ::testing::Test
{
protected:
  slot_message_gateway_spy                                                                          gateway_spy;
  slot_last_message_notifier_spy                                                                    notifier_spy;
  mac_cell_slot_handler_spy                                                                         slot_handler_spy;
  const unsigned                                                                                    nof_prbs = 51U;
  std::pair<std::unique_ptr<precoding_matrix_mapper>, std::unique_ptr<precoding_matrix_repository>> pm_tools =
      generate_precoding_matrix_tables(1, 0);
  std::pair<std::unique_ptr<uci_part2_correspondence_mapper>, std::unique_ptr<uci_part2_correspondence_repository>>
                         uci_part2_tools = generate_uci_part2_correspondence(1);
  mac_to_fapi_translator translator;

  mac_to_fapi_translator_fixture() :
    translator({0, nof_prbs},
               {srslog::fetch_basic_logger("FAPI"),
                gateway_spy,
                notifier_spy,
                std::move(std::get<0>(pm_tools)),
                std::move(std::get<0>(uci_part2_tools))})
  {
  }

  void configure_handler() { translator.set_cell_slot_handler(slot_handler_spy); }
};

} // namespace

TEST_F(mac_to_fapi_translator_fixture, valid_dl_sched_results_generate_correct_dl_tti_request)
{
  ASSERT_FALSE(gateway_spy.has_dl_tti_request_method_called());

  const unittests::mac_dl_sched_result_test_helper& result_test = srsran::unittests::build_valid_mac_dl_sched_result();
  const mac_dl_sched_result&                        result      = result_test.result;
  translator.on_new_downlink_scheduler_results(result);

  ASSERT_TRUE(gateway_spy.has_dl_tti_request_method_called());
  const fapi::dl_tti_request_message& msg = gateway_spy.dl_tti_request_msg();
  ASSERT_EQ(msg.pdus.size(), 5U);
  ASSERT_EQ((msg.pdus.begin() + 1)->pdu_type, fapi::dl_pdu_type::PDCCH);
  ASSERT_EQ(msg.pdus.back().pdu_type, fapi::dl_pdu_type::PDSCH);
  ASSERT_EQ(msg.pdus.front().pdu_type, fapi::dl_pdu_type::PDCCH);
  ASSERT_EQ(msg.pdus.front().pdcch_pdu.dl_dci.size(), 1U);
  ASSERT_EQ((msg.pdus.begin() + 1)->pdcch_pdu.dl_dci.size(), 3U);
  ASSERT_EQ((msg.pdus.end() - 2)->pdu_type, fapi::dl_pdu_type::SSB);
  ASSERT_EQ((msg.pdus.end() - 3)->pdu_type, fapi::dl_pdu_type::SSB);
  ASSERT_FALSE(slot_handler_spy.has_error_been_notified());
}

TEST_F(mac_to_fapi_translator_fixture, invalid_dl_sched_results_with_no_mac_slot_handler_aborts)
{
  ASSERT_FALSE(gateway_spy.has_dl_tti_request_method_called());

  unittests::mac_dl_sched_result_test_helper result_test = unittests::build_valid_mac_dl_sched_result();
  mac_dl_sched_result&                       result      = result_test.result;
  result.ssb_pdus.front().pci                            = INVALID_PCI;

  ASSERT_DEATH(
      { translator.on_new_downlink_scheduler_results(result); },
      "srsRAN ERROR: Dummy MAC cell slot handler cannot handle error indication");
}

TEST_F(mac_to_fapi_translator_fixture, invalid_ssb_in_dl_sched_results_reports_error)
{
  configure_handler();
  ASSERT_FALSE(gateway_spy.has_dl_tti_request_method_called());

  unittests::mac_dl_sched_result_test_helper result_test = unittests::build_valid_mac_dl_sched_result();
  mac_dl_sched_result&                       result      = result_test.result;
  result.ssb_pdus.front().pci                            = INVALID_PCI;

  translator.on_new_downlink_scheduler_results(result);

  ASSERT_TRUE(slot_handler_spy.has_error_been_notified());
  const auto& error = slot_handler_spy.get_error();
  ASSERT_TRUE(error.pdcch_discarded);
  ASSERT_TRUE(error.pdsch_discarded);
  ASSERT_FALSE(error.pusch_and_pucch_discarded);
}

TEST_F(mac_to_fapi_translator_fixture, invalid_ul_pdcch_in_dl_sched_results_reports_error)
{
  configure_handler();
  ASSERT_FALSE(gateway_spy.has_dl_tti_request_method_called());

  unittests::mac_dl_sched_result_test_helper result_test = unittests::build_valid_mac_dl_sched_result();
  const mac_dl_sched_result&                 result      = result_test.result;
  result_test.sched_result.ul_pdcchs.front().ctx.rnti    = rnti_t::INVALID_RNTI;

  translator.on_new_downlink_scheduler_results(result);

  ASSERT_TRUE(slot_handler_spy.has_error_been_notified());
  const auto& error = slot_handler_spy.get_error();
  ASSERT_TRUE(error.pdcch_discarded);
  ASSERT_FALSE(error.pdsch_discarded);
  ASSERT_FALSE(error.pusch_and_pucch_discarded);
}

TEST_F(mac_to_fapi_translator_fixture, valid_ul_sched_results_generate_correct_ul_tti_request)
{
  ASSERT_FALSE(gateway_spy.has_ul_tti_request_method_called());

  const unittests::mac_ul_sched_result_test_helper& result_test = unittests::build_valid_mac_ul_sched_result();
  const mac_ul_sched_result&                        result      = result_test.result;
  translator.on_new_uplink_scheduler_results(result);

  ASSERT_TRUE(gateway_spy.has_ul_tti_request_method_called());
  const fapi::ul_tti_request_message& msg = gateway_spy.ul_tti_request_msg();
  ASSERT_EQ(msg.pdus.size(), 4U);
  ASSERT_EQ(msg.pdus.front().pdu_type, fapi::ul_pdu_type::PRACH);
  ASSERT_EQ((msg.pdus.begin() + 1)->pdu_type, fapi::ul_pdu_type::PUSCH);
  ASSERT_EQ((msg.pdus.end() - 2)->pdu_type, fapi::ul_pdu_type::PUCCH);
  ASSERT_EQ(msg.pdus.back().pdu_type, fapi::ul_pdu_type::PUCCH);
  ASSERT_FALSE(slot_handler_spy.has_error_been_notified());
}

TEST_F(mac_to_fapi_translator_fixture, invalid_ul_sched_results_reports_error)
{
  configure_handler();
  ASSERT_FALSE(gateway_spy.has_ul_tti_request_method_called());

  unittests::mac_ul_sched_result_test_helper result_test = unittests::build_valid_mac_ul_sched_result();
  const mac_ul_sched_result&                 result      = result_test.result;
  result_test.sched_result.pucchs.front().crnti          = rnti_t::INVALID_RNTI;

  translator.on_new_uplink_scheduler_results(result);

  ASSERT_TRUE(gateway_spy.has_ul_tti_request_method_called());
  ASSERT_TRUE(slot_handler_spy.has_error_been_notified());
  const auto& error = slot_handler_spy.get_error();
  ASSERT_FALSE(error.pdcch_discarded);
  ASSERT_FALSE(error.pdsch_discarded);
  ASSERT_TRUE(error.pusch_and_pucch_discarded);
}

TEST_F(mac_to_fapi_translator_fixture, valid_dl_data_results_generate_correct_tx_data_request)
{
  ASSERT_FALSE(gateway_spy.has_tx_data_request_method_called());

  const unittests::mac_dl_sched_result_test_helper& result_test = unittests::build_valid_mac_dl_sched_result();
  const mac_dl_sched_result&                        result      = result_test.result;
  translator.on_new_downlink_scheduler_results(result);
  const unittests::mac_dl_data_result_test_helper& data_result = unittests::build_valid_mac_data_result();
  translator.on_new_downlink_data(data_result.result);

  ASSERT_TRUE(gateway_spy.has_tx_data_request_method_called());
  const fapi::tx_data_request_message& msg = gateway_spy.tx_data_request_msg();
  ASSERT_EQ(msg.pdus.size(), 1);
  ASSERT_FALSE(slot_handler_spy.has_error_been_notified());
}

TEST_F(mac_to_fapi_translator_fixture, invalid_dl_data_results_report_error)
{
  configure_handler();
  ASSERT_FALSE(gateway_spy.has_tx_data_request_method_called());

  const unittests::mac_dl_sched_result_test_helper& result_test = unittests::build_valid_mac_dl_sched_result();
  const mac_dl_sched_result&                        result      = result_test.result;
  translator.on_new_downlink_scheduler_results(result);

  unittests::mac_dl_data_result_test_helper data_result = unittests::build_valid_mac_data_result();
  data_result.result.si_pdus.clear();
  data_result.result.si_pdus.push_back({1, {}});
  translator.on_new_downlink_data(data_result.result);

  ASSERT_TRUE(gateway_spy.has_tx_data_request_method_called());
  ASSERT_TRUE(slot_handler_spy.has_error_been_notified());
  const auto& error = slot_handler_spy.get_error();
  ASSERT_FALSE(error.pdcch_discarded);
  ASSERT_TRUE(error.pdsch_discarded);
  ASSERT_FALSE(error.pusch_and_pucch_discarded);
}

TEST_F(mac_to_fapi_translator_fixture, valid_dl_data_results_generate_correct_ul_dci_request)
{
  ASSERT_FALSE(gateway_spy.has_ul_dci_request_method_called());

  const unittests::mac_dl_sched_result_test_helper& result_test = unittests::build_valid_mac_dl_sched_result();
  const mac_dl_sched_result&                        result      = result_test.result;
  translator.on_new_downlink_scheduler_results(result);

  ASSERT_TRUE(gateway_spy.has_ul_dci_request_method_called());
  const fapi::ul_dci_request_message& msg = gateway_spy.ul_dci_request_msg();
  ASSERT_EQ(msg.pdus.size(), 1);
  ASSERT_FALSE(slot_handler_spy.has_error_been_notified());
}

TEST_F(mac_to_fapi_translator_fixture, last_message_is_notified)
{
  slot_point slot(1, 1, 1);

  ASSERT_FALSE(notifier_spy.has_on_last_message_method_called());
  translator.on_cell_results_completion(slot);
  ASSERT_TRUE(notifier_spy.has_on_last_message_method_called());
  ASSERT_EQ(slot, notifier_spy.slot());
}

TEST_F(mac_to_fapi_translator_fixture, dl_tti_message_with_all_pdus_passes)
{
  const unittests::mac_dl_sched_result_test_helper& result_test =
      unittests::build_valid_mac_dl_sched_result_with_all_supported_pdus();
  const mac_dl_sched_result& result = result_test.result;
  translator.on_new_downlink_scheduler_results(result);

  const fapi::ul_dci_request_message& ul_dci_msg = gateway_spy.ul_dci_request_msg();
  ASSERT_EQ(ul_dci_msg.pdus.size(), MAX_UL_PDCCH_PDUS_PER_SLOT);

  const fapi::dl_tti_request_message& dl_tti_msg = gateway_spy.dl_tti_request_msg();
  // As the MAC struct does still not contain MAX_PRS_PDUS_PER_SLOT, substract the value.
  ASSERT_EQ(dl_tti_msg.pdus.size(), MAX_DL_PDUS_PER_SLOT - MAX_PRS_PDUS_PER_SLOT);

  ASSERT_FALSE(slot_handler_spy.has_error_been_notified());
}

TEST_F(mac_to_fapi_translator_fixture, tx_data_message_with_all_pdus_passes)
{
  const unittests::mac_dl_sched_result_test_helper& result_test =
      unittests::build_valid_mac_dl_sched_result_with_all_supported_pdus();
  const mac_dl_sched_result& result = result_test.result;
  translator.on_new_downlink_scheduler_results(result);

  const unittests::mac_dl_data_result_test_helper& data_result =
      unittests::build_valid_mac_data_result_with_all_supported_pdu();
  translator.on_new_downlink_data(data_result.result);

  const fapi::tx_data_request_message& tx_data_msg = gateway_spy.tx_data_request_msg();
  ASSERT_EQ(tx_data_msg.pdus.size(),
            MAX_SI_PDUS_PER_SLOT + MAX_RAR_PDUS_PER_SLOT + MAX_UE_PDUS_PER_SLOT + MAX_PAGING_PDUS_PER_SLOT);

  const fapi::ul_dci_request_message& ul_dci_msg = gateway_spy.ul_dci_request_msg();
  ASSERT_EQ(ul_dci_msg.pdus.size(), MAX_UL_PDCCH_PDUS_PER_SLOT);

  const fapi::dl_tti_request_message& dl_tti_msg = gateway_spy.dl_tti_request_msg();
  // As the MAC struct does still not contain MAX_PRS_PDUS_PER_SLOT, substract the value.
  ASSERT_EQ(dl_tti_msg.pdus.size(), MAX_DL_PDUS_PER_SLOT - MAX_PRS_PDUS_PER_SLOT);

  ASSERT_FALSE(slot_handler_spy.has_error_been_notified());
}

TEST_F(mac_to_fapi_translator_fixture, ul_tti_message_with_all_pdus_passes)
{
  srslog::fetch_basic_logger("FAPI").set_level(srslog::basic_levels::debug);

  ASSERT_FALSE(gateway_spy.has_ul_tti_request_method_called());

  const unittests::mac_ul_sched_result_test_helper& result_test =
      unittests::build_valid_mac_ul_sched_result_with_all_supported_pdus();
  const mac_ul_sched_result& result = result_test.result;
  translator.on_new_uplink_scheduler_results(result);

  const fapi::ul_tti_request_message& msg = gateway_spy.ul_tti_request_msg();
  ASSERT_EQ(msg.pdus.size(), MAX_UL_PDUS_PER_SLOT);

  ASSERT_FALSE(slot_handler_spy.has_error_been_notified());
}
