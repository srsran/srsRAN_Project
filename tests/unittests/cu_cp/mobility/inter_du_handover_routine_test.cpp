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

#include "mobility_test_helpers.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
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
    du_index_t          du_index = source_du_index;
    gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
    gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(0);
    source_rnti                  = to_rnti(0x4601);
    source_pci                   = 1;
    amf_ue_id_t amf_ue_id        = uint_to_amf_ue_id(
        test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));
    ran_ue_id_t                   ran_ue_id        = uint_to_ran_ue_id(0);
    std::vector<pdu_session_id_t> psis             = {uint_to_pdu_session_id(1)};
    gnb_cu_cp_ue_e1ap_id_t        cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(0);
    gnb_cu_up_ue_e1ap_id_t        cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(0);

    // Connect AMF, DU, CU-UP.
    test_preamble_all_connected(du_index, source_pci);
    // Attach target DU.
    test_du_attach(target_du_index, target_du_id, target_nrcell_id, target_pci);
    // Attach UE.
    test_preamble_ue_full_attach(
        du_index, du_ue_id, cu_ue_id, source_rnti, amf_ue_id, ran_ue_id, psis, cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

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
                                         make_byte_buffer("000800410004015f741fe0804bf183fcaa6e9699").value());
    test_logger.info("Injecting UL RRC message (RRC Measurement Report)");
    f1c_gw.get_du(source_du_index).on_new_message(ul_rrc_msg);
  }

  /// \brief Start the inter-DU handover procedure.
  void start_procedure(pci_t source_pci_, rnti_t crnti, pci_t target_pci_)
  {
    // Not needed anymore
    cu_cp_obj->get_command_handler().get_mobility_command_handler().trigger_handover(source_pci_, crnti, target_pci_);
  }

  /// \brief Inject UE Context Setup Failure.
  void inject_ue_context_setup_failure()
  {
    f1ap_message ue_context_setup_fail =
        generate_ue_context_setup_failure(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0));
    f1c_gw.get_du(target_du_index).on_new_message(ue_context_setup_fail);
  }

  /// \brief Inject UE Context Setup Response.
  void inject_ue_context_setup_response()
  {
    f1ap_message ue_context_setup_resp = generate_ue_context_setup_response(
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
            "0120044014c00004620090e3800c")
            .value());
    f1c_gw.get_du(target_du_index).on_new_message(ue_context_setup_resp);
  }

  /// \brief Inject Bearer Context Modification Failure.
  void inject_bearer_context_modification_failure()
  {
    e1ap_message bearer_context_modification_fail =
        generate_bearer_context_modification_failure(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
    e1ap_gw.get_cu_up(0).on_new_message(bearer_context_modification_fail);
  }

  /// \brief Inject Bearer Context Modification Response.
  void inject_bearer_context_modification_response()
  {
    e1ap_message bearer_context_modification_resp =
        generate_bearer_context_modification_response(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
    e1ap_gw.get_cu_up(0).on_new_message(bearer_context_modification_resp);
  }

  /// \brief Inject Bearer Context Release Complete.
  void inject_bearer_context_release_complete()
  {
    e1ap_message bearer_context_release_complete =
        generate_bearer_context_release_complete(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
    e1ap_gw.get_cu_up(0).on_new_message(bearer_context_release_complete);
  }

  /// \brief Inject UE Context Modification Response.
  void inject_ue_context_modification_response()
  {
    f1ap_message ue_context_mod_resp = generate_ue_context_modification_response(
        int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0), to_rnti(0x4601));
    f1c_gw.get_du(source_du_index).on_new_message(ue_context_mod_resp);
  }

  /// \brief Inject RRC Reconfiguration Complete.
  void inject_rrc_reconfig_complete(std::optional<unsigned> transaction_id = {})
  {
    f1ap_message rrc_recfg_complete = generate_ul_rrc_message_transfer(int_to_gnb_cu_ue_f1ap_id(0),
                                                                       int_to_gnb_du_ue_f1ap_id(0),
                                                                       srb_id_t::srb1,
                                                                       make_byte_buffer("8000080035c41efd").value());
    f1c_gw.get_du(target_du_index).on_new_message(rrc_recfg_complete);
  }

  /// \brief Inject UE Context Release Complete.
  void inject_ue_context_release_complete(du_index_t du_index)
  {
    f1ap_message ue_context_release_complete =
        generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0));
    f1c_gw.get_du(du_index).on_new_message(ue_context_release_complete);
  }

  du_index_t get_source_du_index() { return source_du_index; }

  ue_index_t get_source_ue() { return source_ue_index; }

  unsigned get_target_pci() { return target_pci; }

  du_index_t get_target_du_index() { return target_du_index; }

  nr_cell_global_id_t get_target_cgi() { return target_cgi; }

  size_t get_nof_ues_in_target_du() const { return nof_du_ues(target_du_index); }
  size_t get_nof_ues_in_source_du() const { return nof_du_ues(source_du_index); }

