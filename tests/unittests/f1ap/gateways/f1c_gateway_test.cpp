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
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/gateways/f1c_local_connector_factory.h"
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

class f1c_link : public srs_cu_cp::cu_cp_f1c_handler
{
public:
  class rx_pdu_notifier : public f1ap_message_notifier
  {
  public:
    rx_pdu_notifier(const std::string&            name_,
                    blocking_queue<f1ap_message>& rx_pdus_,
                    std::promise<void>            eof_received_) :
      name(name_), rx_pdus(rx_pdus_), eof_received(std::move(eof_received_))
    {
    }
    ~rx_pdu_notifier()
    {
      eof_received.set_value();
      logger.info("{}: RX PDU notifier destroyed", name);
    }

    void on_new_message(const f1ap_message& msg) override { rx_pdus.push_blocking(msg); }

    const std::string             name;
    blocking_queue<f1ap_message>& rx_pdus;
    std::promise<void>            eof_received;
    srslog::basic_logger&         logger = srslog::fetch_basic_logger("TEST");
  };

  f1c_link(bool use_sctp, bool pcap_enabled)
  {
    pcap.enabled = pcap_enabled;

    if (use_sctp) {
      broker    = create_io_broker(io_broker_type::epoll);
      connector = create_f1c_local_connector(f1c_local_sctp_connector_config{pcap, *broker});
    } else {
      connector = create_f1c_local_connector(f1c_local_connector_config{pcap});
    }

    connector->attach_cu_cp(*this);

    // Connect client to server.
    connect_client();
  }

  std::unique_ptr<f1ap_message_notifier>
  handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier) override
  {
    // Note: May be called from io broker thread.
    cu_cp_tx_pdu_notifier = std::move(f1ap_tx_pdu_notifier);
    std::promise<void> eof_signal;
    cu_gw_assoc_close_signaled = eof_signal.get_future();

    logger.info("CU-CP handled new DU connection");
    connection_complete_signal.set_value();

    return std::make_unique<rx_pdu_notifier>("CU-CP", cu_rx_pdus, std::move(eof_signal));
  }

  std::unique_ptr<io_broker>           broker;
  dummy_dlt_pcap                       pcap;
  std::unique_ptr<f1c_local_connector> connector;
  srslog::basic_logger&                logger = srslog::fetch_basic_logger("TEST");

  blocking_queue<f1ap_message> cu_rx_pdus{128};
  blocking_queue<f1ap_message> du_rx_pdus{128};

  std::future<void>                      cu_gw_assoc_close_signaled;
  std::future<void>                      du_gw_assoc_close_signaled;
  std::unique_ptr<f1ap_message_notifier> cu_cp_tx_pdu_notifier;
  std::unique_ptr<f1ap_message_notifier> du_tx_pdu_notifier;

protected:
  void connect_client()
  {
    // Connect client to server.
    std::promise<void> eof_signal;
    du_gw_assoc_close_signaled = eof_signal.get_future();
    du_tx_pdu_notifier         = connector->handle_du_connection_request(
        std::make_unique<rx_pdu_notifier>("DU", du_rx_pdus, std::move(eof_signal)));

    // Wait for server to receive connection.
    std::future<void> connection_completed = connection_complete_signal.get_future();
    connection_completed.wait();
    logger.info("DU connection to CU-CP is complete");
  }

  std::promise<void> connection_complete_signal;
};

class f1c_gateway_link_test : public ::testing::TestWithParam<bool>
{
protected:
  f1c_gateway_link_test()
  {
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SCTP-GW").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("CU-CP-F1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("DU-F1").set_level(srslog::basic_levels::debug);
  }
  ~f1c_gateway_link_test() override { srslog::flush(); }

  void create_link(bool pcap_enabled = false)
  {
    bool use_sctp = GetParam();
    link          = std::make_unique<f1c_link>(use_sctp, pcap_enabled);
  }

  void send_to_du(const f1ap_message& msg) { link->cu_cp_tx_pdu_notifier->on_new_message(msg); }

  void send_to_cu(const f1ap_message& msg) { link->du_tx_pdu_notifier->on_new_message(msg); }

  bool pop_cu_rx_pdu(f1ap_message& msg)
  {
    bool res;
    msg = link->cu_rx_pdus.pop_blocking(&res);
    return res;
  }

  bool pop_du_rx_pdu(f1ap_message& msg)
  {
    bool res;
    msg = link->du_rx_pdus.pop_blocking(&res);
    return res;
  }

