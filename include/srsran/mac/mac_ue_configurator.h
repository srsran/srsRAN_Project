
#pragma once

#include "mac_cell_group_config.h"
#include "srsran/mac/bsr_format.h"
#include "srsran/mac/mac_sdu_handler.h"
#include "srsran/mac/phr_config.h"
#include "srsran/mac/time_alignment_group_config.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/physical_cell_group.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sr_configuration.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class unique_timer;

/// Parameters passed to MAC concerning a created logical channel.
struct mac_logical_channel_to_setup {
  lcid_t               lcid;
  mac_sdu_rx_notifier* ul_bearer;
  mac_sdu_tx_builder*  dl_bearer;
};

/// Input parameters used to create a UE in the scheduler.
struct mac_ue_create_request_message {
  du_cell_index_t                           cell_index;
  du_ue_index_t                             ue_index;
  rnti_t                                    crnti;
  std::vector<mac_logical_channel_to_setup> bearers;
  mac_cell_group_config                     mac_cell_group_cfg;
  physical_cell_group_config                phy_cell_group_cfg;
  const byte_buffer*                        ul_ccch_msg;
  // Scheduler-only params.
  sched_ue_config_request sched_cfg;
};

/// Outcome of a MAC UE creation request procedure.
struct mac_ue_create_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

/// Input parameters used to reconfigure a UE in the scheduler.
struct mac_ue_reconfiguration_request_message {
  du_ue_index_t                             ue_index;
  du_cell_index_t                           pcell_index;
  rnti_t                                    crnti;
  std::vector<mac_logical_channel_to_setup> bearers_to_addmod;
  std::vector<lcid_t>                       bearers_to_rem;
  mac_cell_group_config                     mac_cell_group_cfg;
  physical_cell_group_config                phy_cell_group_cfg;
  // Scheduler-only params.
  sched_ue_config_request sched_cfg;
};

struct mac_ue_reconfiguration_response_message {
  du_ue_index_t ue_index;
  bool          result;
};

/// Input parameters used to delete a UE in the scheduler.
struct mac_ue_delete_request_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
};

struct mac_ue_delete_response_message {
  bool result;
};

/// Interface used to manage the creation/reconfiguration/deletion of UEs in MAC
class mac_ue_configurator
{
public:
  virtual ~mac_ue_configurator() = default;
  virtual async_task<mac_ue_create_response_message>
  handle_ue_create_request(const mac_ue_create_request_message& cfg) = 0;
  virtual async_task<mac_ue_reconfiguration_response_message>
  handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& cfg) = 0;
  virtual async_task<mac_ue_delete_response_message>
               handle_ue_delete_request(const mac_ue_delete_request_message& cfg) = 0;
  virtual void handle_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu)        = 0;
};

} // namespace srsran
