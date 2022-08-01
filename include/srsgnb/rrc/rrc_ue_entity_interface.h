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

struct rrc_pdu_message {
  rrc_pdu_message(byte_buffer_slice pdu_) : pdu(std::move(pdu_)) {}
  byte_buffer_slice pdu;
};

/// Interface to notify about a new SRB PDU.
class rrc_pdu_notifier
{
public:
  virtual ~rrc_pdu_notifier() = default;

  /// \brief Notify about a new PDU.
  /// \param[in] msg The RRC PDU message.
  virtual void on_new_pdu(const rrc_pdu_message& msg) = 0;
};

/// Non-owning handlers to PDU notifiers.
using srb_notifiers = std::array<rrc_pdu_notifier*, MAX_NOF_SRBS>;

/// Dummy notifier that just logs the PDU.
/// An object of this type is instantiated upon creation of the SRB context to avoid nullptr checks.
class rrc_pdu_null_notifier : public rrc_pdu_notifier
{
public:
  rrc_pdu_null_notifier() = default;
  void on_new_pdu(const rrc_pdu_message& msg) override
  {
    srsgnb_assertion_failure("Received PDU on unconnected notifier. Discarding.");
    logger.error("Received PDU on unconnected notifier. Discarding.");
  };

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC");
};

/// Interface class for an RRC UE object.
/// It will contain getters for the interfaces for the various logical channels handled by RRC.
class rrc_ue_entity_interface
{
public:
  rrc_ue_entity_interface()          = default;
  virtual ~rrc_ue_entity_interface() = default;

  virtual rrc_ul_ccch_pdu_handler& get_ul_ccch_pdu_handler()                                         = 0;
  virtual rrc_ul_dcch_pdu_handler& get_ul_dcch_pdu_handler()                                         = 0;
  virtual void                     connect_srb_notifier(srb_id_t srb_id, rrc_pdu_notifier& notifier) = 0;
};

/// Interface to notify about RRC UE Context messages.
class rrc_ue_du_processor_notifier
{
public:
  virtual ~rrc_ue_du_processor_notifier() = default;

  /// \brief Notify about the need to create an SRB
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
