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

#include "inter_du_handover_routine_test_helpers.h"
#include "lib/e1ap/cu_cp/e1ap_cu_cp_asn1_helpers.h"
#include "mobility_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class inter_du_handover_routine_test : public mobility_test
{
protected:
  inter_du_handover_routine_test() {}

  /// \brief Create two DUs and attach a single UE to the first DU.
  void create_dus_and_attach_ue()
  {
    // Test preamble to create CU-CP, attach to 5GC, attach CU-UP, create and attach DU and attach UE.
    du_index_t          du_index  = source_du_index;
    gnb_cu_ue_f1ap_id_t cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(0);
    gnb_du_ue_f1ap_id_t du_ue_id  = int_to_gnb_du_ue_f1ap_id(0);
    rnti_t              crnti     = to_rnti(0x4601);
    pci_t               pci       = 1;
    amf_ue_id_t         amf_ue_id = uint_to_amf_ue_id(
        test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
    ran_ue_id_t            ran_ue_id        = uint_to_ran_ue_id(0);
    gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(0);
    gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(0);

    test_preamble_ue_full_attach(
        du_index, du_ue_id, cu_ue_id, pci, crnti, amf_ue_id, ran_ue_id, cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

    // Attach target DU.
    test_du_attach(target_du_index, target_du_id, target_nrcell_id, target_pci);

    // Assert single UE attached to source DU.
    ASSERT_EQ(get_nof_ues_in_source_du(), 1);
    ASSERT_EQ(get_nof_ues_in_target_du(), 0);
  }

  /// \brief Inject an RRC measurement report to trigger handover.
  void inject_rrc_meas_report()
  {
    // Inject UL RRC message containing RRC measurement report to trigger HO
    f1ap_message ul_rrc_msg =
        generate_ul_rrc_message_transfer(int_to_gnb_cu_ue_f1ap_id(0),
                                         int_to_gnb_du_ue_f1ap_id(0),
                                         srb_id_t::srb1,
                                         make_byte_buffer("000800410004015f741fe0804bf183fcaa6e9699"));
    test_logger.info("Injecting UL RRC message (RRC Measurement Report)");
    cu_cp_obj->get_connected_dus().get_du(source_du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg);
  }

  /// \brief Start the inter-DU handover procedure.
  void start_procedure(const cu_cp_inter_du_handover_request& msg)
  {
    // Not needed anymore
    t = cu_cp_obj->get_connected_dus()
            .get_du(source_du_index)
            .get_mobility_handler()
            .handle_inter_du_handover_request(
                msg,
                cu_cp_obj->get_connected_dus().get_du(target_du_index).get_f1ap_ue_context_notifier(),
                cu_cp_obj->get_connected_dus().get_du(target_du_index).get_du_processor_ue_context_notifier());
    t_launcher.emplace(t);
  }

  /// \brief Inject UE Context Setup Failure.
  void inject_ue_context_setup_failure()
  {
    f1ap_message ue_context_setup_fail =
        generate_ue_context_setup_failure(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0));
    cu_cp_obj->get_connected_dus()
        .get_du(target_du_index)
        .get_f1ap_message_handler()
        .handle_message(ue_context_setup_fail);
  }

  /// \brief Inject UE Context Setup Response.
  void inject_ue_context_setup_response()
  {
    f1ap_message ue_context_setup_fail = generate_ue_context_setup_response(
        int_to_gnb_cu_ue_f1ap_id(0),
        int_to_gnb_du_ue_f1ap_id(0),
        to_rnti(0x4601),
        make_byte_buffer(
            "5c06c0060030258380f80408d07810000929dc601349798002692f1200000464c6b6c61b3704020000080800041a235246c0134978"
            "90000023271adb19127c03033255878092748837146e30dc71b9637dfab6387580221603400c162300e20981950001ff0000000003"
            "06e10840000402ca0041904000040d31a01100102000e3888448004080038e2221400102000e3888c60004080038e24060088681aa"
            "b2420e0008206102860e4a60c9a3670e8f00000850000800b50001000850101800b50102000850202800b50203000850303800b503"
            "0b8c8b5040c00504032014120d00505036014160e0050603a0141a120c506a0496302a72fd159e26f2681d2083c5df81821c000000"
            "38ffd294a5294f28160000219760000000000005000001456aa28023800c00041000710804e20070101084000e21009c200e040220"
            "8001c420138401c0c042100038840270c038200882000710804e18004000000410c04080c100e0d0000e388000000400800100c001"
            "0120044014c00004620090e3800c"));
    cu_cp_obj->get_connected_dus()
        .get_du(target_du_index)
        .get_f1ap_message_handler()
        .handle_message(ue_context_setup_fail);
  }

  /// \brief Inject Bearer Context Modification Failure.
  void inject_bearer_context_modification_failure()
  {
    e1ap_message bearer_context_modification_fail =
        generate_bearer_context_modification_failure(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
    cu_cp_obj->get_connected_cu_ups()
        .get_cu_up(uint_to_cu_up_index(0))
        .get_e1ap_message_handler()
        .handle_message(bearer_context_modification_fail);
  }

  /// \brief Inject Bearer Context Modification Response.
  void inject_bearer_context_modification_response()
  {
    e1ap_message bearer_context_modification_resp =
        generate_bearer_context_modification_response(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
    cu_cp_obj->get_connected_cu_ups()
        .get_cu_up(uint_to_cu_up_index(0))
        .get_e1ap_message_handler()
        .handle_message(bearer_context_modification_resp);
  }

  /// \brief Inject Bearer Context Release Complete.
  void inject_bearer_context_release_complete()
  {
    e1ap_message bearer_context_release_complete =
        generate_bearer_context_release_complete(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
    cu_cp_obj->get_connected_cu_ups()
        .get_cu_up(uint_to_cu_up_index(0))
        .get_e1ap_message_handler()
        .handle_message(bearer_context_release_complete);
  }

  /// \brief Inject RRC Reconfiguration Complete.
  void inject_rrc_reconfig_complete()
  {
    f1ap_message rrc_recfg_complete = generate_ul_rrc_message_transfer(
        int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0), srb_id_t::srb1, make_byte_buffer("8000080035c41efd"));
    cu_cp_obj->get_connected_dus()
        .get_du(target_du_index)
        .get_f1ap_message_handler()
        .handle_message(rrc_recfg_complete);
  }

  /// \brief Inject UE Context Release Complete.
  void inject_ue_context_release_complete(du_index_t du_index)
  {
    f1ap_message ue_context_release_complete =
        generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0));
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(
        ue_context_release_complete);
  }

  bool procedure_ready() const { return t.ready(); }

  const cu_cp_inter_du_handover_response& get_result() { return t.get(); }

  du_index_t get_source_du_index() { return source_du_index; }

  ue_index_t get_source_ue() { return source_ue_index; }

  unsigned get_target_pci() { return target_pci; }

  du_index_t get_target_du_index() { return target_du_index; }

  nr_cell_global_id_t get_target_cgi() { return target_cgi; }

  size_t get_nof_ues_in_target_du()
  {
    return cu_cp_obj->get_connected_dus().get_du(target_du_index).get_f1ap_statistics_handler().get_nof_ues();
  }
  size_t get_nof_ues_in_source_du()
  {
    return cu_cp_obj->get_connected_dus().get_du(source_du_index).get_f1ap_statistics_handler().get_nof_ues();
  }

private:
  // source DU parameters.
  du_index_t source_du_index = uint_to_du_index(0);

  // target DU parameters.
  du_index_t          target_du_index  = uint_to_du_index(1);
  unsigned            target_du_id     = 0x22;
  nr_cell_id_t        target_nrcell_id = 34;
  nr_cell_global_id_t target_cgi       = {001, 01, "00101", "00f110", 0x22};
  unsigned            target_pci       = 2;

  ue_index_t source_ue_index = uint_to_ue_index(0);

  async_task<cu_cp_inter_du_handover_response>                   t;
  optional<lazy_task_launcher<cu_cp_inter_du_handover_response>> t_launcher;
};

TEST_F(inter_du_handover_routine_test, when_invalid_pci_is_used_then_ho_fails)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  cu_cp_inter_du_handover_request request = generate_inter_du_handover_request();
  request.target_pci                      = INVALID_PCI;
  request.source_ue_index                 = get_source_ue();
  request.target_du_index                 = get_target_du_index();
  request.cgi                             = get_target_cgi();

  // it should be ready immediately
  start_procedure(request);

  ASSERT_TRUE(procedure_ready());

  // HO failed.
  ASSERT_FALSE(get_result().success);
}

