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

#include "srsran/adt/blocking_queue.h"
#include "srsran/asn1/e1ap/common.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_e1_handler.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/gateways/e1_local_connector_factory.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/io/io_broker_factory.h"
#include <future>
#include <gtest/gtest.h>

using namespace srsran;

class dummy_dlt_pcap final : public dlt_pcap
{
public:
  bool                        enabled = false;
  bool                        closed  = false;
  blocking_queue<byte_buffer> last_sdus{16};

  void         close() override { closed = true; }
  bool         is_write_enabled() const override { return enabled; }
  void         push_pdu(const_span<uint8_t> pdu) override { last_sdus.push_blocking(byte_buffer::create(pdu).value()); }
  virtual void push_pdu(byte_buffer pdu) override { last_sdus.push_blocking(std::move(pdu)); }
};

class e1_link : public srs_cu_cp::cu_cp_e1_handler
{
public:
  class rx_pdu_notifier : public e1ap_message_notifier
  {
  public:
    rx_pdu_notifier(const std::string&            name_,
                    blocking_queue<e1ap_message>& rx_pdus_,
                    std::promise<void>            eof_received_) :
      name(name_), rx_pdus(rx_pdus_), eof_received(std::move(eof_received_))
    {
    }
    ~rx_pdu_notifier() override
    {
      eof_received.set_value();
      logger.info("{}: RX PDU notifier destroyed", name);
    }

    void on_new_message(const e1ap_message& msg) override { rx_pdus.push_blocking(msg); }

    const std::string             name;
    blocking_queue<e1ap_message>& rx_pdus;
    std::promise<void>            eof_received;
    srslog::basic_logger&         logger = srslog::fetch_basic_logger("TEST");
  };

  e1_link(bool use_sctp, bool pcap_enabled)
  {
    pcap.enabled = pcap_enabled;

    if (use_sctp) {
      broker    = create_io_broker(io_broker_type::epoll);
      connector = create_e1_local_connector(e1_local_sctp_connector_config{pcap, *broker});
    } else {
      connector = create_e1_local_connector(e1_local_connector_config{pcap});
    }

    connector->attach_cu_cp(*this);

    // Connect client to server.
    connect_client();
  }

  std::unique_ptr<e1ap_message_notifier>
  handle_new_cu_up_connection(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier) override
  {
    // Note: May be called from io broker thread.
    cu_cp_tx_pdu_notifier = std::move(e1ap_tx_pdu_notifier);
    std::promise<void> eof_signal;
    cu_cp_gw_assoc_close_signaled = eof_signal.get_future();

    logger.info("CU-CP handled new DU connection");
    connection_complete_signal.set_value();

    return std::make_unique<rx_pdu_notifier>("CU-CP", cu_rx_pdus, std::move(eof_signal));
  }

  std::unique_ptr<io_broker>          broker;
  dummy_dlt_pcap                      pcap;
  std::unique_ptr<e1_local_connector> connector;
  srslog::basic_logger&               logger = srslog::fetch_basic_logger("TEST");

  blocking_queue<e1ap_message> cu_rx_pdus{128};
  blocking_queue<e1ap_message> cu_up_rx_pdus{128};

  std::future<void>                      cu_cp_gw_assoc_close_signaled;
  std::future<void>                      cu_up_gw_assoc_close_signaled;
  std::unique_ptr<e1ap_message_notifier> cu_cp_tx_pdu_notifier;
  std::unique_ptr<e1ap_message_notifier> cu_up_tx_pdu_notifier;

protected:
  void connect_client()
  {
    // Connect client to server.
    std::promise<void> eof_signal;
    cu_up_gw_assoc_close_signaled = eof_signal.get_future();
    cu_up_tx_pdu_notifier         = connector->handle_cu_up_connection_request(
        std::make_unique<rx_pdu_notifier>("CU-UP", cu_up_rx_pdus, std::move(eof_signal)));

    // Wait for server to receive connection.
    std::future<void> connection_completed = connection_complete_signal.get_future();
    connection_completed.wait();
    logger.info("CU-UP connection to CU-CP is complete");
  }

  std::promise<void> connection_complete_signal;
};

class e1_gateway_link_test : public ::testing::TestWithParam<bool>
{
protected:
  e1_gateway_link_test()
  {
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SCTP-GW").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("CU-CP-E1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("CU-UP-E1").set_level(srslog::basic_levels::debug);
  }
  ~e1_gateway_link_test() override { srslog::flush(); }

  void create_link(bool pcap_enabled = false)
  {
    bool use_sctp = GetParam();
    link          = std::make_unique<e1_link>(use_sctp, pcap_enabled);
  }

  void send_to_cu_up(const e1ap_message& msg) { link->cu_cp_tx_pdu_notifier->on_new_message(msg); }

  void send_to_cu_cp(const e1ap_message& msg) { link->cu_up_tx_pdu_notifier->on_new_message(msg); }

  bool pop_cu_rx_pdu(e1ap_message& msg)
  {
    bool res;
    msg = link->cu_rx_pdus.pop_blocking(&res);
    return res;
  }

  bool pop_cu_up_rx_pdu(e1ap_message& msg)
  {
    bool res;
    msg = link->cu_up_rx_pdus.pop_blocking(&res);
    return res;
  }

  srslog::basic_logger&    logger = srslog::fetch_basic_logger("TEST");
  std::unique_ptr<e1_link> link;
};

static e1ap_message create_test_message()
{
  e1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);
  asn1::e1ap::bearer_context_mod_request_s& bearer_mod = msg.pdu.init_msg().value.bearer_context_mod_request();
  bearer_mod->gnb_cu_cp_ue_e1ap_id                     = 0;
  bearer_mod->gnb_cu_up_ue_e1ap_id                     = 1;
  return msg;
}

static byte_buffer pack(const e1ap_message& msg)
{
  byte_buffer pdu;
  {
    asn1::bit_ref bref{pdu};
    report_fatal_error_if_not(msg.pdu.pack(bref) == asn1::SRSASN_SUCCESS, "Failed to pack E1AP PDU");
  }
  return pdu;
}

static bool is_equal(const e1ap_message& lhs, const e1ap_message& rhs)
{
  byte_buffer lhs_pdu = pack(lhs);
  byte_buffer rhs_pdu = pack(rhs);
  return lhs_pdu == rhs_pdu;
}

TEST_P(e1_gateway_link_test, when_cu_up_sends_msg_then_cu_receives_msg)
{
  create_link();

  e1ap_message orig_msg = create_test_message();
  send_to_cu_cp(orig_msg);

  e1ap_message dest_msg;
  ASSERT_TRUE(pop_cu_rx_pdu(dest_msg));
  ASSERT_TRUE(is_equal(orig_msg, dest_msg));
}

TEST_P(e1_gateway_link_test, when_cu_cp_sends_msg_then_cu_up_receives_msg)
{
  create_link();

  e1ap_message orig_msg = create_test_message();
  send_to_cu_up(orig_msg);

  e1ap_message dest_msg;
  ASSERT_TRUE(pop_cu_up_rx_pdu(dest_msg));
  ASSERT_TRUE(is_equal(orig_msg, dest_msg));
}

TEST_P(e1_gateway_link_test, when_pcap_writer_disabled_then_no_pcap_is_written)
{
  create_link(false);

  e1ap_message orig_msg = create_test_message();
  send_to_cu_up(orig_msg);
  e1ap_message dest_msg;
  ASSERT_TRUE(pop_cu_up_rx_pdu(dest_msg));
  byte_buffer sdu;
  ASSERT_FALSE(link->pcap.last_sdus.try_pop(sdu));

  send_to_cu_cp(orig_msg);
  ASSERT_TRUE(pop_cu_rx_pdu(dest_msg));
  ASSERT_FALSE(link->pcap.last_sdus.try_pop(sdu));
}

TEST_P(e1_gateway_link_test, when_pcap_writer_enabled_then_pcap_is_written)
{
  create_link(true);

  e1ap_message orig_msg = create_test_message();

  send_to_cu_up(orig_msg);
  e1ap_message dest_msg;
  ASSERT_TRUE(pop_cu_up_rx_pdu(dest_msg));
  bool        popped = false;
  byte_buffer sdu    = link->pcap.last_sdus.pop_blocking(&popped);
  ASSERT_TRUE(popped);
  ASSERT_FALSE(link->pcap.last_sdus.try_pop(sdu));

  send_to_cu_cp(orig_msg);
  ASSERT_TRUE(pop_cu_rx_pdu(dest_msg));
  popped = false;
  sdu    = link->pcap.last_sdus.pop_blocking(&popped);
  ASSERT_TRUE(popped);
  ASSERT_FALSE(link->pcap.last_sdus.try_pop(sdu));
}

TEST_P(e1_gateway_link_test, when_cu_tx_pdu_notifier_is_closed_then_connection_closes)
{
  create_link();

  // The CU-CP resets its E1 Tx notifier.
  logger.info("Closing CU-CP Tx path...");
  link->cu_cp_tx_pdu_notifier.reset();

  // Wait for GW to report to CU-UP that the association is closed.
  link->cu_up_gw_assoc_close_signaled.wait();
}

TEST_P(e1_gateway_link_test, when_cu_up_tx_pdu_notifier_is_closed_then_connection_closes)
{
  create_link();

  // The CU-UP resets its E1 Tx notifier.
  logger.info("Closing CU-UP Tx path...");
  link->cu_up_tx_pdu_notifier.reset();

  // Wait for GW to report to CU that the association is closed.
  link->cu_cp_gw_assoc_close_signaled.wait();
}

INSTANTIATE_TEST_SUITE_P(e1_gateway_link_tests, e1_gateway_link_test, ::testing::Values(true, false));
