/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "lib/rrc/ue/rrc_ue_impl.h"
#include "rrc_ue_test_messages.h"
#include "test_helpers.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/rrc/rrc_config.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Helper class to setup RRC UE for testing specific
/// RRC procedures
class rrc_ue_test_helper
{
protected:
  void init()
  {
    task_sched_handle = std::make_unique<dummy_ue_task_scheduler>(timers, ctrl_worker);
    for (std::unique_ptr<dummy_rrc_pdu_notifier>& srb : rrc_srb_pdu_notifiers) {
      srb = std::make_unique<dummy_rrc_pdu_notifier>();
    }
    tx_security_notifier = std::make_unique<dummy_rrc_tx_security_notifier>();
    rx_security_notifier = std::make_unique<dummy_rrc_rx_security_notifier>();

    // create RRC UE
    rrc_ue_creation_message rrc_ue_create_msg{};
    rrc_ue_create_msg.ue_index = ALLOCATED_UE_INDEX;
    rrc_ue_create_msg.c_rnti   = to_rnti(0x1234);
    rrc_ue_create_msg.du_to_cu_container.resize(1);
    for (uint32_t i = 0; i < srb_id_to_uint(srb_id_t::srb2); i++) { // don't automatically create SRB2
      rrc_ue_create_msg.srbs[i].pdu_notifier    = rrc_srb_pdu_notifiers[i].get();
      rrc_ue_create_msg.srbs[i].tx_sec_notifier = tx_security_notifier.get();
      rrc_ue_create_msg.srbs[i].rx_sec_notifier = rx_security_notifier.get();
    }
    rrc_ue_create_msg.cell.bands.push_back(nr_band::n78);
    rrc_ue_cfg_t ue_cfg;
    // Add meas timing
    rrc_meas_timing meas_timing;
    meas_timing.freq_and_timing.emplace();
    meas_timing.freq_and_timing.value().carrier_freq                                    = 535930;
    meas_timing.freq_and_timing.value().ssb_subcarrier_spacing                          = subcarrier_spacing::kHz15;
    meas_timing.freq_and_timing.value().ssb_meas_timing_cfg.dur                         = 5;
    meas_timing.freq_and_timing.value().ssb_meas_timing_cfg.periodicity_and_offset.sf10 = 0;

    ue_cfg.meas_timings.push_back(meas_timing);
    rrc_ue = std::make_unique<rrc_ue_impl>(*up_resource_mng,
                                           rrc_ue_ev_notifier,
                                           rrc_ue_ngap_notifier,
                                           rrc_ue_ngap_notifier,
                                           rrc_ue_cu_cp_notifier,
                                           cell_meas_mng,
                                           rrc_ue_create_msg.ue_index,
                                           rrc_ue_create_msg.c_rnti,
                                           rrc_ue_create_msg.cell,
                                           ue_cfg,
                                           rrc_ue_create_msg.srbs,
                                           std::move(rrc_ue_create_msg.du_to_cu_container),
                                           *task_sched_handle,
                                           reject_users);

    ASSERT_NE(rrc_ue, nullptr);
  }

  asn1::rrc_nr::dl_ccch_msg_s get_srb0_pdu()
  {
    // generated PDU must not be empty
    EXPECT_GT(rrc_srb_pdu_notifiers[0]->last_pdu.length(), 0);

    // Unpack received PDU
    byte_buffer    rx_pdu{rrc_srb_pdu_notifiers[0]->last_pdu.begin(), rrc_srb_pdu_notifiers[0]->last_pdu.end()};
    asn1::cbit_ref bref(rx_pdu);
    asn1::rrc_nr::dl_ccch_msg_s dl_ccch;
    EXPECT_EQ(dl_ccch.unpack(bref), asn1::SRSASN_SUCCESS);
    return dl_ccch;
  }

  asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::options get_srb0_pdu_type()
  {
    asn1::rrc_nr::dl_ccch_msg_s dl_ccch = get_srb0_pdu();
    return dl_ccch.msg.c1().type();
  }

