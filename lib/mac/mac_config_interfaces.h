/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/mac/mac.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class mac_ul_configurator
{
public:
  virtual ~mac_ul_configurator()                                                             = default;
  virtual async_task<bool> add_ue(const mac_ue_create_request_message& msg)                  = 0;
  virtual async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& msg) = 0;
  virtual async_task<void> remove_ue(const mac_ue_delete_request_message& msg)               = 0;
  virtual void             flush_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu)        = 0;
};

class mac_dl_configurator : public mac_cell_manager
{
public:
  virtual ~mac_dl_configurator()                                                             = default;
  virtual async_task<bool> add_ue(const mac_ue_create_request_message& msg)                  = 0;
  virtual async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& msg) = 0;
  virtual async_task<void> remove_ue(const mac_ue_delete_request_message& msg)               = 0;
};

class mac_ctrl_configurator
{
public:
  virtual ~mac_ctrl_configurator()                                                      = default;
  virtual bool add_ue(du_ue_index_t ue_index, rnti_t rnti, du_cell_index_t pcell_index) = 0;
  virtual void remove_ue(du_ue_index_t ue_index)                                        = 0;
};

class rach_handler_configurator
{
public:
  virtual ~rach_handler_configurator()                 = default;
  virtual void add_cell(du_cell_index_t cell_index)    = 0;
  virtual void remove_cell(du_cell_index_t cell_index) = 0;
};

} // namespace srsran
