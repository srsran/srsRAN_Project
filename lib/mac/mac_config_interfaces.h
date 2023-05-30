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
  virtual void             flush_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu)                         = 0;
};

class mac_dl_configurator : public mac_cell_manager
{
public:
  virtual ~mac_dl_configurator()                                                                           = default;
  virtual async_task<bool> add_ue(const mac_ue_create_request& msg)                                        = 0;
  virtual async_task<bool> addmod_bearers(du_ue_index_t                                  ue_index,
                                          du_cell_index_t                                pcell_index,
                                          const std::vector<mac_logical_channel_config>& logical_channels) = 0;
  virtual async_task<bool>
  remove_bearers(du_ue_index_t ue_index, du_cell_index_t pcell_index, span<const lcid_t> lcids_to_rem) = 0;
  virtual async_task<void> remove_ue(const mac_ue_delete_request& msg)                                 = 0;
};

class mac_ctrl_configurator
{
public:
  virtual ~mac_ctrl_configurator()                                                      = default;
  virtual bool add_ue(du_ue_index_t ue_index, rnti_t rnti, du_cell_index_t pcell_index) = 0;
  virtual void remove_ue(du_ue_index_t ue_index)                                        = 0;
};

} // namespace srsran