  asn1::rrc_nr::dl_dcch_msg_s get_srb1_pdu()
  {
    // generated PDU must not be empty
    EXPECT_GT(rrc_srb_pdu_notifiers[1]->last_pdu.length(), 0);

    // Unpack received PDU
    byte_buffer    rx_pdu{rrc_srb_pdu_notifiers[1]->last_pdu.begin(), rrc_srb_pdu_notifiers[1]->last_pdu.end()};
    asn1::cbit_ref bref(rx_pdu);
    asn1::rrc_nr::dl_dcch_msg_s dl_dcch;
    EXPECT_EQ(dl_dcch.unpack(bref), asn1::SRSASN_SUCCESS);
    return dl_dcch;
  }

  asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::options get_srb1_pdu_type()
  {
    asn1::rrc_nr::dl_dcch_msg_s dl_dcch = get_srb1_pdu();
    return dl_dcch.msg.c1().type();
  }

  ue_index_t get_old_ue_index() { return rrc_srb_pdu_notifiers[1]->last_ue_index; }

  asn1::rrc_nr::dl_dcch_msg_s get_srb2_pdu()
  {
    // generated PDU must not be empty
    EXPECT_GT(rrc_srb_pdu_notifiers[2]->last_pdu.length(), 0);

    // Unpack received PDU
    byte_buffer    rx_pdu{rrc_srb_pdu_notifiers[2]->last_pdu.begin(), rrc_srb_pdu_notifiers[2]->last_pdu.end()};
    asn1::cbit_ref bref(rx_pdu);
    asn1::rrc_nr::dl_dcch_msg_s dl_dcch;
    EXPECT_EQ(dl_dcch.unpack(bref), asn1::SRSASN_SUCCESS);
    return dl_dcch;
  }

  asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::options get_srb2_pdu_type()
  {
    asn1::rrc_nr::dl_dcch_msg_s dl_dcch = get_srb2_pdu();
    return dl_dcch.msg.c1().type();
  }

  rrc_ue_init_security_context_handler* get_rrc_ue_security_handler()
  {
    return &rrc_ue->get_rrc_ue_init_security_context_handler();
  }

  rrc_ue_control_message_handler* get_rrc_ue_control_message_handler()
  {
    return &rrc_ue->get_rrc_ue_control_message_handler();
  }

  void connect_amf()
  {
    // Notify RRC about successful AMF connection
    reject_users = false;
  }

  void create_srb2()
  {
    rrc_ue->connect_srb_notifier(
        srb_id_t::srb2, *rrc_srb_pdu_notifiers[2].get(), tx_security_notifier.get(), rx_security_notifier.get());
  }

  void receive_setup_request()
  {
    // inject RRC setup into UE object
    rrc_ue->get_ul_ccch_pdu_handler().handle_ul_ccch_pdu(byte_buffer{rrc_setup_pdu});
  }

  void receive_invalid_reestablishment_request(pci_t pci, rnti_t c_rnti)
  {
    // inject RRC Reestablishment Request into UE object
    rrc_ue->get_ul_ccch_pdu_handler().handle_ul_ccch_pdu(generate_invalid_rrc_reestablishment_request_pdu(pci, c_rnti));
  }

  void receive_valid_reestablishment_request(pci_t pci, rnti_t c_rnti)
  {
    // inject RRC Reestablishment Request into UE object
    rrc_ue->get_ul_ccch_pdu_handler().handle_ul_ccch_pdu(generate_valid_rrc_reestablishment_request_pdu(pci, c_rnti));
  }

  void receive_reestablishment_complete()
  {
    // inject RRC Reestablishment complete
    rrc_ue->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(generate_rrc_reestablishment_complete_pdu());
  }

  void receive_setup_complete()
  {
    // inject RRC setup complete
    rrc_ue->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(byte_buffer{rrc_setup_complete_pdu});
  }

  void send_dl_info_transfer(byte_buffer nas_msg)
  {
    dl_nas_transport_message msg{std::move(nas_msg)};
    // inject RRC setup complete
    rrc_ue->handle_dl_nas_transport_message(msg);
  }

  void check_srb1_exists() { ASSERT_EQ(rrc_ue_ev_notifier.srb1_created, true); }

