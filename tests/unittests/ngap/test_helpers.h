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

#pragma once

#include "ngap_test_messages.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/security/security.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include <gtest/gtest.h>
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct dummy_ngap_ue_task_scheduler : public ngap_ue_task_scheduler {
public:
  dummy_ngap_ue_task_scheduler(timer_manager& timers_, task_executor& exec_) : timer_db(timers_), exec(exec_) {}
  void schedule_async_task(ue_index_t ue_index, async_task<void>&& task) override
  {
    ctrl_loop.schedule(std::move(task));
  }
  unique_timer   make_unique_timer() override { return timer_db.create_unique_timer(exec); }
  timer_manager& get_timer_manager() override { return timer_db; }

  void tick_timer() { timer_db.tick(); }

private:
  fifo_async_task_scheduler ctrl_loop{16};
  timer_manager&            timer_db;
  task_executor&            exec;
};

/// Reusable notifier class that a) stores the received msg for test inspection and b)
/// calls the registered msg handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_ngap_amf_notifier : public ngap_message_notifier
{
public:
  dummy_ngap_amf_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  void attach_handler(ngap_message_handler* handler_) { handler = handler_; }

  void on_new_message(const ngap_message& msg) override
  {
    logger.info("Received message");

    // Verify correct packing of outbound PDU.
    byte_buffer   pack_buffer;
    asn1::bit_ref bref(pack_buffer);
    ASSERT_EQ(msg.pdu.pack(bref), asn1::SRSASN_SUCCESS);

    if (logger.debug.enabled()) {
      asn1::json_writer js;
      msg.pdu.to_json(js);
      logger.debug("Tx NGAP PDU: {}", js.to_string());
    }
    last_ngap_msgs.push_back(msg);

    if (handler != nullptr) {
      logger.info("Forwarding PDU");
      handler->handle_message(msg);
    }
  }

  std::vector<ngap_message> last_ngap_msgs = {};

private:
  srslog::basic_logger& logger;
  ngap_message_handler* handler = nullptr;
};

/// Dummy handler storing and printing the received PDU.
class dummy_ngap_message_handler : public ngap_message_handler
{
public:
  dummy_ngap_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const ngap_message& msg) override
  {
    last_msg = msg;
    logger.info("Received a PDU of type {}", msg.pdu.type().to_string());
  }
  ngap_message last_msg;

private:
  srslog::basic_logger& logger;
};

/// Dummy NGAP to RRC UE notifier
class dummy_ngap_rrc_ue_notifier : public ngap_rrc_ue_pdu_notifier, public ngap_rrc_ue_control_notifier
{
public:
  dummy_ngap_rrc_ue_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_new_pdu(byte_buffer nas_pdu) override
  {
    last_nas_pdu = std::move(nas_pdu);
    logger.info("Received a NAS PDU");
  }

  async_task<bool> on_new_security_context(const security::security_context& sec_context) override
  {
    logger.info("Received a new security context");

    bool result = true;

    // NIA0 is not allowed
    security::preferred_integrity_algorithms inc_algo_pref_list  = {security::integrity_algorithm::nia2,
                                                                    security::integrity_algorithm::nia1,
                                                                    security::integrity_algorithm::nia3,
                                                                    security::integrity_algorithm::nia0};
    security::preferred_ciphering_algorithms ciph_algo_pref_list = {security::ciphering_algorithm::nea0,
                                                                    security::ciphering_algorithm::nea2,
                                                                    security::ciphering_algorithm::nea1,
                                                                    security::ciphering_algorithm::nea3};

    security::security_context tmp_ctxt;
    tmp_ctxt = sec_context;

    result = tmp_ctxt.select_algorithms(inc_algo_pref_list, ciph_algo_pref_list);

    return launch_async([result](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(result);
    });
  }

  bool on_security_enabled() override { return security_enabled; }

  byte_buffer on_handover_preparation_message_required() override { return ho_preparation_message.copy(); }

  bool on_new_rrc_handover_command(byte_buffer cmd) override
  {
    last_handover_command = std::move(cmd);
    return true;
  }

  void set_ho_preparation_message(byte_buffer ho_preparation_message_)
  {
    ho_preparation_message = std::move(ho_preparation_message_);
  }
  void set_security_enabled(bool enabled) { security_enabled = enabled; }

  byte_buffer last_nas_pdu;
  byte_buffer ho_preparation_message;
  bool        security_enabled = true;
  byte_buffer last_handover_command;

private:
  srslog::basic_logger& logger;
};

