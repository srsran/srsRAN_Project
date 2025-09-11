/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/adt/byte_buffer.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/rrc/rrc_types.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/async/coroutine.h"
#include <chrono>
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

struct dummy_rrc_ue : public rrc_ue_interface, public rrc_ue_controller {
public:
  dummy_rrc_ue() = default;

  void set_rrc_reconfiguration_outcome(bool outcome) { rrc_reconfiguration_outcome = outcome; }

  void set_transaction_id(unsigned transaction_id_) { test_transaction_id = transaction_id_; }

  // RRC UE Controller
  void stop() override {}

  // RRC UL PDU handler
  void handle_ul_ccch_pdu(byte_buffer pdu) override {}
  void handle_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdu) override {}

  // RRC NGAP Message handler
  void        handle_dl_nas_transport_message(byte_buffer nas_pdu) override {}
  byte_buffer get_packed_ue_radio_access_cap_info() const override { return byte_buffer{}; }
  byte_buffer get_packed_handover_preparation_message() override { return byte_buffer{}; }

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

  async_task<bool> handle_handover_reconfiguration_complete_expected(uint8_t                   transaction_id_,
                                                                     std::chrono::milliseconds timeout_ms) override
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

  rrc_ue_release_context get_rrc_ue_release_context(bool                                requires_rrc_msg,
                                                    std::optional<std::chrono::seconds> release_wait_time) override
  {
    logger.info("Received a new request to get RRC UE release context");
    rrc_ue_release_context release_context;
    // TODO: Add values
    return release_context;
  }

  rrc_ue_transfer_context get_transfer_context() override
  {
    logger.info("Received a new request to get RRC UE trasnfer context");
    return rrc_ue_transfer_context{};
  }

  std::optional<rrc_meas_cfg>
  generate_meas_config(const std::optional<rrc_meas_cfg>& current_meas_config = std::nullopt) override
  {
    logger.info("Received a new request to generate RRC UE meas config");
    std::optional<rrc_meas_cfg> meas_config;
    return meas_config;
  }

  byte_buffer get_packed_meas_config() override
  {
    logger.info("Received a new request to get packed RRC UE meas config");
    return {};
  }

  byte_buffer handle_rrc_handover_command(byte_buffer cmd) override { return byte_buffer{}; }

  byte_buffer get_rrc_handover_command(const rrc_reconfiguration_procedure_request& request,
                                       unsigned                                     transaction_id_) override
  {
    logger.info("Received a new request to get a RRC Handover Command");
    return byte_buffer{};
  }

  bool handle_rrc_handover_preparation_info(byte_buffer pdu) override
  {
    logger.info("Received HandoverPreparationInfo");
    return true;
  }

  void create_srb(const srb_creation_message& msg) override
  {
    logger.info("ue={} Creating {}", msg.ue_index, msg.srb_id);
    last_srb_id = msg.srb_id;
    srb_vec.push_back(msg.srb_id);
  }

  static_vector<srb_id_t, MAX_NOF_SRBS> get_srbs() override { return srb_vec; }

  rrc_state get_rrc_state() const override { return rrc_state::connected; }

  void cancel_handover_reconfiguration_transaction(uint8_t transaction_id) override
  {
    logger.info("Received a new request to cancel RRC UE handover reconfiguration transaction");
  }

  // RRC UE Setup proc notifier
  void on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg) override {}
  void on_ue_release_required(const ngap_cause_t& cause) override {}

  // RRC UE Security Mode Command proc notifier
  void on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg) override {}

  // RRC UE Reconfiguration proc notifier

  // RRC UE Context handler
  rrc_ue_reestablishment_context_response get_context() override
  {
    logger.info("Received a new request to get RRC UE reestablishment context");
    return rrc_ue_reestablishment_context_response{};
  }

  // RRC UE Reestablishment proc notifier
  void on_new_as_security_context() override {}

  // interface functions
  rrc_ue_controller&              get_controller() override { return *this; }
  rrc_ul_pdu_handler&             get_ul_pdu_handler() override { return *this; }
  rrc_ngap_message_handler&       get_rrc_ngap_message_handler() override { return *this; }
  rrc_ue_control_message_handler& get_rrc_ue_control_message_handler() override { return *this; }
  rrc_ue_context_handler&         get_rrc_ue_context_handler() override { return *this; }

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
                                                          srs_cu_cp::du_cell_index_t::min);
    source_ue                  = get_ue_manager()->find_ue(source_ue_index);
    ASSERT_NE(source_ue, nullptr);
    source_rrc_ue.set_transaction_id(transaction_id_);
    source_ue->set_rrc_ue(source_rrc_ue);

    ue_index_t target_ue_index = get_ue_manager()->add_ue(target_du_index,
                                                          plmn_identity::test_value(),
                                                          int_to_gnb_du_id(0),
                                                          target_pci,
                                                          target_rnti,
                                                          srs_cu_cp::du_cell_index_t::min);
    target_ue                  = get_ue_manager()->find_ue(target_ue_index);
    ASSERT_NE(target_ue, nullptr);
    source_f1ap_ue_ctxt_mng.set_ue_context_modification_outcome(
        {procedure_outcome,
         {},
         {},
         {},
         make_byte_buffer(
             "5c04c00604b0701f00811a0f020001273b8c01692f30004d25e24040008c8040a26418d6d8d76006e08040000101000083446a48"
             "d802692f1200000464e35b63224f8060664abff0124e9106e28dc61b8e372c6fbf56c70eb00442c0680182c4602c020521004930"
             "b2a0003fe00000000060dc2108000780594008300000020698101450a000e3890000246aac90838002081840a1839389142c60d1"
             "c3c81100000850400800b50401000850501800b50502000850602800b506030c88500038c8b500040c88501048c5048143141405"
             "8c50581831418068c50681c3141c0780904020024120880905204963028811d159e26f2681d2083c5df81821c00000038ffd294a"
             "5294f2816000021976000000000000000000088ad5450047001800082000e21005c400e0202108001c4200b8401c080441000388"
             "401708038180842000710802e18070401104000e21005c300080000008218081018201c1a0001c71000000080100020180020240"
             "088029800008c48089c7001800")
             .value()});
    target_ue->set_rrc_ue(target_rrc_ue);
  }

  void start_procedure()
  {
    rrc_reconfiguration_procedure_request request;
    rrc_recfg_v1530_ies                   recfg_v1530_ies;
    recfg_v1530_ies.master_cell_group =
        make_byte_buffer(
            "5c00b001117aec701061e0007c0204683c080004120981950001ff000000000306e10840003c02ca00418000001034c080a2850007"
            "1c4800081aab2420e0008206102860e4e2450b183470f204400002140002002d40004002140406002d4040800214080a002d4080c0"
            "02140c0e002d40c1032214041201410050050481601414060050581a01418070050681e0141c080c5040220241481258c0a1c47456"
            "789bc9a074820f177e060870000000e3ff4a5294a53ca0580000865d8000000000000000000315aa8a008e003000104001c4200b88"
            "01c040421000388401708038100882000710802e10070301084000e21005c300e0802208001c4200b8600100000010430102030403"
            "83400038e20000001002000403000404801100530000118801138e0030")
            .value();
    recfg_v1530_ies.ded_sib1_delivery =
        make_byte_buffer("d20405c04100100800003800000cd8601584268000800119008144c831adb186cdc1008000020200010688d490484"
                         "c109a4bc48000011938d6d8c893e0181992affc0493a441b8a37186e38dcb1befd5b1c3a8021603400c162da08a")
            .value();
    request.non_crit_ext = recfg_v1530_ies;

    e1ap_bearer_context_modification_request target_bearer_context_modification_request;

    t = launch_async<handover_reconfiguration_routine>(request,
                                                       target_bearer_context_modification_request,
                                                       target_ue->get_ue_index(),
                                                       *source_ue,
                                                       source_f1ap_ue_ctxt_mng,
                                                       cu_cp_handler,
                                                       test_logger);
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
