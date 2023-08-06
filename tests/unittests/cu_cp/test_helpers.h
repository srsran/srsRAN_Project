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

#pragma once

#include "du_processor_test_messages.h"
#include "lib/cu_cp/cu_cp_impl_interface.h"
#include "lib/e1ap/common/e1ap_asn1_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/ngap/ngap_test_helpers.h"
#include "srsran/adt/variant.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/cu_up_processor.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/support/async/async_task_loop.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <cstdint>
#include <list>

namespace srsran {
namespace srs_cu_cp {

/// \brief Generate DU-to-CU RRC Container with CellGroupConfig.
byte_buffer generate_container_with_cell_group_config();

/// \brief Generate RRC Container with RRC Setup Complete message.
byte_buffer generate_rrc_setup_complete();

struct dummy_du_processor_ue_task_scheduler : public du_processor_ue_task_scheduler {
public:
  dummy_du_processor_ue_task_scheduler(timer_manager& timers_, task_executor& exec_) : timer_db(timers_), exec(exec_) {}
  void schedule_async_task(ue_index_t ue_index, async_task<void>&& task) override
  {
    ctrl_loop.schedule(std::move(task));
  }
  unique_timer   make_unique_timer() override { return timer_db.create_unique_timer(exec); }
  timer_manager& get_timer_manager() override { return timer_db; }

  void tick_timer() { timer_db.tick(); }

private:
  async_task_sequencer ctrl_loop{16};
  timer_manager&       timer_db;
  task_executor&       exec;
};

struct dummy_du_processor_cu_cp_notifier : public du_processor_cu_cp_notifier {
public:
  dummy_ngap_du_processor_notifier ngap_notifier;

  explicit dummy_du_processor_cu_cp_notifier(cu_cp_du_event_handler* cu_cp_handler_ = nullptr) :
    cu_cp_handler(cu_cp_handler_)
  {
  }

  void attach_handler(cu_cp_du_event_handler* cu_cp_handler_) { cu_cp_handler = cu_cp_handler_; }

  void on_rrc_ue_created(du_index_t du_index, ue_index_t ue_index, rrc_ue_interface& rrc_ue) override
  {
    logger.info("Received a RRC UE creation notification");

    if (cu_cp_handler != nullptr) {
      cu_cp_handler->handle_rrc_ue_creation(du_index, ue_index, rrc_ue, ngap_notifier);
    }
  }

private:
  srslog::basic_logger&   logger        = srslog::fetch_basic_logger("TEST");
  cu_cp_du_event_handler* cu_cp_handler = nullptr;
};

// Configuration struct to parameterize the modification outcome
struct pdu_session_modified_outcome_t {
  pdu_session_id_t      psi;
  std::vector<drb_id_t> drb_added;
  std::vector<drb_id_t> drb_removed;
};

// Stuct to configure Bearer Context Setup/Modification result content.
struct bearer_context_outcome_t {
  bool                outcome = false;
  std::list<unsigned> pdu_sessions_setup_list;  // List of PDU session IDs that were successful to setup.
  std::list<unsigned> pdu_sessions_failed_list; // List of PDU sessions IDs that failed to be setup.
  std::list<pdu_session_modified_outcome_t>
      pdu_sessions_modified_list; // List of PDU session IDs that were successfully modified.
};

struct dummy_du_processor_e1ap_control_notifier : public du_processor_e1ap_control_notifier {
public:
  dummy_du_processor_e1ap_control_notifier() = default;

  void set_first_message_outcome(const bearer_context_outcome_t& outcome) { first_e1ap_response = outcome; }

  void set_second_message_outcome(const bearer_context_outcome_t& outcome) { second_e1ap_response = outcome; }

