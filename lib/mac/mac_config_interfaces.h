/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/mac/mac.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class mac_ul_configurator
{
public:
  virtual ~mac_ul_configurator()                                                                              = default;
  virtual async_task<bool> add_ue(const mac_ue_create_request& msg)                                           = 0;
  virtual async_task<bool> addmod_bearers(du_ue_index_t                                  ue_index,
                                          const std::vector<mac_logical_channel_config>& ul_logical_channels) = 0;
  virtual async_task<bool> remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids_to_rem)            = 0;
  virtual async_task<void> remove_ue(const mac_ue_delete_request& msg)                                        = 0;
  virtual bool             flush_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu)                         = 0;

  /// Handle the confirmation that the UE applied the new configuration.
  virtual void handle_ue_config_applied(du_ue_index_t ue_index) = 0;
};

class mac_dl_configurator : public mac_cell_manager
{
public:
  virtual ~mac_dl_configurator() = default;

  /// \brief Create new UE context in the MAC DL processor.
  virtual async_task<bool> add_ue(const mac_ue_create_request& msg) = 0;

  /// \brief Update the bearers of an existing UE in the MAC DL processor.
  virtual async_task<bool> addmod_bearers(du_ue_index_t                                  ue_index,
                                          du_cell_index_t                                pcell_index,
                                          const std::vector<mac_logical_channel_config>& logical_channels) = 0;

  /// \brief Remove the bearers of an existing UE in the MAC DL processor.
  virtual async_task<bool>
  remove_bearers(du_ue_index_t ue_index, du_cell_index_t pcell_index, span<const lcid_t> lcids_to_rem) = 0;

  /// \brief Remove an existing UE in the MAC DL processor.
  virtual async_task<void> remove_ue(const mac_ue_delete_request& msg) = 0;
};

class mac_ctrl_configurator
{
public:
  virtual ~mac_ctrl_configurator() = default;

  /// Adds new UE to the MAC controller.
  /// \param ue_index DU UE index of the UE to be created.
  /// \param pcell_index DU-specific UE Serving Cell Index.
  /// \param tc_rnti Temporary C-RNTI assigned to the UE. If no TC-RNTI was assigned, INVALID_RNTI is passed. The MAC
  /// controller can decide to reuse the TC-RNTI as C-RNTI or assign a new one.
  /// \return RNTI assigned to the UE. INVALID_RNTI if the UE cannot be added.
  virtual rnti_t add_ue(du_ue_index_t ue_index, du_cell_index_t pcell_index, rnti_t tc_rnti) = 0;

  /// Remove UE from the MAC controller.
  virtual void remove_ue(du_ue_index_t ue_index) = 0;
};

} // namespace srsran
