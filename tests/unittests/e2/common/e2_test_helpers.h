/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/e2/common/e2_subscription_manager_impl.h"
#include "lib/e2/common/e2ap_asn1_packer.h"
#include "lib/e2/e2sm/e2sm_kpm_asn1_packer.h"
#include "lib/e2/e2sm/e2sm_kpm_impl.h"
#include "lib/pcap/dlt_pcap_impl.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_factory.h"
#include "srsran/e2/e2ap_configuration_helpers.h"
#include "srsran/e2/e2sm/e2sm_factory.h"
#include "srsran/gateways/network_gateway.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/timers.h"
#include <gtest/gtest.h>

namespace srsran {

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_e2_pdu_notifier : public e2_message_notifier
{
public:
  dummy_e2_pdu_notifier(e2_message_handler* handler_) : logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(e2_message_handler* handler_) { handler = handler_; };
  void on_new_message(const e2_message& msg) override
  {
    switch (msg.pdu.type().value) {
      case asn1::e2ap::e2_ap_pdu_c::types_opts::init_msg:
        logger.info("Received a E2AP PDU of type {}.{}",
                    msg.pdu.type().to_string(),
                    msg.pdu.init_msg().value.type().to_string());
        break;
      case asn1::e2ap::e2_ap_pdu_c::types_opts::successful_outcome:
        logger.info("Received a E2AP PDU of type {}.{}",
                    msg.pdu.type().to_string(),
                    msg.pdu.successful_outcome().value.type().to_string());
        break;
      case asn1::e2ap::e2_ap_pdu_c::types_opts::unsuccessful_outcome:
        logger.info("Received a E2AP PDU of type {}.{}",
                    msg.pdu.type().to_string(),
                    msg.pdu.unsuccessful_outcome().value.type().to_string());
        break;
      case asn1::e2ap::e2_ap_pdu_c::types_opts::nulltype:
        logger.info("Received a E2AP PDU of type nulltype");
        break;
    }

    last_e2_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding E2AP PDU");
      handler->handle_message(msg);
    }
  }
  e2_message last_e2_msg;

private:
  srslog::basic_logger& logger;
  e2_message_handler*   handler = nullptr;
};

class dummy_e2ap_pcap : public dlt_pcap
{
public:
  void open(const std::string& filename_) override {}
  void close() override {}
  bool is_write_enabled() override { return false; }
  void push_pdu(const_span<uint8_t> pdu) override {}
  void push_pdu(byte_buffer pdu) override {}
};

inline e2_message generate_e2_setup_request()
{
  using namespace asn1::e2ap;
  e2_message  e2_msg;
  init_msg_s& initmsg = e2_msg.pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_E2SETUP);
  e2setup_request_s& setup = initmsg.value.e2setup_request();

  //  Transaction ID
  setup->transaction_id.crit        = asn1::crit_opts::reject;
  setup->transaction_id.value.value = 1;

  // Global e2 node ID
  setup->global_e2node_id.crit = asn1::crit_opts::reject;
  auto& gnb_id                 = setup->global_e2node_id.value.set_gnb();
  gnb_id.global_g_nb_id.plmn_id.from_number(3617847);
  gnb_id.global_g_nb_id.gnb_id.gnb_id().from_number(381210353);

  // RAN functions added
  setup->ra_nfunctions_added.crit = asn1::crit_opts::reject;
  setup->ra_nfunctions_added.id   = ASN1_E2AP_ID_RA_NFUNCTIONS_ADDED;
  asn1::protocol_ie_single_container_s<ra_nfunction_item_ies_o> ran_func_item;
  ran_func_item.load_info_obj(ASN1_E2AP_ID_RA_NFUNCTION_ITEM);
  ran_func_item.value().ra_nfunction_item().ran_function_id = 0;
  ran_func_item.value().ra_nfunction_item().ran_function_definition.from_string(
      "20C04F52414E2D4532534D2D4B504D0000054F494431323305004B504D206D6F6E69746F720860283861AAE33F0060000101070050657269"
      "6F646963207265706F727401051401011D004F2D4455204D6561737572656D656E7420436F6E7461696E657220666F722074686520354743"
      "20636F6E6E6563746564206465706C6F796D656E74010101010001021D004F2D4455204D6561737572656D656E7420436F6E7461696E6572"
      "20666F72207468652045504320636F6E6E6563746564206465706C6F796D656E74010101010001031E804F2D43552D4350204D6561737572"
      "656D656E7420436F6E7461696E657220666F72207468652035474320636F6E6E6563746564206465706C6F796D656E74010101010001041E"
      "804F2D43552D4350204D6561737572656D656E7420436F6E7461696E657220666F72207468652045504320636F6E6E656374656420646570"
      "6C6F796D656E74010101010001051E804F2D43552D5550204D6561737572656D656E7420436F6E7461696E657220666F7220746865203547"
      "4320636F6E6E6563746564206465706C6F796D656E74010101010001061E804F2D43552D5550204D6561737572656D656E7420436F6E7461"
      "696E657220666F72207468652045504320636F6E6E6563746564206465706C6F796D656E7401010101");
  ran_func_item.value().ra_nfunction_item().ran_function_oid.resize(30);
  ran_func_item.value().ra_nfunction_item().ran_function_oid.from_string("1.3.6.1.4.1.53148.1.2.2.2");
  setup->ra_nfunctions_added.value.push_back(ran_func_item);

  // E2 node component config
  setup->e2node_component_cfg_addition.crit = asn1::crit_opts::reject;
  auto& list                                = setup->e2node_component_cfg_addition.value;
  list.resize(1);
  list[0].load_info_obj(ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM);
  e2node_component_cfg_addition_item_s& e2node_cfg_item = list[0].value().e2node_component_cfg_addition_item();
  e2node_cfg_item.e2node_component_interface_type       = e2node_component_interface_type_opts::ng;
  e2node_cfg_item.e2node_component_id.set_e2node_component_interface_type_ng().amf_name.from_string("nginterf");
  e2node_cfg_item.e2node_component_cfg.e2node_component_request_part.from_string("72657170617274");
  e2node_cfg_item.e2node_component_cfg.e2node_component_resp_part.from_string("72657370617274");

  return e2_msg;
}

