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

#include "lib/cu_cp/routines/mobility/handover_reconfiguration_routine.h"
#include "mobility_test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/async/coroutine.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

struct dummy_rrc_ue : public rrc_ue_interface, public rrc_ue_controller {
public:
  dummy_rrc_ue() = default;

  void set_rrc_reconfiguration_outcome(bool outcome) { rrc_reconfiguration_outcome = outcome; }

  void set_transaction_id(unsigned transaction_id_) { test_transaction_id = transaction_id_; }

  // RRC UE Controller
  void stop() override{};

  // RRC UL PDU handler
  void handle_ul_ccch_pdu(byte_buffer pdu) override{};
  void handle_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdu) override{};

  // RRC NGAP Message handler
  void        handle_dl_nas_transport_message(byte_buffer nas_pdu) override{};
  byte_buffer get_packed_ue_radio_access_cap_info() const override { return byte_buffer{}; };
  byte_buffer get_packed_handover_preparation_message() override { return byte_buffer{}; };

  // RRC UE Control Message handler

  rrc_ue_security_mode_command_context get_security_mode_command_context() override { return {}; }

  async_task<bool> handle_security_mode_complete_expected(uint8_t transaction_id) override
  {
    logger.info("Awaiting a RRC Security Mode Complete (transaction_id={})", transaction_id);
    return launch_async([](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  byte_buffer get_packed_ue_capability_rat_container_list() const override
  {
    logger.info("Received a new request to get packed UE capabilities");
    return byte_buffer{};
  }
  async_task<bool> handle_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override
  {
    logger.info("Received a new RRC reconfiguration request");
    last_radio_bearer_cfg = msg.radio_bearer_cfg;

    return launch_async([this](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(rrc_reconfiguration_outcome);
    });
  }

  rrc_ue_handover_reconfiguration_context
  get_rrc_ue_handover_reconfiguration_context(const rrc_reconfiguration_procedure_request& request) override
  {
    logger.info("Received a new handover reconfiguration request (transaction_id={})", test_transaction_id);
    last_radio_bearer_cfg = request.radio_bearer_cfg;
    return {test_transaction_id, byte_buffer{}};
  }

  async_task<bool> handle_handover_reconfiguration_complete_expected(uint8_t transaction_id_) override
  {
    logger.info("Awaiting a RRC Reconfiguration Complete (transaction_id={})", transaction_id_);
    last_transaction_id = transaction_id_;
    return launch_async([this](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(rrc_reconfiguration_outcome);
    });
  }

  bool store_ue_capabilities(byte_buffer ue_capabilities) override
  {
    last_ue_capabilities = std::move(ue_capabilities);
    return true;
  }

  async_task<bool> handle_rrc_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) override
  {
    logger.info("Received a new UE capability transfer request");

    return launch_async([this](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(ue_cap_transfer_outcome);
    });
  }

  rrc_ue_release_context get_rrc_ue_release_context(bool requires_rrc_msg) override
  {
    logger.info("Received a new request to get RRC UE release context");
    rrc_ue_release_context release_context;
    // TODO: Add values
    return release_context;
  }

  rrc_ue_transfer_context get_transfer_context() override { return rrc_ue_transfer_context{}; }

  std::optional<rrc_meas_cfg> generate_meas_config(std::optional<rrc_meas_cfg> current_meas_config = {}) override
  {
    std::optional<rrc_meas_cfg> meas_config;
    return meas_config;
  }

  byte_buffer handle_rrc_handover_command(byte_buffer cmd) override { return byte_buffer{}; }

  byte_buffer get_rrc_handover_command(const rrc_reconfiguration_procedure_request& request,
                                       unsigned                                     transaction_id_) override
  {
    logger.info("Received a new request to get a RRC Handover Command.");
    return byte_buffer{};
  }

  void create_srb(const srb_creation_message& msg) override
  {
    logger.info("ue={} Creating {}", msg.ue_index, msg.srb_id);
    last_srb_id = msg.srb_id;
    srb_vec.push_back(msg.srb_id);
  }

  static_vector<srb_id_t, MAX_NOF_SRBS> get_srbs() override { return srb_vec; }

  // RRC UE Setup proc notifier
  void on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg) override{};
  void on_ue_release_required(const ngap_cause_t& cause) override{};

  // RRC UE Security Mode Command proc notifier
  void on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg) override{};

  // RRC UE Reconfiguration proc notifier

  // RRC UE Context handler
  rrc_ue_reestablishment_context_response get_context() override
  {
    logger.info("Received a new request to get RRC UE reestablishment context");
    return rrc_ue_reestablishment_context_response{};
  }

  // RRC UE Reestablishment proc notifier
  void on_new_as_security_context() override{};

  // interface functions
  rrc_ue_controller&              get_controller() override { return *this; };
  rrc_ul_pdu_handler&             get_ul_pdu_handler() override { return *this; };
  rrc_ngap_message_handler&       get_rrc_ngap_message_handler() override { return *this; };
  rrc_ue_control_message_handler& get_rrc_ue_control_message_handler() override { return *this; };
  rrc_ue_context_handler&         get_rrc_ue_context_handler() override { return *this; };

  std::optional<rrc_radio_bearer_config> last_radio_bearer_cfg;
  void                                   reset() { last_radio_bearer_cfg.reset(); }

  unsigned    last_transaction_id;
  srb_id_t    last_srb_id;
  byte_buffer last_ue_capabilities;