  void tick_timer()
  {
    unsigned setup_complete_timeout_ms = 1000;
    for (unsigned i = 0; i < setup_complete_timeout_ms; ++i) {
      task_sched_handle->tick_timer();
      ctrl_worker.run_pending_tasks();
    }
  }

  void check_ue_release_not_requested()
  {
    ASSERT_NE(rrc_ue_ev_notifier.last_rrc_ue_context_release_command.ue_index, ALLOCATED_UE_INDEX);
  }

  void check_ue_release_requested()
  {
    ASSERT_EQ(rrc_ue_ev_notifier.last_rrc_ue_context_release_command.ue_index, ALLOCATED_UE_INDEX);
  }

  void receive_smc_complete()
  {
    // inject RRC SMC complete into UE object
    rrc_ue->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(byte_buffer{rrc_smc_complete_pdu});
  }

  void check_smc_pdu() { ASSERT_EQ(rrc_srb_pdu_notifiers[1]->last_pdu, rrc_smc_pdu); }

  void check_initial_ue_message_sent() { ASSERT_TRUE(rrc_ue_ngap_notifier.initial_ue_msg_received); }

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
      ASSERT_EQ(rx_security_notifier->last_sec_cfg.k_128_enc, sec_cfg.k_128_enc);
      ASSERT_EQ(rx_security_notifier->last_sec_cfg.k_128_int, sec_cfg.k_128_int);
      ASSERT_EQ(rx_security_notifier->last_sec_cfg.domain, sec_cfg.domain);
    }
  }

  void check_rrc_ue_enquiry_pdu(uint8_t transaction_id)
  {
    byte_buffer                 dl_dcch_msg_pdu(span<uint8_t>{rrc_ue_capability_enquiry_pdu});
    asn1::cbit_ref              bref{dl_dcch_msg_pdu};
    asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
    if (dl_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Error unpacking UE Capability Enquiry");
      return;
    }
    dl_dcch_msg.msg.c1().ue_cap_enquiry().rrc_transaction_id = transaction_id;

    dl_dcch_msg_pdu.clear();
    asn1::bit_ref bref2{dl_dcch_msg_pdu};
    if (dl_dcch_msg.pack(bref2) != asn1::SRSASN_SUCCESS) {
      logger.error("Error re-packing UE Capability Enquiry");
      return;
    }

    ASSERT_EQ(rrc_srb_pdu_notifiers[1]->last_pdu, byte_buffer_slice{dl_dcch_msg_pdu});
  }

  void receive_ue_capability_information(uint8_t transaction_id)
  {
    byte_buffer                 ul_dcch_msg_pdu(span<uint8_t>{rrc_ue_capability_information_pdu});
    asn1::cbit_ref              bref{ul_dcch_msg_pdu};
    asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
    if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Error unpacking UE Capability Info");
      return;
    }
    ul_dcch_msg.msg.c1().ue_cap_info().rrc_transaction_id = transaction_id;

    ul_dcch_msg_pdu.clear();
    asn1::bit_ref bref2{ul_dcch_msg_pdu};
    if (ul_dcch_msg.pack(bref2) != asn1::SRSASN_SUCCESS) {
      logger.error("Error re-packing UE Capability Info");
      return;
    }

    // inject RRC UE capability information into UE object
    rrc_ue->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(ul_dcch_msg_pdu);
  }

  void check_rrc_reconfig_pdu() { ASSERT_EQ(rrc_srb_pdu_notifiers[1]->last_pdu, rrc_reconfig_pdu); }

  void receive_reconfig_complete()
  {
    // inject RRC Reconfiguration complete into UE object
    rrc_ue->get_ul_dcch_pdu_handler().handle_ul_dcch_pdu(byte_buffer{rrc_reconfig_complete_pdu});
  }

  security::security_context generate_security_context()
  {
    const char* sk_gnb_cstr = "8d2abb1a4349319ea4276295c33d107a6e274495cb9bc2433fb7d7ca4c3f7646";

    // Pack hex strings into srsgnb types
    security::sec_key sk_gnb = make_sec_key(sk_gnb_cstr);

    // Initialize security context and capabilities.
    security::security_context sec_ctxt = {};
    sec_ctxt.k                          = sk_gnb;
    std::fill(sec_ctxt.supported_int_algos.begin(), sec_ctxt.supported_int_algos.end(), true);
    std::fill(sec_ctxt.supported_enc_algos.begin(), sec_ctxt.supported_enc_algos.end(), true);

    // Select preferred integrity algorithm.
    security::preferred_integrity_algorithms inc_algo_pref_list  = {security::integrity_algorithm::nia2,
                                                                    security::integrity_algorithm::nia1,
                                                                    security::integrity_algorithm::nia3,
                                                                    security::integrity_algorithm::nia0};
    security::preferred_ciphering_algorithms ciph_algo_pref_list = {security::ciphering_algorithm::nea0,
                                                                    security::ciphering_algorithm::nea2,
                                                                    security::ciphering_algorithm::nea1,
                                                                    security::ciphering_algorithm::nea3};

    sec_ctxt.select_algorithms(inc_algo_pref_list, ciph_algo_pref_list);

    // Generate K_rrc_enc and K_rrc_int
    sec_ctxt.generate_as_keys();

    return sec_ctxt;
  }

  void add_ue_reestablishment_context(ue_index_t ue_index)
  {
    rrc_reestablishment_ue_context_t reest_context = {};
    reest_context.ue_index                         = ue_index;
    reest_context.sec_context                      = generate_security_context();

    logger.debug("Adding reestablishment context for ue={}", ue_index);

    rrc_ue_cu_cp_notifier.add_ue_context(reest_context);
  }

  void check_meas_results(const rrc_meas_results& meas_results)
  {
    ASSERT_EQ(meas_results.meas_id, uint_to_meas_id(1));
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.size(), 1);
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()->serv_cell_id, 0);
    ASSERT_TRUE(meas_results.meas_result_serving_mo_list.begin()->meas_result_serving_cell.pci.has_value());
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()->meas_result_serving_cell.pci.value(), 0);
    ASSERT_TRUE(meas_results.meas_result_serving_mo_list.begin()
                    ->meas_result_serving_cell.cell_results.results_ssb_cell->rsrp.has_value());
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()
                  ->meas_result_serving_cell.cell_results.results_ssb_cell->rsrp.value(),
              113);
    ASSERT_TRUE(meas_results.meas_result_serving_mo_list.begin()
                    ->meas_result_serving_cell.cell_results.results_ssb_cell->rsrq.has_value());
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()
                  ->meas_result_serving_cell.cell_results.results_ssb_cell->rsrq.value(),
              65);
    ASSERT_TRUE(meas_results.meas_result_serving_mo_list.begin()
                    ->meas_result_serving_cell.cell_results.results_ssb_cell->sinr.has_value());
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()
                  ->meas_result_serving_cell.cell_results.results_ssb_cell->sinr.value(),
              92);

    ASSERT_TRUE(meas_results.meas_result_serving_mo_list.begin()->meas_result_serving_cell.rs_idx_results.has_value());
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()
                  ->meas_result_serving_cell.rs_idx_results.value()
                  .results_ssb_idxes.size(),
              1);
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()
                  ->meas_result_serving_cell.rs_idx_results.value()
                  .results_ssb_idxes[0]
                  .ssb_idx,
              0);
    ASSERT_TRUE(meas_results.meas_result_serving_mo_list.begin()
                    ->meas_result_serving_cell.rs_idx_results.value()
                    .results_ssb_idxes[0]
                    .ssb_results.has_value());
    ASSERT_TRUE(meas_results.meas_result_serving_mo_list.begin()
                    ->meas_result_serving_cell.rs_idx_results.value()
                    .results_ssb_idxes[0]
                    .ssb_results.value()
                    .rsrp.has_value());
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()
                  ->meas_result_serving_cell.rs_idx_results.value()
                  .results_ssb_idxes[0]
                  .ssb_results.value()
                  .rsrp.value(),
              113);
    ASSERT_TRUE(meas_results.meas_result_serving_mo_list.begin()
                    ->meas_result_serving_cell.rs_idx_results.value()
                    .results_ssb_idxes.begin()
                    ->ssb_results.value()
                    .rsrq.has_value());
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()
                  ->meas_result_serving_cell.rs_idx_results.value()
                  .results_ssb_idxes.begin()
                  ->ssb_results.value()
                  .rsrq.value(),
              66);
    ASSERT_TRUE(meas_results.meas_result_serving_mo_list.begin()
                    ->meas_result_serving_cell.rs_idx_results.value()
                    .results_ssb_idxes.begin()
                    ->ssb_results.value()
                    .sinr.has_value());
    ASSERT_EQ(meas_results.meas_result_serving_mo_list.begin()
                  ->meas_result_serving_cell.rs_idx_results.value()
                  .results_ssb_idxes.begin()
                  ->ssb_results.value()
                  .sinr.value(),
              92);
  }