class dummy_e2_du_metrics : public e2_du_metrics_interface
{
public:
  void get_metrics(scheduler_ue_metrics& ue_metrics) override {}

  void connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider) override
  {
    e2_meas_provider = std::move(meas_provider);
  }

private:
  std::unique_ptr<scheduler_ue_metrics_notifier> e2_meas_provider;
};

class dummy_e2sm_kpm_du_meas_provider : public e2sm_kpm_meas_provider
{
public:
  dummy_e2sm_kpm_du_meas_provider() { supported_metrics = {"CQI", "RSRP", "RSRQ", "DRB.UEThpDl"}; }
  std::vector<std::string> get_supported_metric_names(e2sm_kpm_metric_level_enum level) override
  {
    return supported_metrics;
  }
  virtual bool cell_supported(const asn1::e2sm_kpm::cgi_c& cell_global_id) override { return true; };
  virtual bool ue_supported(const asn1::e2sm_kpm::ueid_c& ueid) override { return true; };
  virtual bool test_cond_supported(const asn1::e2sm_kpm::test_cond_type_c& test_cond_type) override { return true; };

  virtual bool get_ues_matching_test_conditions(const asn1::e2sm_kpm::matching_cond_list_l& matching_cond_list,
                                                std::vector<asn1::e2sm_kpm::ueid_c>&        ues) override
  {
    return false;
  }

  virtual bool
  get_ues_matching_test_conditions(const asn1::e2sm_kpm::matching_ue_cond_per_sub_list_l& matching_ue_cond_list,
                                   std::vector<asn1::e2sm_kpm::ueid_c>&                   ues) override
  {
    return false;
  }

  virtual bool metric_supported(const asn1::e2sm_kpm::meas_type_c&  meas_type,
                                const asn1::e2sm_kpm::meas_label_s& label,
                                const e2sm_kpm_metric_level_enum    level,
                                const bool&                         cell_scope) override
  {
    if (std::find(supported_metrics.begin(), supported_metrics.end(), meas_type.meas_name().to_string()) !=
        supported_metrics.end()) {
      return true;
    } else {
      return false;
    }
  };
  /// \return Returns True if measurement collection was successful
  virtual bool get_meas_data(const asn1::e2sm_kpm::meas_type_c&               meas_type,
                             const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                             const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                             const optional<asn1::e2sm_kpm::cgi_c>            cell_global_id,
                             std::vector<asn1::e2sm_kpm::meas_record_item_c>& items) override
  {
    uint32_t           nof_records = ues.size() == 0 ? 1 : ues.size();
    meas_record_item_c meas_record_item;
    for (unsigned i = 0; i < nof_records; ++i) {
      if (last_measurements.size()) {
        meas_record_item.set_integer() = last_measurements[i];
      } else {
        meas_record_item.set_integer() = 1;
      }

      items.push_back(meas_record_item);
    }

    return true;
  };