  void fill_pdu_session_setup_list(
      slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>& e1ap_setup_list,
      const std::list<unsigned>&                                                              outcome_setup_list)
  {
    for (const auto& psi : outcome_setup_list) {
      // add only the most relevant items
      e1ap_pdu_session_resource_setup_modification_item res_setup_item;
      res_setup_item.pdu_session_id = uint_to_pdu_session_id(psi);

      // add a single DRB with the same ID like the PDU session it belongs to
      drb_id_t                   drb_id = uint_to_drb_id(psi); // Note: we use the PDU session ID here also as DRB ID
      e1ap_drb_setup_item_ng_ran drb_item;
      drb_item.drb_id = drb_id;

      // add a QoS flow
      e1ap_qos_flow_item qos_item;
      qos_item.qos_flow_id = uint_to_qos_flow_id(psi); // Note: use the PSI again as QoS flow ID
      drb_item.flow_setup_list.emplace(qos_item.qos_flow_id, qos_item);

      // add one UP transport item
      e1ap_up_params_item up_item;
      up_item.cell_group_id = 0;
      up_item.up_tnl_info   = {transport_layer_address{"127.0.0.1"}, int_to_gtpu_teid(0x1)};
      drb_item.ul_up_transport_params.push_back(up_item);
      res_setup_item.drb_setup_list_ng_ran.emplace(drb_id, drb_item);

      e1ap_setup_list.emplace(res_setup_item.pdu_session_id, res_setup_item);
    }
  }

  void fill_pdu_session_failed_list(
      slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>& e1ap_failed_list,
      const std::list<unsigned>&                                                  outcome_failed_list)
  {
    for (const auto& id : outcome_failed_list) {
      e1ap_pdu_session_resource_failed_item res_failed_item;
      res_failed_item.pdu_session_id = uint_to_pdu_session_id(id);
      e1ap_failed_list.emplace(res_failed_item.pdu_session_id, res_failed_item);
    }
  }

  void fill_bearer_context_response(e1ap_bearer_context_setup_response& result, const bearer_context_outcome_t& outcome)
  {
    result.success = outcome.outcome;
    fill_pdu_session_setup_list(result.pdu_session_resource_setup_list, outcome.pdu_sessions_setup_list);
    fill_pdu_session_failed_list(result.pdu_session_resource_failed_list, outcome.pdu_sessions_failed_list);
  }

  void fill_pdu_session_modified_list(
      slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>& e1ap_modified_list,
      const std::list<pdu_session_modified_outcome_t>&                              outcome_modified_list)
  {
    for (const auto& modified_item : outcome_modified_list) {
      // add only the most relevant items
      e1ap_pdu_session_resource_modified_item res_mod_item;
      res_mod_item.pdu_session_id = modified_item.psi;

      for (const auto& drb_id : modified_item.drb_added) {
        // add a single DRB with the same ID like the PDU session it belongs to
        e1ap_drb_setup_item_ng_ran drb_item;
        drb_item.drb_id = drb_id;

        // add a QoS flow
        e1ap_qos_flow_item qos_item;
        qos_item.qos_flow_id = uint_to_qos_flow_id(drb_id_to_uint(drb_id)); // QoS flow has same ID like DRB
        drb_item.flow_setup_list.emplace(qos_item.qos_flow_id, qos_item);

        // add one UP transport item
        e1ap_up_params_item up_item;
        drb_item.ul_up_transport_params.push_back(up_item);
        res_mod_item.drb_setup_list_ng_ran.emplace(drb_id, drb_item);
      }

      e1ap_modified_list.emplace(res_mod_item.pdu_session_id, res_mod_item);
    }
  }

  void fill_bearer_context_response(e1ap_bearer_context_modification_response& result,
                                    const bearer_context_outcome_t&            outcome)
  {
    result.success = outcome.outcome;
    fill_pdu_session_setup_list(result.pdu_session_resource_setup_list, outcome.pdu_sessions_setup_list);
    fill_pdu_session_failed_list(result.pdu_session_resource_failed_list, outcome.pdu_sessions_failed_list);
    fill_pdu_session_modified_list(result.pdu_session_resource_modified_list, outcome.pdu_sessions_modified_list);
  }

