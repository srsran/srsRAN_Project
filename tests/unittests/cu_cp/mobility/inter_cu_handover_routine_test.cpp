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
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class inter_cu_handover_routine_test : public mobility_test
{
protected:
  inter_cu_handover_routine_test() {}

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
    ran_ue_id_t                   ran_ue_id        = uint_to_ran_ue_id(0);
    std::vector<pdu_session_id_t> psis             = {uint_to_pdu_session_id(1)};
    gnb_cu_cp_ue_e1ap_id_t        cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(0);
    gnb_cu_up_ue_e1ap_id_t        cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(0);

    // Connect AMF, DU, CU-UP.
    test_preamble_all_connected(du_index, pci);

    // Attach UE.
    test_preamble_ue_full_attach(
        du_index, du_ue_id, cu_ue_id, crnti, amf_ue_id, ran_ue_id, psis, cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);

    // Assert single UE attached to source DU.
    ASSERT_EQ(get_nof_ues_in_source_du(), 1);
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

  du_index_t get_source_du_index() { return source_du_index; }

  ue_index_t get_source_ue() { return source_ue_index; }

  unsigned get_target_pci() const { return target_pci; }

  size_t get_nof_ues_in_source_du() const { return nof_du_ues(source_du_index); }

  void check_handover_required_was_sent_to_amf() const
  {
    ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
    ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
              asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ho_required);
  }

private:
  size_t nof_du_ues(du_index_t idx) const
  {
    const metrics_report report = cu_cp_obj->get_metrics_handler().request_metrics_report();
    gnb_du_id_t          du_id  = report.dus.at((size_t)idx).id;
    return std::count_if(report.ues.begin(), report.ues.end(), [du_id](const auto& u) { return u.du_id == du_id; });
  }

  // source DU parameters.
  du_index_t source_du_index = uint_to_du_index(0);

  // target DU parameters.
  unsigned target_pci = 3;

  ue_index_t source_ue_index = uint_to_ue_index(0);
};

TEST_F(inter_cu_handover_routine_test, when_measurement_report_is_received_then_handover_required_is_send_to_amf)
{
  // Test Preamble.
  create_dus_and_attach_ue();

  // Start handover by injecting measurement report.
  inject_rrc_meas_report();

  // Check that handover required was sent to AMF.
  check_handover_required_was_sent_to_amf();
}
