/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_indication_procedure.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"

#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace asn1::e2ap;

e2_indication_procedure::e2_indication_procedure(e2_message_notifier&     notif_,
                                                 e2_event_manager&        ev_mng_,
                                                 timer_factory            timers,
                                                 e2_subscription_info_t   subscription_info_,
                                                 e2_du_metrics_interface& du_metrics_interface_,
                                                 srslog::basic_logger&    logger_) :
  notifier(notif_),
  ev_mng(ev_mng_),
  e2_indication_wait_timer(timers.create_timer()),
  logger(logger_),
  du_metrics_interface(du_metrics_interface_)
{
  subscription = subscription_info_;
}

void e2_indication_procedure::operator()(coro_context<eager_async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);
  while (running) {
    du_metrics_interface.get_metrics(_metrics);
    // request metrics from DU
    transaction_sink.subscribe_to(ev_mng.sub_del_request, (std::chrono::milliseconds)1000);
    CORO_AWAIT(transaction_sink);
    if (!transaction_sink.timeout_expired()) {
      logger.info("Subscription deleted");
      running = false;
    }
    for (auto action : subscription.action_list) {
      e2_indication_message e2_ind                        = {};
      e2_ind.request_id                                   = subscription.request_id;
      e2_ind.indication->ra_nfunction_id.value            = 1;
      e2_ind.indication->ri_caction_id.value              = action.ric_action_id;
      e2_ind.indication->ri_ccall_process_id_present      = false;
      e2_ind.indication->ri_cind_sn_present               = false;
      e2_ind.indication->ri_crequest_id->ric_instance_id  = subscription.request_id.ric_instance_id;
      e2_ind.indication->ri_crequest_id->ric_requestor_id = subscription.request_id.ric_requestor_id;
      e2_ind.indication->ri_cind_type.value               = ri_cind_type_opts::report;
      auto& action_def                                    = action.action_definition;
      handle_action(e2_ind, action_def);
      logger.info("Sending E2 indication");
      send_e2_indication(e2_ind);
    }
  }
  CORO_RETURN();
}

void e2_indication_procedure::handle_action(e2_indication_message&                        e2_ind,
                                            asn1::e2sm_kpm::e2_sm_kpm_action_definition_s action_def)
{
  auto action_type = action_def.action_definition_formats.type().value;
  switch (action_type) {
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format1:
      logger.warning("Action type 1 not supported yet");
      break;
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format2:
      logger.warning("Action type 2 not supported yet");
      break;
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format3: {
      handle_action_definition_format3(e2_ind, action_def.action_definition_formats.action_definition_format3());
    } break;
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format4:
      logger.warning("Action type 4 not supported yet");
      break;
    case asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
        action_definition_format5: {
      logger.warning("Action type 5 not supported yet");
    }
    default:
      logger.error("Action type not supported");
      break;
  }
}

void e2_indication_procedure::handle_action_definition_format3(
    e2_indication_message&                                e2_ind,
    asn1::e2sm_kpm::e2_sm_kpm_action_definition_format3_s action_def)
{
  // Used to carry UE-level measurement report for a group of UEs per measurement type matching subscribed conditions
  // from a target E2 Node
  ric_ind_message.ind_msg_formats.set_ind_msg_format2();
  ric_ind_message.ind_msg_formats.ind_msg_format2().granul_period_present = true;
  ric_ind_message.ind_msg_formats.ind_msg_format2().granul_period         = action_def.granul_period;
  scheduler_ue_metrics ue_metrics                                         = {};
  du_metrics_interface.get_metrics(ue_metrics);
  size_t num_ues = 1; // TODO get number of UEs from DU
  ric_ind_message.ind_msg_formats.ind_msg_format2().meas_data.resize(num_ues);
  ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list.resize(num_ues);
  for (unsigned int j = 0; j < num_ues; j++) {
    auto& meas_cond_list = action_def.meas_cond_list;
    for (auto& meas_cond : meas_cond_list) {
      if (check_measurement_name(meas_cond.meas_type, "CQI")) {
        matching_cond_item_s match_cond_item;
        match_cond_item.matching_cond_choice.set_test_cond_info();
        match_cond_item.matching_cond_choice.test_cond_info().test_type.set_cqi();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.set_value_int();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.value_int() = ue_metrics.cqi;
        ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list[j].meas_type.set_meas_name().from_string(
            "CQI");
        ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list[j].matching_cond.push_back(
            match_cond_item);
      } else if (check_measurement_name(meas_cond.meas_type, "RSRP")) {
        matching_cond_item_s match_cond_item;
        match_cond_item.matching_cond_choice.set_test_cond_info();
        match_cond_item.matching_cond_choice.test_cond_info().test_type.set_rsrp();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.set_value_int();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.value_int() = ue_metrics.pusch_snr_db;
        ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list[j].meas_type.set_meas_name().from_string(
            "RSRP");
        ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list[j].matching_cond.push_back(
            match_cond_item);
      } else if (check_measurement_name(meas_cond.meas_type, "RSRQ")) {
        matching_cond_item_s match_cond_item;
        match_cond_item.matching_cond_choice.set_test_cond_info();
        match_cond_item.matching_cond_choice.test_cond_info().test_type.set_rsrq();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.set_value_int();
        match_cond_item.matching_cond_choice.test_cond_info().test_value.value_int() = ue_metrics.pusch_snr_db;
        ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list[j].meas_type.set_meas_name().from_string(
            "RSRQ");
        ric_ind_message.ind_msg_formats.ind_msg_format2().meas_cond_ueid_list[j].matching_cond.push_back(
            match_cond_item);
      } else {
        logger.error("Unknown meas type %s", meas_cond.meas_type.meas_name().to_string().c_str());
      }
    }
  }
}

bool e2_indication_procedure::check_measurement_name(meas_type_c meas_type, const char* meas)
{
  if (strcmp(meas_type.meas_name().to_string().c_str(), meas) == 0) {
    return true;
  }
  return false;
}

void e2_indication_procedure::send_e2_indication(e2_indication_message& e2_ind)
{
  byte_buffer   ind_hdr_bytes;
  byte_buffer   ind_msg_bytes;
  asn1::bit_ref bref_ind_hdr(ind_hdr_bytes);
  if (ric_ind_header.pack(bref_ind_hdr) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack RIC Indication Header");
    return;
  };
  e2_ind.indication->ri_cind_hdr.value.resize(ind_hdr_bytes.length());
  std::copy(ind_hdr_bytes.begin(), ind_hdr_bytes.end(), e2_ind.indication->ri_cind_hdr.value.begin());
  asn1::bit_ref bref_ind_msg(ind_msg_bytes);
  if (ric_ind_message.pack(bref_ind_msg) != asn1::SRSASN_SUCCESS) {
    logger.error("Failed to pack RIC Indication Message");
    return;
  };
  e2_ind.indication->ri_cind_msg.value.resize(ind_msg_bytes.length());
  std::copy(ind_msg_bytes.begin(), ind_msg_bytes.end(), e2_ind.indication->ri_cind_msg.value.begin());
  e2_message e2_msg;
  e2_msg.pdu.set_init_msg();
  e2_msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_RI_CIND);
  e2_msg.pdu.init_msg().value.ri_cind() = e2_ind.indication;
  notifier.on_new_message(e2_msg);
}
