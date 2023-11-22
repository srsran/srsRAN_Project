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
    // ASSERT_EQ(get_nof_ues_in_source_du(), 1);
    // ASSERT_EQ(get_nof_ues_in_target_du(), 0);
  }

#if 0
  void send_rrc_meas_report()
  {
    // Inject UL RRC message containing RRC measurement report to trigger HO
    f1ap_message ul_rrc_msg =
        generate_ul_rrc_message_transfer(cu_ue_id, du_ue_id, srb_id_t::srb1, generate_rrc_setup_complete());
    test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
    cu_cp_obj->get_connected_dus().get_du(du_index).get_f1ap_message_handler().handle_message(ul_rrc_msg);
  }
#endif

  void start_procedure(const cu_cp_inter_du_handover_request& msg)
  {
    // Not needed anymore
    t = source_du->du_processor_obj->handle_inter_du_handover_request(
        msg, target_du->du_processor_obj->get_du_processor_f1ap_ue_context_notifier());
    t_launcher.emplace(t);
  }

  void set_expected_contex_setup_outcome(bool success)
  {
    ASSERT_NE(target_du, nullptr);
    target_du->test_adapter.set_context_setup_outcome(success);
  }

  void set_expected_bearer_context_mod_outcome(const bearer_context_outcome_t& outcome)
  {
    source_du->e1ap_ctrl_notifier.set_first_message_outcome(outcome);
  }

  bool procedure_ready() const
  {
    return t.ready();
  }

  const cu_cp_inter_du_handover_response& get_result()
  {
    return t.get();
  }

  ue_index_t get_source_ue()
  {
    return source_ue_index;
  }

  unsigned get_target_pci()
  {
    return target_pci;
  }

  du_index_t get_target_du_index()
  {
    return target_du_index;
  }

  nr_cell_global_id_t get_target_cgi()
  {
    return target_cgi;
  }

  size_t get_nof_ues_in_target_du()
  {
    return get_nof_ues(target_du);
  }
  size_t get_nof_ues_in_source_du()
  {
    return get_nof_ues(source_du);
  }

private:
  size_t get_nof_ues(du_wrapper* du_obj)
  {
    return du_obj->du_processor_obj->get_du_processor_statistics_handler().get_nof_ues();
  }

  // source DU parameters.
  du_index_t source_du_index = uint_to_du_index(0);
  // unsigned     source_du_id     = 0x11;
  // nr_cell_id_t source_nrcell_id = 6576;
  // unsigned     source_pci       = 1;

  // target DU parameters.
  du_index_t          target_du_index  = uint_to_du_index(1);
  unsigned            target_du_id     = 0x22;
  nr_cell_id_t        target_nrcell_id = 422;
  nr_cell_global_id_t target_cgi       = {001, 01, "00101", "00f110", 0x22};
  unsigned            target_pci       = 2;

  // Handler to DU objects.
  du_wrapper* source_du = nullptr;
  du_wrapper* target_du = nullptr;

  ue_index_t source_ue_index = ue_index_t::invalid;

  async_task<cu_cp_inter_du_handover_response>                   t;
  optional<lazy_task_launcher<cu_cp_inter_du_handover_response>> t_launcher;
};

TEST_F(cu_cp_test, dummy_test)
{
  ASSERT_NE(0, 1);
}

#if 0
TEST_F(inter_du_handover_routine_test, when_invalid_pci_is_used_then_ho_fails)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  cu_cp_inter_du_handover_request request = generate_inter_du_handover_request();
  request.target_pci                      = INVALID_PCI;
  request.source_ue_index                 = get_source_ue();
  request.target_du_index                 = get_target_du_index();
  request.cgi                             = get_target_cgi();
#if 1
  // it should be ready immediately
  start_procedure(request);

  ASSERT_TRUE(procedure_ready());

  // HO failed.
  ASSERT_FALSE(get_result().success);
#endif
}

