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

#include "procedures/rrc_reconfiguration_procedure.h"
#include "procedures/rrc_security_mode_command_procedure.h"
#include "procedures/rrc_setup_procedure.h"
#include "procedures/rrc_ue_capability_transfer_procedure.h"
#include "procedures/rrc_ue_event_manager.h"
#include "rrc_ue_context.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg.h"
#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/rrc/rrc_ue.h"

namespace srsran {
namespace srs_cu_cp {

/// Main UE representation in RRC
class rrc_ue_impl final : public rrc_ue_interface
{
public:
  rrc_ue_impl(up_resource_manager&             up_resource_mng_,
              rrc_ue_du_processor_notifier&    du_proc_notif_,
              rrc_pdu_f1ap_notifier&           f1ap_pdu_notifier_,
              rrc_ue_nas_notifier&             nas_notif_,
              rrc_ue_control_notifier&         ngap_ctrl_notif_,
              rrc_ue_reestablishment_notifier& cu_cp_notif_,
              cell_meas_manager&               cell_meas_mng_,
              const ue_index_t                 ue_index_,
              const rnti_t                     c_rnti_,
              const rrc_cell_context           cell_,
              const rrc_ue_cfg_t&              cfg_,
              const byte_buffer                du_to_cu_container,
              rrc_ue_task_scheduler&           task_sched,
              bool&                            reject_users_,
              bool                             is_inter_cu_handover_ = false);
  ~rrc_ue_impl() = default;

  // rrc_ul_ccch_pdu_handler
  void handle_ul_ccch_pdu(byte_buffer pdu) override;
  // rrc_ul_dcch_pdu_handler
  void handle_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdcp_pdu) override;

  // rrc_ue_interface
  rrc_ul_ccch_pdu_handler&              get_ul_ccch_pdu_handler() override { return *this; }
  rrc_ul_dcch_pdu_handler&              get_ul_dcch_pdu_handler() override { return *this; }
  rrc_dl_nas_message_handler&           get_rrc_dl_nas_message_handler() override { return *this; }
  rrc_ue_srb_handler&                   get_rrc_ue_srb_handler() override { return *this; }
  rrc_ue_control_message_handler&       get_rrc_ue_control_message_handler() override { return *this; }
  rrc_ue_init_security_context_handler& get_rrc_ue_init_security_context_handler() override { return *this; }
  security::security_context&           get_rrc_ue_security_context() override { return context.sec_context; }
  rrc_ue_context_handler&               get_rrc_ue_context_handler() override { return *this; }
  rrc_ue_handover_preparation_handler&  get_rrc_ue_handover_preparation_handler() override { return *this; }

  // rrc_ue_srb_handler
  void                                  create_srb(const srb_creation_message& msg) override;
  static_vector<srb_id_t, MAX_NOF_SRBS> get_srbs() override;

  // rrc_dl_nas_message_handler
  void handle_dl_nas_transport_message(byte_buffer nas_pdu) override;

  // rrc_ue_control_message_handler
  async_task<bool> handle_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override;
  uint8_t          handle_handover_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override;
  async_task<bool> handle_handover_reconfiguration_complete_expected(uint8_t transaction_id) override;
  async_task<bool> handle_rrc_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) override;
  rrc_ue_release_context get_rrc_ue_release_context() override;
  optional<rrc_meas_cfg> get_rrc_ue_meas_config() override;
  bool                   handle_new_security_context(const security::security_context& sec_context) override;
  byte_buffer            get_rrc_handover_command(const rrc_reconfiguration_procedure_request& request,
                                                  unsigned                                     transaction_id) override;

  // rrc_ue_handover_preparation_handler
  byte_buffer get_packed_handover_preparation_message() override;
  bool        handle_rrc_handover_command(byte_buffer cmd) override;

  // rrc_ue_context_handler
  rrc_reestablishment_ue_context_t get_context() override;

private:
  // message handlers
  void handle_pdu(const srb_id_t srb_id, byte_buffer rrc_pdu);
  void handle_rrc_setup_request(const asn1::rrc_nr::rrc_setup_request_s& msg);
  void handle_rrc_reest_request(const asn1::rrc_nr::rrc_reest_request_s& msg);
  void handle_ul_info_transfer(const asn1::rrc_nr::ul_info_transfer_ies_s& ul_info_transfer);
  void handle_rrc_transaction_complete(const asn1::rrc_nr::ul_dcch_msg_s& msg, uint8_t transaction_id_);
  void handle_measurement_report(const asn1::rrc_nr::meas_report_s& msg);

  // message senders
  /// \remark Send RRC Reject, see section 5.3.15 in TS 38.331
  void send_rrc_reject(uint8_t reject_wait_time_secs);

  /// Packs a DL-CCCH message and logs the message
  void send_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg);
  void send_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg);

  // rrc_ue_setup_proc_notifier
  void on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg) override;
  void on_ue_release_required(const cause_t& cause) override;

  // rrc_ue_security_mode_command_proc_notifier
  void on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_ccch_msg) override;
  void on_new_as_security_context() override;
  bool get_security_enabled() override { return context.security_enabled; }

  // initializes the security context and triggers the SMC procedure
  async_task<bool> handle_init_security_context(const security::security_context& sec_ctx) override;

  rrc_ue_context_t                 context;
  up_resource_manager&             up_resource_mng;       // UP resource manager
  rrc_ue_du_processor_notifier&    du_processor_notifier; // notifier to the DU processor
  rrc_pdu_f1ap_notifier&           f1ap_pdu_notifier;     // PDU notifier to the F1AP
  rrc_ue_nas_notifier&             nas_notifier;          // PDU notifier to the NGAP
  rrc_ue_control_notifier&         ngap_ctrl_notifier;    // Control message notifier to the NGAP
  rrc_ue_reestablishment_notifier& cu_cp_notifier;        // notifier to the CU-CP
  cell_meas_manager&               cell_meas_mng;         // cell measurement manager
  byte_buffer                      du_to_cu_container;    // initial RRC message from DU to CU
  rrc_ue_task_scheduler&           task_sched;
  bool&                            reject_users;
  srslog::basic_logger&            logger;

  // RRC procedures handling
  std::unique_ptr<rrc_ue_event_manager> event_mng;

  /// RRC timer constants
  const unsigned rrc_reject_max_wait_time_s = 16;
};

} // namespace srs_cu_cp
} // namespace srsran
