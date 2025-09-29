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

#pragma once

#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "ngap_test_messages.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/ue_task_scheduler.h"
#include "srsran/ngap/gateways/n2_connection_client.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/security/security.h"
#include <gtest/gtest.h>
#include <optional>

namespace srsran {
namespace srs_cu_cp {

/// Reusable class that a) stores the messages sent to the AMF for test inspection and b)
/// calls the registered msg handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_n2_gateway : public n2_connection_client
{
public:
  dummy_n2_gateway() : logger(srslog::fetch_basic_logger("TEST")) {}

  void attach_handler(ngap_message_handler* handler_) { handler = handler_; }

  std::unique_ptr<ngap_message_notifier>
  handle_cu_cp_connection_request(std::unique_ptr<ngap_rx_message_notifier> cu_cp_rx_pdu_notifier) override
  {
    class dummy_ngap_message_notifier : public ngap_message_notifier
    {
    public:
      dummy_ngap_message_notifier(dummy_n2_gateway& parent_) : parent(parent_) {}
      ~dummy_ngap_message_notifier() { parent.rx_pdu_notifier.reset(); }

      [[nodiscard]] bool on_new_message(const ngap_message& msg) override
      {
        parent.logger.info("Received message");

        // Verify correct packing of outbound PDU.
        byte_buffer   pack_buffer;
        asn1::bit_ref bref(pack_buffer);
        if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
          parent.logger.error("Failed to pack message");
          return false;
        }

        parent.last_ngap_msgs.push_back(msg);

        if (parent.handler == nullptr) {
          return false;
        }
        parent.logger.info("Forwarding PDU");
        parent.handler->handle_message(msg);
        return true;
      }

    private:
      dummy_n2_gateway& parent;
    };

    rx_pdu_notifier = std::move(cu_cp_rx_pdu_notifier);

    return std::make_unique<dummy_ngap_message_notifier>(*this);
  }

  std::vector<ngap_message> last_ngap_msgs = {};

private:
  srslog::basic_logger& logger;
  ngap_message_handler* handler = nullptr;

  std::unique_ptr<ngap_rx_message_notifier> rx_pdu_notifier;
};

/// Dummy handler storing and printing the received PDU.
class dummy_ngap_message_notifier : public ngap_message_notifier
{
public:
  dummy_ngap_message_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}
  [[nodiscard]] bool on_new_message(const ngap_message& msg) override
  {
    last_msg = msg;
    logger.info("Transmitted a PDU of type {}", msg.pdu.type().to_string());
    return true;
  }
  ngap_message last_msg;

private:
  srslog::basic_logger& logger;
};

/// Dummy handler storing and printing the received PDU.
class dummy_ngap_message_handler : public ngap_message_handler
{
public:
  dummy_ngap_message_handler() : logger(srslog::fetch_basic_logger("TEST")) {}
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
class dummy_ngap_rrc_ue_notifier : public ngap_rrc_ue_notifier
{
public:
  dummy_ngap_rrc_ue_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  void on_new_pdu(byte_buffer nas_pdu) override
  {
    last_nas_pdu = std::move(nas_pdu);
    logger.info("Received a NAS PDU");
  }

  byte_buffer on_ue_radio_access_cap_info_required() override { return make_byte_buffer("deadbeef").value(); }

  byte_buffer on_handover_preparation_message_required() override { return ho_preparation_message.copy(); }

  void set_ho_preparation_message(byte_buffer ho_preparation_message_)
  {
    ho_preparation_message = std::move(ho_preparation_message_);
  }

  byte_buffer last_nas_pdu;
  byte_buffer ho_preparation_message;
  byte_buffer last_handover_command;

private:
  srslog::basic_logger& logger;
};

class dummy_ngap_cu_cp_notifier : public ngap_cu_cp_notifier
{
public:
  dummy_ngap_cu_cp_notifier(ue_manager& ue_mng_) : ue_mng(ue_mng_), logger(srslog::fetch_basic_logger("TEST")) {}

  void connect_ngap(ngap_ue_context_removal_handler& ngap_handler_) { ngap_handler = &ngap_handler_; }

  ngap_cu_cp_ue_notifier* on_new_ngap_ue(ue_index_t ue_index) override
  {
    last_ue = ue_index;

    auto* ue = ue_mng.find_ue(ue_index);
    if (ue == nullptr) {
      logger.error("ue={}: Failed to create UE", ue_index);
      return nullptr;
    }

    logger.info("ue={}: NGAP UE was created", ue_index);
    return &ue->get_ngap_cu_cp_ue_notifier();
  }

  bool schedule_async_task(ue_index_t ue_index, async_task<void> task) override
  {
    srsran_assert(ue_mng.find_ue_task_scheduler(ue_index) != nullptr, "UE task scheduler must be present");
    return ue_mng.find_ue_task_scheduler(ue_index)->schedule_async_task(std::move(task));
  }