  void push_measurements(std::vector<uint32_t> measurements_) { last_measurements = measurements_; }

private:
  std::vector<std::string> supported_metrics;
  std::vector<uint32_t>    last_measurements;
};

class dummy_e2_subscription_mngr : public e2_subscription_manager
{
public:
  dummy_e2_subscription_mngr() : logger(srslog::fetch_basic_logger("TEST")){};
  e2_subscribe_reponse_message handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& request) override
  {
    last_subscription = request;
    logger.info("Received a subscription request with action list size {}",
                last_subscription->ricsubscription_details.value.ric_action_to_be_setup_list.size());
    e2_subscribe_reponse_message msg;
    get_subscription_result(msg);
    return msg;
  }

  e2_subscribe_delete_response_message
  handle_subscription_delete(const asn1::e2ap::ricsubscription_delete_request_s& msg) override
  {
    e2_subscribe_delete_response_message outcome;
    return outcome;
  };

  void start_subscription(int ric_instance_id, e2_event_manager& ev_mng, uint16_t ran_func_id) override {}

  void stop_subscription(int                                                 ric_instance_id,
                         e2_event_manager&                                   ev_mng,
                         const asn1::e2ap::ricsubscription_delete_request_s& msg) override
  {
  }

  void add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface) override {}

  e2sm_interface* get_e2sm_interface(std::string oid) override { return nullptr; }

  void add_ran_function_oid(uint16_t ran_func_id, std::string oid) override {}

private:
  void get_subscription_result(e2_subscribe_reponse_message& msg)
  {
    msg.success    = true;
    msg.request_id = last_subscription->ri_crequest_id.value;

    logger.info("Sending subscription result for Request instance ID {}", msg.request_id.ric_instance_id);
    unsigned action_list_size = last_subscription->ricsubscription_details.value.ric_action_to_be_setup_list.size();
    msg.admitted_list.resize(action_list_size);
    for (unsigned i = 0; i < action_list_size; i++) {
      auto& item = last_subscription->ricsubscription_details.value.ric_action_to_be_setup_list[i]
                       .value()
                       .ri_caction_to_be_setup_item();
      msg.admitted_list[i].value().ri_caction_admitted_item().ric_action_id = item.ric_action_id;
    }
  }

private:
  asn1::e2ap::ricsubscription_request_s last_subscription;
  srslog::basic_logger&                 logger;
};

inline e2_message generate_e2_setup_request_message()
{
  e2_message e2_msg = {};
  e2_msg.pdu.set_init_msg();
  e2_msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  e2_msg.pdu.init_msg().value.e2setup_request()->transaction_id.value.value = 1;
  e2_msg.pdu.init_msg().value.e2setup_request() = generate_e2_setup_request().pdu.init_msg().value.e2setup_request();

  return e2_msg;
}

inline e2_message generate_e2_setup_response(unsigned transaction_id)
{
  e2_message e2_setup_response = {};
  e2_setup_response.pdu.set_successful_outcome();
  e2_setup_response.pdu.successful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  auto& setup                           = e2_setup_response.pdu.successful_outcome().value.e2setup_resp();
  setup->transaction_id.value.value     = transaction_id;
  setup->ra_nfunctions_accepted_present = true;
  asn1::protocol_ie_single_container_s<asn1::e2ap::ra_nfunction_id_item_ies_o> ran_func_item;
  ran_func_item.value().ra_nfunction_id_item().ran_function_id       = 1;
  ran_func_item.value().ra_nfunction_id_item().ran_function_revision = 0;
  setup->ra_nfunctions_accepted.value.push_back(ran_func_item);
  setup->global_ric_id.value.plmn_id.from_number(131014);
  setup->global_ric_id.value.ric_id.from_number(699598);
  return e2_setup_response;
}

inline e2_message generate_e2_setup_failure(unsigned transaction_id)
{
  e2_message e2_setup_failure = {};
  e2_setup_failure.pdu.set_unsuccessful_outcome();
  e2_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);

  auto& setup                       = e2_setup_failure.pdu.unsuccessful_outcome().value.e2setup_fail();
  setup->transaction_id.value.value = transaction_id;
  setup->cause.value.set_misc();
  return e2_setup_failure;
}

