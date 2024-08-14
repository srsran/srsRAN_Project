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

#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"

namespace srsran {

/// RU received symbol handler for the Open Fronthaul interface.
class ru_ofh_rx_symbol_handler_impl : public ofh::uplane_rx_symbol_notifier
{
public:
  explicit ru_ofh_rx_symbol_handler_impl(ru_uplink_plane_rx_symbol_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void on_new_uplink_symbol(const ofh::uplane_rx_symbol_context& context, shared_resource_grid grid) override;

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) override;

private:
  ru_uplink_plane_rx_symbol_notifier& notifier;
};

} // namespace srsran
