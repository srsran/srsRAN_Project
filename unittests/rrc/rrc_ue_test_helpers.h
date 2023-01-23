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

#include "lib/rrc/ue/rrc_ue_impl.h"
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
    task_sched_handle    = std::make_unique<dummy_ue_task_scheduler>(timers);
    rrc_pdu_notifier     = std::make_unique<dummy_rrc_pdu_notifier>();
    tx_security_notifier = std::make_unique<dummy_rrc_tx_security_notifier>();
    rx_security_notifier = std::make_unique<dummy_rrc_rx_security_notifier>();

    // create RRC UE
    rrc_ue_creation_message rrc_ue_create_msg{};
    rrc_ue_create_msg.ue_index = ALLOCATED_UE_INDEX;
    rrc_ue_create_msg.c_rnti   = to_rnti(0x1234);
    rrc_ue_create_msg.du_to_cu_container.resize(1);
    for (uint32_t i = 0; i < MAX_NOF_SRBS; i++) {
      rrc_ue_create_msg.srbs[i].pdu_notifier    = rrc_pdu_notifier.get();
      rrc_ue_create_msg.srbs[i].tx_sec_notifier = tx_security_notifier.get();
      rrc_ue_create_msg.srbs[i].rx_sec_notifier = rx_security_notifier.get();
    }
    rrc_ue = std::make_unique<rrc_ue_impl>(rrc_ue_ev_notifier,
                                           rrc_ue_ngc_notifier,
                                           rrc_ue_ngc_notifier,
                                           rrc_ue_create_msg.ue_index,
                                           rrc_ue_create_msg.c_rnti,
                                           rrc_ue_create_msg.cell,
                                           cfg.ue_default_cfg,
                                           rrc_ue_create_msg.srbs,
                                           rrc_ue_create_msg.du_to_cu_container,
                                           *task_sched_handle.get(),
                                           reject_users);

    ASSERT_NE(rrc_ue, nullptr);
  }

  asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::options get_srb0_pdu_type()
  {
    // generated PDU must not be empty
    EXPECT_GT(rrc_pdu_notifier->last_pdu.length(), 0);

    // Unpack received PDU
    byte_buffer                 rx_pdu{rrc_pdu_notifier->last_pdu.begin(), rrc_pdu_notifier->last_pdu.end()};
    asn1::cbit_ref              bref(rx_pdu);
    asn1::rrc_nr::dl_ccch_msg_s dl_ccch;
    EXPECT_EQ(dl_ccch.unpack(bref), asn1::SRSASN_SUCCESS);
    return dl_ccch.msg.c1().type();
  }

  rrc_ue_init_security_context_handler* get_rrc_ue_security_handler()
  {
    return &rrc_ue->get_rrc_ue_init_security_context_handler();
  }

  rrc_ue_reconfiguration_handler* get_rrc_ue_reconfiguration_handler()
  {
    return &rrc_ue->get_rrc_ue_reconfiguration_handler();
  }

  void connect_amf()
  {
    // Notify RRC about successful AMF connection
    reject_users = false;
  }

  void receive_setup_request()
  {
    // inject RRC setup into UE object
    rrc_ue->get_ul_ccch_pdu_handler().handle_ul_ccch_pdu(byte_buffer{rrc_setup_pdu});
  }

  void receive_setup_complete()
  {
    // inject RRC setup complete
    rrc_ue->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(byte_buffer{rrc_setup_complete_pdu});
  }

  void check_srb1_exists() { ASSERT_EQ(rrc_ue_ev_notifier.srb1_created, true); }

  void tick_timer()
  {
    unsigned setup_complete_timeout_ms = 1000;
    for (unsigned i = 0; i < setup_complete_timeout_ms; ++i) {
      task_sched_handle->tick_timer();
    }
  }

  void check_ue_release_not_requested()
  {
    ASSERT_NE(rrc_ue_ev_notifier.last_ue_context_release_command_message.ue_index, ALLOCATED_UE_INDEX);
  }

  void check_ue_release_requested()
  {
    ASSERT_EQ(rrc_ue_ev_notifier.last_ue_context_release_command_message.ue_index, ALLOCATED_UE_INDEX);
  }

  void receive_smc_complete()
  {
    // inject RRC SMC complete into UE object
    rrc_ue->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(byte_buffer{rrc_smc_complete_pdu});
  }

  void check_smc_pdu() { ASSERT_EQ(rrc_pdu_notifier->last_pdu, rrc_smc_pdu); }

  void check_initial_ue_message_sent() { ASSERT_TRUE(rrc_ue_ngc_notifier.initial_ue_msg_received); }

  void check_integrity_enabled(bool enabled)
  {
    ASSERT_EQ(tx_security_notifier->integ_enabled, enabled);
    ASSERT_EQ(rx_security_notifier->integ_enabled, enabled);
  }

  void check_ciphering_enabled(bool enabled)
  {
    ASSERT_EQ(tx_security_notifier->cipher_enabled, enabled);
    ASSERT_EQ(rx_security_notifier->integ_enabled, enabled);
  }

  void check_security_configured(bool configured, const security::sec_128_as_config& sec_cfg)
  {
    ASSERT_EQ(tx_security_notifier->sec_configured, configured);
    ASSERT_EQ(rx_security_notifier->sec_configured, configured);
    if (configured) {
      ASSERT_EQ(tx_security_notifier->last_sec_cfg.integ_algo, sec_cfg.integ_algo);
      ASSERT_EQ(tx_security_notifier->last_sec_cfg.cipher_algo, sec_cfg.cipher_algo);
      ASSERT_EQ(rx_security_notifier->last_sec_cfg.integ_algo, sec_cfg.integ_algo);
      ASSERT_EQ(rx_security_notifier->last_sec_cfg.cipher_algo, sec_cfg.cipher_algo);
      ASSERT_EQ(rx_security_notifier->last_sec_cfg.k_128_rrc_enc, sec_cfg.k_128_rrc_enc);
      ASSERT_EQ(rx_security_notifier->last_sec_cfg.k_128_rrc_int, sec_cfg.k_128_rrc_int);
      ASSERT_EQ(tx_security_notifier->last_sec_cfg.k_128_rrc_enc, sec_cfg.k_128_rrc_enc);
      ASSERT_EQ(tx_security_notifier->last_sec_cfg.k_128_rrc_int, sec_cfg.k_128_rrc_int);
    }
  }

  void check_rrc_reconfig_pdu() { ASSERT_EQ(rrc_pdu_notifier->last_pdu, rrc_reconfig_pdu); }

  void receive_reconfig_complete()
  {
    // inject RRC Reconfiguration complete into UE object
    rrc_ue->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(byte_buffer{rrc_reconfig_complete_pdu});
  }

private:
  const ue_index_t ALLOCATED_UE_INDEX = int_to_ue_index(23);
  rrc_cfg_t        cfg{}; // empty config

  dummy_rrc_ue_du_processor_adapter               rrc_ue_ev_notifier;
  dummy_rrc_ue_ngc_adapter                        rrc_ue_ngc_notifier;
  timer_manager                                   timers;
  std::unique_ptr<dummy_rrc_pdu_notifier>         rrc_pdu_notifier;
  std::unique_ptr<dummy_rrc_tx_security_notifier> tx_security_notifier;
  std::unique_ptr<dummy_rrc_rx_security_notifier> rx_security_notifier;
  std::unique_ptr<dummy_ue_task_scheduler>        task_sched_handle;
  std::unique_ptr<rrc_ue_interface>               rrc_ue;

  bool reject_users = true;

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
