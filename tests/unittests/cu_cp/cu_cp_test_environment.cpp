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

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/e1ap/e1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/unittests/cu_cp/test_doubles/mock_cu_up.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/asn1/rrc_nr/dl_ccch_msg.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "srsran/cu_cp/cell_meas_manager_config.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/security/integrity.h"
#include "srsran/support/executors/task_worker.h"

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_test_environment::worker_manager
{
  const unsigned WORKER_QUEUE_SIZE = 1024;

public:
  void stop() { worker.stop(); }

  void wait_pending_tasks() { worker.wait_pending_tasks(); }

  task_worker                    worker{"cu_cp_worker", WORKER_QUEUE_SIZE};
  std::unique_ptr<task_executor> exec{std::make_unique<task_worker_executor>(worker)};
};

// ////

cu_cp_test_environment::cu_cp_test_environment(cu_cp_test_env_params params_) :
  params(std::move(params_)),
  cu_cp_workers(std::make_unique<worker_manager>()),
  timers(64),
  amf_configs(std::move(params.amf_configs))
{
  // Initialize logging
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("NRPPA").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("PDCP").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("NGAP").set_hex_dump_max_size(32);
  srslog::fetch_basic_logger("RRC").set_hex_dump_max_size(32);
  srslog::fetch_basic_logger("SEC").set_hex_dump_max_size(32);
  srslog::init();

  // Create CU-CP config
  cu_cp_cfg                               = config_helpers::make_default_cu_cp_config();
  cu_cp_cfg.services.cu_cp_executor       = cu_cp_workers->exec.get();
  cu_cp_cfg.services.timers               = &timers;
  cu_cp_cfg.admission.max_nof_dus         = params.max_nof_dus;
  cu_cp_cfg.admission.max_nof_cu_ups      = params.max_nof_cu_ups;
  cu_cp_cfg.admission.max_nof_ues         = params.max_nof_ues;
  cu_cp_cfg.admission.max_nof_drbs_per_ue = params.max_nof_drbs_per_ue;
  cu_cp_cfg.bearers.drb_config            = config_helpers::make_default_cu_cp_qos_config_list();
  // > NGAP config
  for (const auto& [amf_index, amf_config] : amf_configs) {
    cu_cp_cfg.ngap.ngaps.push_back(cu_cp_configuration::ngap_config{&*amf_config.amf_stub, amf_config.supported_tas});
  }
  // > Security config.
  cu_cp_cfg.security.int_algo_pref_list = {security::integrity_algorithm::nia2,
                                           security::integrity_algorithm::nia1,
                                           security::integrity_algorithm::nia3,
                                           security::integrity_algorithm::nia0};
  cu_cp_cfg.security.enc_algo_pref_list = {security::ciphering_algorithm::nea0,
                                           security::ciphering_algorithm::nea2,
                                           security::ciphering_algorithm::nea1,
                                           security::ciphering_algorithm::nea3};

  // > Logging and metrics config.
  cu_cp_cfg.f1ap.json_log_enabled          = true;
  cu_cp_cfg.e1ap.json_log_enabled          = true;
  cu_cp_cfg.metrics.layers_cfg.enable_ngap = true;
  cu_cp_cfg.metrics.layers_cfg.enable_rrc  = true;

  // > Mobility config
  cu_cp_cfg.mobility.mobility_manager_config.trigger_handover_from_measurements = params.trigger_ho_from_measurements;
  {
    // > Meas manager config
    cell_meas_manager_cfg meas_mng_cfg;
    {
      // Generate NCIs.
      gnb_id_t         gnb_id1 = cu_cp_cfg.node.gnb_id;
      nr_cell_identity nci1    = nr_cell_identity::create(gnb_id1, 0).value();
      nr_cell_identity nci2    = nr_cell_identity::create(gnb_id1, 1).value();
      gnb_id_t         gnb_id2 = {cu_cp_cfg.node.gnb_id.id + 1, cu_cp_cfg.node.gnb_id.bit_length};
      nr_cell_identity nci3    = nr_cell_identity::create(gnb_id2, 0).value();

      // Cell 1
      {
        cell_meas_config cell_cfg_1;
        cell_cfg_1.periodic_report_cfg_id             = uint_to_report_cfg_id(1);
        cell_cfg_1.serving_cell_cfg.gnb_id_bit_length = gnb_id1.bit_length;
        cell_cfg_1.serving_cell_cfg.nci               = nci1;
        cell_cfg_1.ncells.push_back({nci2, {uint_to_report_cfg_id(2)}});
        // Add external cell (for inter CU handover tests)
        cell_cfg_1.ncells.push_back({nci3, {uint_to_report_cfg_id(2)}});

        meas_mng_cfg.cells.emplace(nci1, cell_cfg_1);
      }

      // Cell 2
      {
        cell_meas_config cell_cfg_2;
        cell_cfg_2.periodic_report_cfg_id             = uint_to_report_cfg_id(1);
        cell_cfg_2.serving_cell_cfg.gnb_id_bit_length = gnb_id1.bit_length;
        cell_cfg_2.serving_cell_cfg.nci               = nci2;
        cell_cfg_2.ncells.push_back({nci1, {uint_to_report_cfg_id(2)}});
        meas_mng_cfg.cells.emplace(nci2, cell_cfg_2);
      }

      // Add an external cell
      {
        cell_meas_config cell_cfg_3;
        cell_cfg_3.periodic_report_cfg_id             = uint_to_report_cfg_id(1);
        cell_cfg_3.serving_cell_cfg.gnb_id_bit_length = gnb_id2.bit_length;
        cell_cfg_3.serving_cell_cfg.nci               = nci3;
        cell_cfg_3.serving_cell_cfg.pci               = 3;
        cell_cfg_3.serving_cell_cfg.ssb_arfcn         = 632628;
        cell_cfg_3.serving_cell_cfg.band              = nr_band::n78;
        cell_cfg_3.serving_cell_cfg.ssb_scs           = subcarrier_spacing::kHz15;
        cell_cfg_3.serving_cell_cfg.ssb_mtc = rrc_ssb_mtc{{rrc_periodicity_and_offset::periodicity_t::sf20, 0}, 5};

        cell_cfg_3.ncells.push_back({nci1, {uint_to_report_cfg_id(2)}});
        meas_mng_cfg.cells.emplace(nci3, cell_cfg_3);
      }

      // Add periodic event
      {
        rrc_periodical_report_cfg periodical_cfg;
        periodical_cfg.rs_type                 = srs_cu_cp::rrc_nr_rs_type::ssb;
        periodical_cfg.report_interv           = 1024;
        periodical_cfg.report_amount           = -1;
        periodical_cfg.report_quant_cell.rsrp  = true;
        periodical_cfg.report_quant_cell.rsrq  = true;
        periodical_cfg.report_quant_cell.sinr  = true;
        periodical_cfg.max_report_cells        = 4;
        periodical_cfg.include_beam_meass      = true;
        periodical_cfg.use_allowed_cell_list   = false;
        periodical_cfg.periodic_ho_rsrp_offset = 2;

        meas_mng_cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), rrc_report_cfg_nr{periodical_cfg});
      }

      // Add event A3
      {
        rrc_event_trigger_cfg event_trigger_cfg = {};

        rrc_event_id& event_a3 = event_trigger_cfg.event_id;
        event_a3.id            = rrc_event_id::event_id_t::a3;
        event_a3.meas_trigger_quant_thres_or_offset.emplace();
        event_a3.meas_trigger_quant_thres_or_offset.value().rsrp.emplace() = 6;
        event_a3.hysteresis                                                = 0;
        event_a3.time_to_trigger                                           = 100;
        event_a3.use_allowed_cell_list                                     = false;

        event_trigger_cfg.rs_type                = srs_cu_cp::rrc_nr_rs_type::ssb;
        event_trigger_cfg.report_interv          = 1024;
        event_trigger_cfg.report_amount          = -1;
        event_trigger_cfg.report_quant_cell.rsrp = true;
        event_trigger_cfg.report_quant_cell.rsrq = true;
        event_trigger_cfg.report_quant_cell.sinr = true;
        event_trigger_cfg.max_report_cells       = 4;
        event_trigger_cfg.include_beam_meass     = true;

        rrc_meas_report_quant report_quant_rs_idxes;
        report_quant_rs_idxes.rsrp              = true;
        report_quant_rs_idxes.rsrq              = true;
        report_quant_rs_idxes.sinr              = true;
        event_trigger_cfg.report_quant_rs_idxes = report_quant_rs_idxes;

        meas_mng_cfg.report_config_ids.emplace(uint_to_report_cfg_id(2), rrc_report_cfg_nr{event_trigger_cfg});
      }
    }
    cu_cp_cfg.mobility.meas_manager_config = meas_mng_cfg;
  }

  // > RRC config
  cu_cp_cfg.rrc.rrc_procedure_guard_time_ms =
      std::chrono::milliseconds(10000); // procedure timeouts should only occur intentionally

  // > F1AP config
  cu_cp_cfg.f1ap.proc_timeout = std::chrono::milliseconds(10000); // procedure timeouts should only occur intentionally

  // > E1AP config
  cu_cp_cfg.e1ap.proc_timeout = std::chrono::milliseconds(10000); // procedure timeouts should only occur intentionally

  // > UE config
  cu_cp_cfg.ue.request_pdu_session_timeout =
      std::chrono::seconds(10); // procedure timeouts should only occur intentionally

  // create CU-CP instance.
  cu_cp_inst = create_cu_cp(cu_cp_cfg);
}

