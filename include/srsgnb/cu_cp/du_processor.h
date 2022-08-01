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

#include "cu_cp_types.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/ran/nr_cgi.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/rrc/rrc.h"
#include "srsgnb/rrc/rrc_config.h"
#include "srsgnb/support/timers.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// Forward declared messages.
struct f1_setup_request_message;

struct srb_creation_message {
  ue_index_t               ue_index;
  srb_id_t                 srb_id;
  asn1::rrc_nr::pdcp_cfg_s pdcp_cfg;
};

/// Interface to request SRB creations at the DU processor.
class du_processor_srb_interface
{
public:
  virtual ~du_processor_srb_interface() = default;

  /// \brief Instruct the DU processor to create a new SRB for a given UE. Depending on the config it creates all
  /// required intermediate objects (e.g. PDCP) and connects them with one another.
  /// \param[in] msg The UE index, SRB ID and config.
  virtual void create_srb(const srb_creation_message& msg) = 0;
};

struct ue_creation_message {
  nr_cell_global_identity         cgi;
  asn1::unbounded_octstring<true> du_to_cu_rrc_container;
  rnti_t                          c_rnti;
};

/// Interface for an F1AP notifier to communicate with the DU processor.
class du_processor_f1c_interface : public du_processor_srb_interface
{
public:
  virtual ~du_processor_f1c_interface() = default;

  /// \brief Get the DU index.
  /// \return The DU index.
  virtual du_index_t get_du_index() = 0;

  /// \brief Handle the reception of a F1 Setup Request message and transmit the F1 Setup Response or F1 Setup Failure.
  /// \param[in] msg The received F1 Setup Request message.
  virtual void handle_f1_setup_request(const f1_setup_request_message& msg) = 0;

  /// \brief Create a new UE context.
  /// \param[in] msg The UE creation message.
  /// \return Returns a UE creation complete message containing the index of the created UE and its SRB notifiers.
  virtual ue_creation_complete_message handle_ue_creation_request(const ue_creation_message& msg) = 0;
};

/// Interface for an RRC UE entity to communicate with the DU processor.
class du_processor_rrc_ue_interface : public du_processor_srb_interface
{
public:
  virtual ~du_processor_rrc_ue_interface() = default;

  /// \brief Handle a UE Context Release Command and notify F1AP.
  /// \param[in] msg The UE Context Release Command Message.
  virtual void handle_ue_context_release_command(const ue_context_release_command_message& msg) = 0;
};

/// \brief Service provided by DU processor to schedule async tasks for a given UE.
class du_processor_ue_task_scheduler
{
public:
  virtual ~du_processor_ue_task_scheduler()                                               = default;
  virtual void         handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task) = 0;
  virtual unique_timer make_unique_timer()                                                = 0;
};

} // namespace srs_cu_cp
} // namespace srsgnb