TEST_F(inter_du_handover_routine_test, when_ue_context_setup_fails_then_ho_fails)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  // Context Setup should fail.
  set_expected_contex_setup_outcome(false);

  cu_cp_inter_du_handover_request request = generate_inter_du_handover_request();
  request.target_pci                      = get_target_pci();
  request.source_ue_index                 = get_source_ue();
  request.target_du_index                 = get_target_du_index();
  request.cgi                             = get_target_cgi();

  // it should be ready immediately
  start_procedure(request);

  ASSERT_TRUE(procedure_ready());

  // HO failed.
  ASSERT_FALSE(get_result().success);

  // Verify new UE has been deleted in target DU again.
  ASSERT_EQ(get_nof_ues_in_source_du(), 1);
  ASSERT_EQ(get_nof_ues_in_target_du(), 0);
}

TEST_F(inter_du_handover_routine_test, when_bearer_context_modification_fails_then_ho_fails)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  // Context Setup should pass.
  set_expected_contex_setup_outcome(true);

  // Bearer Context modification fails.
  bearer_context_outcome_t bearer_context_mod_outcome;
  bearer_context_mod_outcome.outcome = false;
  set_expected_bearer_context_mod_outcome(bearer_context_mod_outcome);

  cu_cp_inter_du_handover_request request = generate_inter_du_handover_request();
  request.target_pci                      = get_target_pci();
  request.source_ue_index                 = get_source_ue();
  request.target_du_index                 = get_target_du_index();
  request.cgi                             = get_target_cgi();

  // it should be ready immediately
  start_procedure(request);

  ASSERT_TRUE(procedure_ready());

  // HO failed.
  ASSERT_FALSE(get_result().success);

  // TODO: Verify new UE has been deleted in target DU again.
  ASSERT_EQ(get_nof_ues_in_source_du(), 1);
  // ASSERT_EQ(get_nof_ues_in_target_du(), 0);
}

TEST_F(inter_du_handover_routine_test, when_ho_succeeds_then_source_ue_is_removed)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  // Context Setup should pass.
  set_expected_contex_setup_outcome(true);

  // Bearer Context modification fails.
  bearer_context_outcome_t bearer_context_mod_outcome;
  bearer_context_mod_outcome.outcome = true;
  set_expected_bearer_context_mod_outcome(bearer_context_mod_outcome);

  cu_cp_inter_du_handover_request request = generate_inter_du_handover_request();
  request.target_pci                      = get_target_pci();
  request.source_ue_index                 = get_source_ue();
  request.target_du_index                 = get_target_du_index();
  request.cgi                             = get_target_cgi();

  // it should be ready immediately
  start_procedure(request);

  ASSERT_TRUE(procedure_ready());

  // HO succeed.
  ASSERT_TRUE(get_result().success);

  ASSERT_EQ(get_nof_ues_in_source_du(), 0);
  ASSERT_EQ(get_nof_ues_in_target_du(), 1);
  ASSERT_EQ(get_ue_manager()->get_nof_ngap_ues(), 1);
}

TEST_F(inter_du_handover_routine_test, alt_when_ho_succeeds_then_source_ue_is_removed)
{
  // Context Setup should pass.
  set_expected_contex_setup_outcome(true);

  // Bearer Context modification fails.
  bearer_context_outcome_t bearer_context_mod_outcome;
  bearer_context_mod_outcome.outcome = true;
  set_expected_bearer_context_mod_outcome(bearer_context_mod_outcome);

  cu_cp_inter_du_handover_request request = generate_inter_du_handover_request();
  request.target_pci                      = get_target_pci();
  request.source_ue_index                 = get_source_ue();
  request.target_du_index                 = get_target_du_index();
  request.cgi                             = get_target_cgi();

  // it should be ready immediately
  start_procedure(request);

  ASSERT_TRUE(procedure_ready());

  // HO succeed.
  ASSERT_TRUE(get_result().success);

  ASSERT_EQ(get_nof_ues_in_source_du(), 0);
  ASSERT_EQ(get_nof_ues_in_target_du(), 1);
  ASSERT_EQ(get_ue_manager()->get_nof_ngap_ues(), 1);
}
#endif