cu_cp_test_environment::~cu_cp_test_environment()
{
  cu_cp_inst->stop();
  dus.clear();
  cu_ups.clear();
  cu_cp_workers->stop();

  srslog::flush();
}

void cu_cp_test_environment::tick()
{
  // Dispatch clock ticking to CU-CP worker
  cu_cp_workers->worker.push_task_blocking([this]() { timers.tick(); });
}

bool cu_cp_test_environment::tick_until(std::chrono::milliseconds    timeout,
                                        const std::function<bool()>& stop_condition,
                                        bool                         real_time)
{
  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    done = false;

  // Tick up to "timeout" times, waiting for stop_condition() to return true.
  for (unsigned i = 0; i != timeout.count(); ++i) {
    if (stop_condition()) {
      return true;
    }

    // Push to CU-CP worker task that checks the state of the condition.
    done = false;
    cu_cp_workers->worker.push_task_blocking([&]() {
      // Need to tick the clock.
      tick();

      std::lock_guard<std::mutex> lock(mutex);
      done = true;
      cvar.notify_one();
    });

    // Wait for tick to be processed.
    {
      std::unique_lock<std::mutex> lock(mutex);
      cvar.wait(lock, [&done]() { return done; });
    }
    if (real_time) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  return stop_condition();
}

bool cu_cp_test_environment::wait_for_ngap_tx_pdu(ngap_message&             pdu,
                                                  std::chrono::milliseconds timeout,
                                                  unsigned                  amf_idx)
{
  return tick_until(timeout, [&]() { return amf_configs.at(amf_idx).amf_stub->try_pop_rx_pdu(pdu); });
}

bool cu_cp_test_environment::wait_for_e1ap_tx_pdu(unsigned                  cu_up_idx,
                                                  e1ap_message&             pdu,
                                                  std::chrono::milliseconds timeout)
{
  return tick_until(timeout, [&]() { return cu_ups.at(cu_up_idx)->try_pop_rx_pdu(pdu); });
}

bool cu_cp_test_environment::wait_for_f1ap_tx_pdu(unsigned du_idx, f1ap_message& pdu, std::chrono::milliseconds timeout)
{
  report_fatal_error_if_not(dus.size() >= du_idx and dus[du_idx] != nullptr, "DU index out of range");

  return tick_until(timeout, [&]() {
    if (du_idx >= dus.size() or dus[du_idx] == nullptr) {
      return false;
    }
    return dus[du_idx]->try_pop_dl_pdu(pdu);
  });
}

const cu_cp_test_environment::ue_context* cu_cp_test_environment::find_ue_context(unsigned            du_idx,
                                                                                  gnb_du_ue_f1ap_id_t du_ue_id) const
{
  auto it = du_ue_id_to_ran_ue_id_map.at(du_idx).find(du_ue_id);
  if (it == du_ue_id_to_ran_ue_id_map.at(du_idx).end()) {
    return nullptr;
  }
  return &attached_ues.at(it->second);
}

void cu_cp_test_environment::run_ng_setup()
{
  for (const auto& [amf_index, amf_config] : amf_configs) {
    get_amf(amf_index).enqueue_next_tx_pdu(srs_cu_cp::generate_ng_setup_response());
  }
  report_fatal_error_if_not(get_cu_cp().start(), "Failed to start CU-CP");

  ngap_message ngap_pdu;
  for (const auto& [amf_index, amf_config] : amf_configs) {
    report_fatal_error_if_not(get_amf(amf_index).try_pop_rx_pdu(ngap_pdu),
                              "CU-CP did not send the NG Setup Request to the AMF {}",
                              amf_index);
    report_fatal_error_if_not(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request),
                              "CU-CP did not setup the AMF connection");
  }
}

