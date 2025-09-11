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

#include "f1ap_test_mode_adapter.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/f1ap/du/f1ap_du_factory.h"
#include "srsran/f1ap/f1ap_message.h"

using namespace srsran;
using namespace srs_du;

namespace {

class f1ap_test_mode_adapter final : public f1ap_du, public f1c_connection_client
{
public:
  f1ap_test_mode_adapter(const du_test_mode_config::test_mode_ue_config& test_ue_cfg_,
                         f1c_connection_client&                          f1c_client_handler_) :
    test_ue_cfg(test_ue_cfg_), f1c_client(f1c_client_handler_)
  {
  }

  void connect(std::unique_ptr<f1ap_du> adapted_) { adapted = std::move(adapted_); }

  // F1AP interface.
  void handle_message(const f1ap_message& msg) override { adapted->handle_message(msg); }
  void handle_rrc_delivery_report(const f1ap_rrc_delivery_report_msg& report) override
  {
    adapted->handle_rrc_delivery_report(report);
  }
  [[nodiscard]] bool          connect_to_cu_cp() override { return adapted->connect_to_cu_cp(); }
  async_task<f1_setup_result> handle_f1_setup_request(const f1_setup_request_message& request) override
  {
    return adapted->handle_f1_setup_request(request);
  }
  async_task<void> handle_f1_removal_request() override { return adapted->handle_f1_removal_request(); }
  async_task<f1_reset_acknowledgement> handle_f1_reset_request(const f1_reset_request& req) override
  {
    return adapted->handle_f1_reset_request(req);
  }
  async_task<gnbdu_config_update_response> handle_du_config_update(const gnbdu_config_update_request& request) override
  {
    return adapted->handle_du_config_update(request);
  }
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

  f1ap_metrics_collector& get_metrics_collector() override { return adapted->get_metrics_collector(); }

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

  const du_test_mode_config::test_mode_ue_config& test_ue_cfg;
  f1c_connection_client&                          f1c_client;
  srslog::basic_logger&                           logger = srslog::fetch_basic_logger("DU-F1");

  std::unordered_map<gnb_du_ue_f1ap_id_t, rnti_t> du_ue_to_rnti;

  std::unique_ptr<f1ap_du> adapted;

  std::unique_ptr<f1ap_message_notifier> tx_notifier;
};

/// Class that intercepts F1AP PDUs sent by the F1AP-DU to the F1-C GW.
class f1ap_test_mode_adapter::f1ap_to_gw_pdu_interceptor : public f1ap_message_notifier
{
public:
  f1ap_to_gw_pdu_interceptor(f1ap_test_mode_adapter& parent_) : parent(parent_) {}
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
        case f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request: {
          const asn1::f1ap::f1_setup_request_s& f1_setup = msg.pdu.init_msg().value.f1_setup_request();

          parent.logger.info("Test Mode: Intercepted F1AP F1 Setup Request");

          f1ap_message pdu_resp;
          pdu_resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
          asn1::f1ap::f1_setup_resp_s& f1_setup_resp = pdu_resp.pdu.successful_outcome().value.f1_setup_resp();

          f1_setup_resp->transaction_id = f1_setup->transaction_id;
          f1_setup_resp->gnb_cu_name.from_string("srsgnb01");
          if (f1_setup->gnb_du_served_cells_list_present) {
            f1_setup_resp->cells_to_be_activ_list_present = true;
            for (const auto& cell : f1_setup->gnb_du_served_cells_list) {
              asn1::f1ap::cells_to_be_activ_list_item_s cell_item;
              cell_item.nr_cgi         = cell->gnb_du_served_cells_item().served_cell_info.nr_cgi;
              cell_item.nr_pci_present = true;
              cell_item.nr_pci         = cell->gnb_du_served_cells_item().served_cell_info.nr_pci;
              f1_setup_resp->cells_to_be_activ_list.push_back({});
              f1_setup_resp->cells_to_be_activ_list.back().load_info_obj(ASN1_F1AP_ID_CELLS_TO_BE_ACTIV_LIST_ITEM);
              f1_setup_resp->cells_to_be_activ_list.back().value().cells_to_be_activ_list_item() = cell_item;
            }
          }
          f1_setup_resp->gnb_cu_rrc_version = f1_setup->gnb_du_rrc_version;

          parent.logger.info("Test Mode: Injected F1AP F1 Setup Response Message");
          parent.handle_message(pdu_resp);

        } break;
        case f1ap_elem_procs_o::init_msg_c::types_opts::init_ul_rrc_msg_transfer: {
          const asn1::f1ap::init_ul_rrc_msg_transfer_s& ie   = msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
          rnti_t                                        rnti = to_rnti(ie->c_rnti);

          f1ap_message pdu_resp;

          if (ie->du_to_cu_rrc_container_present) {
            // In case of test mode, save gNB-DU-UE-F1AP-ID of test mode UE.
            if (parent.is_test_mode_ue(rnti)) {
              gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(ie->gnb_du_ue_f1ap_id);
              parent.du_ue_to_rnti.insert(std::make_pair(du_ue_id, rnti));
            }

            pdu_resp.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER);
            asn1::f1ap::dl_rrc_msg_transfer_s& rrc_setup = pdu_resp.pdu.init_msg().value.dl_rrc_msg_transfer();
            rrc_setup->gnb_du_ue_f1ap_id                 = ie->gnb_du_ue_f1ap_id;
            rrc_setup->gnb_cu_ue_f1ap_id                 = next_cu_ue_id;
            rrc_setup->srb_id                            = 0;
            rrc_setup->rrc_container.from_string(
                "204004094ae00580088bd76380830f0003e0102341e0400020904c0ca8000ff800000000183708420001e01650020c00000081"
                "a6040514280038e2400040d55f21070004103081430727122858c1a3879022000010a00010016a00021910a00031916a000400"
                "90a00050096a00061890a00071896a00080210a0009032080280c8240b0320a0300c82c0d0320c0380c8340f0320e040588201"
                "103a0a4092e4a9286050e23a2b3c4de4d03a41078bbf03043800000071ffa5294a529e502c0000432ec0000000000000000001"
                "8ad5450047001800082000e21005c400e0202108001c4200b8401c080441000388401708038180842000710802e18070401104"
                "000e21005c300080000008218081018201c1a0001c71000000080100020180020240088029800008c40089c7001800");

            parent.logger.info("Test Mode: Injected F1AP DL RRC Message (containing rrcSetup) for rnti={}", rnti);
          } else {
            // In case the DU failed to allocate UE resources.
            parent.logger.warning("Test Mode: Injected F1AP UE Context Release (containing rrcReject) for rnti={}",
                                  rnti);

            pdu_resp.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
            asn1::f1ap::ue_context_release_cmd_s& ue_context_release =
                pdu_resp.pdu.init_msg().value.ue_context_release_cmd();

            ue_context_release->gnb_du_ue_f1ap_id = ie->gnb_du_ue_f1ap_id;
            ue_context_release->gnb_cu_ue_f1ap_id = next_cu_ue_id;
            ue_context_release->cause.set_radio_network();
            ue_context_release->cause.radio_network().value = cause_radio_network_opts::no_radio_res_available;
            ue_context_release->srb_id_present              = true;
            ue_context_release->srb_id                      = 0;
            ue_context_release->rrc_container_present       = true;
            ue_context_release->rrc_container.from_string("012345");
          }

