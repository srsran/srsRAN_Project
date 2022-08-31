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
#include "srsgnb/rrc/rrc_du_factory.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/test_utils.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Fixture class RRC Setup tests preparation
class rrc_setup : public ::testing::Test
{
protected:
  static void SetUpTestSuite() { srslog::init(); }

  void SetUp() override
  {
    // create RRC entity
    du_proc_rrc_ue = std::make_unique<dummy_du_processor_rrc_ue_interface>(ue_ctxt);

    rrc_du_creation_message msg(cfg, rrc_ue_ev_notifier, rrc_ue_ngap_ev_notifier);
    rrc = srsgnb::srs_cu_cp::create_rrc_du(msg);
    rrc_ue_ev_notifier.connect(*du_proc_rrc_ue);

    // create single UE context and add RRC user
    ue_ctxt.ue_index   = ALLOCATED_UE_INDEX;
    ue_ctxt.c_rnti     = to_rnti(0x1234);
    ue_ctxt.task_sched = std::make_unique<dummy_ue_task_scheduler>();
    rrc_ue_creation_message rrc_ue_create_msg{};
    rrc_ue_create_msg.ue_index = ue_ctxt.ue_index;
    rrc_ue_create_msg.c_rnti   = ue_ctxt.c_rnti;
    rrc_ue_create_msg.du_to_cu_container.resize(1);
    rrc_ue_create_msg.ue_task_sched = ue_ctxt.task_sched.get();
    ue_ctxt.rrc                     = rrc->add_user(std::move(rrc_ue_create_msg));

    // connect SRB0 with RRC to "F1" adapter
    ue_ctxt.srbs[srb_id_to_uint(srb_id_t::srb0)].rrc_tx_notifier =
        std::make_unique<dummy_rrc_pdu_notifier>(tx_pdu_handler);
    ue_ctxt.rrc->connect_srb_notifier(srb_id_t::srb0,
                                      *ue_ctxt.srbs[srb_id_to_uint(srb_id_t::srb0)].rrc_tx_notifier.get());

    task_sched_handle = static_cast<dummy_ue_task_scheduler*>(ue_ctxt.task_sched.get());
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
    ue_ctxt.rrc->get_ul_ccch_pdu_handler().handle_ul_ccch_pdu(byte_buffer{rrc_setup_pdu});
  }

  void receive_setup_complete()
  {
    // inject RRC setup complete
    ue_ctxt.rrc->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(byte_buffer{rrc_setup_complete_pdu});
  }

  void check_srb1_exists() { EXPECT_EQ(ue_ctxt.srbs[srb_id_to_uint(srb_id_t::srb1)].rrc_tx_notifier, nullptr); }

  void tick_timer()
  {
    unsigned setup_complete_timeout_ms = 1000;
    for (unsigned i = 0; i < setup_complete_timeout_ms; ++i) {
      task_sched_handle->tick_timer();
    }
  }

  void check_ue_release_not_requested()
  {
    EXPECT_NE(du_proc_rrc_ue->last_ue_ctxt_rel_cmd.ue_index, ALLOCATED_UE_INDEX);
  }

  void check_ue_release_requested() { EXPECT_EQ(du_proc_rrc_ue->last_ue_ctxt_rel_cmd.ue_index, ALLOCATED_UE_INDEX); }

private:
  const ue_index_t                                     ALLOCATED_UE_INDEX = int_to_ue_index(23);
  rrc_cfg_t                                            cfg{}; // empty config
  ue_context                                           ue_ctxt{};
  dummy_tx_pdu_handler                                 tx_pdu_handler; // Object to handle the generated RRC message
  std::unique_ptr<dummy_du_processor_rrc_ue_interface> du_proc_rrc_ue;
  dummy_rrc_ue_du_processor_adapter                    rrc_ue_ev_notifier;
  dummy_rrc_ue_ngap_adapter                            rrc_ue_ngap_ev_notifier;
  dummy_ue_task_scheduler*                             task_sched_handle = nullptr;
  std::unique_ptr<rrc_du_ue_manager>                   rrc;

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

  // check if the RRC reject message was generated
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

/// Test the correct handling of missing RRC setup complete message
TEST_F(rrc_setup, when_setup_complete_timeout_then_ue_deleted)
{
  connect_amf();
  receive_setup_request();

  // check that UE has been created and was not requested to be released
  check_ue_release_not_requested();

  // tick timer until RRC setup complete timer fires
  tick_timer();

  // verify that RRC requested UE context release
  check_ue_release_requested();
}
