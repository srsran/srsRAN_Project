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

#include "ngap_test_messages.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/ue_manager.h"
#include "srsran/pcap/pcap.h"
#include "srsran/support/async/async_task_loop.h"
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
  async_task_sequencer ctrl_loop{16};
  timer_manager&       timer_db;
  task_executor&       exec;
};

struct dummy_ngap_ue : public ngap_ue {
public:
  dummy_ngap_ue(ue_index_t ue_index_) : ue_index(ue_index_) {}

  ue_index_t get_ue_index() override { return ue_index; }

  void set_aggregate_maximum_bit_rate_dl(uint64_t aggregate_maximum_bit_rate_dl_) override
  {
    aggregate_maximum_bit_rate_dl = aggregate_maximum_bit_rate_dl_;
  }

  ngap_rrc_ue_pdu_notifier&           get_rrc_ue_pdu_notifier() override { return *rrc_ue_pdu_notifier; }
  ngap_rrc_ue_control_notifier&       get_rrc_ue_control_notifier() override { return *rrc_ue_ctrl_notifier; }
  ngap_du_processor_control_notifier& get_du_processor_control_notifier() override
  {
    return *du_processor_ctrl_notifier;
  }

  amf_ue_id_t get_amf_ue_id() override { return amf_ue_id; }
  ran_ue_id_t get_ran_ue_id() override { return ran_ue_id; }

  uint64_t get_aggregate_maximum_bit_rate_dl() override { return aggregate_maximum_bit_rate_dl; }

  bool du_ue_created   = false;
  bool ngap_ue_created = false;

  void set_ran_ue_id(ran_ue_id_t ran_ue_id_) { ran_ue_id = ran_ue_id_; }

  void set_rrc_ue_pdu_notifier(ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier_)
  {
    rrc_ue_pdu_notifier = &rrc_ue_pdu_notifier_;
  }

  void set_rrc_ue_ctrl_notifier(ngap_rrc_ue_control_notifier& rrc_ue_ctrl_notifier_)
  {
    rrc_ue_ctrl_notifier = &rrc_ue_ctrl_notifier_;
  }

  void set_du_processor_ctrl_notifier(ngap_du_processor_control_notifier& du_processor_ctrl_notifier_)
  {
    du_processor_ctrl_notifier = &du_processor_ctrl_notifier_;
  }

  /// \brief Set the AMF UE ID in the UE.
  /// \param[in] amf_ue_id The AMF UE ID to set.
  void set_amf_ue_id(amf_ue_id_t amf_ue_id_) { amf_ue_id = amf_ue_id_; }

private:
  ue_index_t ue_index = ue_index_t::invalid;

  amf_ue_id_t amf_ue_id                     = amf_ue_id_t::invalid;
  ran_ue_id_t ran_ue_id                     = ran_ue_id_t::invalid;
  uint64_t    aggregate_maximum_bit_rate_dl = 0;

  ngap_rrc_ue_pdu_notifier*           rrc_ue_pdu_notifier        = nullptr;
  ngap_rrc_ue_control_notifier*       rrc_ue_ctrl_notifier       = nullptr;
  ngap_du_processor_control_notifier* du_processor_ctrl_notifier = nullptr;
};

struct dummy_ngap_ue_manager : public ngap_ue_manager {
public:
  void             set_ue_config(ue_configuration ue_config_) { ue_config = ue_config_; }
  ue_configuration get_ue_config() override { return ue_config; }

  ue_index_t get_ue_index(pci_t pci, rnti_t c_rnti) override { return ue_index_t::invalid; }

  ngap_ue* add_ue(ue_index_t                          ue_index,
                  ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
                  ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
                  ngap_du_processor_control_notifier& du_processor_ctrl_notifier_) override
  {
    ran_ue_id_t ran_ue_id = get_next_ran_ue_id();
    if (ran_ue_id == ran_ue_id_t::invalid) {
      logger.error("No free RAN UE ID available");
      return nullptr;
    }

    // Create UE object
    ues.emplace(ue_index, ue_index);
    auto& ue = ues.at(ue_index);

    ue.set_ran_ue_id(ran_ue_id);
    ue.set_rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_);
    ue.set_rrc_ue_ctrl_notifier(rrc_ue_ctrl_notifier_);
    ue.set_du_processor_ctrl_notifier(du_processor_ctrl_notifier_);

