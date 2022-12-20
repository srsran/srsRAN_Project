/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "test_helpers.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/rrc/rrc_du_factory.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_cp {

/// Helper class to setup RRC UE for testing specific
/// RRC procedures
class rrc_ue_test_helper
{
protected:
  void init()
  {
    // create RRC entity
    du_proc_rrc_ue = std::make_unique<dummy_du_processor_rrc_ue_interface>(ue_ctxt);

    rrc_du_creation_message msg(cfg,
                                rrc_ue_ev_notifier,
                                rrc_ue_e1_ctrl_notifier,
                                rrc_ue_f1c_ctrl_notifier,
                                rrc_ue_ngc_notifier,
                                rrc_ue_ngc_notifier);
    rrc = srsgnb::srs_cu_cp::create_rrc_du(msg);
    rrc_ue_ev_notifier.connect(*du_proc_rrc_ue);

    // create single UE context and add RRC user
    ue_ctxt.ue_index   = ALLOCATED_UE_INDEX;
    ue_ctxt.c_rnti     = to_rnti(0x1234);
    ue_ctxt.task_sched = std::make_unique<dummy_ue_task_scheduler>(timers);
    rrc_ue_creation_message rrc_ue_create_msg{};
    rrc_ue_create_msg.ue_index = ue_ctxt.ue_index;
    rrc_ue_create_msg.c_rnti   = ue_ctxt.c_rnti;
    rrc_ue_create_msg.du_to_cu_container.resize(1);
    rrc_ue_create_msg.ue_task_sched = ue_ctxt.task_sched.get();
    ue_ctxt.rrc                     = rrc->add_ue(std::move(rrc_ue_create_msg));
    ASSERT_NE(ue_ctxt.rrc, nullptr);

    // connect SRB0 with RRC to "F1" adapter
    auto srb0_pdu_handler                                        = std::make_unique<dummy_rrc_pdu_notifier>();
    du_proc_rrc_ue->srb0_tx_pdu_handler                          = srb0_pdu_handler.get();
    ue_ctxt.srbs[srb_id_to_uint(srb_id_t::srb0)].rrc_tx_notifier = std::move(srb0_pdu_handler);
    ue_ctxt.rrc->connect_srb_notifier(
        srb_id_t::srb0, *ue_ctxt.srbs[srb_id_to_uint(srb_id_t::srb0)].rrc_tx_notifier, nullptr, nullptr);

    task_sched_handle = static_cast<dummy_ue_task_scheduler*>(ue_ctxt.task_sched.get());
  }

  asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::options get_srb0_pdu_type()
  {
    auto& tx_pdu_handler = *du_proc_rrc_ue->srb0_tx_pdu_handler;

    // generated PDU must not be empty
    EXPECT_GT(tx_pdu_handler.last_pdu.length(), 0);

    // Unpack received PDU
    byte_buffer                 rx_pdu{tx_pdu_handler.last_pdu.begin(), tx_pdu_handler.last_pdu.end()};
    asn1::cbit_ref              bref(rx_pdu);
    asn1::rrc_nr::dl_ccch_msg_s dl_ccch;
    EXPECT_EQ(dl_ccch.unpack(bref), asn1::SRSASN_SUCCESS);
    return dl_ccch.msg.c1().type();
  }

  rrc_ue_init_security_context_handler* get_rrc_ue_security_handler() { return ue_ctxt.rrc; }

  rrc_ue_reconfiguration_handler* get_rrc_ue_reconfiguration_handler() { return ue_ctxt.rrc; }

  rrc_ue_pdu_session_resource_handler* get_rrc_ue_pdu_session_resource_handler() { return ue_ctxt.rrc; }

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

  void check_srb1_exists() { ASSERT_EQ(du_proc_rrc_ue->srb1_created, true); }

  void tick_timer()
  {
    unsigned setup_complete_timeout_ms = 1000;
    for (unsigned i = 0; i < setup_complete_timeout_ms; ++i) {
      task_sched_handle->tick_timer();
    }
  }

  void check_ue_release_not_requested()
  {
    ASSERT_NE(du_proc_rrc_ue->last_ue_ctxt_rel_cmd.ue_index, ALLOCATED_UE_INDEX);
  }

  void check_ue_release_requested() { ASSERT_EQ(du_proc_rrc_ue->last_ue_ctxt_rel_cmd.ue_index, ALLOCATED_UE_INDEX); }

  void receive_smc_complete()
  {
    // inject RRC SMC complete into UE object
    ue_ctxt.rrc->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(byte_buffer{rrc_smc_complete_pdu});
  }

  void check_smc_pdu()
  {
    auto& tx_pdu_handler = *du_proc_rrc_ue->srb1_tx_pdu_handler;
    ASSERT_EQ(tx_pdu_handler.last_pdu, rrc_smc_pdu);
  }