  srslog::basic_logger&     logger = srslog::fetch_basic_logger("TEST");
  std::unique_ptr<f1c_link> link;
};

static f1ap_message create_test_message()
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER);
  asn1::f1ap::dl_rrc_msg_transfer_s& transfer = msg.pdu.init_msg().value.dl_rrc_msg_transfer();
  transfer->gnb_cu_ue_f1ap_id                 = 0;
  transfer->gnb_du_ue_f1ap_id                 = 1;
  transfer->srb_id                            = 0;
  transfer->rrc_container                     = byte_buffer::create({0x01, 0x02, 0x03, 0x04}).value();
  return msg;
}

static byte_buffer pack(const f1ap_message& msg)
{
  byte_buffer pdu;
  {
    asn1::bit_ref bref{pdu};
    report_fatal_error_if_not(msg.pdu.pack(bref) == asn1::SRSASN_SUCCESS, "Failed to pack F1AP PDU");
  }
  return pdu;
}

static bool is_equal(const f1ap_message& lhs, const f1ap_message& rhs)
{
  byte_buffer lhs_pdu = pack(lhs);
  byte_buffer rhs_pdu = pack(rhs);
  return lhs_pdu == rhs_pdu;
}

TEST_P(f1c_gateway_link_test, when_du_sends_msg_then_cu_receives_msg)
{
  create_link();

  f1ap_message orig_msg = create_test_message();
  send_to_cu(orig_msg);

  f1ap_message dest_msg;
  ASSERT_TRUE(pop_cu_rx_pdu(dest_msg));
  ASSERT_TRUE(is_equal(orig_msg, dest_msg));
}

TEST_P(f1c_gateway_link_test, when_cu_sends_msg_then_du_receives_msg)
{
  create_link();

  f1ap_message orig_msg = create_test_message();
  send_to_du(orig_msg);

  f1ap_message dest_msg;
  ASSERT_TRUE(pop_du_rx_pdu(dest_msg));
  ASSERT_TRUE(is_equal(orig_msg, dest_msg));
}

TEST_P(f1c_gateway_link_test, when_pcap_writer_disabled_then_no_pcap_is_written)
{
  create_link(false);

  f1ap_message orig_msg = create_test_message();
  send_to_du(orig_msg);
  f1ap_message dest_msg;
  ASSERT_TRUE(pop_du_rx_pdu(dest_msg));
  byte_buffer sdu;
  ASSERT_FALSE(link->pcap.last_sdus.try_pop(sdu));

  send_to_cu(orig_msg);
  ASSERT_TRUE(pop_cu_rx_pdu(dest_msg));
  ASSERT_FALSE(link->pcap.last_sdus.try_pop(sdu));
}

TEST_P(f1c_gateway_link_test, when_pcap_writer_enabled_then_pcap_is_written)
{
  create_link(true);

  f1ap_message orig_msg = create_test_message();

  send_to_du(orig_msg);
  f1ap_message dest_msg;
  ASSERT_TRUE(pop_du_rx_pdu(dest_msg));
  bool        popped = false;
  byte_buffer sdu    = link->pcap.last_sdus.pop_blocking(&popped);
  ASSERT_TRUE(popped);
  ASSERT_FALSE(link->pcap.last_sdus.try_pop(sdu));

  send_to_cu(orig_msg);
  ASSERT_TRUE(pop_cu_rx_pdu(dest_msg));
  popped = false;
  sdu    = link->pcap.last_sdus.pop_blocking(&popped);
  ASSERT_TRUE(popped);
  ASSERT_FALSE(link->pcap.last_sdus.try_pop(sdu));
}

TEST_P(f1c_gateway_link_test, when_cu_tx_pdu_notifier_is_closed_then_connection_closes)
{
  create_link();

  // The CU-CP resets its F1-C Tx notifier.
  logger.info("Closing CU-CP Tx path...");
  link->cu_cp_tx_pdu_notifier.reset();

  // Wait for GW to report to DU that the association is closed.
  link->du_gw_assoc_close_signaled.wait();
}

TEST_P(f1c_gateway_link_test, when_du_tx_pdu_notifier_is_closed_then_connection_closes)
{
  create_link();

  // The DU resets its F1-C Tx notifier.
  logger.info("Closing DU Tx path...");
  link->du_tx_pdu_notifier.reset();

  // Wait for GW to report to CU that the association is closed.
  link->cu_gw_assoc_close_signaled.wait();
}

INSTANTIATE_TEST_SUITE_P(f1c_gateway_link_tests, f1c_gateway_link_test, ::testing::Values(true, false));