private:
  const ue_index_t ALLOCATED_UE_INDEX = uint_to_ue_index(23);
  rrc_cfg_t        cfg{}; // empty config

  std::unique_ptr<up_resource_manager> up_resource_mng =
      create_up_resource_manager(up_resource_manager_cfg{cfg.drb_config});
  dummy_rrc_ue_du_processor_adapter                      rrc_ue_ev_notifier;
  dummy_rrc_ue_ngap_adapter                              rrc_ue_ngap_notifier;
  dummy_rrc_ue_cu_cp_adapter                             rrc_ue_cu_cp_notifier;
  dummy_cell_meas_manager                                cell_meas_mng;
  timer_manager                                          timers;
  std::array<std::unique_ptr<dummy_rrc_pdu_notifier>, 3> rrc_srb_pdu_notifiers;
  std::unique_ptr<dummy_rrc_tx_security_notifier>        tx_security_notifier;
  std::unique_ptr<dummy_rrc_rx_security_notifier>        rx_security_notifier;
  std::unique_ptr<dummy_ue_task_scheduler>               task_sched_handle;
  std::unique_ptr<rrc_ue_interface>                      rrc_ue;
  manual_task_worker                                     ctrl_worker{64};

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
      0x98, 0x0b, 0x01, 0x80, 0x10, 0x17, 0x40, 0x00, 0x09, 0x05, 0x30, 0x10, 0x10};

  // DL-DCCH with RRC security mode command
  std::array<uint8_t, 3> rrc_smc_pdu = {0x22, 0x08, 0x10};

  // UL-DCCH with RRC security mode complete
  std::array<uint8_t, 2> rrc_smc_complete_pdu = {0x2a, 0x00};

  // DL-DCCH with RRC UE capability enquiry with freqBand filter for n78
  std::array<uint8_t, 8> rrc_ue_capability_enquiry_pdu = {0x34, 0x02, 0x01, 0x20, 0x01, 0x01, 0x34, 0x00};

  // UL-DCCH with RRC ue capability information
  std::array<uint8_t, 1014> rrc_ue_capability_information_pdu = {
      0x4c, 0x82, 0x20, 0xfc, 0x7a, 0x68, 0x04, 0x00, 0x01, 0xd3, 0xd6, 0x84, 0xc0, 0x80, 0x00, 0x80, 0x50, 0x12, 0x09,
      0x31, 0x60, 0x00, 0x98, 0x35, 0x03, 0x04, 0xd8, 0x06, 0x1c, 0xf9, 0x86, 0x3c, 0xa1, 0x60, 0x70, 0x4c, 0x02, 0x03,
      0x11, 0x81, 0x0a, 0x83, 0x62, 0x76, 0xb0, 0x46, 0x10, 0xd0, 0x40, 0x83, 0xe5, 0x00, 0x8b, 0x2d, 0x93, 0x15, 0x43,
      0x43, 0x9f, 0x11, 0xc7, 0xa0, 0x67, 0xd8, 0x13, 0xe3, 0x1c, 0xf9, 0x86, 0x3c, 0xa1, 0x60, 0x70, 0x38, 0x78, 0x2e,
      0x00, 0x01, 0xe0, 0x03, 0x80, 0x00, 0x78, 0x00, 0x0e, 0x00, 0x70, 0x2a, 0x0d, 0x89, 0xba, 0xc1, 0x18, 0x43, 0x41,
      0x02, 0x0f, 0x94, 0x02, 0x2c, 0xb6, 0x4c, 0x55, 0x0d, 0x0e, 0x7c, 0x47, 0x1e, 0x81, 0x9f, 0x60, 0x4f, 0x8c, 0x73,
      0xe6, 0x18, 0xf2, 0x85, 0x81, 0xc0, 0xe1, 0xe0, 0xb8, 0x00, 0x2f, 0x80, 0x0e, 0x00, 0x0b, 0xe0, 0x00, 0x39, 0x01,
      0xc0, 0x06, 0xca, 0x00, 0x00, 0xa0, 0x00, 0x00, 0xa8, 0x36, 0x26, 0x6b, 0x04, 0x61, 0x0d, 0x04, 0x08, 0x3e, 0x50,
      0x08, 0xb2, 0xd9, 0x31, 0x54, 0x34, 0x39, 0xf1, 0x1c, 0x7a, 0x06, 0x7d, 0x81, 0x3e, 0x31, 0xcf, 0x98, 0x63, 0xca,
      0x16, 0x07, 0x03, 0x87, 0x82, 0xe0, 0x00, 0x9e, 0x00, 0x38, 0x00, 0x27, 0x80, 0x00, 0xe0, 0x07, 0x02, 0xa0, 0xd8,
      0x8d, 0xac, 0x11, 0x84, 0x34, 0x10, 0x20, 0xf9, 0x40, 0x22, 0x4b, 0x64, 0xc5, 0x50, 0x50, 0xe7, 0xc4, 0x71, 0xe3,
      0x1c, 0xf9, 0x86, 0x3c, 0xa1, 0x60, 0x70, 0x38, 0x78, 0x13, 0xf8, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x03,
      0x80, 0x1c, 0x0a, 0x83, 0x62, 0x0e, 0xb0, 0x46, 0x10, 0xd0, 0x40, 0x83, 0xe5, 0x00, 0x89, 0x2d, 0x93, 0x15, 0x41,
      0x43, 0x9f, 0x11, 0xc7, 0x8c, 0x73, 0xe6, 0x18, 0xf2, 0x85, 0x81, 0xc0, 0xe1, 0xe0, 0x4f, 0xec, 0x00, 0x00, 0x03,
      0xfb, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x70, 0x3a, 0x0d, 0x85, 0x18, 0x1a, 0xc1, 0x18, 0x43, 0x41, 0x02, 0x0f, 0x94,
      0x02, 0x2c, 0xb6, 0x4c, 0x55, 0x0d, 0x0e, 0x7c, 0x47, 0x1e, 0x81, 0x9f, 0x60, 0x4f, 0x8c, 0x73, 0xe6, 0x18, 0xf2,
      0x85, 0x81, 0xc0, 0xe1, 0xe0, 0xb8, 0x00, 0x2f, 0x80, 0x0e, 0x00, 0x0b, 0xe0, 0x00, 0x38, 0x01, 0xc0, 0xa8, 0x36,
      0x13, 0xeb, 0x04, 0x61, 0x0d, 0x04, 0x08, 0x3e, 0x50, 0x08, 0xb2, 0xd9, 0x31, 0x54, 0x34, 0x39, 0xf1, 0x1c, 0x7a,
      0x06, 0x7d, 0x81, 0x3e, 0x31, 0xcf, 0x98, 0x63, 0xca, 0x16, 0x07, 0x03, 0x87, 0x84, 0xe0, 0x00, 0xbe, 0x00, 0x38,
      0x00, 0x2f, 0x80, 0x00, 0xe0, 0x07, 0x02, 0xa0, 0xd8, 0x4b, 0xac, 0x11, 0x84, 0x34, 0x10, 0x20, 0xf9, 0x40, 0x22,
      0xcb, 0x64, 0xc5, 0x50, 0xd0, 0xe7, 0xc4, 0x71, 0xe8, 0x19, 0xf6, 0x04, 0xf8, 0xc7, 0x3e, 0x61, 0x8f, 0x28, 0x58,
      0x1c, 0x0e, 0x1e, 0x13, 0x80, 0x02, 0x00, 0x00, 0xe0, 0x00, 0x80, 0x00, 0x03, 0x80, 0x1c, 0x0a, 0x83, 0x60, 0xde,
      0xb0, 0x46, 0x10, 0xd0, 0x40, 0x83, 0xe5, 0x00, 0x89, 0x2d, 0x93, 0x15, 0x41, 0x43, 0x9f, 0x11, 0xc7, 0x8c, 0x73,
      0xe6, 0x18, 0xf2, 0x85, 0x81, 0xc0, 0xe1, 0xe0, 0x4f, 0xe8, 0x00, 0x00, 0x03, 0xfa, 0x00, 0x00, 0x00, 0x0e, 0x00,
      0x70, 0x2a, 0x0d, 0x82, 0x7a, 0xc1, 0x18, 0x43, 0x41, 0x02, 0x0f, 0x94, 0x02, 0x24, 0xb6, 0x4c, 0x55, 0x05, 0x0e,
      0x7c, 0x47, 0x1e, 0x31, 0xcf, 0x98, 0x63, 0xca, 0x16, 0x07, 0x03, 0x87, 0x81, 0x3f, 0x80, 0x00, 0x00, 0x0f, 0xe0,
      0x00, 0x00, 0x00, 0x38, 0x01, 0xc0, 0xa8, 0x36, 0x05, 0xeb, 0x04, 0x61, 0x0d, 0x04, 0x08, 0x3e, 0x50, 0x08, 0x92,
      0xd9, 0x31, 0x54, 0x14, 0x39, 0xf1, 0x1c, 0x78, 0xc7, 0x3e, 0x61, 0x8f, 0x28, 0x58, 0x1c, 0x0e, 0x1e, 0x04, 0xfc,
      0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x02, 0xa0, 0xd8, 0x0f, 0xac, 0x11, 0x84, 0x34, 0x10,
      0x20, 0xf9, 0x40, 0x22, 0x4b, 0x64, 0xc5, 0x50, 0x50, 0xe7, 0xc4, 0x71, 0xe3, 0x1c, 0xf9, 0x86, 0x3c, 0xa1, 0x60,
      0x70, 0x38, 0x78, 0x13, 0xf8, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x03, 0x80, 0x1c, 0x0a, 0x83, 0x60, 0x36,
      0xb0, 0x46, 0x10, 0xd0, 0x40, 0x83, 0xe5, 0x00, 0x89, 0x2d, 0x93, 0x15, 0x41, 0x43, 0x9f, 0x11, 0xc7, 0x8c, 0x73,
      0xe6, 0x18, 0xf2, 0x85, 0x81, 0xc0, 0xe1, 0xe0, 0x4f, 0xe0, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x0e, 0x00,
      0x70, 0x2a, 0x0d, 0x80, 0x9a, 0xc1, 0x18, 0x43, 0x41, 0x02, 0x0f, 0x94, 0x02, 0x24, 0xb6, 0x4c, 0x55, 0x05, 0x0e,
      0x7c, 0x47, 0x1e, 0x31, 0xcf, 0x98, 0x63, 0xca, 0x16, 0x07, 0x03, 0x87, 0x81, 0x3f, 0x80, 0x00, 0x00, 0x0f, 0xe0,
      0x00, 0x00, 0x00, 0x38, 0x01, 0xc0, 0xa8, 0x36, 0x01, 0x6b, 0x04, 0x61, 0x0d, 0x04, 0x08, 0x3e, 0x50, 0x08, 0x92,
      0xd9, 0x31, 0x54, 0x14, 0x39, 0xf1, 0x1c, 0x78, 0xc7, 0x3e, 0x61, 0x8f, 0x28, 0x58, 0x1c, 0x0e, 0x1e, 0x04, 0xff,
      0x80, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x02, 0xa0, 0xd8, 0x03, 0xac, 0x11, 0x84, 0x34, 0x10,
      0x20, 0xf9, 0x40, 0x22, 0x4b, 0x64, 0xc5, 0x50, 0x50, 0xe7, 0xc4, 0x71, 0xe3, 0x1c, 0xf9, 0x86, 0x3c, 0xa1, 0x60,
      0x70, 0x38, 0x78, 0x13, 0xf8, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x03, 0x80, 0x1c, 0x0a, 0x83, 0x60, 0x06,
      0xb0, 0x46, 0x10, 0xd0, 0x40, 0x83, 0xe5, 0x00, 0x89, 0x2d, 0x93, 0x15, 0x41, 0x43, 0x9f, 0x11, 0xc7, 0x8c, 0x73,
      0xe6, 0x18, 0xf2, 0x85, 0x81, 0xc0, 0xe1, 0xe0, 0x4f, 0xec, 0x00, 0x00, 0x03, 0xfb, 0x00, 0x00, 0x00, 0x0e, 0x00,
      0x70, 0x00, 0x82, 0x0e, 0x26, 0x80, 0x00, 0x00, 0x08, 0x38, 0x50, 0x00, 0x00, 0x04, 0x14, 0x0b, 0x80, 0xa0, 0x42,
      0xd8, 0xe7, 0xc3, 0x81, 0x13, 0x63, 0x9f, 0x0c, 0xf4, 0x0b, 0x00, 0xe0, 0x00, 0xa0, 0x70, 0x30, 0x0c, 0x06, 0x00,
      0xa8, 0x0e, 0x00, 0x0c, 0x01, 0x80, 0x60, 0x14, 0x0f, 0x80, 0xc0, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x08,
      0x02, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x06, 0x00, 0x80, 0x00, 0x04, 0x00, 0x40, 0x00, 0x02, 0x80, 0x20, 0x00,
      0x01, 0x80, 0x66, 0x54, 0xde, 0x54, 0xd6, 0x06, 0x56, 0x06, 0xd6, 0x46, 0xd6, 0x04, 0x56, 0x0a, 0xd0, 0x1c, 0x10,
      0x00, 0x00, 0x96, 0x52, 0x40, 0x40, 0x00, 0x06, 0x59, 0x49, 0x01, 0x00, 0x00, 0x29, 0x65, 0x24, 0x04, 0x00, 0x00,
      0xe5, 0x94, 0x94, 0x10, 0x00, 0x04, 0x96, 0x52, 0x40, 0x40, 0x00, 0x16, 0x59, 0x49, 0x01, 0x00, 0x00, 0x69, 0x65,
      0x24, 0x04, 0x00, 0x00, 0x45, 0x94, 0x90, 0x06, 0x52, 0xa8, 0xba, 0x55, 0x15, 0x40, 0xe2, 0xe9, 0x55, 0x55, 0x23,
      0x8a, 0xa0, 0x51, 0x54, 0x16, 0x28, 0x08, 0x54, 0x01, 0xa8, 0x14, 0x0a, 0x05, 0x02, 0x81, 0x40, 0xa0, 0x00, 0x02,
      0x00, 0x10, 0x04, 0x00, 0x80, 0x02, 0x00, 0x80, 0x20, 0xc5, 0x0f, 0x80, 0x00, 0x81, 0x01, 0x82, 0x03, 0x03, 0x85,
      0x86, 0x06, 0x88, 0x08, 0x89, 0x09, 0x8c, 0x0c, 0x8d, 0x8e, 0x0e, 0x8f, 0x90, 0x92, 0x93, 0x13, 0x94, 0x14, 0x95,
      0x16, 0x97, 0xa0, 0xa1, 0x23, 0x20, 0x00};

  // DL-DCCH with RRC reconfiguration
  std::array<uint8_t, 48> rrc_reconfig_pdu = {0x02, 0xa8, 0xa0, 0x49, 0x40, 0xbc, 0x3e, 0x00, 0x61, 0x4a, 0xa0, 0x00,
                                              0x3c, 0xa0, 0x09, 0x92, 0x2a, 0x08, 0x06, 0x18, 0x91, 0x80, 0x00, 0x81,
                                              0x42, 0x68, 0x00, 0x00, 0x64, 0xfd, 0xf1, 0xc0, 0x00, 0x00, 0x09, 0x0d,
                                              0x0d, 0x40, 0x09, 0xbd, 0x5b, 0x7d, 0xde, 0x00, 0x3a, 0xab, 0xbc, 0xc0};

  // UL-DCCH with RRC reconfiguration complete
  std::array<uint8_t, 2> rrc_reconfig_complete_pdu = {0x0a, 0x00};
};

} // namespace srs_cu_cp
} // namespace srsran