inline ueid_gnb_du_s generate_ueid_gnb_du(uint32_t ue_idx)
{
  ueid_gnb_du_s ueid;
  ueid.gnb_cu_ue_f1_ap_id = ue_idx;
  ueid.ran_ueid_present   = false;
  return ueid;
}

inline asn1::e2ap::ri_caction_to_be_setup_item_s generate_e2sm_kpm_ric_action(e2_sm_kpm_action_definition_s& action_def)
{
  asn1::e2ap::ri_caction_to_be_setup_item_s ric_action;
  ric_action.ric_action_type               = asn1::e2ap::ri_caction_type_opts::report;
  ric_action.ric_action_id                 = 0;
  ric_action.ric_subsequent_action_present = false;

  byte_buffer   buf;
  asn1::bit_ref bref(buf);
  if (action_def.pack(bref) != asn1::SRSASN_SUCCESS) {
    return {};
  }

  ric_action.ric_action_definition.resize(buf.length());
  std::copy(buf.begin(), buf.end(), ric_action.ric_action_definition.begin());
  return ric_action;
}

inline e2_message generate_e2sm_kpm_subscription_request(asn1::e2ap::ri_caction_to_be_setup_item_s& ric_action)
{
  asn1::e2ap::ricsubscription_request_s ric_subscript_reqs;
  ric_subscript_reqs->ra_nfunction_id->value           = 147;
  ric_subscript_reqs->ri_crequest_id->ric_requestor_id = 1;
  ric_subscript_reqs->ri_crequest_id->ric_instance_id  = 0;
  ric_subscript_reqs->ricsubscription_details->ric_action_to_be_setup_list.resize(1);
  ric_subscript_reqs->ricsubscription_details->ric_action_to_be_setup_list[0]->ri_caction_to_be_setup_item() =
      ric_action;
  e2_sm_kpm_event_trigger_definition_s e2sm_kpm_event_trigger_def;
  e2sm_kpm_event_trigger_def.event_definition_formats.event_definition_format1().report_period = 1000;

  byte_buffer   buf;
  asn1::bit_ref bref(buf);
  if (e2sm_kpm_event_trigger_def.pack(bref) != asn1::SRSASN_SUCCESS) {
    return {};
  }

  ric_subscript_reqs->ricsubscription_details->ric_event_trigger_definition.resize(buf.length());
  std::copy(buf.begin(), buf.end(), ric_subscript_reqs->ricsubscription_details->ric_event_trigger_definition.begin());

  e2_message e2_msg;
  e2_msg.pdu.set_init_msg();
  e2_msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_RICSUBSCRIPTION);
  e2_msg.pdu.init_msg().value.ricsubscription_request() = ric_subscript_reqs;
  return e2_msg;
}

inline e2_message generate_e2_ind_msg(byte_buffer& ind_hdr_bytes, byte_buffer& ind_msg_bytes)
{
  e2_indication_message e2_ind;
  e2_ind.request_id.ric_requestor_id                  = 1;
  e2_ind.request_id.ric_instance_id                   = 0;
  e2_ind.indication->ri_cind_msg.crit                 = asn1::crit_opts::reject;
  e2_ind.indication->ri_crequest_id->ric_requestor_id = 1;
  e2_ind.indication->ri_crequest_id->ric_instance_id  = 0;
  e2_ind.indication->ra_nfunction_id.value            = 147;
  e2_ind.indication->ri_caction_id.value              = 4;
  e2_ind.indication->ri_cind_sn_present               = true;
  e2_ind.indication->ri_cind_sn->value                = 1234;
  e2_ind.indication->ri_cind_type.value               = asn1::e2ap::ri_cind_type_e::ri_cind_type_opts::report;
  e2_ind.indication->ri_ccall_process_id_present      = false;

  // put RIC indication content into message
  e2_ind.indication->ri_cind_msg.value.resize(ind_msg_bytes.length());
  std::copy(ind_msg_bytes.begin(), ind_msg_bytes.end(), e2_ind.indication->ri_cind_msg.value.begin());
  e2_ind.indication->ri_cind_hdr.value.resize(ind_hdr_bytes.length());
  std::copy(ind_hdr_bytes.begin(), ind_hdr_bytes.end(), e2_ind.indication->ri_cind_hdr.value.begin());

  e2_message e2_msg;
  e2_msg.pdu.set_init_msg();
  e2_msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_RI_CIND);
  e2_msg.pdu.init_msg().value.ri_cind() = e2_ind.indication;
  return e2_msg;
}

/// Dummy handler just printing the received PDU.
class dummy_e2_message_handler : public e2_message_handler
{
public:
  dummy_e2_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const e2_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a E2AP PDU of type {}", msg.pdu.type().to_string());
  }

  e2_message last_msg;

private:
  srslog::basic_logger& logger;
};

/// Dummy PDU handler
class dummy_network_gateway_data_handler : public srsran::sctp_network_gateway_data_handler
{
public:
  dummy_network_gateway_data_handler(){};
  void handle_pdu(const byte_buffer& pdu) override { last_pdu = pdu.copy(); }

  byte_buffer last_pdu;
};

class dummy_e2sm_handler : public e2sm_handler
{
  asn1::e2sm_kpm::e2_sm_kpm_action_definition_s
  handle_packed_e2sm_kpm_action_definition(const srsran::byte_buffer& buf) override
  {
    e2_sm_kpm_action_definition_s action_def;
    action_def.ric_style_type = 3;
    action_def.action_definition_formats.set_action_definition_format3();
    action_def.action_definition_formats.action_definition_format3().meas_cond_list.resize(1);
    action_def.action_definition_formats.action_definition_format3()
        .meas_cond_list[0]
        .meas_type.set_meas_name()
        .from_string("RSRP");
    action_def.action_definition_formats.action_definition_format3().granul_period = 10;
    return action_def;
  }
  e2_sm_event_trigger_definition_s handle_packed_event_trigger_definition(const srsran::byte_buffer& buf) override
  {
    e2_sm_event_trigger_definition_s e2sm_event_trigger_def;
    e2sm_event_trigger_def.report_period = 10;
    return e2sm_event_trigger_def;
  }
  asn1::unbounded_octstring<true> pack_ran_function_description() override { return {}; };
};

class dummy_e2_connection_client final : public e2_connection_client
{
public:
  explicit dummy_e2_connection_client(e2ap_packer& packer_) :
    logger(srslog::fetch_basic_logger("TEST")), packer(packer_){};

  // E2 Agent interface.
  std::unique_ptr<e2_message_notifier> handle_connection_request() override
  {
    return std::make_unique<dummy_e2_pdu_notifier>(nullptr);
  }

  void connect_e2ap(e2_message_notifier* e2_rx_pdu_notifier,
                    e2_message_handler*  e2ap_msg_handler_,
                    e2_event_handler*    event_handler_) override
  {
    msg_notifier = dynamic_cast<dummy_e2_pdu_notifier*>(e2_rx_pdu_notifier);
    msg_notifier->attach_handler(&packer);
  }

  dummy_e2_pdu_notifier* get_e2_msg_notifier() { return msg_notifier; }

  void close() override{};

private:
  srslog::basic_logger&  logger;
  e2ap_packer&           packer;
  dummy_e2_pdu_notifier* msg_notifier;
};

/// Fixture class for E2AP
class e2_test_base : public ::testing::Test
{
protected:
  void tick()
  {
    timers.tick();
    task_worker.run_pending_tasks();
  }
  e2ap_configuration                                  cfg = {};
  timer_factory                                       factory;
  timer_manager                                       timers;
  std::unique_ptr<dummy_network_gateway_data_handler> gw;
  std::unique_ptr<e2_interface>                       e2;
  std::unique_ptr<dummy_e2ap_pcap>                    pcap;
  std::unique_ptr<srsran::e2ap_asn1_packer>           packer;
  std::unique_ptr<e2sm_interface>                     e2sm_iface;
  std::unique_ptr<e2sm_handler>                       e2sm_packer;
  std::unique_ptr<e2_subscription_manager>            e2_subscription_mngr;
  std::unique_ptr<e2_du_metrics_interface>            du_metrics;
  std::unique_ptr<dummy_e2sm_kpm_du_meas_provider>    du_meas_provider;
  manual_task_worker                                  task_worker{64};
  std::unique_ptr<dummy_e2_pdu_notifier>              msg_notifier;
  std::unique_ptr<dummy_e2_connection_client>         e2_client;
  srslog::basic_logger&                               test_logger = srslog::fetch_basic_logger("TEST");
};

class e2_test : public e2_test_base
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    msg_notifier         = std::make_unique<dummy_e2_pdu_notifier>(nullptr);
    e2_subscription_mngr = std::make_unique<dummy_e2_subscription_mngr>();
    du_metrics           = std::make_unique<dummy_e2_du_metrics>();
    factory              = timer_factory{timers, task_worker};
    e2                   = create_e2(cfg, factory, *msg_notifier, *e2_subscription_mngr);
    gw                   = std::make_unique<dummy_network_gateway_data_handler>();
    pcap                 = std::make_unique<dummy_e2ap_pcap>();
    packer               = std::make_unique<srsran::e2ap_asn1_packer>(*gw, *e2, *pcap);
    msg_notifier->attach_handler(&(*packer));
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

class e2_external_test : public e2_test_base
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg                  = srsran::config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    msg_notifier         = std::make_unique<dummy_e2_pdu_notifier>(nullptr);
    du_metrics           = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider     = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_packer          = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_iface           = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*msg_notifier);
    e2_subscription_mngr->add_e2sm_service("1.3.6.1.4.1.53148.1.2.2.2", std::move(e2sm_iface));
    factory = timer_factory{timers, task_worker};
    e2      = create_e2_with_task_exec(cfg, factory, *msg_notifier, *e2_subscription_mngr, task_worker);
    gw      = std::make_unique<dummy_network_gateway_data_handler>();
    pcap    = std::make_unique<dummy_e2ap_pcap>();
    packer  = std::make_unique<srsran::e2ap_asn1_packer>(*gw, *e2, *pcap);
    msg_notifier->attach_handler(&(*packer));
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

class e2_entity_test : public e2_test_base
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg                  = srsran::config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    gw         = std::make_unique<dummy_network_gateway_data_handler>();
    pcap       = std::make_unique<dummy_e2ap_pcap>();
    packer     = std::make_unique<srsran::e2ap_asn1_packer>(*gw, *e2, *pcap);
    e2_client  = std::make_unique<dummy_e2_connection_client>(*packer);
    du_metrics = std::make_unique<dummy_e2_du_metrics>();
    factory    = timer_factory{timers, task_worker};
    e2         = create_e2_entity(cfg, e2_client.get(), *du_metrics, factory, task_worker);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

class e2_test_subscriber : public e2_test_base
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();
    cfg                  = srsran::config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    factory              = timer_factory{timers, task_worker};
    msg_notifier         = std::make_unique<dummy_e2_pdu_notifier>(nullptr);
    e2sm_packer          = std::make_unique<dummy_e2sm_handler>();
    du_metrics           = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider     = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_iface           = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*msg_notifier);
    e2_subscription_mngr->add_e2sm_service("1.3.6.1.4.1.53148.1.2.2.2", std::move(e2sm_iface));
    e2_subscription_mngr->add_ran_function_oid(1, "1.3.6.1.4.1.53148.1.2.2.2");
    e2     = create_e2(cfg, factory, *msg_notifier, *e2_subscription_mngr);
    gw     = std::make_unique<dummy_network_gateway_data_handler>();
    pcap   = std::make_unique<dummy_e2ap_pcap>();
    packer = std::make_unique<srsran::e2ap_asn1_packer>(*gw, *e2, *pcap);
    msg_notifier->attach_handler(&(*packer));
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};

class e2_test_setup : public e2_test_base
{
  void SetUp() override
  {
    cfg                  = srsran::config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    factory              = timer_factory{timers, task_worker};
    msg_notifier         = std::make_unique<dummy_e2_pdu_notifier>(nullptr);
    du_metrics           = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider     = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_packer          = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_iface           = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    e2_subscription_mngr = std::make_unique<e2_subscription_manager_impl>(*msg_notifier);
    e2_subscription_mngr->add_e2sm_service("1.3.6.1.4.1.53148.1.2.2.2", std::move(e2sm_iface));
    e2     = create_e2(cfg, factory, *msg_notifier, *e2_subscription_mngr);
    gw     = std::make_unique<dummy_network_gateway_data_handler>();
    pcap   = std::make_unique<dummy_e2ap_pcap>();
    packer = std::make_unique<srsran::e2ap_asn1_packer>(*gw, *e2, *pcap);
    msg_notifier->attach_handler(&(*packer));
  }
  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }
};
} // namespace srsran