  bool on_handover_request_received(ue_index_t                        ue_index,
                                    const plmn_identity&              selected_plmn,
                                    const security::security_context& sec_ctxt) override
  {
    srsran_assert(ue_mng.find_ue(ue_index) != nullptr, "UE must be present");
    logger.info("Received a handover request");

    if (!ue_mng.find_ue(ue_index)->get_security_manager().init_security_context(sec_ctxt)) {
      logger.info("Failed to initialize security context");
      return false;
    }

    return true;
  }

  async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>
  on_new_initial_context_setup_request(ngap_init_context_setup_request& request) override
  {
    logger.info("Received a new initial context setup request");

    last_init_ctxt_setup_request = std::move(request);

    return launch_async(
        [this, resp = ngap_init_context_setup_response{}, fail = ngap_init_context_setup_failure{}](
            coro_context<async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>>&
                ctx) mutable {
          CORO_BEGIN(ctx);

          if (!ue_mng.find_ue(last_init_ctxt_setup_request.ue_index)
                   ->get_security_manager()
                   .init_security_context(last_init_ctxt_setup_request.security_context)) {
            // Add failed PDU session setup responses
            if (last_init_ctxt_setup_request.pdu_session_res_setup_list_cxt_req.has_value()) {
              for (const auto& session : last_init_ctxt_setup_request.pdu_session_res_setup_list_cxt_req.value()
                                             .pdu_session_res_setup_items) {
                cu_cp_pdu_session_res_setup_failed_item failed_item;
                failed_item.pdu_session_id              = session.pdu_session_id;
                failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::unspecified;

                fail.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
              }
            }
            CORO_EARLY_RETURN(make_unexpected(fail));
          }

          // Add successful PDU session setup responses
          if (last_init_ctxt_setup_request.pdu_session_res_setup_list_cxt_req.has_value()) {
            for (const auto& session :
                 last_init_ctxt_setup_request.pdu_session_res_setup_list_cxt_req.value().pdu_session_res_setup_items) {
              cu_cp_pdu_session_res_setup_response_item response_item;
              response_item.pdu_session_id = session.pdu_session_id;
              response_item.pdu_session_resource_setup_response_transfer.dlqos_flow_per_tnl_info.up_tp_layer_info =
                  up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"),
                                          int_to_gtpu_teid(1)};
              response_item.pdu_session_resource_setup_response_transfer.dlqos_flow_per_tnl_info
                  .associated_qos_flow_list.emplace(uint_to_qos_flow_id(5),
                                                    cu_cp_associated_qos_flow{uint_to_qos_flow_id(5)});
              resp.pdu_session_res_setup_response_items.emplace(response_item.pdu_session_id, response_item);
            }
          }

          CORO_RETURN(resp);
        });
  }