          // Increment next_cu_ue_id for the next test mode UE.
          next_cu_ue_id++;

          parent.handle_message(pdu_resp);

        } break;
        case f1ap_elem_procs_o::init_msg_c::types_opts::f1_removal_request: {
          const asn1::f1ap::f1_removal_request_s& ie = msg.pdu.init_msg().value.f1_removal_request();
          f1ap_message                            pdu_resp;
          pdu_resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);
          asn1::f1ap::f1_removal_resp_s& f1_removal_resp = pdu_resp.pdu.successful_outcome().value.f1_removal_resp();
          f1_removal_resp->transaction_id                = ie->begin()->value().transaction_id();

          parent.logger.info("Test Mode: Injected F1AP F1 Removal Response Message");
          parent.handle_message(pdu_resp);

        } break;
        default:
          break;
      }
    }
  }

private:
  unsigned next_cu_ue_id = 0;

  f1ap_test_mode_adapter& parent;
};

class f1ap_test_mode_adapter::gw_to_f1ap_pdu_interceptor : public f1ap_message_notifier
{
public:
  gw_to_f1ap_pdu_interceptor(std::unique_ptr<f1ap_message_notifier> adapted_notif_) :
    adapted_notif(std::move(adapted_notif_))
  {
  }

  void on_new_message(const f1ap_message& msg) override
  {
    // Forward message to F1AP-DU.
    adapted_notif->on_new_message(msg);
  }

private:
  std::unique_ptr<f1ap_message_notifier> adapted_notif;
};

} // namespace

/// F1-C client interface.
std::unique_ptr<f1ap_message_notifier>
f1ap_test_mode_adapter::handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier)
{
  tx_notifier = f1c_client.handle_du_connection_request(
      std::make_unique<gw_to_f1ap_pdu_interceptor>(std::move(du_rx_pdu_notifier)));
  if (tx_notifier == nullptr) {
    return nullptr;
  }
  return std::make_unique<f1ap_to_gw_pdu_interceptor>(*this);
}

std::unique_ptr<f1ap_du> srsran::srs_du::create_du_high_f1ap(f1c_connection_client&      f1c_client_handler,
                                                             f1ap_du_configurator&       du_mng,
                                                             task_executor&              ctrl_exec,
                                                             du_high_ue_executor_mapper& ue_exec_mapper,
                                                             f1ap_du_paging_notifier&    paging_notifier,
                                                             timer_manager&              timers,
                                                             const du_test_mode_config&  test_cfg)
{
  if (not test_cfg.test_ue.has_value()) {
    return create_f1ap(f1c_client_handler, du_mng, ctrl_exec, ue_exec_mapper, paging_notifier, timers);
  }

  // Create a F1AP test mode adapter that wraps the real F1AP and intercepts messages to the F1-C client.
  auto f1ap_testmode = std::make_unique<f1ap_test_mode_adapter>(*test_cfg.test_ue, f1c_client_handler);
  f1ap_testmode->connect(create_f1ap(*f1ap_testmode, du_mng, ctrl_exec, ue_exec_mapper, paging_notifier, timers));
  return f1ap_testmode;
}
