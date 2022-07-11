/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_UPPER_PHY_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_UPPER_PHY_IMPL_H

#include "srsgnb/phy/resource_grid_pool.h"
#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/phy/upper/upper_phy.h"

// :TODO: remove these includes when the objects are created.
#include "srsgnb/phy/prach_buffer.h"
#include "srsgnb/phy/prach_buffer_context.h"
#include "srsgnb/phy/upper/upper_phy_rx_symbol_handler.h"
#include "srsgnb/phy/upper/upper_phy_timing_handler.h"
#include "srsgnb/phy/upper/upper_phy_timing_notifier.h"

namespace srsgnb {

/// \brief Implementation of the upper_phy interface. This object will handle the ownership of the upper PHY components.
class upper_phy_impl : public upper_phy
{
  // :TODO: remove these 2 dummies when the implementations are created.
  /// Dummy Upper PHY timing handler.
  class upper_phy_timing_handler_dummy : public upper_phy_timing_handler
  {
    std::reference_wrapper<upper_phy_timing_notifier> notifier;

  public:
    explicit upper_phy_timing_handler_dummy(upper_phy_timing_notifier& notifier) : notifier(notifier) {}

    void handle_tti_boundary(const upper_phy_timing_context& context) override
    {
      notifier.get().on_tti_boundary(context.slot);
    }

    void handle_ul_half_slot_boundary(const upper_phy_timing_context& context) override {}
    void handle_ul_full_slot_boundary(const upper_phy_timing_context& context) override {}

    void set_upper_phy_notifier(upper_phy_timing_notifier& notif) { notifier = std::ref(notif); }
  };

  /// Dummy Upper PHY rx symbol handler.
  class upper_phy_rx_symbol_handler_dummy : public upper_phy_rx_symbol_handler
  {
  public:
    void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const resource_grid_reader& grid) override {}

    void handle_rx_prach_window(const prach_buffer_context& context, const prach_buffer* buffer) override {}

    void handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context) override {}
  };

public:
  upper_phy_impl(unsigned                                 sector_id_,
                 std::unique_ptr<downlink_processor_pool> dl_processor_pool,
                 std::unique_ptr<resource_grid_pool>      rg_pool_);

  // See interface for documentation.
  upper_phy_rx_symbol_handler& get_upper_phy_rx_symbol_handler() override;

  // See interface for documentation.
  upper_phy_timing_handler& get_upper_phy_timing_handler() override;

  // See interface for documentation.
  downlink_processor_pool& get_downlink_processor_pool() override;

  // See interface for documentation.
  resource_grid_pool& get_downlink_resource_grid_pool() override;

  // See interface for documentation.
  void set_upper_phy_notifier(upper_phy_timing_notifier& notifier) override;

private:
  const unsigned                           sector_id;
  std::unique_ptr<downlink_processor_pool> dl_processor_pool;
  std::unique_ptr<resource_grid_pool>      rg_pool;

  // :TODO: Create implementations for these 2 handlers, and remove the unique_ptr.
  upper_phy_rx_symbol_handler_dummy symbol_handler;
  upper_phy_timing_handler_dummy    timing_handler;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_UPPER_PHY_IMPL_H
