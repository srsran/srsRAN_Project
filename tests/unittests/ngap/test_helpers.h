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

#include "lib/ngap/ngap_asn1_helpers.h"
#include "ngap_test_messages.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/ue_manager.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/gateways/network_gateway.h"
#include "srsran/ngap/ngap.h"
#include "srsran/pcap/pcap.h"
#include "srsran/support/async/async_task_loop.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct dummy_ngap_ue_task_scheduler : public ngap_ue_task_scheduler {
public:
  dummy_ngap_ue_task_scheduler(timer_manager2& timers_, task_executor& exec_) : timer_db(timers_), exec(exec_) {}
  void schedule_async_task(ue_index_t ue_index, async_task<void>&& task) override
  {
    ctrl_loop.schedule(std::move(task));
  }
  unique_timer2   make_unique_timer() override { return timer_db.create_unique_timer(exec); }
  timer_manager2& get_timer_manager() override { return timer_db; }

  void tick_timer() { timer_db.tick(); }

private:
  async_task_sequencer ctrl_loop{16};
  timer_manager2&      timer_db;
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

  byte_buffer last_nas_pdu;

private:
  srslog::basic_logger& logger;
};

/// Dummy NGAP to DU processor notifier
class dummy_ngap_du_processor_notifier : public ngap_du_processor_control_notifier
{
public:
  dummy_ngap_du_processor_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  async_task<cu_cp_pdu_session_resource_setup_response>
  on_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) override
  {
    logger.info("Received a new pdu session resource setup request");

    last_request = std::move(request);

    return launch_async([this, res = cu_cp_pdu_session_resource_setup_response{}](
                            coro_context<async_task<cu_cp_pdu_session_resource_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (last_request.pdu_session_res_setup_items.size() == 0) {
        res.pdu_session_res_failed_to_setup_items.emplace(uint_to_pdu_session_id(1),
                                                          cu_cp_pdu_session_res_setup_failed_item{});
      } else {
        res = generate_cu_cp_pdu_session_resource_setup_response(uint_to_pdu_session_id(1));
      }

      CORO_RETURN(res);
    });
  }

  void on_new_ue_context_release_command(cu_cp_ue_context_release_command& command) override
  {
    logger.info("Received a new UE Context Release Command");

    last_command = command;
  }

  cu_cp_ue_context_release_command         last_command;
  cu_cp_pdu_session_resource_setup_request last_request;

private:
  srslog::basic_logger& logger;
};

class dummy_ngap_cu_cp_paging_notifier : public ngap_cu_cp_paging_notifier
{
public:
  dummy_ngap_cu_cp_paging_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_paging_message(cu_cp_paging_message& msg) override
  {
    logger.info("Received a new Paging message");
    last_msg = std::move(msg);
  }

  cu_cp_paging_message last_msg;

private:
  srslog::basic_logger& logger;
};

class dummy_ngap_pcap : public ngap_pcap
{
public:
  void open(const char* filename_) override {}
  void close() override {}
  bool is_write_enabled() override { return false; }
  void push_pdu(const_span<uint8_t> pdu) override {}
  void push_pdu(byte_buffer pdu) override {}
};

} // namespace srs_cu_cp
} // namespace srsran