private:
  srslog::basic_logger&                 logger                      = srslog::fetch_basic_logger("TEST");
  bool                                  ue_cap_transfer_outcome     = true;
  bool                                  rrc_reconfiguration_outcome = false;
  unsigned                              test_transaction_id;
  static_vector<srb_id_t, MAX_NOF_SRBS> srb_vec;
};

class handover_reconfiguration_routine_test : public mobility_test
{
protected:
  handover_reconfiguration_routine_test() {}

  void create_ues(bool procedure_outcome, unsigned transaction_id_)
  {
    ue_index_t source_ue_index = get_ue_manager()->add_ue(source_du_index,
                                                          plmn_identity::test_value(),
                                                          int_to_gnb_du_id(0),
                                                          source_pci,
                                                          source_rnti,
                                                          du_cell_index_t::min);
    source_ue                  = get_ue_manager()->find_ue(source_ue_index);
    ASSERT_NE(source_ue, nullptr);
    source_rrc_ue.set_transaction_id(transaction_id_);
    source_ue->set_rrc_ue(source_rrc_ue);

    ue_index_t target_ue_index = get_ue_manager()->add_ue(target_du_index,
                                                          plmn_identity::test_value(),
                                                          int_to_gnb_du_id(0),
                                                          target_pci,
                                                          target_rnti,
                                                          du_cell_index_t::min);
    target_ue                  = get_ue_manager()->find_ue(target_ue_index);
    ASSERT_NE(target_ue, nullptr);
    cu_cp_handler.set_rrc_reconfiguration_outcome(procedure_outcome);
    target_ue->set_rrc_ue(target_rrc_ue);
  }

  void start_procedure()
  {
    rrc_reconfiguration_procedure_request request;

    t = launch_async<handover_reconfiguration_routine>(
        request, target_ue->get_ue_index(), *source_ue, source_f1ap_ue_ctxt_mng, cu_cp_handler, test_logger);
    t_launcher.emplace(t);
  }

  void set_sub_procedure_outcome(bool outcome)
  {
    source_f1ap_ue_ctxt_mng.set_ue_context_modification_outcome(ue_context_outcome_t{outcome, {}, {}, {}});
  }

  bool procedure_ready() const { return t.ready(); }

  const bool get_result() { return t.get(); }

  bool check_transaction_id(unsigned transaction_id) { return cu_cp_handler.last_transaction_id == transaction_id; }

private:
  // source UE parameters.
  du_index_t                    source_du_index = uint_to_du_index(0);
  pci_t                         source_pci      = 1;
  rnti_t                        source_rnti     = to_rnti(0x4601);
  dummy_rrc_ue                  source_rrc_ue;
  dummy_f1ap_ue_context_manager source_f1ap_ue_ctxt_mng;
  cu_cp_ue*                     source_ue = nullptr;

  // target UE parameters.
  du_index_t   target_du_index = uint_to_du_index(1);
  pci_t        target_pci      = 2;
  rnti_t       target_rnti     = to_rnti(0x4601);
  dummy_rrc_ue target_rrc_ue;
  cu_cp_ue*    target_ue = nullptr;

  async_task<bool>                        t;
  std::optional<lazy_task_launcher<bool>> t_launcher;
};

TEST_F(handover_reconfiguration_routine_test, when_reconfiguration_successful_then_return_true)
{
  unsigned test_transaction_id = 99;

  // Test Preamble.
  create_ues(true, test_transaction_id);

  set_sub_procedure_outcome(true);

  // it should be ready immediately
  start_procedure();

  ASSERT_TRUE(procedure_ready());

  // Reconfiguration complete was received.
  ASSERT_TRUE(get_result());

  ASSERT_TRUE(check_transaction_id(test_transaction_id));
}

TEST_F(handover_reconfiguration_routine_test, when_ue_context_mod_unsuccessful_then_return_false)
{
  unsigned test_transaction_id = 35;

  // Test Preamble.
  create_ues(false, test_transaction_id);

  set_sub_procedure_outcome(false);

  // it should be ready immediately
  start_procedure();

  ASSERT_TRUE(procedure_ready());

  // Reconfiguration complete was received.
  ASSERT_FALSE(get_result());

  ASSERT_FALSE(check_transaction_id(test_transaction_id));
}

TEST_F(handover_reconfiguration_routine_test, when_reconfiguration_unsuccessful_then_return_false)
{
  unsigned test_transaction_id = 17;

  // Test Preamble.
  create_ues(false, test_transaction_id);

  set_sub_procedure_outcome(true);

  // it should be ready immediately
  start_procedure();

  ASSERT_TRUE(procedure_ready());

  // Reconfiguration complete was received.
  ASSERT_FALSE(get_result());

  ASSERT_TRUE(check_transaction_id(test_transaction_id));
}