bool cu_cp_test_environment::drop_amf_connection(unsigned amf_idx)
{
  auto it = amf_configs.find(amf_idx);
  if (it == amf_configs.end()) {
    return false;
  }
  it->second.amf_stub->drop_connection();
  // Wait for the CU-CP to process the disconnection.
  cu_cp_workers->wait_pending_tasks();
  return true;
}

std::optional<unsigned> cu_cp_test_environment::connect_new_du()
{
  auto du_stub = create_mock_du({get_cu_cp().get_f1c_handler()});
  if (not du_stub) {
    return std::nullopt;
  }
  for (; dus.count(next_du_idx) != 0; ++next_du_idx) {
  }
  auto ret = dus.insert(std::make_pair(next_du_idx, std::move(du_stub)));
  report_fatal_error_if_not(ret.second, "Race condition detected");
  return next_du_idx;
}

bool cu_cp_test_environment::drop_du_connection(unsigned du_idx)
{
  auto it = dus.find(du_idx);
  if (it == dus.end()) {
    return false;
  }
  dus.erase(it);
  return true;
}

bool cu_cp_test_environment::run_f1_setup(unsigned                                         du_idx,
                                          gnb_du_id_t                                      gnb_du_id,
                                          std::vector<test_helpers::served_cell_item_info> cells)
{
  f1ap_message f1_setup_req = test_helpers::generate_f1_setup_request(gnb_du_id, cells);
  rrc_test_timer_values     = get_timers(f1_setup_req.pdu.init_msg().value.f1_setup_request());
  get_du(du_idx).push_ul_pdu(f1_setup_req);
  f1ap_message f1ap_pdu;
  bool         result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  return result;
}

std::optional<unsigned> cu_cp_test_environment::connect_new_cu_up()
{
  auto cu_up_obj = create_mock_cu_up(get_cu_cp().get_e1_handler());
  if (not cu_up_obj) {
    return std::nullopt;
  }
  for (; cu_ups.count(next_cu_up_idx) != 0; ++next_cu_up_idx) {
  }
  auto ret = cu_ups.insert(std::make_pair(next_cu_up_idx, std::move(cu_up_obj)));
  report_fatal_error_if_not(ret.second, "Race condition detected");
  return next_cu_up_idx;
}

bool cu_cp_test_environment::drop_cu_up_connection(unsigned cu_up_idx)
{
  auto it = cu_ups.find(cu_up_idx);
  if (it == cu_ups.end()) {
    return false;
  }
  cu_ups.erase(it);
  return true;
}

bool cu_cp_test_environment::run_e1_setup(unsigned cu_up_idx)
{
  get_cu_up(cu_up_idx).push_tx_pdu(generate_valid_cu_up_e1_setup_request());
  e1ap_message e1ap_pdu;
  bool         result = this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu, std::chrono::milliseconds{1000});
  return result;
}

bool cu_cp_test_environment::connect_new_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id, rnti_t crnti)
{
  ngap_message ngap_pdu;
  srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
  f1ap_message f1ap_pdu;
  srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");

  // Inject Initial UL RRC message
  f1ap_message init_ul_rrc_msg = test_helpers::generate_init_ul_rrc_message_transfer(du_ue_id, crnti);
  test_logger.info("c-rnti={} du_ue={}: Injecting Initial UL RRC message", crnti, fmt::underlying(du_ue_id));
  get_du(du_idx).push_ul_pdu(init_ul_rrc_msg);

  // Wait for DL RRC message transfer (containing RRC Setup)
  bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000});
  if (not result) {
    return false;
  }

  // Check if the DL RRC Message with Msg4 is valid.
  report_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu), "invalid DL RRC message");

  // Check if the UE Id matches.
  auto& dl_rrc_msg = *f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
  report_error_if_not(int_to_gnb_du_ue_f1ap_id(dl_rrc_msg.gnb_du_ue_f1ap_id) == du_ue_id, "invalid gNB-DU-UE-F1AP-ID");
  report_error_if_not(int_to_srb_id(dl_rrc_msg.srb_id) == srb_id_t::srb0, "invalid SRB-Id");

  // Send RRC Setup Complete.
  // > Generate UL DCCH message (containing RRC Setup Complete).
  byte_buffer pdu = test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete());
  // > Generate UL RRC Message (containing RRC Setup Complete) with PDCP SN=0.
  get_du(du_idx).push_rrc_ul_dcch_message(du_ue_id, srb_id_t::srb1, std::move(pdu));

  // CU-CP should send an NGAP Initial UE Message.
  result = this->wait_for_ngap_tx_pdu(ngap_pdu);
  if (not result) {
    return false;
  }
  report_fatal_error_if_not(test_helpers::is_valid_init_ue_message(ngap_pdu), "Invalid init UE message");

  ue_context ue_ctx{};
  ue_ctx.crnti     = crnti;
  ue_ctx.du_ue_id  = du_ue_id;
  ue_ctx.cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(dl_rrc_msg.gnb_cu_ue_f1ap_id);
  ue_ctx.ran_ue_id = uint_to_ran_ue_id(ngap_pdu.pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id);

  report_fatal_error_if_not(attached_ues.insert(std::make_pair(ue_ctx.ran_ue_id.value(), ue_ctx)).second,
                            "UE already exists");
  report_fatal_error_if_not(
      du_ue_id_to_ran_ue_id_map[du_idx].insert(std::make_pair(du_ue_id, ue_ctx.ran_ue_id.value())).second,
      "DU UE ID already exists");

  return true;
}

