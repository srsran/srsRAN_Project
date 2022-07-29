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

#include "srsgnb/rrc/rrc.h"

namespace srsgnb {

namespace srs_cu_cp {

/// Interface class for an RRC UE object.
/// It will contain getters for the interfaces for the various logical channels handled by RRC.
class rrc_ue_entity_interface
{
public:
  rrc_ue_entity_interface()          = default;
  virtual ~rrc_ue_entity_interface() = default;

  virtual rrc_ul_ccch_pdu_handler* get_ul_ccch_pdu_handler() = 0;
};

/// Interface to notify about a new SRB PDU.
class rrc_pdu_notifier
{
public:
  virtual ~rrc_pdu_notifier() = default;

  /// \brief Notify about a new PDU.
  /// \param[in] msg The RRC PDU message.
  virtual void on_new_pdu(const rrc_pdu_message msg) = 0;
};

/// Interface to notify about RRC UE Context messages.
class rrc_ue_du_processor_notifier
{
public:
  virtual ~rrc_ue_du_processor_notifier() = default;

  /// \brief Notify about hte need to create an SRB
  /// \param[in] msg The SRB creation message
  virtual void on_create_srb(const srb_creation_message& msg) = 0;

  /// \brief Notify about a UE Context Release Command
  /// \param[in] msg The UE Context Release Command message
  virtual void on_ue_context_release_command(const ue_context_release_command_message& msg) = 0;
};

/// Schedules asynchronous tasks associated with an UE.
class rrc_ue_task_scheduler
{
public:
  virtual ~rrc_ue_task_scheduler()                                  = default;
  virtual void         schedule_async_task(async_task<void>&& task) = 0;
  virtual unique_timer make_unique_timer()                          = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb
