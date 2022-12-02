/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/cu_cp/ue_manager.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {

namespace srs_cu_cp {

class ngc_event_manager;

class ngc_impl final : public ngc_interface
{
public:
  ngc_impl(ngc_ue_task_scheduler& task_sched_, ngc_ue_manager& ue_manager_, ngc_message_notifier& ngc_notifier_);
  ~ngc_impl();

  // ngc ue control manager functions
  void create_ngc_ue(du_index_t                   du_index,
                     ue_index_t                   ue_index,
                     ngc_rrc_ue_pdu_notifier&     rrc_ue_pdu_notifier,
                     ngc_rrc_ue_control_notifier& rrc_ue_ctrl_notifier) override;

  // ngc connection manager functions
  async_task<ng_setup_response_message> handle_ng_setup_request(const ng_setup_request_message& request) override;

  void handle_initial_ue_message(const ngap_initial_ue_message& msg) override;

  void handle_ul_nas_transport_message(const ngap_ul_nas_transport_message& msg) override;

  // ngc ue context manager
  void handle_initial_context_setup_response_message(const ngap_initial_context_setup_response_message& msg) override;

  // ngc message handler functions
  void handle_message(const ngc_message& msg) override;
  void handle_connection_loss() override {}

  // ngc_statistic_interface
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

  /// \brief Notify about the reception of a successful outcome message.
  /// \param[in] outcome The successful outcome message.
  void handle_successful_outcome(const asn1::ngap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome message.
  /// \param[in] outcome The unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::ngap::unsuccessful_outcome_s& outcome);

  srslog::basic_logger&  logger;
  ngc_ue_task_scheduler& task_sched;
  ngc_ue_manager&        ue_manager;
  ngc_message_notifier&  ngc_notifier;

  std::unique_ptr<ngc_event_manager> events;
};

} // namespace srs_cu_cp

} // namespace srsgnb