bool cu_cp_test_environment::authenticate_ue(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id, amf_ue_id_t amf_ue_id)
{
  ngap_message ngap_pdu;
  srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
  f1ap_message f1ap_pdu;
  srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");

  auto& ue_ctx     = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));
  ue_ctx.amf_ue_id = amf_ue_id;

  // Inject NGAP DL message (authentication request)
  ngap_message dl_nas_transport =
      srs_cu_cp::generate_downlink_nas_transport_message(ue_ctx.amf_ue_id.value(), ue_ctx.ran_ue_id.value());
  get_amf().push_tx_pdu(dl_nas_transport);

  // Wait for DL RRC message transfer (containing NAS message)
  bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  if (not result) {
    return false;
  }
  report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                            "Invalid DL RRC message transfer");

  // Inject UL RRC msg transfer (authentication response)
  f1ap_message ul_rrc_msg_transfer = test_helpers::generate_ul_rrc_message_transfer(
      du_ue_id,
      ue_ctx.cu_ue_id.value(),
      srb_id_t::srb1,
      make_byte_buffer("00013a0abf002b96882dac46355c4f34464ddaf7b43fde37ae8000000000").value());
  get_du(du_idx).push_ul_pdu(ul_rrc_msg_transfer);

  // Wait for UL NAS Message (containing authentication response)
  result = this->wait_for_ngap_tx_pdu(ngap_pdu);
  if (not result) {
    return false;
  }

  // Inject DL NAS Transport message (ue security mode command)
  dl_nas_transport = generate_downlink_nas_transport_message(amf_ue_id, ue_ctx.ran_ue_id.value());
  get_amf().push_tx_pdu(dl_nas_transport);

  result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  if (not result) {
    return false;
  }
  report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                            "Invalid DL RRC message transfer");

  // Inject UL RRC msg transfer (ue security mode complete)
  ul_rrc_msg_transfer = test_helpers::generate_ul_rrc_message_transfer(
      du_ue_id,
      ue_ctx.cu_ue_id.value(),
      srb_id_t::srb1,
      make_byte_buffer("00023a1cbf0243241cb5003f002f3b80048290a1b283800000f8b880103f0020bc800680807888787f800008192a3b4"
                       "c080080170170700c0080a980808000000000")
          .value());
  get_du(du_idx).push_ul_pdu(ul_rrc_msg_transfer);

  // Wait for UL NAS Message (containing ue security mode complete)
  result = this->wait_for_ngap_tx_pdu(ngap_pdu);
  if (not result) {
    return false;
  }

  return true;
}

bool cu_cp_test_environment::setup_ue_security(unsigned du_idx, gnb_du_ue_f1ap_id_t du_ue_id)
{
  ngap_message ngap_pdu;
  srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
  f1ap_message f1ap_pdu;
  srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");

  auto& ue_ctx = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));

  // Inject NGAP Initial Context Setup Request
  ngap_message init_ctxt_setup_req =
      generate_valid_initial_context_setup_request_message(ue_ctx.amf_ue_id.value(), ue_ctx.ran_ue_id.value());
  get_amf().push_tx_pdu(init_ctxt_setup_req);

  // Wait for F1AP UE Context Setup Request (containing Security Mode Command).
  bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive Security Mode Command");
  report_fatal_error_if_not(test_helpers::is_valid_ue_context_setup_request(f1ap_pdu),
                            "Invalid UE Context Setup Request");
  {
    const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
    report_fatal_error_if_not(
        test_helpers::is_valid_rrc_security_mode_command(test_helpers::extract_dl_dcch_msg(rrc_container)),
        "Invalid Security Mode command");
  }

  // Inject UE Context Setup Response
  f1ap_message ue_ctxt_setup_response =
      test_helpers::generate_ue_context_setup_response(ue_ctx.cu_ue_id.value(), du_ue_id);
  get_du(du_idx).push_ul_pdu(ue_ctxt_setup_response);

  // Inject RRC Security Mode Complete
  f1ap_message ul_rrc_msg_transfer = test_helpers::generate_ul_rrc_message_transfer(
      du_ue_id, ue_ctx.cu_ue_id.value(), srb_id_t::srb1, make_byte_buffer("00032a00fd5ec7ff").value());
  get_du(du_idx).push_ul_pdu(ul_rrc_msg_transfer);

  // Wait for UE Capability Enquiry
  result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive DL RRC Message, containing RRC UE Capability Enquiry");
  report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                            "Invalid DL RRC Message Transfer");
  {
    const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
    report_fatal_error_if_not(
        test_helpers::is_valid_rrc_ue_capability_enquiry(test_helpers::extract_dl_dcch_msg(rrc_container)),
        "Invalid UE Capability Enquiry");
  }

  // Inject UL RRC Message Transfer (containing UE Capability Info)
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
      du_ue_id,
      ue_ctx.cu_ue_id.value(),
      srb_id_t::srb1,
      make_byte_buffer("00044c821930680ce811d1968097e360e1480005824c5c00060fc2c00637fe002e00131401a0000000880058d006007"
                       "a071e439f0000240400e0300000000100186c0000700809df000000000000030368000800004b2ca000a07143c001c0"
                       "03c000000100200409028098a8660c")
          .value()));

  // Wait for DL RRC Message Transfer (containing NAS Registration Accept)
  result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive DL RRC Message, containing NAS Registration Accept");
  report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                            "Invalid DL RRC Message Transfer");

  // Wait for Initial Context Setup Response.
  result = this->wait_for_ngap_tx_pdu(ngap_pdu);
  report_fatal_error_if_not(result, "Failed to receive Initial Context Setup Response");
  report_fatal_error_if_not(test_helpers::is_valid_initial_context_setup_response(ngap_pdu), "Invalid init ctxt setup");

  // Wait for UE Radio Capability Info Indication.
  result = this->wait_for_ngap_tx_pdu(ngap_pdu);
  report_fatal_error_if_not(result, "Failed to receive UE Radio Capability Info Indication");
  report_fatal_error_if_not(test_helpers::is_valid_ue_radio_capability_info_indication(ngap_pdu),
                            "Invalid UE Radio Capability Info Indication");

  return true;
}

