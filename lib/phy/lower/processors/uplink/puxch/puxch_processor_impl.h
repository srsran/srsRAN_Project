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
#include "../../resource_grid_request_pool.h"
#include "srsran/adt/circular_array.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/phy/lower/modulation/ofdm_demodulator.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_notifier.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_request_handler.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Implements PUxCH baseband processor.
class puxch_processor_impl : public puxch_processor,
                             private puxch_processor_baseband,
                             private puxch_processor_request_handler
{
public:
  struct configuration {
    cyclic_prefix cp;
    unsigned      nof_rx_ports;
    unsigned      request_queue_size;
  };

  puxch_processor_impl(std::unique_ptr<ofdm_symbol_demodulator> demodulator_, const configuration& config) :
    nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
    nof_rx_ports(config.nof_rx_ports),
    demodulator(std::move(demodulator_))
  {
    srsran_assert(demodulator, "Invalid demodulator.");
  }

  // See interface for documentation.
  void connect(puxch_processor_notifier& notifier_) override { notifier = &notifier_; }

  // See interface for documentation.
  puxch_processor_request_handler& get_request_handler() override { return *this; }

  // See interface for documentation.
  puxch_processor_baseband& get_baseband() override { return *this; }

private:
  // See interface for documentation.
  bool process_symbol(const baseband_gateway_buffer_reader& samples,
                      const lower_phy_rx_symbol_context&    context) override;

  // See interface for documentation.
  void handle_request(resource_grid& grid, const resource_grid_context& context) override;

  unsigned                                  nof_symbols_per_slot;
  unsigned                                  nof_rx_ports;
  puxch_processor_notifier*                 notifier = nullptr;
  std::unique_ptr<ofdm_symbol_demodulator>  demodulator;
  slot_point                                current_slot;
  resource_grid*                            current_grid = nullptr;
  resource_grid_request_pool<resource_grid> requests;
};

} // namespace srsran