    // Add RAN UE ID to lookup
    ran_ue_id_to_ue_index.emplace(ran_ue_id, ue_index);

    return &ue;
  }

  void remove_ngap_ue(ue_index_t ue_index) override
  {
    // Remove UE from lookups
    ran_ue_id_t ran_ue_id = ues.at(ue_index).get_ran_ue_id();
    if (ran_ue_id != ran_ue_id_t::invalid) {
      ran_ue_id_to_ue_index.erase(ran_ue_id);
    }

    amf_ue_id_t amf_ue_id = ues.at(ue_index).get_amf_ue_id();
    if (amf_ue_id != amf_ue_id_t::invalid) {
      amf_ue_id_to_ue_index.erase(amf_ue_id);
    }

    ues.erase(ue_index);
  }

  ngap_ue* find_ngap_ue(ue_index_t ue_index) override
  {
    if (ues.find(ue_index) != ues.end()) {
      return &ues.at(ue_index);
    }
    return nullptr;
  }

  size_t get_nof_ngap_ues() override { return ues.size(); }

  ue_index_t get_ue_index(ran_ue_id_t ran_ue_id) override
  {
    if (ran_ue_id_to_ue_index.find(ran_ue_id) == ran_ue_id_to_ue_index.end()) {
      logger.info("UE with ran_ue_id_t={} does not exist. Dropping PDU", ran_ue_id);
      return ue_index_t::invalid;
    }
    return ran_ue_id_to_ue_index[ran_ue_id];
  }

  ue_index_t get_ue_index(amf_ue_id_t amf_ue_id) override
  {
    if (amf_ue_id_to_ue_index.find(amf_ue_id) == amf_ue_id_to_ue_index.end()) {
      logger.info("UE with amf_ue_id_t={} does not exist. Dropping PDU", amf_ue_id);
      return ue_index_t::invalid;
    }
    return amf_ue_id_to_ue_index[amf_ue_id];
  }

  void set_amf_ue_id(ue_index_t ue_index, amf_ue_id_t amf_ue_id) override
  {
    if (ue_index == ue_index_t::invalid) {
      logger.error("Invalid ue_index={}", ue_index);
      return;
    }

    ues.at(ue_index).set_amf_ue_id(amf_ue_id);
    // Add AMF UE ID to lookup
    amf_ue_id_to_ue_index.emplace(amf_ue_id, ue_index);
  }

  void transfer_ngap_ue_context(ue_index_t new_ue_index, ue_index_t old_ue_index) override
  {
    // Update ue index at lookups
    ran_ue_id_to_ue_index.at(find_ran_ue_id(old_ue_index)) = new_ue_index;
    amf_ue_id_to_ue_index.at(find_amf_ue_id(old_ue_index)) = new_ue_index;

    // transfer UE NGAP IDs to new UE
    auto& old_ue = ues.at(old_ue_index);
    auto& new_ue = ues.at(new_ue_index);
    new_ue.set_ran_ue_id(old_ue.get_ran_ue_id());
    new_ue.set_amf_ue_id(old_ue.get_amf_ue_id());

    // transfer aggregate maximum bit rate dl
    new_ue.set_aggregate_maximum_bit_rate_dl(old_ue.get_aggregate_maximum_bit_rate_dl());

    logger.debug(
        "Transferred NGAP UE context from ueId={} (ran_ue_id={} amf_ue_id={}) to ueId={} (ran_ue_id={} amf_ue_id={})",
        old_ue_index,
        old_ue.get_ran_ue_id(),
        old_ue.get_amf_ue_id(),
        new_ue_index,
        new_ue.get_ran_ue_id(),
        new_ue.get_amf_ue_id());

    // Remove old ue
    ues.erase(old_ue_index);
  }