bool cu_cp_test_environment::finish_ue_registration(unsigned du_idx, unsigned cu_up_idx, gnb_du_ue_f1ap_id_t du_ue_id)
{
  ngap_message ngap_pdu;
  srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");

  auto& ue_ctx = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));

  // Inject Registration Complete and wait UL NAS message.
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
      du_ue_id,
      ue_ctx.cu_ue_id.value(),
      srb_id_t::srb1,
      make_byte_buffer("00053a053f015362c51680bf00218086b09a5b").value()));
  bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
  report_fatal_error_if_not(result, "Failed to receive Registration Complete");

  return true;
}

bool cu_cp_test_environment::request_pdu_session_resource_setup(unsigned            du_idx,
                                                                unsigned            cu_up_idx,
                                                                gnb_du_ue_f1ap_id_t du_ue_id)
{
  ngap_message ngap_pdu;
  srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
  f1ap_message f1ap_pdu;
  srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");

  auto& ue_ctx = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));

  // Inject PDU Session Establishment Request and wait UL NAS message.
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
      du_ue_id,
      ue_ctx.cu_ue_id.value(),
      srb_id_t::srb1,
      make_byte_buffer("00063a253f011ffa9203013f0033808018970080e0ffffc9d8bd8013404010880080000840830000000041830000000"
                       "00000800001800005000006000006800008800900c092838339b939b0b83700e03a21bb")
          .value()));
  bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
  report_fatal_error_if_not(result, "Failed to receive Registration Complete");

  // Inject Configuration Update Command
  ngap_message dl_nas_transport_msg = generate_downlink_nas_transport_message(
      ue_ctx.amf_ue_id.value(),
      ue_ctx.ran_ue_id.value(),
      make_byte_buffer("7e0205545bfc027e0054430f90004f00700065006e00350047005346004732800131235200490100").value());
  get_amf().push_tx_pdu(dl_nas_transport_msg);
  result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive NAS Configuration Update Command");
  report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                            "Invalid DL RRC Message Transfer");

  return true;
}

bool cu_cp_test_environment::send_pdu_session_resource_setup_request_and_await_bearer_context_setup_request(
    const ngap_message& pdu_session_resource_setup_request,
    unsigned            du_idx,
    unsigned            cu_up_idx,
    gnb_du_ue_f1ap_id_t du_ue_id)
{
  e1ap_message e1ap_pdu;
  srsran_assert(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                "there are still E1AP messages to pop from CU-UP");

  // Inject PDU Session Resource Setup Request and wait for Bearer Context Setup Request
  get_amf().push_tx_pdu(pdu_session_resource_setup_request);
  bool result = this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive Bearer Context Setup Request");
  report_fatal_error_if_not(test_helpers::is_valid_bearer_context_setup_request(e1ap_pdu),
                            "Invalid Bearer Context Setup Request");

  auto& ue_ctx = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));
  ue_ctx.cu_cp_e1ap_id =
      int_to_gnb_cu_cp_ue_e1ap_id(e1ap_pdu.pdu.init_msg().value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id);

  return true;
}

bool cu_cp_test_environment::send_pdu_session_resource_setup_request_and_await_bearer_context_modification_request(
    const ngap_message& pdu_session_resource_setup_request,
    unsigned            cu_up_idx)
{
  e1ap_message e1ap_pdu;
  srsran_assert(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                "there are still E1AP messages to pop from CU-UP");

  // Inject PDU Session Resource Setup Request and wait for Bearer Context Setup Request
  get_amf().push_tx_pdu(pdu_session_resource_setup_request);
  bool result = this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive Bearer Context Modification Request");
  report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                            "Invalid Bearer Context Modification Request");

  return true;
}

bool cu_cp_test_environment::send_bearer_context_setup_response_and_await_ue_context_modification_request(
    unsigned               du_idx,
    unsigned               cu_up_idx,
    gnb_du_ue_f1ap_id_t    du_ue_id,
    gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id,
    pdu_session_id_t       psi,
    qos_flow_id_t          qfi)
{
  f1ap_message f1ap_pdu;
  srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");

  auto& ue_ctx         = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));
  ue_ctx.cu_up_e1ap_id = cu_up_e1ap_id;

  // Inject Bearer Context Setup Response and wait for UE Context Modification Request
  get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_setup_response(
      ue_ctx.cu_cp_e1ap_id.value(), ue_ctx.cu_up_e1ap_id.value(), {{psi, drb_test_params{drb_id_t::drb1, qfi}}}));
  bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive UE Context Modification Request");
  report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                            "Invalid UE Context Modification Request");

  return true;
}

