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

#include "f1ap_test_mode_adapter.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/du/f1ap_du_factory.h"

using namespace srsran;
using namespace srs_du;

namespace {

class f1ap_test_mode_adapter final : public f1ap_du, public f1c_connection_client
{
public:
  f1ap_test_mode_adapter(const du_test_config::test_ue_config& test_ue_cfg_,
                         f1c_connection_client&                f1c_client_handler_,
                         task_executor&                        ctrl_exec_) :
    test_ue_cfg(test_ue_cfg_), f1c_client(f1c_client_handler_), ctrl_exec(ctrl_exec_)
  {
  }

  void connect(std::unique_ptr<f1ap_du> adapted_) { adapted = std::move(adapted_); }

  // F1AP interface.
  void handle_message(const f1ap_message& msg) override { adapted->handle_message(msg); }
  void handle_rrc_delivery_report(const f1ap_rrc_delivery_report_msg& report) override
  {
    adapted->handle_rrc_delivery_report(report);
  }
  SRSRAN_NODISCARD bool                 connect_to_cu_cp() override { return adapted->connect_to_cu_cp(); }
  async_task<f1_setup_response_message> handle_f1_setup_request(const f1_setup_request_message& request) override
  {
    return adapted->handle_f1_setup_request(request);
  }
  async_task<void>          handle_f1_removal_request() override { return adapted->handle_f1_removal_request(); }
  f1ap_ue_creation_response handle_ue_creation_request(const f1ap_ue_creation_request& msg) override
  {
    return adapted->handle_ue_creation_request(msg);
  }
  f1ap_ue_configuration_response handle_ue_configuration_request(const f1ap_ue_configuration_request& msg) override
  {
    return adapted->handle_ue_configuration_request(msg);
  }
  void handle_ue_deletion_request(du_ue_index_t ue_index) override
  {
    return adapted->handle_ue_deletion_request(ue_index);
  }
  void handle_ue_context_release_request(const f1ap_ue_context_release_request& request) override
  {
    return adapted->handle_ue_context_release_request(request);
  }
  async_task<f1ap_ue_context_modification_confirm>
  handle_ue_context_modification_required(const f1ap_ue_context_modification_required& msg) override
  {
    return adapted->handle_ue_context_modification_required(msg);
  }
  void handle_ue_inactivity_notification(const f1ap_ue_inactivity_notification_message& msg) override
  {
    return adapted->handle_ue_inactivity_notification(msg);
  }
  void                handle_notify(const f1ap_notify_message& msg) override { return adapted->handle_notify(msg); }
  gnb_cu_ue_f1ap_id_t get_gnb_cu_ue_f1ap_id(const du_ue_index_t& ue_index) override
  {
    return adapted->get_gnb_cu_ue_f1ap_id(ue_index);
  }
  gnb_cu_ue_f1ap_id_t get_gnb_cu_ue_f1ap_id(const gnb_du_ue_f1ap_id_t& gnb_du_ue_f1ap_id) override
  {
    return adapted->get_gnb_cu_ue_f1ap_id(gnb_du_ue_f1ap_id);
  }
  gnb_du_ue_f1ap_id_t get_gnb_du_ue_f1ap_id(const du_ue_index_t& ue_index) override
  {
    return adapted->get_gnb_du_ue_f1ap_id(ue_index);
  }
  gnb_du_ue_f1ap_id_t get_gnb_du_ue_f1ap_id(const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id) override
  {
    return adapted->get_gnb_du_ue_f1ap_id(gnb_cu_ue_f1ap_id);
  }
  du_ue_index_t get_ue_index(const gnb_du_ue_f1ap_id_t& gnb_du_ue_f1ap_id) override
  {
    return adapted->get_ue_index(gnb_du_ue_f1ap_id);
  }
  du_ue_index_t get_ue_index(const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id) override
  {
    return adapted->get_ue_index(gnb_cu_ue_f1ap_id);
  }

  // F1-C client interface.
  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

private:
  class f1ap_to_gw_pdu_interceptor;
  class gw_to_f1ap_pdu_interceptor;

  bool is_test_mode_ue(rnti_t rnti) const
  {
    if (rnti >= test_ue_cfg.rnti and rnti < to_rnti((unsigned)test_ue_cfg.rnti + test_ue_cfg.nof_ues)) {
      return true;
    }
    return false;
  }

  void handle_cu_cp_message(const f1ap_message& msg)
  {
    using namespace asn1::f1ap;

    if (msg.pdu.type().value == f1ap_pdu_c::types_opts::init_msg) {
      if (msg.pdu.init_msg().value.type().value == f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer) {
        auto&               ie       = msg.pdu.init_msg().value.dl_rrc_msg_transfer();
        gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(ie->gnb_du_ue_f1ap_id);
        auto                it       = du_ue_to_rnti.find(du_ue_id);
        if (it != du_ue_to_rnti.end()) {
          // It is a test mode UE.
          if (ie->srb_id == 0) {
            // An RRC Setup was sent to the DU. We should generate and send the RRC Setup Complete back to the CU-CP
            // to complete the RRC Setup procedure.
            f1ap_message pdu_resp;
            pdu_resp.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER);
            auto& resp_ie              = pdu_resp.pdu.init_msg().value.ul_rrc_msg_transfer();
            resp_ie->srb_id            = 1;
            resp_ie->gnb_du_ue_f1ap_id = ie->gnb_du_ue_f1ap_id;
            resp_ie->gnb_cu_ue_f1ap_id = ie->gnb_cu_ue_f1ap_id;
            resp_ie->rrc_container.from_string("0000100005df80105e400340403c443c3fc000040c951da60b80b8380000000000");
            logger.info("Test Mode: Injected F1AP UL RRC Message (containing rrcSetupComplete)");
            tx_notifier->on_new_message(pdu_resp);
          }
        }
      }
    }
  }

