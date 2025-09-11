/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "downlink_processor_impl.h"
#include "srsran/phy/lower/lower_phy_timing_context.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/support/resource_grid_context.h"

using namespace srsran;

downlink_processor_impl::downlink_processor_impl(std::unique_ptr<pdxch_processor>                 pdxch_proc_,
                                                 const downlink_processor_baseband_configuration& config) :
  pdxch_proc(std::move(pdxch_proc_)), downlink_proc_baseband(pdxch_proc->get_baseband(), config)
{
  srsran_assert(pdxch_proc, "Invalid PDxCH processor.");
}

void downlink_processor_impl::connect(downlink_processor_notifier& notifier, pdxch_processor_notifier& pdxch_notifier)
{
  downlink_proc_baseband.connect(notifier);
  pdxch_proc->connect(pdxch_notifier);
}

pdxch_processor_request_handler& downlink_processor_impl::get_downlink_request_handler()
{
  return pdxch_proc->get_request_handler();
}

downlink_processor_baseband& downlink_processor_impl::get_baseband()
{
  return downlink_proc_baseband;
}

baseband_cfo_processor& downlink_processor_impl::get_cfo_control()
{
  return downlink_proc_baseband.get_cfo_control();
}

lower_phy_center_freq_controller& downlink_processor_impl::get_carrier_center_frequency_control()
{
  return pdxch_proc->get_center_freq_control();
}

lower_phy_tx_time_offset_controller& downlink_processor_impl::get_tx_time_offset_control()
{
  return downlink_proc_baseband;
}