bool cu_cp_test_environment::send_bearer_context_modification_response_and_await_ue_context_modification_request(
    unsigned            du_idx,
    unsigned            cu_up_idx,
    gnb_du_ue_f1ap_id_t du_ue_id,
    pdu_session_id_t    psi,
    drb_id_t            drb_id,
    qos_flow_id_t       qfi)
{
  f1ap_message f1ap_pdu;
  srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");

  auto& ue_ctx = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));

  // Inject Bearer Context Modification Response and wait for UE Context Modification Request
  get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(
      ue_ctx.cu_cp_e1ap_id.value(), ue_ctx.cu_up_e1ap_id.value(), {{psi, drb_test_params{drb_id, qfi}}}, {}));
  bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive UE Context Modification Request");
  report_fatal_error_if_not(test_helpers::is_valid_ue_context_modification_request(f1ap_pdu),
                            "Invalid UE Context Modification Request");

  return true;
}

bool cu_cp_test_environment::send_ue_context_modification_response_and_await_bearer_context_modification_request(
    unsigned            du_idx,
    unsigned            cu_up_idx,
    gnb_du_ue_f1ap_id_t du_ue_id,
    rnti_t              crnti)
{
  e1ap_message e1ap_pdu;
  srsran_assert(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                "there are still E1AP messages to pop from CU-UP");

  auto& ue_ctx = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));

  // Inject UE Context Modification Response and wait for Bearer Context Modification Request
  get_du(du_idx).push_ul_pdu(
      test_helpers::generate_ue_context_modification_response(du_ue_id, ue_ctx.cu_ue_id.value(), crnti));
  bool result = this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive Bearer Context Modification Request");
  report_fatal_error_if_not(test_helpers::is_valid_bearer_context_modification_request(e1ap_pdu),
                            "Invalid Bearer Context Modification Request");

  return true;
}

bool cu_cp_test_environment::send_bearer_context_modification_response_and_await_rrc_reconfiguration(
    unsigned                                           du_idx,
    unsigned                                           cu_up_idx,
    gnb_du_ue_f1ap_id_t                                du_ue_id,
    const std::map<pdu_session_id_t, drb_test_params>& pdu_sessions_to_add,
    const std::map<pdu_session_id_t, drb_id_t>&        pdu_sessions_to_modify,
    const std::optional<std::vector<srb_id_t>>&        expected_srbs_to_add_mod,
    const std::optional<std::vector<drb_id_t>>&        expected_drbs_to_add_mod,
    const std::vector<pdu_session_id_t>&               pdu_sessions_failed_to_modify)
{
  f1ap_message f1ap_pdu;
  srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");

  auto& ue_ctx = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));

  // Inject E1AP Bearer Context Modification Response and wait for DL RRC Message (containing RRC Reconfiguration)
  get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(ue_ctx.cu_cp_e1ap_id.value(),
                                                                                 ue_ctx.cu_up_e1ap_id.value(),
                                                                                 pdu_sessions_to_add,
                                                                                 pdu_sessions_to_modify,
                                                                                 pdu_sessions_failed_to_modify));
  bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  report_fatal_error_if_not(result, "Failed to receive F1AP DL RRC Message (containing RRC Reconfiguration)");
  report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                            "Invalid DL RRC Message Transfer");
  {
    const byte_buffer& rrc_container = test_helpers::get_rrc_container(f1ap_pdu);
    report_fatal_error_if_not(
        test_helpers::is_valid_rrc_reconfiguration(
            test_helpers::extract_dl_dcch_msg(rrc_container), true, expected_srbs_to_add_mod, expected_drbs_to_add_mod),
        "Invalid RRC Reconfiguration");
  }

  return true;
}

bool cu_cp_test_environment::send_rrc_reconfiguration_complete_and_await_pdu_session_setup_response(
    unsigned                             du_idx,
    gnb_du_ue_f1ap_id_t                  du_ue_id,
    byte_buffer                          rrc_reconfiguration_complete,
    const std::vector<pdu_session_id_t>& expected_pdu_sessions_to_setup,
    const std::vector<pdu_session_id_t>& expected_pdu_sessions_failed_to_setup)
{
  ngap_message ngap_pdu;
  srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");

  auto& ue_ctx = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));

  // Inject UL RRC Message (containing RRC Reconfiguration Complete) and wait for PDU Session Resource Setup Response
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
      du_ue_id, ue_ctx.cu_ue_id.value(), srb_id_t::srb1, std::move(rrc_reconfiguration_complete)));
  bool result = this->wait_for_ngap_tx_pdu(ngap_pdu);
  report_fatal_error_if_not(result, "Failed to receive PDU Session Resource Setup Response");
  report_fatal_error_if_not(test_helpers::is_valid_pdu_session_resource_setup_response(ngap_pdu),
                            "Invalid PDU Session Resource Setup Response");
  report_fatal_error_if_not(test_helpers::is_expected_pdu_session_resource_setup_response(
                                ngap_pdu, expected_pdu_sessions_to_setup, expected_pdu_sessions_failed_to_setup),
                            "Unsuccessful PDU Session Resource Setup Response");

  return true;
}