  const du_test_config::test_ue_config& test_ue_cfg;
  f1c_connection_client&                f1c_client;
  task_executor&                        ctrl_exec;
  srslog::basic_logger&                 logger = srslog::fetch_basic_logger("DU-F1");

  std::unordered_map<gnb_du_ue_f1ap_id_t, rnti_t> du_ue_to_rnti;

  std::unique_ptr<f1ap_du> adapted;

  std::unique_ptr<f1ap_message_notifier> tx_notifier;
};

/// Class that intercepts F1AP PDUs sent by the F1AP-DU to the F1-C GW.
class f1ap_test_mode_adapter::f1ap_to_gw_pdu_interceptor : public f1ap_message_notifier
{
public:
  f1ap_to_gw_pdu_interceptor(f1ap_test_mode_adapter& parent_) : parent(parent_), adapted_notif(*parent.tx_notifier) {}
  ~f1ap_to_gw_pdu_interceptor() override
  {
    // Called by F1AP-DU thread when the DU wants to release the connection.
    parent.tx_notifier.reset();
  }

  void on_new_message(const f1ap_message& msg) override
  {
    using namespace asn1::f1ap;

    if (msg.pdu.type().value == f1ap_pdu_c::types_opts::init_msg) {
      switch (msg.pdu.init_msg().value.type().value) {
        case f1ap_elem_procs_o::init_msg_c::types_opts::init_ul_rrc_msg_transfer: {
          auto& ie = msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
          // In case of test mode, save gNB-DU-UE-F1AP-ID of test mode UE.
          rnti_t rnti = to_rnti(ie->c_rnti);
          if (parent.is_test_mode_ue(rnti)) {
            gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(ie->gnb_du_ue_f1ap_id);
            parent.du_ue_to_rnti.insert(std::make_pair(du_ue_id, rnti));
          }
        } break;
        default:
          break;
      }
    }

    // Forward message to adapted notifier.
    adapted_notif.on_new_message(msg);
  }

private:
  f1ap_test_mode_adapter& parent;
  f1ap_message_notifier&  adapted_notif;
};

class f1ap_test_mode_adapter::gw_to_f1ap_pdu_interceptor : public f1ap_message_notifier
{
public:
  gw_to_f1ap_pdu_interceptor(f1ap_test_mode_adapter& parent_, std::unique_ptr<f1ap_message_notifier> adapted_notif_) :
    parent(parent_), adapted_notif(std::move(adapted_notif_))
  {
  }

  void on_new_message(const f1ap_message& msg) override
  {
    // Forward message to F1AP-DU.
    adapted_notif->on_new_message(msg);

    // Handle intercepted message from F1AP-CU.
    if (not parent.ctrl_exec.defer([this, msg]() { parent.handle_cu_cp_message(msg); })) {
      parent.logger.error("Failed to handle DL F1AP PDU");
    }
  }

private:
  f1ap_test_mode_adapter&                parent;
  std::unique_ptr<f1ap_message_notifier> adapted_notif;
};

// F1-C client interface.
std::unique_ptr<f1ap_message_notifier>
f1ap_test_mode_adapter::handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier)
{
  tx_notifier = f1c_client.handle_du_connection_request(
      std::make_unique<gw_to_f1ap_pdu_interceptor>(*this, std::move(du_rx_pdu_notifier)));
  return std::make_unique<f1ap_to_gw_pdu_interceptor>(*this);
}

} // namespace

std::unique_ptr<f1ap_du> srsran::srs_du::create_du_high_f1ap(f1c_connection_client&      f1c_client_handler,
                                                             f1ap_du_configurator&       du_mng,
                                                             task_executor&              ctrl_exec,
                                                             du_high_ue_executor_mapper& ue_exec_mapper,
                                                             f1ap_du_paging_notifier&    paging_notifier,
                                                             const du_test_config&       test_cfg)
{
  if (not test_cfg.test_ue.has_value()) {
    return create_f1ap(f1c_client_handler, du_mng, ctrl_exec, ue_exec_mapper, paging_notifier);
  }

  // Create a F1AP test mode adapter that wraps the real F1AP and intercepts messages to the F1-C client.
  auto f1ap_testmode = std::make_unique<f1ap_test_mode_adapter>(*test_cfg.test_ue, f1c_client_handler, ctrl_exec);
  f1ap_testmode->connect(create_f1ap(*f1ap_testmode, du_mng, ctrl_exec, ue_exec_mapper, paging_notifier));
  return f1ap_testmode;
}
