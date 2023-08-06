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
              rrc_ue_nas_notifier&             nas_notif_,
              rrc_ue_control_notifier&         ngap_ctrl_notif_,
              rrc_ue_reestablishment_notifier& cu_cp_notif_,
              cell_meas_manager&               cell_meas_mng_,
              const ue_index_t                 ue_index_,
              const rnti_t                     c_rnti_,
              const rrc_cell_context           cell_,
              const rrc_ue_cfg_t&              cfg_,
              const srb_notifiers_array&       srbs_,
              const byte_buffer                du_to_cu_container,
              rrc_ue_task_scheduler&           task_sched,
              bool&                            reject_users_);
  ~rrc_ue_impl() = default;

  // rrc_ul_ccch_pdu_handler
  void handle_ul_ccch_pdu(byte_buffer_slice pdu) override;
  void handle_ul_dcch_pdu(byte_buffer_slice pdu) override;

  // rrc_ue_interface
  rrc_ul_ccch_pdu_handler&              get_ul_ccch_pdu_handler() override { return *this; }
  rrc_ul_dcch_pdu_handler&              get_ul_dcch_pdu_handler() override { return *this; }
  rrc_dl_nas_message_handler&           get_rrc_dl_nas_message_handler() override { return *this; }
  rrc_ue_control_message_handler&       get_rrc_ue_control_message_handler() override { return *this; }
  rrc_ue_init_security_context_handler& get_rrc_ue_init_security_context_handler() override { return *this; }
  security::security_context&           get_rrc_ue_security_context() override { return context.sec_context; }
  rrc_ue_context_handler&               get_rrc_ue_context_handler() override { return *this; }
  rrc_ue_handover_preparation_handler&  get_rrc_ue_handover_preparation_handler() override { return *this; }

  void connect_srb_notifier(srb_id_t                  srb_id,
                            rrc_pdu_notifier&         notifier,
                            rrc_tx_security_notifier* tx_sec,
                            rrc_rx_security_notifier* rx_sec) override;

  // rrc_dl_nas_message_handler
  void handle_dl_nas_transport_message(const dl_nas_transport_message& msg) override;

  // rrc_ue_control_message_handler
  async_task<bool> handle_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override;
  uint8_t          handle_handover_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override;
  async_task<bool> handle_handover_reconfiguration_complete_expected(uint8_t transaction_id) override;
  async_task<bool> handle_rrc_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) override;
  rrc_ue_release_context get_rrc_ue_release_context() override;
  optional<rrc_meas_cfg> get_rrc_ue_meas_config() override;
  byte_buffer            get_packed_handover_preparation_message() override;

  // rrc_ue_context_handler
  rrc_reestablishment_ue_context_t get_context() override;

private:
  // message handlers
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
  void send_dl_dcch(srb_id_t                           srb_id,
                    const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg,
                    ue_index_t                         old_ue_index = ue_index_t::invalid);
  void send_srb_pdu(srb_id_t srb_id, byte_buffer pdu, ue_index_t old_ue_index = ue_index_t::invalid);

  // rrc_ue_setup_proc_notifier
  void on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg) override;
  void on_ue_delete_request(const cause_t& cause) override;

  // rrc_ue_security_mode_command_proc_notifier
  void on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_ccch_msg) override;
  void on_new_as_security_context() override;

  // rrc_ue_reestablishment_proc_notifier
  void
  on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg, ue_index_t old_ue_index) override;

  // initializes the security context and triggers the SMC procedure
  async_task<bool> handle_init_security_context(const security::security_context& sec_ctx) override;

  // Helper to create PDU from RRC message
  template <class T>
  byte_buffer pack_into_pdu(const T& msg, const char* context_name = nullptr)
  {
    context_name = context_name == nullptr ? __FUNCTION__ : context_name;
    byte_buffer   pdu{};
    asn1::bit_ref bref{pdu};
    if (msg.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
      logger.error("Failed to pack message in {}. Discarding it.", context_name);
    }
    return pdu;
  }

  // Logging
  typedef enum { Rx = 0, Tx } direction_t;
  template <class T>
  void
  log_rrc_message(const char* source, const direction_t dir, byte_buffer_view pdu, const T& msg, const char* msg_type);
  void log_rx_pdu_fail(ue_index_t       ue_index,
                       const char*      source,
                       byte_buffer_view pdu,
                       const char*      cause_str,
                       bool             log_hex = true);

  rrc_ue_context_t                 context;
  up_resource_manager&             up_resource_mng;       // UP resource manager
  rrc_ue_du_processor_notifier&    du_processor_notifier; // notifier to the DU processor
  rrc_ue_nas_notifier&             nas_notifier;          // PDU notifier to the NGAP
  rrc_ue_control_notifier&         ngap_ctrl_notifier;    // Control message notifier to the NGAP
  rrc_ue_reestablishment_notifier& cu_cp_notifier;        // notifier to the CU-CP
  cell_meas_manager&               cell_meas_mng;         // cell measurement manager
  srb_notifiers_array              srbs;                  // set notifiers for all SRBs
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
