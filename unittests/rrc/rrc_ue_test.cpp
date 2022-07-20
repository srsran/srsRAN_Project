/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/rrc/rrc_factory.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

class ngap_dummy : public ngap
{
public:
  ngap_dummy(){};
  ~ngap_dummy() override = default;
};

class dummy_tx_pdu_handler
{
public:
  dummy_tx_pdu_handler() = default;
  void handle_pdu(byte_buffer_slice pdu) { last_pdu = std::move(pdu); }

  byte_buffer_slice last_pdu;
};

class dummy_rrc_pdu_notifier : public rrc_pdu_notifier
{
public:
  dummy_rrc_pdu_notifier(dummy_tx_pdu_handler& handler_) : handler(handler_) {}

  void on_new_pdu(byte_buffer_slice pdu) override { handler.handle_pdu(std::move(pdu)); }

private:
  dummy_tx_pdu_handler& handler;
};

/// Fixture class RRC Setup tests preparation
class rrc_setup : public ::testing::Test
{
protected:
  void SetUp() override
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    // create RRC entity
    rrc_entity_creation_message msg(cfg, &ngap);
    rrc = srsgnb::srs_cu_cp::create_rrc_entity(msg);

    // create single UE context and add RRC user
    ue_ctxt.c_rnti = to_rnti(0x1234);
    ue_ctxt.rrc    = rrc->add_user(ue_ctxt);

    // create SRB0 with RRC to "F1" adapter
    ue_ctxt.srbs.emplace(LCID_SRB0);
    cu_srb_context& srb0 = ue_ctxt.srbs[LCID_SRB0];
    srb0.lcid            = LCID_SRB0;
    srb0.tx_notifier     = std::make_unique<dummy_rrc_pdu_notifier>(tx_pdu_handler);
  }

  asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::options get_pdu_type()
  {
    // generated PDU must not be empty
    EXPECT_GT(tx_pdu_handler.last_pdu.length(), 0);

    // Unpack received PDU
    byte_buffer                 rx_pdu{tx_pdu_handler.last_pdu.begin(), tx_pdu_handler.last_pdu.end()};
    asn1::cbit_ref              bref(rx_pdu);
    asn1::rrc_nr::dl_ccch_msg_s dl_ccch;
    EXPECT_EQ(dl_ccch.unpack(bref), asn1::SRSASN_SUCCESS);
    return dl_ccch.msg.c1().type();
  }

  void connect_amf()
  {
    // Notify RRC about successful AMF connection
    rrc->handle_amf_connection();
  }

  void receive_setup_request()
  {
    // inject RRC setup into UE object
    ue_ctxt.rrc->get_ul_ccch_pdu_handler()->handle_ul_ccch_pdu(byte_buffer{rrc_setup_pdu});
  }

private:
  ngap_dummy                               ngap;
  rrc_cfg_t                                cfg{}; // empty config
  std::unique_ptr<rrc_entity_du_interface> rrc;
  ue_context                               ue_ctxt{};
  dummy_tx_pdu_handler                     tx_pdu_handler; // Object to handle the generated RRC message

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);

  // canned RRC setup message from UE
  std::array<uint8_t, 6> rrc_setup_pdu = {0x1d, 0xec, 0x89, 0xd0, 0x57, 0x66};
};

/// Test the RRC setup with connected AMF
TEST_F(rrc_setup, when_amf_connected_then_rrc_setup_sent)
{
  connect_amf();
  receive_setup_request();

  // check if the RRC setup message was generated
  EXPECT_EQ(get_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);
}

/// Test the RRC setup with disconnected AMF
TEST_F(rrc_setup, when_amf_disconnected_then_rrc_reject_sent)
{
  receive_setup_request();

  // check if the RRC setup message was generated
  EXPECT_EQ(get_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_reject);
}