bool cu_cp_test_environment::setup_pdu_session(unsigned               du_idx,
                                               unsigned               cu_up_idx,
                                               gnb_du_ue_f1ap_id_t    du_ue_id,
                                               rnti_t                 crnti,
                                               gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id,
                                               pdu_session_id_t       psi,
                                               drb_id_t               drb_id,
                                               qos_flow_id_t          qfi,
                                               byte_buffer            rrc_reconfiguration_complete,
                                               bool                   is_initial_session)
{
  ngap_message ngap_pdu;
  srsran_assert(not this->get_amf().try_pop_rx_pdu(ngap_pdu), "there are still NGAP messages to pop from AMF");
  f1ap_message f1ap_pdu;
  srsran_assert(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu), "there are still F1AP DL messages to pop from DU");
  e1ap_message e1ap_pdu;
  srsran_assert(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                "there are still E1AP messages to pop from CU-UP");

  auto& ue_ctx = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(du_ue_id));

  ngap_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message(
      ue_ctx.amf_ue_id.value(), ue_ctx.ran_ue_id.value(), {{psi, {pdu_session_type_t::ipv4, {{qfi, 9}}}}});

  if (is_initial_session) {
    // Inject PDU Session Resource Setup Request and wait for Bearer Context Setup Request.
    if (not send_pdu_session_resource_setup_request_and_await_bearer_context_setup_request(
            pdu_session_resource_setup_request, du_idx, cu_up_idx, du_ue_id)) {
      return false;
    }

    // Inject Bearer Context Setup Response and wait for F1AP UE Context Modification Request.
    if (not send_bearer_context_setup_response_and_await_ue_context_modification_request(
            du_idx, cu_up_idx, du_ue_id, cu_up_e1ap_id, psi, qfi)) {
      return false;
    }
  } else {
    // Inject PDU Session Resource Setup Request and wait for Bearer Context Modification Request.
    if (not send_pdu_session_resource_setup_request_and_await_bearer_context_modification_request(
            pdu_session_resource_setup_request, du_idx)) {
      return false;
    }

    // Inject Bearer Context Modification Response and wait for F1AP UE Context Modification Request.
    if (not send_bearer_context_modification_response_and_await_ue_context_modification_request(
            du_idx, cu_up_idx, du_ue_id, psi, drb_id, qfi)) {
      return false;
    }
  }

  // Inject UE Context Modification Response and wait for Bearer Context Modification to be sent to CU-UP.
  if (not send_ue_context_modification_response_and_await_bearer_context_modification_request(
          du_idx, cu_up_idx, du_ue_id, crnti)) {
    return false;
  }

  // Inject Bearer Context Modification Response and wait for DL RRC Message (containing RRC Reconfiguration)
  if (not send_bearer_context_modification_response_and_await_rrc_reconfiguration(
          du_idx, cu_up_idx, du_ue_id, {}, {{psi, drb_id}})) {
    return false;
  }

  // Inject RRC Reconfiguration Complete and wait for PDU Session Resource Setup Response to be sent to AMF.
  if (not send_rrc_reconfiguration_complete_and_await_pdu_session_setup_response(
          du_idx, du_ue_id, std::move(rrc_reconfiguration_complete), {psi}, {})) {
    return false;
  }

  return true;
}

bool cu_cp_test_environment::attach_ue(unsigned               du_idx,
                                       unsigned               cu_up_idx,
                                       gnb_du_ue_f1ap_id_t    du_ue_id,
                                       rnti_t                 crnti,
                                       amf_ue_id_t            amf_ue_id,
                                       gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id,
                                       pdu_session_id_t       psi,
                                       drb_id_t               drb_id,
                                       qos_flow_id_t          qfi,
                                       byte_buffer            rrc_reconfiguration_complete)
{
  if (not connect_new_ue(du_idx, du_ue_id, crnti)) {
    return false;
  }
  if (not authenticate_ue(du_idx, du_ue_id, amf_ue_id)) {
    return false;
  }
  if (not setup_ue_security(du_idx, du_ue_id)) {
    return false;
  }
  if (not finish_ue_registration(du_idx, cu_up_idx, du_ue_id)) {
    return false;
  }
  if (not request_pdu_session_resource_setup(du_idx, cu_up_idx, du_ue_id)) {
    return false;
  }
  if (not setup_pdu_session(du_idx,
                            cu_up_idx,
                            du_ue_id,
                            crnti,
                            cu_up_e1ap_id,
                            psi,
                            drb_id,
                            qfi,
                            std::move(rrc_reconfiguration_complete),
                            true)) {
    return false;
  }

  return true;
}