  async_task<e1ap_bearer_context_setup_response>
  on_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override
  {
    logger.info("Received a new bearer context setup request");

    first_e1ap_request = msg; // store msg to verify content in TC

    return launch_async([res = e1ap_bearer_context_setup_response{}, msg, this](
                            coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (first_e1ap_response.has_value()) {
        const auto& response = first_e1ap_response.value();
        fill_bearer_context_response(res, response);

        // Invalidate E1 response so it's not consumed again.
        first_e1ap_response.reset();
      }

      CORO_RETURN(res);
    });
  }

  async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override
  {
    logger.info("Received a new bearer context modification request");

    // store msg to be verify content in TC
    if (first_e1ap_request.has_value()) {
      second_e1ap_request = msg;
    } else {
      first_e1ap_request = msg;
    }

    return launch_async([res = e1ap_bearer_context_modification_response{},
                         this](coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (first_e1ap_response.has_value()) {
        // first E1AP message is already a bearer modification
        const auto& response = first_e1ap_response.value();
        fill_bearer_context_response(res, response);
        first_e1ap_response.reset(); // Make sure it's not consumed again.
      } else if (second_e1ap_response.has_value()) {
        // second E1AP message is a bearer modification
        const auto& response = second_e1ap_response.value();
        fill_bearer_context_response(res, response);
        second_e1ap_response.reset(); // Make sure it's not consumed again.
      }
      CORO_RETURN(res);
    });
  }

