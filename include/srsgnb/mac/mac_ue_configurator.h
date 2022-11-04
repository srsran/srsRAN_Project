
#pragma once

#include "srsgnb/mac/bsr_format.h"
#include "srsgnb/mac/mac_sdu_handler.h"
#include "srsgnb/mac/phr_config.h"
#include "srsgnb/mac/time_alignment_group_config.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/physical_cell_group.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/sr_configuration.h"
#include "srsgnb/scheduler/scheduler_configurator.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

/// \c MAC-CellGroupConfig, TS 38.331.
struct mac_cell_group_config {
  /// \c schedulingRequestConfig.
  std::vector<scheduling_request_to_addmod> scheduling_request_config;
  /// \c bsr-Config.
  optional<bsr_config> bsr_cfg;
  /// \c tag-Config.
  static_vector<tag, MAX_NOF_TAGS> tag_config;
  /// \c phr-Config.
  optional<phr_config> phr_cfg;
  /// \c skipUplinkTxDynamic.
  bool skip_uplink_tx_dynamic;
  // TODO: add remaining fields.
};

/// Parameters passed to MAC concerning a created logical channel.
struct mac_logical_channel_addmod {
  lcid_t               lcid;
  mac_sdu_rx_notifier* ul_bearer;
  mac_sdu_tx_builder*  dl_bearer;
  // MAC Logical Channel Config.
  logical_channel_config lc_config;
};

/// Input parameters used to create a UE in the scheduler.
struct mac_ue_create_request_message {
  du_cell_index_t                         cell_index;
  du_ue_index_t                           ue_index;
  rnti_t                                  crnti;
  std::vector<mac_logical_channel_addmod> bearers;
  const byte_buffer*                      ul_ccch_msg;
  mac_cell_group_config                   mac_cell_group_cfg;
  physical_cell_group_config              phy_cell_group_cfg;
  // Scheduler-only params.
  optional<serving_cell_config> serv_cell_cfg;
};

/// Outcome of a MAC UE creation request procedure.
struct mac_ue_create_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

/// Input parameters used to reconfigure a UE in the scheduler.
struct mac_ue_reconfiguration_request_message {
  du_ue_index_t                           ue_index;
  du_cell_index_t                         pcell_index;
  rnti_t                                  crnti;
  std::vector<mac_logical_channel_addmod> bearers_to_addmod;
  std::vector<lcid_t>                     bearers_to_rem;
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

} // namespace srsgnb