/// Dummy NGAP to DU processor notifier
class dummy_ngap_du_processor_notifier : public ngap_du_processor_control_notifier
{
public:
  dummy_ngap_du_processor_notifier(ngap_ue_context_removal_handler& ngap_handler_) :
    logger(srslog::fetch_basic_logger("TEST")), ngap_handler(ngap_handler_){};

  ue_index_t on_new_ue_index_required() override
  {
    logger.info("Requested to allocate a new ue index.");
    return allocate_ue_index();
  }

  async_task<cu_cp_pdu_session_resource_setup_response>
  on_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) override
  {
    logger.info("Received a new pdu session resource setup request.");

    last_request = std::move(request);

    return launch_async([this, res = cu_cp_pdu_session_resource_setup_response{}](
                            coro_context<async_task<cu_cp_pdu_session_resource_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (last_request.pdu_session_res_setup_items.size() == 0) {
        cu_cp_pdu_session_res_setup_failed_item failed_item;
        failed_item.pdu_session_id              = uint_to_pdu_session_id(1);
        failed_item.unsuccessful_transfer.cause = cause_radio_network_t::unspecified;
        res.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
      } else {
        res = generate_cu_cp_pdu_session_resource_setup_response(uint_to_pdu_session_id(1));
      }

      CORO_RETURN(res);
    });
  }

  async_task<cu_cp_pdu_session_resource_modify_response>
  on_new_pdu_session_resource_modify_request(cu_cp_pdu_session_resource_modify_request& request) override
  {
    logger.info("Received a new pdu session resource modify request");

    last_modify_request = std::move(request);

    return launch_async([res = cu_cp_pdu_session_resource_modify_response{}](
                            coro_context<async_task<cu_cp_pdu_session_resource_modify_response>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(res);
    });
  }

  async_task<cu_cp_pdu_session_resource_release_response>
  on_new_pdu_session_resource_release_command(cu_cp_pdu_session_resource_release_command& command) override
  {
    logger.info("Received a new pdu session resource release command");

    last_release_command = std::move(command);

    return launch_async([res = cu_cp_pdu_session_resource_release_response{}](
                            coro_context<async_task<cu_cp_pdu_session_resource_release_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      res = generate_cu_cp_pdu_session_resource_release_response(uint_to_pdu_session_id(1));

      CORO_RETURN(res);
    });
  }

  async_task<cu_cp_ue_context_release_complete>
  on_new_ue_context_release_command(const cu_cp_ngap_ue_context_release_command& command) override
  {
    logger.info("Received a new UE Context Release Command");

    last_command.ue_index = command.ue_index;
    last_command.cause    = command.cause;

    cu_cp_ue_context_release_complete release_complete;
    release_complete.ue_index = command.ue_index;

    ngap_handler.remove_ue_context(command.ue_index);

    return launch_async([release_complete](coro_context<async_task<cu_cp_ue_context_release_complete>>& ctx) mutable {
      CORO_BEGIN(ctx);
      // TODO: Add values
      CORO_RETURN(release_complete);
    });
  }

  cu_cp_ue_context_release_command           last_command;
  cu_cp_pdu_session_resource_setup_request   last_request;
  cu_cp_pdu_session_resource_modify_request  last_modify_request;
  cu_cp_pdu_session_resource_release_command last_release_command;

  ue_index_t allocate_ue_index()
  {
    ue_index_t ue_index = ue_index_t::invalid;
    if (ue_id < ue_index_to_uint(ue_index_t::max)) {
      ue_index              = uint_to_ue_index(ue_id);
      last_created_ue_index = ue_index;
      ue_id++;
    }

    return ue_index;
  }

  optional<ue_index_t> last_created_ue_index;

private:
  srslog::basic_logger&            logger;
  uint64_t                         ue_id = ue_index_to_uint(srs_cu_cp::ue_index_t::min);
  ngap_ue_context_removal_handler& ngap_handler;
};

class dummy_ngap_cu_cp_paging_notifier : public ngap_cu_cp_du_repository_notifier
{
public:
  dummy_ngap_cu_cp_paging_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_paging_message(cu_cp_paging_message& msg) override
  {
    logger.info("Received a new Paging message");
    last_msg = std::move(msg);
  }

  ue_index_t request_new_ue_index_allocation(nr_cell_global_id_t /*cgi*/) override { return ue_index_t::invalid; }

  async_task<ngap_handover_resource_allocation_response>
  on_ngap_handover_request(const ngap_handover_request& request) override
  {
    return launch_async([res = ngap_handover_resource_allocation_response{}](
                            coro_context<async_task<ngap_handover_resource_allocation_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      CORO_RETURN(res);
    });
  }

  cu_cp_paging_message last_msg;

private:
  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
