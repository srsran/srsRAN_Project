/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/blocking_queue.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/cu_cp/f1c_gateway_remote_server.h"
#include "srsran/f1ap/du/f1c_gateway_remote_client.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/io/io_broker_factory.h"
#include <future>
#include <gtest/gtest.h>

using namespace srsran;

class dummy_dlt_pcap final : public dlt_pcap
{
public:
  bool        enabled = false;
  bool        closed  = false;
  byte_buffer last_pdu;

  void         close() override { closed = true; }
  bool         is_write_enabled() const override { return enabled; }
  void         push_pdu(const_span<uint8_t> pdu) override { last_pdu = byte_buffer::create(pdu).value(); }
  virtual void push_pdu(byte_buffer pdu) override { last_pdu = std::move(pdu); }
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

  std::unique_ptr<f1ap_message_notifier>
  handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier) override
  {
    // Note: Called from io broker thread.
    cu_cp_tx_pdu_notifier = std::move(f1ap_tx_pdu_notifier);
    std::promise<void> eof_signal;
    cu_gw_assoc_close_signaled = eof_signal.get_future();

    logger.info("CU-CP handled new DU connection");
    connection_complete_signal.set_value();

    return std::make_unique<rx_pdu_notifier>("CU-CP", cu_rx_pdus, std::move(eof_signal));
  }

  std::future<void>                      cu_gw_assoc_close_signaled;
  std::future<void>                      du_gw_assoc_close_signaled;
  std::unique_ptr<f1ap_message_notifier> cu_cp_tx_pdu_notifier;
  std::unique_ptr<f1ap_message_notifier> du_tx_pdu_notifier;
  blocking_queue<f1ap_message>           cu_rx_pdus{128};
  blocking_queue<f1ap_message>           du_rx_pdus{128};
  std::unique_ptr<io_broker>             broker = create_io_broker(io_broker_type::epoll);
  dummy_dlt_pcap                         du_pcap;
  dummy_dlt_pcap                         cu_pcap;
  srslog::basic_logger&                  logger = srslog::fetch_basic_logger("TEST");

  std::promise<void> connection_complete_signal;
};

class f1c_sctp_link : public f1c_link
{
public:
  f1c_sctp_link()
  {
    sctp_network_gateway_config sctp;
    sctp.ppid         = F1AP_PPID;
    sctp.bind_address = "127.0.0.1";
    sctp.bind_port    = 0;

    // Create server.
    cu_server = create_f1c_gateway_server(f1c_cu_sctp_gateway_config{sctp, *broker, cu_pcap});
    cu_server->attach_cu_cp(*this);

    // Create client.
    sctp_network_connector_config sctp_client;
    sctp_client.connection_name = "F1-C";
    sctp_client.connect_address = "127.0.0.1";
    sctp_client.connect_port    = cu_server->get_listen_port().value();
    sctp_client.ppid            = F1AP_PPID;
    du_client                   = create_f1c_gateway_client(f1c_du_sctp_gateway_config{sctp_client, *broker, du_pcap});

    // Connect client to server.
    std::promise<void> eof_signal;
    du_gw_assoc_close_signaled = eof_signal.get_future();
    du_tx_pdu_notifier         = du_client->handle_du_connection_request(
        std::make_unique<rx_pdu_notifier>("DU", du_rx_pdus, std::move(eof_signal)));

    // Wait for server to receive SCTP connection.
    std::future<void> connection_completed = connection_complete_signal.get_future();
    connection_completed.wait();
  }

  std::unique_ptr<srs_cu_cp::f1c_connection_server> cu_server;
  std::unique_ptr<srs_du::f1c_connection_client>    du_client;
};

class f1c_gateway_link_test : public ::testing::Test
{
protected:
  f1c_gateway_link_test()
  {
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("SCTP-GW").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("CU-CP-F1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("DU-F1").set_level(srslog::basic_levels::debug);

    link = std::make_unique<f1c_sctp_link>();
  }
  ~f1c_gateway_link_test() { srslog::flush(); }

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

TEST_F(f1c_gateway_link_test, when_du_sends_msg_then_cu_receives_msg)
{
  f1ap_message orig_msg = create_test_message();
  send_to_cu(orig_msg);

  f1ap_message dest_msg;
  ASSERT_TRUE(pop_cu_rx_pdu(dest_msg));
  ASSERT_TRUE(is_equal(orig_msg, dest_msg));
}

TEST_F(f1c_gateway_link_test, when_cu_sends_msg_then_du_receives_msg)
{
  f1ap_message orig_msg = create_test_message();
  send_to_du(orig_msg);

  f1ap_message dest_msg;
  ASSERT_TRUE(pop_du_rx_pdu(dest_msg));
  ASSERT_TRUE(is_equal(orig_msg, dest_msg));
}

TEST_F(f1c_gateway_link_test, when_pcap_writer_disabled_then_no_pcap_is_written)
{
  link->du_pcap.enabled = false;
  link->cu_pcap.enabled = false;

  f1ap_message orig_msg = create_test_message();
  send_to_du(orig_msg);
  f1ap_message dest_msg;
  ASSERT_TRUE(pop_du_rx_pdu(dest_msg));
  send_to_cu(orig_msg);
  ASSERT_TRUE(pop_cu_rx_pdu(dest_msg));

  ASSERT_TRUE(link->cu_pcap.last_pdu.empty());
  ASSERT_TRUE(link->du_pcap.last_pdu.empty());
}

TEST_F(f1c_gateway_link_test, when_pcap_writer_enabled_then_pcap_is_written)
{
  link->du_pcap.enabled = true;
  link->cu_pcap.enabled = true;

  f1ap_message orig_msg = create_test_message();

  send_to_du(orig_msg);
  f1ap_message dest_msg;
  ASSERT_TRUE(pop_du_rx_pdu(dest_msg));
  ASSERT_EQ(link->cu_pcap.last_pdu, pack(orig_msg));
  ASSERT_EQ(link->du_pcap.last_pdu, pack(orig_msg));

  link->cu_pcap.last_pdu = {};
  link->du_pcap.last_pdu = {};
  send_to_cu(orig_msg);
  ASSERT_TRUE(pop_cu_rx_pdu(dest_msg));
  ASSERT_EQ(link->cu_pcap.last_pdu, pack(orig_msg));
  ASSERT_EQ(link->du_pcap.last_pdu, pack(orig_msg));
}

TEST_F(f1c_gateway_link_test, when_cu_tx_pdu_notifier_is_closed_then_connection_closes)
{
  // The CU-CP resets its F1-C Tx notifier.
  logger.info("Closing CU-CP Tx path...");
  link->cu_cp_tx_pdu_notifier.reset();

  // Wait for GW to report to both CU and DU that the association is closed.
  link->cu_gw_assoc_close_signaled.wait();
  link->du_gw_assoc_close_signaled.wait();
}

TEST_F(f1c_gateway_link_test, when_du_tx_pdu_notifier_is_closed_then_connection_closes)
{
  // The DU resets its F1-C Tx notifier.
  logger.info("Closing DU Tx path...");
  link->du_tx_pdu_notifier.reset();

  // Wait for GW to report to both CU and DU that the association is closed.
  link->cu_gw_assoc_close_signaled.wait();
  link->du_gw_assoc_close_signaled.wait();
}
