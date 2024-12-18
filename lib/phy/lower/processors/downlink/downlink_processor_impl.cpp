/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "downlink_processor_impl.h"
#include "srsran/phy/lower/lower_phy_timing_context.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/support/resource_grid_context.h"

using namespace srsran;

downlink_processor_impl::downlink_processor_impl(std::unique_ptr<pdxch_processor>                 pdxch_proc_,
                                                 std::unique_ptr<amplitude_controller>            amplitude_control_,
                                                 const downlink_processor_baseband_configuration& config) :
  pdxch_proc(std::move(pdxch_proc_)),
  amplitude_control(std::move(amplitude_control_)),
  downlink_proc_baseband(pdxch_proc->get_baseband(), *amplitude_control, config)
{
  srsran_assert(pdxch_proc, "Invalid PDxCH processor.");
  srsran_assert(amplitude_control, "Invalid amplitude controller.");
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

baseband_cfo_processor& downlink_processor_impl::get_cfo_handler()
{
  return downlink_proc_baseband.get_cfo_control();
}