  void check_initial_ue_message_sent() { ASSERT_TRUE(rrc_ue_ngc_notifier.initial_ue_msg_received); }

  void check_integrity_enabled(bool enabled)
  {
    auto tx_sec_handler = du_proc_rrc_ue->srb1_tx_security_handler;
    auto rx_sec_handler = du_proc_rrc_ue->srb1_rx_security_handler;
    ASSERT_EQ(tx_sec_handler->integ_enabled, enabled);
    ASSERT_EQ(rx_sec_handler->integ_enabled, enabled);
  }

  void check_ciphering_enabled(bool enabled)
  {
    auto* tx_sec_handler = du_proc_rrc_ue->srb1_tx_security_handler;
    auto* rx_sec_handler = du_proc_rrc_ue->srb1_rx_security_handler;
    ASSERT_EQ(tx_sec_handler->cipher_enabled, enabled);
    ASSERT_EQ(rx_sec_handler->integ_enabled, enabled);
  }

  void check_security_configured(bool configured, const security::sec_128_as_config& sec_cfg)
  {
    auto& tx_sec_handler = du_proc_rrc_ue->srb1_tx_security_handler;
    auto& rx_sec_handler = du_proc_rrc_ue->srb1_rx_security_handler;
    ASSERT_EQ(tx_sec_handler->sec_configured, configured);
    ASSERT_EQ(rx_sec_handler->sec_configured, configured);
    if (configured) {
      ASSERT_EQ(tx_sec_handler->last_sec_cfg.integ_algo, sec_cfg.integ_algo);
      ASSERT_EQ(tx_sec_handler->last_sec_cfg.cipher_algo, sec_cfg.cipher_algo);
      ASSERT_EQ(rx_sec_handler->last_sec_cfg.integ_algo, sec_cfg.integ_algo);
      ASSERT_EQ(rx_sec_handler->last_sec_cfg.cipher_algo, sec_cfg.cipher_algo);
      ASSERT_EQ(rx_sec_handler->last_sec_cfg.k_128_rrc_enc, sec_cfg.k_128_rrc_enc);
      ASSERT_EQ(rx_sec_handler->last_sec_cfg.k_128_rrc_int, sec_cfg.k_128_rrc_int);
      ASSERT_EQ(tx_sec_handler->last_sec_cfg.k_128_rrc_enc, sec_cfg.k_128_rrc_enc);
      ASSERT_EQ(tx_sec_handler->last_sec_cfg.k_128_rrc_int, sec_cfg.k_128_rrc_int);
    }
  }

  void check_rrc_reconfig_pdu()
  {
    auto& tx_pdu_handler = *du_proc_rrc_ue->srb1_tx_pdu_handler;
    ASSERT_EQ(tx_pdu_handler.last_pdu, rrc_reconfig_pdu);
  }

  void receive_reconfig_complete()
  {
    // inject RRC Reconfiguration complete into UE object
    ue_ctxt.rrc->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(byte_buffer{rrc_reconfig_complete_pdu});
  }

private:
  const ue_index_t ALLOCATED_UE_INDEX = int_to_ue_index(23);
  rrc_cfg_t        cfg{}; // empty config
  ue_context       ue_ctxt{};

  std::unique_ptr<dummy_du_processor_rrc_ue_interface> du_proc_rrc_ue;
  dummy_rrc_ue_du_processor_adapter                    rrc_ue_ev_notifier;
  dummy_rrc_ue_ngc_adapter                             rrc_ue_ngc_notifier;
  dummy_rrc_ue_e1_control_notifier                     rrc_ue_e1_ctrl_notifier;
  dummy_rrc_ue_f1c_control_notifier                    rrc_ue_f1c_ctrl_notifier;
  timer_manager                                        timers;
  dummy_ue_task_scheduler*                             task_sched_handle = nullptr;
  std::unique_ptr<rrc_du_ue_repository>                rrc;

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

  // DL-DCCH with RRC security mode command
  std::array<uint8_t, 3> rrc_smc_pdu = {0x22, 0x08, 0x10};

  // UL-DCCH with RRC security mode complete
  std::array<uint8_t, 2> rrc_smc_complete_pdu = {0x2a, 0x00};

  // DL-DCCH with RRC reconfiguration
  std::array<uint8_t, 20> rrc_reconfig_pdu = {0x02, 0x88, 0xa0, 0x49, 0x40, 0xbc, 0x3e, 0x00, 0x61, 0x68,
                                              0x01, 0x37, 0xab, 0x6f, 0xbb, 0xc0, 0x07, 0x55, 0x77, 0x98};

  // UL-DCCH with RRC reconfiguration complete
  std::array<uint8_t, 2> rrc_reconfig_complete_pdu = {0x0a, 0x00};
};

} // namespace srs_cu_cp
} // namespace srsgnb