private:
  ran_ue_id_t get_next_ran_ue_id()
  {
    // Search unallocated UE index
    for (uint64_t i = 0; i < MAX_NOF_RAN_UES; i++) {
      ran_ue_id_t next_ran_ue_id = uint_to_ran_ue_id(i);
      if (ran_ue_id_to_ue_index.find(next_ran_ue_id) == ran_ue_id_to_ue_index.end()) {
        return next_ran_ue_id;
      }
    }

    logger.error("No RAN UE ID available");
    return ran_ue_id_t::invalid;
  }

  ran_ue_id_t find_ran_ue_id(ue_index_t ue_index)
  {
    unsigned ran_ue_id_uint = ran_ue_id_to_uint(ran_ue_id_t::min);
    for (auto const& it : ran_ue_id_to_ue_index) {
      if (it.second == ue_index) {
        return uint_to_ran_ue_id(ran_ue_id_uint);
      }
      ran_ue_id_uint++;
    }
    logger.error("RAN UE ID for ue_index={} not found", ue_index);
    return ran_ue_id_t::invalid;
  }

  amf_ue_id_t find_amf_ue_id(ue_index_t ue_index)
  {
    unsigned amf_ue_id_uint = amf_ue_id_to_uint(amf_ue_id_t::min);
    for (auto const& it : amf_ue_id_to_ue_index) {
      if (it.second == ue_index) {
        return uint_to_amf_ue_id(amf_ue_id_uint);
      }
      amf_ue_id_uint++;
    }
    logger.error("AMF UE ID for ue_index={} not found", ue_index);
    return amf_ue_id_t::invalid;
  }

  ue_configuration ue_config;

  std::unordered_map<ue_index_t, dummy_ngap_ue> ues;                   // ues indexed by ue_index
  std::unordered_map<ran_ue_id_t, ue_index_t>   ran_ue_id_to_ue_index; // ue_indexes indexed by ran_ue_ids
  std::unordered_map<amf_ue_id_t, ue_index_t>   amf_ue_id_to_ue_index; // ue_indexes indexed by amf_ue_ids

  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

/// Reusable notifier class that a) stores the received msg for test inspection and b)
/// calls the registered msg handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_ngap_amf_notifier : public ngap_message_notifier
{
public:
  dummy_ngap_amf_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void attach_handler(ngap_message_handler* handler_) { handler = handler_; };

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
    last_ngap_msg = msg;

    if (handler != nullptr) {
      logger.info("Forwarding PDU");
      handler->handle_message(msg);
    }
  };

  ngap_message last_ngap_msg;

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

  async_task<bool> on_new_security_context(const asn1::ngap::ue_security_cap_s&           caps,
                                           const asn1::fixed_bitstring<256, false, true>& key) override
  {
    logger.info("Received a new security context");

    bool result = true;

    // NIA0 is not allowed
    if (caps.nr_integrity_protection_algorithms.to_number() == 0) {
      result = false;
    }

    return launch_async([result](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(result);
    });
  }

  void set_handover_context(ngap_ue_source_handover_context ho_context_) { ho_context = std::move(ho_context_); }
  ngap_ue_source_handover_context on_ue_source_handover_context_required() override { return ho_context; }

  byte_buffer                     last_nas_pdu;
  ngap_ue_source_handover_context ho_context;

private:
  srslog::basic_logger& logger;
};

/// Dummy NGAP to DU processor notifier
class dummy_ngap_du_processor_notifier : public ngap_du_processor_control_notifier
{
public:
  dummy_ngap_du_processor_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

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
        failed_item.pdu_session_id                                         = uint_to_pdu_session_id(1);
        failed_item.pdu_session_resource_setup_unsuccessful_transfer.cause = cause_t::radio_network;
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

  cu_cp_ue_context_release_complete
  on_new_ue_context_release_command(const cu_cp_ngap_ue_context_release_command& command) override
  {
    logger.info("Received a new UE Context Release Command");

    last_command.ue_index = command.ue_index;
    last_command.cause    = command.cause;

    cu_cp_ue_context_release_complete release_complete;
    // TODO: Add values
    return release_complete;
  }

  rrc_ue_context_release_command             last_command;
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
  srslog::basic_logger& logger;
  uint64_t              ue_id = ue_index_to_uint(srs_cu_cp::ue_index_t::min);
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

class dummy_ngap_pcap : public dlt_pcap
{
public:
  void open(const std::string& filename_) override {}
  void close() override {}
  bool is_write_enabled() override { return false; }
  void push_pdu(const_span<uint8_t> pdu) override {}
  void push_pdu(byte_buffer pdu) override {}
};

} // namespace srs_cu_cp
} // namespace srsran