  virtual async_task<void> on_bearer_context_release_command(const e1ap_bearer_context_release_command& cmd) override
  {
    logger.info("Received a new bearer context release command");

    return launch_async([](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  void reset()
  {
    first_e1ap_request.reset();
    second_e1ap_request.reset();
  }

  optional<variant<e1ap_bearer_context_setup_request, e1ap_bearer_context_modification_request>> first_e1ap_request;
  optional<e1ap_bearer_context_modification_request>                                             second_e1ap_request;

private:
  srslog::basic_logger&              logger = srslog::fetch_basic_logger("TEST");
  optional<bearer_context_outcome_t> first_e1ap_response;
  optional<bearer_context_outcome_t> second_e1ap_response;
};

struct dummy_du_processor_ngap_control_notifier : public du_processor_ngap_control_notifier {
public:
  dummy_du_processor_ngap_control_notifier() = default;

  virtual void on_ue_context_release_request(const cu_cp_ue_context_release_request& msg) override
  {
    logger.info("Received a UE Context Release Request");
  }

  async_task<ngap_handover_preparation_response>
  on_ngap_handover_preparation_request(const ngap_handover_preparation_request& request) override
  {
    return launch_async([](coro_context<async_task<ngap_handover_preparation_response>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(ngap_handover_preparation_response{false});
    });
  }

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

struct ue_context_outcome_t {
  bool                outcome = false;
  std::list<unsigned> drb_success_list; // List of DRB IDs that were successful to setup.
  std::list<unsigned> drb_failed_list;  // List of DRB IDs that failed to be setup.
  std::list<unsigned> drb_removed_list; // List of DRB IDs that were removed.
};

struct dummy_du_processor_f1ap_ue_context_notifier : public du_processor_f1ap_ue_context_notifier {
public:
  dummy_du_processor_f1ap_ue_context_notifier() = default;

  void set_ue_context_setup_outcome(bool outcome) { ue_context_setup_outcome = outcome; }

  void set_ue_context_modification_outcome(ue_context_outcome_t outcome) { ue_context_modification_outcome = outcome; }

  async_task<f1ap_ue_context_setup_response>
  on_ue_context_setup_request(const f1ap_ue_context_setup_request& request) override
  {
    logger.info("Received a new UE context setup request");

    return launch_async([res = f1ap_ue_context_setup_response{},
                         this](coro_context<async_task<f1ap_ue_context_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      res.success = ue_context_setup_outcome;

      CORO_RETURN(res);
    });
  }

  async_task<f1ap_ue_context_modification_response>
  on_ue_context_modification_request(const f1ap_ue_context_modification_request& request) override
  {
    logger.info("Received a new UE context modification request");

    // store request so it can be verified in the test code
    make_partial_copy(ue_context_modifcation_request, request);

    return launch_async([res = f1ap_ue_context_modification_response{},
                         this](coro_context<async_task<f1ap_ue_context_modification_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      res.success = ue_context_modification_outcome.outcome;
      for (const auto& drb_id : ue_context_modification_outcome.drb_success_list) {
        // add only the most relevant items
        f1ap_drbs_setup_mod_item drb_item;
        drb_item.drb_id = uint_to_drb_id(drb_id); // set ID
        res.drbs_setup_mod_list.emplace(drb_item.drb_id, drb_item);
      }
      // TODO: add failed list and other fields here ..

      CORO_RETURN(res);
    });
  }

  async_task<ue_index_t> on_ue_context_release_command(const f1ap_ue_context_release_command& msg) override
  {
    logger.info("Received a new UE context release command");

    return launch_async([msg](coro_context<async_task<ue_index_t>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(msg.ue_index);
    });
  }

  const f1ap_ue_context_modification_request& get_ctxt_mod_request() { return ue_context_modifcation_request; }

private:
  void make_partial_copy(f1ap_ue_context_modification_request&       target,
                         const f1ap_ue_context_modification_request& source)
  {
    // only copy fields that are actually checked in unit tests
    target.drbs_to_be_setup_mod_list = source.drbs_to_be_setup_mod_list;
    target.drbs_to_be_released_list  = source.drbs_to_be_released_list;
  }

  srslog::basic_logger& logger                   = srslog::fetch_basic_logger("TEST");
  bool                  ue_context_setup_outcome = false;
  ue_context_outcome_t  ue_context_modification_outcome;

  f1ap_ue_context_modification_request ue_context_modifcation_request;
};

struct dummy_du_processor_rrc_ue_control_message_notifier : public du_processor_rrc_ue_control_message_notifier {
public:
  dummy_du_processor_rrc_ue_control_message_notifier() = default;

  void set_rrc_reconfiguration_outcome(bool outcome) { rrc_reconfiguration_outcome = outcome; }

  void set_transaction_id(unsigned transaction_id_) { transaction_id = transaction_id_; }

  async_task<bool> on_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) override
  {
    logger.info("Received a new UE capability transfer request");

    return launch_async([this](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(ue_cap_transfer_outcome);
    });
  }

  async_task<bool> on_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override
  {
    logger.info("Received a new RRC reconfiguration request");
    last_radio_bearer_cfg = msg.radio_bearer_cfg;

    return launch_async([this](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(rrc_reconfiguration_outcome);
    });
  }

  uint8_t on_handover_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override
  {
    logger.info("Received a new handover reconfiguration request (transaction_id={})", transaction_id);
    last_radio_bearer_cfg = msg.radio_bearer_cfg;
    return transaction_id;
  }

  async_task<bool> on_handover_reconfiguration_complete_expected(uint8_t transaction_id_) override
  {
    logger.info("Awaiting a RRC Reconfiguration Complete (transaction_id={})", transaction_id_);
    last_transaction_id = transaction_id_;
    return launch_async([this](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(rrc_reconfiguration_outcome);
    });
  }

  rrc_ue_release_context get_rrc_ue_release_context() override
  {
    logger.info("Received a new request to get RRC UE release context");
    rrc_ue_release_context release_context;
    // TODO: Add values
    return release_context;
  }
  optional<rrc_meas_cfg> get_rrc_ue_meas_config() override
  {
    optional<rrc_meas_cfg> meas_config;
    return meas_config;
  }

  optional<rrc_radio_bearer_config> last_radio_bearer_cfg;

  void reset() { last_radio_bearer_cfg.reset(); }

  unsigned last_transaction_id;

private:
  srslog::basic_logger& logger                      = srslog::fetch_basic_logger("TEST");
  bool                  ue_cap_transfer_outcome     = true;
  bool                  rrc_reconfiguration_outcome = false;
  unsigned              transaction_id;
};

struct dummy_du_processor_rrc_du_ue_notifier : public du_processor_rrc_du_ue_notifier {
public:
  dummy_du_processor_rrc_du_ue_notifier() = default;

  bool on_new_served_cell_list(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) override
  {
    logger.info("Received a served cell list");
    return true;
  }

  rrc_ue_interface* on_ue_creation_request(up_resource_manager&           up_resource_mng,
                                           const rrc_ue_creation_message& msg) override
  {
    logger.info("Received a UE creation request");
    return nullptr;
  }

  void on_ue_context_release_command(ue_index_t ue_index) override { logger.info("Received a UE Release Command"); }

  /// Send RRC Release to all UEs connected to this DU.
  void on_release_ues() override { logger.info("Releasing all UEs"); }

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

struct dummy_cu_up_processor_cu_up_management_notifier : public cu_up_processor_cu_up_management_notifier {
public:
  dummy_cu_up_processor_cu_up_management_notifier() = default;

  void on_new_cu_up_connection() override { logger.info("Received a new CU-UP connection notification"); }

  void on_cu_up_remove_request_received(const cu_up_index_t cu_up_index) override
  {
    logger.info("Received a CU-UP remove request for cu_up_index={}", cu_up_index);
    last_cu_up_index_to_remove = cu_up_index;
  }

  cu_up_index_t last_cu_up_index_to_remove;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

struct dummy_cu_up_processor_task_scheduler : public cu_up_processor_task_scheduler {
public:
  dummy_cu_up_processor_task_scheduler(timer_manager& timers_, task_executor& exec_) : timer_db(timers_), exec(exec_) {}

  void schedule_async_task(cu_up_index_t cu_up_index, async_task<void>&& task) override
  {
    ctrl_loop.schedule(std::move(task));
  }
  unique_timer   make_unique_timer() override { return timer_db.create_unique_timer(exec); }
  timer_manager& get_timer_manager() override { return timer_db; }

  void tick_timer() { timer_db.tick(); }

private:
  async_task_sequencer ctrl_loop{16};
  timer_manager&       timer_db;
  task_executor&       exec;
};

struct dummy_cu_up_processor_e1ap_control_notifier : public cu_up_processor_e1ap_control_notifier {
public:
  dummy_cu_up_processor_e1ap_control_notifier() = default;

  void set_cu_cp_e1_setup_outcome(bool outcome) { cu_cp_e1_setup_outcome = outcome; }

  async_task<cu_cp_e1_setup_response> on_cu_cp_e1_setup_request(const cu_cp_e1_setup_request& request) override
  {
    logger.info("Received a new CU-CP E1 setup request");

    return launch_async([this](coro_context<async_task<cu_cp_e1_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      cu_cp_e1_setup_response res;
      res.success = cu_cp_e1_setup_outcome;
      if (cu_cp_e1_setup_outcome) {
        fill_e1ap_cu_cp_e1_setup_response(
            res, generate_cu_cp_e1_setup_respose(0).pdu.successful_outcome().value.gnb_cu_cp_e1_setup_resp());
      } else {
        fill_e1ap_cu_cp_e1_setup_response(
            res, generate_cu_cp_e1_setup_failure(0).pdu.unsuccessful_outcome().value.gnb_cu_cp_e1_setup_fail());
      }

      CORO_RETURN(res);
    });
  }

private:
  bool cu_cp_e1_setup_outcome = false;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

} // namespace srs_cu_cp
} // namespace srsran
