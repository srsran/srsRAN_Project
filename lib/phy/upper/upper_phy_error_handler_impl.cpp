/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_error_handler_impl.h"

using namespace srsran;

namespace {
/// Dummy implementation of an upper PHY error notifier.
class upper_phy_error_notifier_dummy : public upper_phy_error_notifier
{
public:
  void on_late_downlink_message(slot_point dl_msg_slot) override {}
  void on_late_uplink_message(slot_point ul_msg_slot) override {}
  void on_late_prach_message(slot_point prach_msg_slot) override {}
};
} // namespace

/// Dummy error notifier used for instantiating this error handler.
static upper_phy_error_notifier_dummy dummy_notifier;

upper_phy_error_handler_impl::upper_phy_error_handler_impl(uplink_slot_processor_pool& ul_proc_pool_) :
  ul_proc_pool(ul_proc_pool_), error_notifier(dummy_notifier)
{
}

void upper_phy_error_handler_impl::handle_late_downlink_message(slot_point slot)
{
  error_notifier.get().on_late_downlink_message(slot);
}

void upper_phy_error_handler_impl::handle_late_uplink_message(slot_point slot)
{
  error_notifier.get().on_late_uplink_message(slot);
  ul_proc_pool.get_slot_processor(slot).discard_slot();
}

void upper_phy_error_handler_impl::handle_late_prach_message(slot_point slot)
{
  error_notifier.get().on_late_prach_message(slot);
}
