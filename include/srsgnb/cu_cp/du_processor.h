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
#include "srsgnb/ran/rnti.h"
#include "srsgnb/rrc/rrc.h"
#include "srsgnb/rrc/rrc_config.h"
#include "srsgnb/support/timers.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// Forward declared messages.
struct f1_setup_request_message;

struct initial_ul_rrc_message {
  du_cell_index_t                           pcell_index;
  unsigned int                              tmp_ue_id;
  asn1::unbounded_octstring<true>           rrc_container;
  asn1::unbounded_octstring<true>           du_to_cu_rrc_container;
  rnti_t                                    c_rnti;
  optional<asn1::unbounded_octstring<true>> rrc_container_rrc_setup_complete;
};

struct ul_rrc_message {
  ue_index_t                      ue_index;
  uint8_t                         srbid;
  asn1::unbounded_octstring<true> rrc_container;
};

/// Interface for an F1AP notifier to communicate with the DU processor.
class du_processor_f1c_interface
{
public:
  virtual ~du_processor_f1c_interface() = default;

  /// \brief Lookup the cell based on a given NR cell ID.
  /// \param[in] packed_nr_cell_id The packed NR cell ID received over F1AP.
  virtual du_cell_index_t find_cell(uint64_t packed_nr_cell_id) = 0;

  /// \brief Get the DU index.
  /// \return The DU index.
  virtual du_index_t get_du_index() = 0;

  /// \brief Handle the reception of a F1 Setup Request message and transmit the F1 Setup Response or F1 Setup Failure.
  /// \param[in] msg The received F1 Setup Request message.
  virtual void handle_f1_setup_request(const f1_setup_request_message& msg) = 0;

  /// \brief Handle the reception of a Initial UL RRC Message Transfer message and create the UE, RRC and SRB0.
  /// \param[in] msg The received Initial UL RRC message.
  virtual void handle_initial_ul_rrc_message_transfer(const initial_ul_rrc_message& msg) = 0;

  /// \brief Handle the reception of a UL RRC Message Transfer message and pass it to SRB.
  /// \param[in] msg The received UL RRC message.
  virtual void handle_ul_rrc_message_transfer(const ul_rrc_message& msg) = 0;
};

/// Interface to notify the reception of an new RRC message.
class du_processor_rrc_message_notifier
{
public:
  virtual ~du_processor_rrc_message_notifier() = default;

  /// This callback is invoked on each received RRC message.
  virtual void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) = 0;
};

struct srb_creation_message {
  ue_index_t               ue_index;
  srb_id_t                 srb_id;
  asn1::rrc_nr::pdcp_cfg_s pdcp_cfg;
};

/// Interface for an RRU UE notifier to communicate with the DU processor.
class du_processor_rrc_ue_interface
{
public:
  virtual ~du_processor_rrc_ue_interface() = default;

  /// \brief Instruct the DU processor to create a new SRB for a given UE. Depending on the config it creates all
  /// required intermediate objects (e.g. PDCP) and connects them with one another.
  /// \param[in] msg The UE index, SRB ID and config.
  virtual void create_srb(const srb_creation_message& msg) = 0;

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
