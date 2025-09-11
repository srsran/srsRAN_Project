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

#include "lib/cu_cp/metrics_handler/metrics_handler_impl.h"
#include "lib/cu_cp/mobility_manager/mobility_manager_impl.h"
#include "lib/cu_cp/ngap_repository.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class dummy_ue_metrics_handler : public ue_metrics_handler,
                                 public du_repository_metrics_handler,
                                 public ngap_repository_metrics_handler,
                                 public mobility_manager_metrics_handler
{
public:
  cu_cp_metrics_report next_metrics;

  std::vector<cu_cp_metrics_report::ue_info> handle_ue_metrics_report_request() const override
  {
    return next_metrics.ues;
  }

  std::vector<cu_cp_metrics_report::du_info> handle_du_metrics_report_request() const override
  {
    return next_metrics.dus;
  }

  std::vector<ngap_info> handle_ngap_metrics_report_request() const override { return next_metrics.ngaps; }

  mobility_management_metrics handle_mobility_metrics_report_request() const override { return next_metrics.mobility; }
};

class dummy_metrics_notifier : public cu_cp_metrics_report_notifier
{
public:
  std::optional<cu_cp_metrics_report> last_metrics_report;

  void notify_metrics_report_request(const cu_cp_metrics_report& report) override { last_metrics_report = report; }
};

TEST(metrics_handler_test, get_periodic_metrics_report_while_session_is_active)
{
  manual_task_worker       worker{16};
  timer_manager            timers{2};
  dummy_ue_metrics_handler metrics_hdlr;
  metrics_handler_impl     metrics{worker, timers, metrics_hdlr, metrics_hdlr, metrics_hdlr, metrics_hdlr};

  std::chrono::milliseconds period{5};
  dummy_metrics_notifier    metrics_notifier;
  auto session = metrics.create_periodic_report_session(periodic_metric_report_request{period, &metrics_notifier});

  // First report.
  metrics_hdlr.next_metrics.ues.emplace_back(
      cu_cp_metrics_report::ue_info{to_rnti(1), int_to_gnb_du_id(0), pci_t{2}, rrc_state::connected});

  cu_cp_metrics_report::cell_info cell_info{
      nr_cell_global_id_t{plmn_identity::test_value(), nr_cell_identity::create(0x22).value()}, pci_t{2}};
  establishment_cause_t connection_cause{establishment_cause_t::mt_access};
  rrc_du_metrics        rrc_metrics{2, 4, {}, {}, 1, 2, 3};
  rrc_metrics.attempted_rrc_connection_establishments.increase(connection_cause);
  rrc_metrics.successful_rrc_connection_establishments.increase(connection_cause);
  metrics_hdlr.next_metrics.dus.emplace_back(
      cu_cp_metrics_report::du_info{int_to_gnb_du_id(0), {cell_info}, rrc_metrics});

  ngap_cause_t          cause{ngap_cause_radio_network_t::multiple_pdu_session_id_instances};
  pdu_session_metrics_t pdu_session_metrics{2, 1, {}};
  pdu_session_metrics.nof_pdu_sessions_failed_to_setup.increase(cause);
  ngap_metrics next_ngap_metrics;
  s_nssai_t    snssai{slice_service_type{1}, slice_differentiator{}};
  next_ngap_metrics.pdu_session_metrics.emplace(snssai, pdu_session_metrics);
  next_ngap_metrics.nof_cn_initiated_paging_requests = 5;
  metrics_hdlr.next_metrics.ngaps.emplace_back(ngap_info{"open5gs-amf0", next_ngap_metrics});

  metrics_hdlr.next_metrics.mobility.nof_handover_executions_requested    = 2;
  metrics_hdlr.next_metrics.mobility.nof_successful_handover_executions   = 1;
  metrics_hdlr.next_metrics.mobility.nof_handover_preparations_requested  = 2;
  metrics_hdlr.next_metrics.mobility.nof_successful_handover_preparations = 1;

  std::string ngap_out_str = format_ngap_metrics(metrics_hdlr.next_metrics.ngaps, metrics_hdlr.next_metrics.mobility);
  std::string ngap_exp_str =
      "[ amf_name=open5gs-amf0 s-nssai=(sst=1 sd=na) nof_pdu_sessions_requested_to_setup=2 "
      "nof_pdu_sessions_successfully_setup=1 nof_pdu_sessions_failed_to_setup=[ radio_network-unspecified=0 "
      "radio_network-txnrelocoverall_expiry=0 radio_network-successful_ho=0 "
      "radio_network-release_due_to_ngran_generated_reason=0 radio_network-release_due_to_5gc_generated_reason=0 "
      "radio_network-ho_cancelled=0 radio_network-partial_ho=0 "
      "radio_network-ho_fail_in_target_5_gc_ngran_node_or_target_sys=0 radio_network-ho_target_not_allowed=0 "
      "radio_network-tngrelocoverall_expiry=0 radio_network-tngrelocprep_expiry=0 radio_network-cell_not_available=0 "
      "radio_network-unknown_target_id=0 radio_network-no_radio_res_available_in_target_cell=0 "
      "radio_network-unknown_local_ue_ngap_id=0 radio_network-inconsistent_remote_ue_ngap_id=0 "
      "radio_network-ho_desirable_for_radio_reason=0 radio_network-time_crit_ho=0 radio_network-res_optim_ho=0 "
      "radio_network-reduce_load_in_serving_cell=0 radio_network-user_inactivity=0 "
      "radio_network-radio_conn_with_ue_lost=0 radio_network-radio_res_not_available=0 "
      "radio_network-invalid_qos_combination=0 radio_network-fail_in_radio_interface_proc=0 "
      "radio_network-interaction_with_other_proc=0 radio_network-unknown_pdu_session_id=0 "
      "radio_network-unkown_qos_flow_id=0 radio_network-multiple_pdu_session_id_instances=1 "
      "radio_network-multiple_qos_flow_id_instances=0 "
      "radio_network-encryption_and_or_integrity_protection_algorithms_not_supported=0 "
      "radio_network-ng_intra_sys_ho_triggered=0 radio_network-ng_inter_sys_ho_triggered=0 "
      "radio_network-xn_ho_triggered=0 radio_network-not_supported_5qi_value=0 radio_network-ue_context_transfer=0 "
      "radio_network-ims_voice_eps_fallback_or_rat_fallback_triggered=0 "
      "radio_network-up_integrity_protection_not_possible=0 radio_network-up_confidentiality_protection_not_possible=0 "
      "radio_network-slice_not_supported=0 radio_network-ue_in_rrc_inactive_state_not_reachable=0 "
      "radio_network-redirection=0 radio_network-res_not_available_for_the_slice=0 "
      "radio_network-ue_max_integrity_protected_data_rate_reason=0 radio_network-release_due_to_cn_detected_mob=0 "
      "radio_network-n26_interface_not_available=0 radio_network-release_due_to_pre_emption=0 "
      "radio_network-multiple_location_report_ref_id_instances=0 radio_network-rsn_not_available_for_the_up=0 "
      "radio_network-npn_access_denied=0 radio_network-cag_only_access_denied=0 radio_network-insufficient_ue_cap=0 "
      "radio_network-redcap_ue_not_supported=0 radio_network-unknown_mbs_session_id=0 "
      "radio_network-indicated_mbs_session_area_info_not_served_by_the_gnb=0 "
      "radio_network-inconsistent_slice_info_for_the_session=0 radio_network-misaligned_assoc_for_multicast_unicast=0 "
      "transport-transport_res_unavailable=0 transport-unspecified=0 nas-normal_release=0 nas-authentication_fail=0 "
      "nas-deregister=0 nas-unspecified=0 protocol-transfer_syntax_error=0 protocol-abstract_syntax_error_reject=0 "
      "protocol-abstract_syntax_error_ignore_and_notify=0 protocol-msg_not_compatible_with_receiver_state=0 "
      "protocol-semantic_error=0 protocol-abstract_syntax_error_falsely_constructed_msg=0 protocol-unspecified=0 "
      "misc-ctrl_processing_overload=0 misc-not_enough_user_plane_processing_res=0 misc-hardware_fail=0 "
      "misc-om_intervention=0 misc-unknown_plmn_or_sn_pn=0 ] nof_cn_initiated_paging_requests=5 ], "
      "nof_handover_preparations_requested=2 nof_successful_handover_preparations=1";

  std::string rrc_out_str = format_rrc_metrics(metrics_hdlr.next_metrics.dus, metrics_hdlr.next_metrics.mobility);
  std::string rrc_exp_str =
      "[ gnb_du_id=0 mean_nof_rrc_connections=2 max_nof_rrc_connections=4 "
      "attempted_rrc_connection_establishments=[ emergency=0 high_prio_access=0 mt_access=1 mo_sig=0 mo_data=0 "
      "mo_voice_call=0 mo_video_call=0 mo_sms=0 mps_prio_access=0 mcs_prio_access=0 ] "
      "successful_rrc_connection_establishments=[ emergency=0 high_prio_access=0 mt_access=1 mo_sig=0 mo_data=0 "
      "mo_voice_call=0 mo_video_call=0 mo_sms=0 mps_prio_access=0 mcs_prio_access=0 ] "
      "attempted_rrc_connection_reestablishments=1 successful_rrc_connection_reestablishments_with_ue_context=2 "
      "successful_rrc_connection_reestablishments_without_ue_context=3 ], nof_handover_executions_requested=2 "
      "nof_successful_handover_executions=1";

  for (unsigned i = 0; i != period.count(); ++i) {
    ASSERT_FALSE(metrics_notifier.last_metrics_report.has_value());
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_TRUE(metrics_notifier.last_metrics_report.has_value());
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues.size(), 1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues[0].rnti, metrics_hdlr.next_metrics.ues[0].rnti);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues[0].pci, metrics_hdlr.next_metrics.ues[0].pci);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues[0].rrc_connection_state,
            metrics_hdlr.next_metrics.ues[0].rrc_connection_state);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus.size(), 1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus[0].rrc_metrics.mean_nof_rrc_connections, 2);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus[0].rrc_metrics.max_nof_rrc_connections, 4);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus[0].rrc_metrics.attempted_rrc_connection_establishments.get_count(
                connection_cause),
            1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus[0].rrc_metrics.successful_rrc_connection_establishments.get_count(
                connection_cause),
            1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus[0].rrc_metrics.attempted_rrc_connection_reestablishments, 1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus[0]
                .rrc_metrics.successful_rrc_connection_reestablishments_with_ue_context,
            2);
  ASSERT_EQ(metrics_notifier.last_metrics_report->dus[0]
                .rrc_metrics.successful_rrc_connection_reestablishments_without_ue_context,
            3);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ngaps[0].amf_name, "open5gs-amf0");
  ASSERT_EQ(metrics_notifier.last_metrics_report->ngaps[0].metrics.pdu_session_metrics.size(), 1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ngaps[0].metrics.pdu_session_metrics.begin()->first, snssai);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ngaps[0]
                .metrics.pdu_session_metrics.begin()
                ->second.nof_pdu_sessions_requested_to_setup,
            2);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ngaps[0]
                .metrics.pdu_session_metrics.begin()
                ->second.nof_pdu_sessions_successfully_setup,
            1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ngaps[0]
                .metrics.pdu_session_metrics.begin()
                ->second.nof_pdu_sessions_failed_to_setup.get_count(cause),
            1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->ngaps[0].metrics.nof_cn_initiated_paging_requests, 5);
  ASSERT_EQ(metrics_notifier.last_metrics_report->mobility.nof_handover_executions_requested, 2);
  ASSERT_EQ(metrics_notifier.last_metrics_report->mobility.nof_successful_handover_executions, 1);
  ASSERT_EQ(metrics_notifier.last_metrics_report->mobility.nof_handover_preparations_requested, 2);
  ASSERT_EQ(metrics_notifier.last_metrics_report->mobility.nof_successful_handover_preparations, 1);

  // Second report.
  metrics_notifier.last_metrics_report.reset();
  metrics_hdlr.next_metrics.ues.emplace_back(
      cu_cp_metrics_report::ue_info{to_rnti(2), int_to_gnb_du_id(0), pci_t{3}, rrc_state::connected});
  for (unsigned i = 0; i != period.count(); ++i) {
    ASSERT_FALSE(metrics_notifier.last_metrics_report.has_value());
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_TRUE(metrics_notifier.last_metrics_report.has_value());
  ASSERT_EQ(metrics_notifier.last_metrics_report->ues.size(), 2);

  // Destroy session.
  metrics_notifier.last_metrics_report.reset();
  session.reset();
  for (unsigned i = 0; i != period.count(); ++i) {
    ASSERT_FALSE(metrics_notifier.last_metrics_report.has_value());
    timers.tick();
    worker.run_pending_tasks();
  }
  ASSERT_FALSE(metrics_notifier.last_metrics_report.has_value());
}