private:
  size_t nof_du_ues(du_index_t idx) const
  {
    const metrics_report report = cu_cp_obj->get_metrics_handler().request_metrics_report();
    gnb_du_id_t          du_id  = report.dus.at((size_t)idx).id;
    return std::count_if(report.ues.begin(), report.ues.end(), [du_id](const auto& u) { return u.du_id == du_id; });
  }

  // source DU parameters.
  du_index_t source_du_index = uint_to_du_index(0);
  pci_t      source_pci;
  rnti_t     source_rnti;

  // target DU parameters.
  du_index_t          target_du_index  = uint_to_du_index(1);
  gnb_du_id_t         target_du_id     = int_to_gnb_du_id(0x22);
  nr_cell_identity    target_nrcell_id = nr_cell_identity::create(gnb_id_t{411, 22}, 1).value();
  nr_cell_global_id_t target_cgi       = {plmn_identity::test_value(), target_nrcell_id};
  unsigned            target_pci       = 2;

  ue_index_t source_ue_index = uint_to_ue_index(0);
};

TEST_F(inter_du_handover_routine_test, when_invalid_pci_is_used_then_ho_fails)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  // it should be ready immediately
  start_procedure(1, to_rnti(0x4601), INVALID_PCI);
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

  // Verify new UE has been deleted in target DU again.
  ASSERT_EQ(get_nof_ues_in_source_du(), 1);
  ASSERT_EQ(get_nof_ues_in_target_du(), 0);
}

TEST_F(inter_du_handover_routine_test, when_ho_succeeds_then_source_ue_is_removed)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  // Start handover by injecting measurement report
  inject_rrc_meas_report();

  // check that the UE Context Setup Request contains the UE capabilities
  ASSERT_EQ(f1c_gw.last_tx_pdus(1).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdus(1).back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request);
  ASSERT_NE(f1c_gw.last_tx_pdus(1)
                .back()
                .pdu.init_msg()
                .value.ue_context_setup_request()
                ->cu_to_du_rrc_info.ue_cap_rat_container_list.size(),
            0U);

  // Inject UE Context Setup Response
  inject_ue_context_setup_response();

  // Make sure Bearer Context Modification contains security info
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);
  ASSERT_TRUE(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.bearer_context_mod_request()->security_info_present);

  // Inject Bearer Context Modification Response
  inject_bearer_context_modification_response();

  // Inject UE Context Modification Response
  inject_ue_context_modification_response();

  // Inject RRC Reconfiguration Complete
  inject_rrc_reconfig_complete();

  // Inject UE Context Release Complete
  inject_ue_context_release_complete(get_source_du_index());

  // Verify that the HO was successful and the UE has been deleted in source DU.
  ASSERT_EQ(get_nof_ues_in_source_du(), 0);
  ASSERT_EQ(get_nof_ues_in_target_du(), 1);
}
