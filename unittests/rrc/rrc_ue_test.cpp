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
#include "srsgnb/support/async/async_task_loop.h"
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

  void on_new_pdu(const rrc_pdu_message msg) override { handler.handle_pdu(std::move(msg.pdu)); }

private:
  dummy_tx_pdu_handler& handler;
};

class dummy_du_processor_rrc_ue_interface : public du_processor_rrc_ue_interface
{
public:
  dummy_du_processor_rrc_ue_interface(ue_context& ue_ctxt_) : ue_ctxt(ue_ctxt_){};

  void create_srb(const srb_creation_message& msg) override
  {
    EXPECT_FALSE(ue_ctxt.srbs.contains(srb_id_to_lcid(msg.srb_id)));
    ue_ctxt.srbs.emplace(srb_id_to_lcid(msg.srb_id));
    last_srb = msg;
  }

  void handle_ue_context_release_command(const ue_context_release_command_message& msg) override
  {
    last_ue_ctxt_rel_cmd = msg;
  }

  ue_context_release_command_message last_ue_ctxt_rel_cmd;
  srb_creation_message               last_srb;

private:
  ue_context& ue_ctxt;
};

class dummy_rrc_ue_du_processor_adapter : public rrc_ue_du_processor_notifier
{
public:
  void connect(dummy_du_processor_rrc_ue_interface& du_processor_rrc_ue_)
  {
    du_processor_rrc_ue_handler = &du_processor_rrc_ue_;
  }

  void on_create_srb(const srb_creation_message& msg) override { du_processor_rrc_ue_handler->create_srb(msg); }

  void on_ue_context_release_command(const ue_context_release_command_message& msg) override
  {
    du_processor_rrc_ue_handler->handle_ue_context_release_command(msg);
  }

private:
  dummy_du_processor_rrc_ue_interface* du_processor_rrc_ue_handler = nullptr;
};

struct dummy_ue_task_scheduler : public rrc_ue_task_scheduler {
public:
  void         schedule_async_task(async_task<void>&& task) override { ctrl_loop.schedule(std::move(task)); }
  unique_timer make_unique_timer() override { return timer_db.create_unique_timer(); }

private:
  async_task_sequencer ctrl_loop{16};
  timer_manager        timer_db;
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
    du_proc_rrc_ue = std::make_unique<dummy_du_processor_rrc_ue_interface>(ue_ctxt);

    rrc_entity_creation_message msg(cfg, &ngap, rrc_ue_ev_notifier);
    rrc = srsgnb::srs_cu_cp::create_rrc_entity(msg);
    rrc_ue_ev_notifier.connect(*du_proc_rrc_ue);

    // create single UE context and add RRC user
    ue_ctxt.c_rnti     = to_rnti(0x1234);
    ue_ctxt.task_sched = std::make_unique<dummy_ue_task_scheduler>();
    ue_creation_message ue_create_msg{};
    ue_create_msg.c_rnti = ue_ctxt.c_rnti;
    ue_create_msg.ctxt   = &ue_ctxt;
    ue_ctxt.rrc          = rrc->add_user(std::move(ue_create_msg));

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

  void receive_setup_complete()
  {
    // inject RRC setup complete
    ue_ctxt.rrc->get_ul_ccch_pdu_handler()->handle_ul_dcch_pdu(byte_buffer{rrc_setup_complete_pdu});
  }

  void check_srb1_exists() { EXPECT_TRUE(ue_ctxt.srbs.contains(LCID_SRB1)); }

private:
  ngap_dummy                                           ngap;
  rrc_cfg_t                                            cfg{}; // empty config
  std::unique_ptr<rrc_entity_du_interface>             rrc;
  ue_context                                           ue_ctxt{};
  dummy_tx_pdu_handler                                 tx_pdu_handler; // Object to handle the generated RRC message
  std::unique_ptr<dummy_du_processor_rrc_ue_interface> du_proc_rrc_ue;
  dummy_rrc_ue_du_processor_adapter                    rrc_ue_ev_notifier;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);

  // UL-CCCH with RRC setup message
  std::array<uint8_t, 6> rrc_setup_pdu = {0x1d, 0xec, 0x89, 0xd0, 0x57, 0x66};

  // UL-DCCH with RRC setup complete message
  std::array<uint8_t, 121> rrc_setup_complete_pdu = {
      0x10, 0xc0, 0x10, 0x00, 0x08, 0x27, 0x27, 0xe0, 0x1c, 0x3f, 0xf1, 0x00, 0xc0, 0x47, 0xe0, 0x04, 0x13, 0x90,
      0x00, 0xbf, 0x20, 0x2f, 0x89, 0x98, 0x00, 0x04, 0x10, 0x00, 0x00, 0x00, 0xf2, 0xe0, 0x4f, 0x07, 0x0f, 0x07,
      0x07, 0x10, 0x05, 0x17, 0xe0, 0x04, 0x13, 0x90, 0x00, 0xbf, 0x20, 0x2f, 0x89, 0x98, 0x00, 0x04, 0x10, 0x00,
      0x00, 0x00, 0xf1, 0x00, 0x10, 0x32, 0xe0, 0x4f, 0x07, 0x0f, 0x07, 0x02, 0xf1, 0xb0, 0x80, 0x10, 0x02, 0x7d,
      0xb0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x10, 0x1b, 0x66, 0x90, 0x00, 0x00, 0x00, 0x00, 0x80, 0x10, 0x00, 0x00,
      0x10, 0x00, 0x00, 0x00, 0x05, 0x20, 0x2f, 0x89, 0x90, 0x00, 0x00, 0x11, 0x70, 0x7f, 0x07, 0x0c, 0x04, 0x01,
      0x98, 0x0b, 0x01, 0x80, 0x10, 0x17, 0x40, 0x00, 0x09, 0x05, 0x30, 0x10, 0x10,
  };
};

/// Test the RRC setup with disconnected AMF
TEST_F(rrc_setup, when_amf_disconnected_then_rrc_reject_sent)
{
  receive_setup_request();

  // check if the RRC setup message was generated
  EXPECT_EQ(get_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_reject);
}

/// Test the RRC setup with connected AMF
TEST_F(rrc_setup, when_amf_connected_then_rrc_setup_sent)
{
  connect_amf();
  receive_setup_request();

  // check if the RRC setup message was generated
  EXPECT_EQ(get_pdu_type(), asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types::rrc_setup);

  // check if SRB1 was created
  check_srb1_exists();

  receive_setup_complete();
}