TEST_F(inter_du_handover_routine_test, when_ue_context_setup_fails_then_ho_fails)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  // Start handover by injecting measurement report
  inject_rrc_meas_report();

  // Inject UE Context Setup Failure
  inject_ue_context_setup_failure();

  // Verify new UE has been deleted in target DU again.
  ASSERT_EQ(get_nof_ues_in_source_du(), 1);
  ASSERT_EQ(get_nof_ues_in_target_du(), 0);
}

TEST_F(inter_du_handover_routine_test, when_bearer_context_modification_fails_then_ho_fails)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  // Start handover by injecting measurement report
  inject_rrc_meas_report();

  // Inject UE Context Setup Response
  inject_ue_context_setup_response();

  // Inject Bearer Context Modification Failure
  inject_bearer_context_modification_failure();

  // Inject UE Context Release Complete
  inject_ue_context_release_complete(get_target_du_index());

  // TODO: Verify new UE has been deleted in target DU again.
  ASSERT_EQ(get_nof_ues_in_source_du(), 1);
  ASSERT_EQ(get_nof_ues_in_target_du(), 0);
}

TEST_F(inter_du_handover_routine_test, when_ho_succeeds_then_source_ue_is_removed)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  cu_cp_inter_du_handover_request request = generate_inter_du_handover_request();
  request.target_pci                      = get_target_pci();
  request.source_ue_index                 = get_source_ue();
  request.target_du_index                 = get_target_du_index();
  request.cgi                             = get_target_cgi();

  start_procedure(request);

  // Start handover by injecting measurement report
  // TODO: Replace manual task worker to allow execution of tasks on source and target UE for context transfer
  // inject_rrc_meas_report();

  // Inject UE Context Setup Response
  inject_ue_context_setup_response();

  // Inject Bearer Context Modification Response
  inject_bearer_context_modification_response();

  // Inject RRC Reconfiguration Complete
  inject_rrc_reconfig_complete();

  // Inject UE Context Release Complete
  inject_ue_context_release_complete(get_source_du_index());

  ASSERT_TRUE(procedure_ready());

  // HO succeed.
  ASSERT_TRUE(get_result().success);

  ASSERT_EQ(get_nof_ues_in_source_du(), 0);
  ASSERT_EQ(get_nof_ues_in_target_du(), 1);
}
