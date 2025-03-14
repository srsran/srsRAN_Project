/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../resource_grid_request_pool.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/modulation/ofdm_modulator.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_notifier.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "srsran/phy/support/resource_grid_context.h"

namespace srsran {

/// Implements PDxCH baseband processor.
class pdxch_processor_impl : public pdxch_processor,
                             private pdxch_processor_baseband,
                             private pdxch_processor_request_handler
{
public:
  struct configuration {
    cyclic_prefix cp;
    unsigned      nof_tx_ports;
    unsigned      request_queue_size;
  };

  pdxch_processor_impl(std::unique_ptr<ofdm_symbol_modulator> modulator_, const configuration& config) :
    nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
    nof_tx_ports(config.nof_tx_ports),
    modulator(std::move(modulator_))
  {
    srsran_assert(modulator, "Invalid modulator.");
  }

  // See pdxch_processor interface for documentation.
  void connect(pdxch_processor_notifier& notifier) override;

  // See pdxch_processor interface for documentation.
  void stop() override { stopped = true; }

  // See pdxch_processor interface for documentation.
  pdxch_processor_request_handler& get_request_handler() override;

  // See pdxch_processor interface for documentation.
  pdxch_processor_baseband& get_baseband() override;

private:
  // See pdxch_processor_baseband interface for documentation.
  bool process_symbol(baseband_gateway_buffer_writer& samples, const symbol_context& context) override;

  // See pdxch_processor_request_handler interface for documentation.
  void handle_request(const shared_resource_grid& grid, const resource_grid_context& context) override;

  std::atomic<bool>                      stopped = false;
  unsigned                               nof_symbols_per_slot;
  unsigned                               nof_tx_ports;
  pdxch_processor_notifier*              notifier = nullptr;
  std::unique_ptr<ofdm_symbol_modulator> modulator;
  slot_point                             current_slot;
  shared_resource_grid                   current_grid;
  resource_grid_request_pool             requests;
};

} // namespace srsran
