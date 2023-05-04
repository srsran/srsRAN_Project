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
#include "srsran/adt/blocking_queue.h"
#include "srsran/gateways/baseband/baseband_gateway_buffer.h"
#include "srsran/phy/lower/modulation/ofdm_modulator.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_baseband.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_notifier.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_reader_empty.h"

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
    modulator(std::move(modulator_)),
    request_queue(config.request_queue_size)
  {
    srsran_assert(modulator, "Invalid modulator.");
  }

  // See interface for documentation.
  void connect(pdxch_processor_notifier& notifier) override;

  // See interface for documentation.
  pdxch_processor_request_handler& get_request_handler() override;

  // See interface for documentation.
  pdxch_processor_baseband& get_baseband() override;

private:
  // See interface for documentation.
  void process_symbol(baseband_gateway_buffer& samples, const symbol_context& context) override;

  // See interface for documentation.
  void handle_request(const resource_grid_reader& grid, const resource_grid_context& context) override;

  /// Pairs a slot and resource grid reader for the queue.
  struct rg_grid_request {
    slot_point                  slot = {};
    const resource_grid_reader* grid = nullptr;
  };

  unsigned                                nof_symbols_per_slot;
  unsigned                                nof_tx_ports;
  pdxch_processor_notifier*               notifier = nullptr;
  std::unique_ptr<ofdm_symbol_modulator>  modulator;
  slot_point                              current_slot = {};
  const resource_grid_reader*             current_grid = &empty_rg;
  blocking_queue<rg_grid_request>         request_queue;
  static const resource_grid_reader_empty empty_rg;
};

} // namespace srsran