bool cu_cp_test_environment::reestablish_ue(unsigned            du_idx,
                                            unsigned            cu_up_idx,
                                            gnb_du_ue_f1ap_id_t new_du_ue_id,
                                            rnti_t              new_crnti,
                                            rnti_t              old_crnti,
                                            pci_t               old_pci)
{
  f1ap_message f1ap_pdu;

  // Send Initial UL RRC Message (containing RRC Reestablishment Request) to CU-CP.
  byte_buffer rrc_container = test_helpers::pack_ul_ccch_msg(
      test_helpers::create_rrc_reestablishment_request(old_crnti, old_pci, "1111010001000010"));
  f1ap_message f1ap_init_ul_rrc_msg =
      test_helpers::generate_init_ul_rrc_message_transfer(new_du_ue_id, new_crnti, {}, std::move(rrc_container));
  get_du(du_idx).push_ul_pdu(f1ap_init_ul_rrc_msg);

  // Wait for DL RRC message transfer (with RRC Reestablishment / RRC Setup / RRC Reject).
  bool result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
  report_fatal_error_if_not(result, "F1AP DL RRC Message Transfer with Msg4 not sent to DU");
  report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu), "Invalid Msg4");

  auto& dl_rrc_msg = *f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
  report_fatal_error_if_not(int_to_gnb_du_ue_f1ap_id(dl_rrc_msg.gnb_du_ue_f1ap_id) == new_du_ue_id,
                            "Invalid gNB-DU-UE-F1AP-ID");

  if (dl_rrc_msg.srb_id == 0) {
    // RRC Setup / RRC Reject.

    // Send RRC Setup Complete.
    // > Generate UL DCCH message (containing RRC Setup Complete).
    byte_buffer pdu = test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete());
    // > Generate UL RRC Message (containing RRC Setup Complete) with PDCP SN=0.
    get_du(du_idx).push_rrc_ul_dcch_message(new_du_ue_id, srb_id_t::srb1, std::move(pdu));

    // CU-CP should send an NGAP Initial UE Message.
    ngap_message ngap_pdu;
    result = this->wait_for_ngap_tx_pdu(ngap_pdu);
    report_fatal_error_if_not(result, "Failed to send NGAP Initial UE Message");
    report_fatal_error_if_not(test_helpers::is_valid_init_ue_message(ngap_pdu), "Invalid init UE message");

    ue_context ue_ctx{};
    ue_ctx.crnti     = new_crnti;
    ue_ctx.du_ue_id  = new_du_ue_id;
    ue_ctx.cu_ue_id  = int_to_gnb_cu_ue_f1ap_id(dl_rrc_msg.gnb_cu_ue_f1ap_id);
    ue_ctx.ran_ue_id = uint_to_ran_ue_id(ngap_pdu.pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id);

    report_fatal_error_if_not(attached_ues.insert(std::make_pair(ue_ctx.ran_ue_id.value(), ue_ctx)).second,
                              "UE already exists");
    report_fatal_error_if_not(
        du_ue_id_to_ran_ue_id_map[du_idx].insert(std::make_pair(new_du_ue_id, ue_ctx.ran_ue_id.value())).second,
        "DU UE ID already exists");

    return false;
  }

  gnb_du_ue_f1ap_id_t old_du_ue_id = int_to_gnb_du_ue_f1ap_id(dl_rrc_msg.old_gnb_du_ue_f1ap_id);
  ue_context&         old_ue       = attached_ues.at(du_ue_id_to_ran_ue_id_map.at(du_idx).at(old_du_ue_id));
  old_ue.du_ue_id                  = new_du_ue_id;
  old_ue.cu_ue_id                  = int_to_gnb_cu_ue_f1ap_id(dl_rrc_msg.gnb_cu_ue_f1ap_id);
  old_ue.crnti                     = new_crnti;
  ran_ue_id_t ran_ue_id            = *old_ue.ran_ue_id;
  du_ue_id_to_ran_ue_id_map.at(du_idx).erase(old_du_ue_id);
  report_fatal_error_if_not(du_ue_id_to_ran_ue_id_map[du_idx].insert(std::make_pair(new_du_ue_id, ran_ue_id)).second,
                            "DU UE ID already exists");

  // EVENT: Send RRC Reestablishment Complete.
  // > Generate UL-DCCH message (containing RRC Reestablishment Complete).
  byte_buffer pdu = test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_reestablishment_complete());
  // > Prepend PDCP header and append MAC.
  report_error_if_not(pdu.prepend(std::array<uint8_t, 2>{0x00U, 0x00U}), "bad alloc");
  report_error_if_not(pdu.append(std::array<uint8_t, 4>{0x85, 0xc1, 0x04, 0xf1}), "bad alloc");
  // > Send UL RRC Message to CU-CP.
  get_du(du_idx).push_ul_pdu(
      test_helpers::generate_ul_rrc_message_transfer(new_du_ue_id, *old_ue.cu_ue_id, srb_id_t::srb1, std::move(pdu)));

  // STATUS: CU-CP sends E1AP Bearer Context Modification Request.
  e1ap_message e1ap_pdu;
  report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(0, e1ap_pdu), "E1AP BearerContextModificationRequest NOT sent");

  gnb_cu_cp_ue_e1ap_id_t cu_cp_e1ap_id =
      int_to_gnb_cu_cp_ue_e1ap_id(e1ap_pdu.pdu.init_msg().value.bearer_context_mod_request()->gnb_cu_cp_ue_e1ap_id);
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id =
      int_to_gnb_cu_up_ue_e1ap_id(e1ap_pdu.pdu.init_msg().value.bearer_context_mod_request()->gnb_cu_up_ue_e1ap_id);

  // EVENT: Inject E1AP Bearer Context Modification Response
  get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));

  // STATUS: CU-CP sends F1AP UE Context Modification Request to DU.
  report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu), "F1AP UEContextModificationRequest NOT sent");
  report_fatal_error_if_not(f1ap_pdu.pdu.init_msg().value.ue_context_mod_request()->drbs_to_be_modified_list_present,
                            "UE Context Modification Request for RRC Reestablishment must contain DRBs to be modified");
  report_fatal_error_if_not(
      not f1ap_pdu.pdu.init_msg().value.ue_context_mod_request()->drbs_to_be_setup_mod_list_present,
      "UE Context Modification Request for RRC Reestablishment must not contain DRBs to be setup");

  // EVENT: Inject F1AP UE Context Modification Response
  get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_modification_response(
      new_du_ue_id, *this->find_ue_context(du_idx, new_du_ue_id)->cu_ue_id, new_crnti));

  // STATUS: CU-CP sends E1AP Bearer Context Modification Request.
  report_fatal_error_if_not(this->wait_for_e1ap_tx_pdu(0, e1ap_pdu), "E1AP BearerContextModificationRequest NOT sent");

  // EVENT: CU-UP sends E1AP Bearer Context Modification Response
  get_cu_up(cu_up_idx).push_tx_pdu(generate_bearer_context_modification_response(cu_cp_e1ap_id, cu_up_e1ap_id));

  // STATUS: CU-CP sends F1AP DL RRC Message Transfer (containing RRC Reconfiguration).
  report_fatal_error_if_not(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu), "F1AP DL RRC Message NOT sent");
  report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer(f1ap_pdu),
                            "Invalid DL RRC Message Transfer");

  // EVENT: DU sends F1AP UL RRC Message Transfer (containing RRC Reconfiguration Complete).
  pdu = test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_reconfiguration_complete(1U));
  // > Prepend PDCP header and append MAC.
  report_error_if_not(pdu.prepend(std::array<uint8_t, 2>{0x00U, 0x01U}), "bad alloc");
  report_error_if_not(pdu.append(std::array<uint8_t, 4>{0xf1, 0x21, 0x02, 0x5e}), "bad alloc");
  get_du(du_idx).push_ul_pdu(
      test_helpers::generate_ul_rrc_message_transfer(new_du_ue_id, *old_ue.cu_ue_id, srb_id_t::srb1, std::move(pdu)));

  return true;
}
