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

#include "procedures/ngap_transaction_manager.h"
#include "srsran/asn1/ngap/ngap.h"
#include "srsran/cu_cp/ue_manager.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_configuration.h"
#include "srsran/pcap/pcap.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {

namespace srs_cu_cp {

class ngap_event_manager;

class ngap_impl final : public ngap_interface
{
public:
  ngap_impl(ngap_configuration&         ngap_cfg_,
            ngap_cu_cp_paging_notifier& cu_cp_paging_notifier_,
            ngap_ue_task_scheduler&     task_sched_,
            ngap_ue_manager&            ue_manager_,
            ngap_message_notifier&      ngap_notifier_,
            task_executor&              ctrl_exec_);
  ~ngap_impl();

  // ngap ue control manager functions
  void create_ngap_ue(ue_index_t                          ue_index,
                      ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                      ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                      ngap_du_processor_control_notifier& du_processor_ctrl_notifier) override;

  // ngap connection manager functions
  async_task<ng_setup_response> handle_ng_setup_request(const ng_setup_request& request) override;

  void handle_initial_ue_message(const ngap_initial_ue_message& msg) override;

  void handle_ul_nas_transport_message(const ngap_ul_nas_transport_message& msg) override;

  // ngap message handler functions
  void handle_message(const ngap_message& msg) override;
  void handle_connection_loss() override {}

  // ngap_statistic_interface
  size_t get_nof_ues() const override;

private:
  /// \brief Notify about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::ngap::init_msg_s& msg);

  /// \brief Notify about the reception of an DL NAS Transport message.
  /// \param[in] msg The received DL NAS Transport message.
  void handle_dl_nas_transport_message(const asn1::ngap::dl_nas_transport_s& msg);

  /// \brief Notify about the reception of an Initial Context Setup Request.
  /// \param[in] msg The received Initial Context Setup Request.
  void handle_initial_context_setup_request(const asn1::ngap::init_context_setup_request_s& request);

  /// \brief Notify about the reception of an PDU Session Resource Setup Request.
  /// \param[in] msg The received PDU Session Resource Setup Request.
  void handle_pdu_session_resource_setup_request(const asn1::ngap::pdu_session_res_setup_request_s& request);

  /// \brief Notify about the reception of an UE Context Release Command.
  /// \param[in] msg The received UE Context Release Command.
  void handle_ue_context_release_command(const asn1::ngap::ue_context_release_cmd_s& cmd);

  /// \brief Notify about the reception of a Paging message.
  /// \param[in] msg The received Paging message.
  void handle_paging(const asn1::ngap::paging_s& msg);

  /// \brief Notify about the reception of a successful outcome message.
  /// \param[in] outcome The successful outcome message.
  void handle_successful_outcome(const asn1::ngap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome message.
  /// \param[in] outcome The unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::ngap::unsuccessful_outcome_s& outcome);

  srslog::basic_logger&       logger;
  ngap_configuration&         ngap_cfg;
  ngap_cu_cp_paging_notifier& cu_cp_paging_notifier;
  ngap_ue_task_scheduler&     task_sched;
  ngap_ue_manager&            ue_manager;
  ngap_message_notifier&      ngap_notifier;
  task_executor&              ctrl_exec;

  ngap_transaction_manager ev_mng;
};

} // namespace srs_cu_cp

} // namespace srsran