  async_task<cu_cp_pdu_session_resource_setup_response>
  on_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) override
  {
    logger.info("Received a new pdu session resource setup request");

    last_request = std::move(request);

    return launch_async([this, res = cu_cp_pdu_session_resource_setup_response{}](
                            coro_context<async_task<cu_cp_pdu_session_resource_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (last_request.pdu_session_res_setup_items.empty()) {
        cu_cp_pdu_session_res_setup_failed_item failed_item;
        for (const auto& pdu_session : last_request.pdu_session_res_setup_items) {
          failed_item.pdu_session_id              = pdu_session.pdu_session_id;
          failed_item.unsuccessful_transfer.cause = ngap_cause_radio_network_t::unspecified;
          res.pdu_session_res_failed_to_setup_items.emplace(failed_item.pdu_session_id, failed_item);
        }
      } else {
        res = generate_cu_cp_pdu_session_resource_setup_response(last_request);
      }

      CORO_RETURN(res);
    });
  }

  async_task<cu_cp_pdu_session_resource_modify_response>
  on_new_pdu_session_resource_modify_request(cu_cp_pdu_session_resource_modify_request& request) override
  {
    logger.info("Received a new pdu session resource modify request");

    last_modify_request = std::move(request);

    return launch_async([this, res = cu_cp_pdu_session_resource_modify_response{}](
                            coro_context<async_task<cu_cp_pdu_session_resource_modify_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (!last_modify_request.pdu_session_res_modify_items.empty()) {
        res = generate_cu_cp_pdu_session_resource_modify_response(uint_to_pdu_session_id(1));
      }

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
  on_new_ue_context_release_command(const cu_cp_ue_context_release_command& command) override
  {
    srsran_assert(ngap_handler != nullptr, "ngap_handler must not be nullptr");

    logger.info("Received a new UE Context Release Command");

    last_command.ue_index = command.ue_index;
    last_command.cause    = command.cause;

    cu_cp_ue_context_release_complete release_complete;
    release_complete.ue_index = command.ue_index;

    ngap_handler->remove_ue_context(command.ue_index);

    return launch_async([release_complete](coro_context<async_task<cu_cp_ue_context_release_complete>>& ctx) mutable {
      CORO_BEGIN(ctx);
      // TODO: Add values
      CORO_RETURN(release_complete);
    });
  }

  void on_transmission_of_handover_required() override { logger.info("Received a new Handover Required"); }

  async_task<bool> on_new_handover_command(ue_index_t ue_index, byte_buffer command) override
  {
    logger.info("Received a new Handover Command");

    last_handover_command = std::move(command);

    return launch_async([](coro_context<async_task<bool>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  void on_n2_disconnection(amf_index_t amf_index) override {}

  cu_cp_ue_context_release_command last_command;
  byte_buffer                      last_handover_command;

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

  void on_paging_message(cu_cp_paging_message& msg) override
  {
    logger.info("Received a new Paging message");
    last_paging_msg = std::move(msg);
  }

  ue_index_t request_new_ue_index_allocation(nr_cell_global_id_t /*cgi*/, const plmn_identity& /*plmn*/) override
  {
    return ue_index_t::invalid;
  }

  async_task<ngap_handover_resource_allocation_response>
  on_ngap_handover_request(const ngap_handover_request& request) override
  {
    return launch_async([res = ngap_handover_resource_allocation_response{}](
                            coro_context<async_task<ngap_handover_resource_allocation_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      CORO_RETURN(res);
    });
  }

  void on_dl_ue_associated_nrppa_transport_pdu(ue_index_t ue_index, const byte_buffer& nrppa_pdu) override
  {
    logger.error("DL UE associated NRPPa transport failed. Cause: NRPPa transport PDUs not supported.");
  }

  void on_dl_non_ue_associated_nrppa_transport_pdu(amf_index_t amf_index, const byte_buffer& nrppa_pdu) override
  {
    logger.error("DL non UE associated NRPPa transport failed. Cause: NRPPa transport PDUs not supported.");
  }

  ue_index_t                                 last_ue = ue_index_t::invalid;
  ngap_init_context_setup_request            last_init_ctxt_setup_request;
  cu_cp_pdu_session_resource_setup_request   last_request;
  cu_cp_pdu_session_resource_modify_request  last_modify_request;
  cu_cp_pdu_session_resource_release_command last_release_command;
  std::optional<ue_index_t>                  last_created_ue_index;
  cu_cp_paging_message                       last_paging_msg;

private:
  ue_manager&           ue_mng;
  srslog::basic_logger& logger;

  ngap_ue_context_removal_handler* ngap_handler = nullptr;

  uint64_t ue_id = ue_index_to_uint(srs_cu_cp::ue_index_t::min);
};

class dummy_rrc_ngap_message_handler : public rrc_ngap_message_handler
{
public:
  dummy_rrc_ngap_message_handler(ue_index_t ue_index_) : ue_index(ue_index_), logger(srslog::fetch_basic_logger("TEST"))
  {
  }

  void handle_dl_nas_transport_message(byte_buffer nas_pdu) override
  {
    logger.info("ue={}: Received a DL NAS transport message", ue_index);
    last_nas_pdu = std::move(nas_pdu);
  }

  byte_buffer get_packed_ue_radio_access_cap_info() const override { return make_byte_buffer("deadbeef").value(); }

  void set_ho_preparation_message(byte_buffer ho_preparation_message_)
  {
    ho_preparation_message = std::move(ho_preparation_message_);
  }

  byte_buffer get_packed_handover_preparation_message() override { return ho_preparation_message.copy(); }

  byte_buffer last_nas_pdu;

private:
  ue_index_t            ue_index = ue_index_t::invalid;
  srslog::basic_logger& logger;
  byte_buffer           ho_preparation_message;
};

class dummy_ngap_cu_cp_ue_notifier : public ngap_cu_cp_ue_notifier
{
public:
  ~dummy_ngap_cu_cp_ue_notifier() = default;

  void set_ue_index(ue_index_t ue_index_) { ue_index = ue_index_; }
  void set_rrc_ue_notifier(dummy_ngap_rrc_ue_notifier& rrc_ue_notifier_) { rrc_ue_notifier = &rrc_ue_notifier_; }

  /// \brief Get the UE index of the UE.
  ue_index_t get_ue_index() override { return ue_index.value(); }

  /// \brief Schedule an async task for the UE.
  bool schedule_async_task(async_task<void> task) override { return true; }

  /// \brief Get the RRC UE notifier of the UE.
  ngap_rrc_ue_notifier& get_ngap_rrc_ue_notifier() override
  {
    srsran_assert(rrc_ue_notifier != nullptr, "RRC UE notifier must not be nullptr");
    return *rrc_ue_notifier;
  }

  /// \brief Notify the CU-CP about a security context
  /// \param[in] sec_ctxt The received security context
  /// \return True if the security context was successfully initialized, false otherwise
  bool init_security_context(const security::security_context& sec_ctxt) override { return true; }

  /// \brief Check if security is enabled
  [[nodiscard]] bool is_security_enabled() const override { return true; }

  std::optional<ue_index_t>   ue_index;
  dummy_ngap_rrc_ue_notifier* rrc_ue_notifier = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
