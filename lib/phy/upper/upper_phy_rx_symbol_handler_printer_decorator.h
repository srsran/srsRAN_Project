/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/support/error_handling.h"
#include "srsgnb/support/executors/task_worker.h"
#include <fstream>

namespace srsgnb {

class upper_phy_rx_symbol_handler_printer_decorator : public upper_phy_rx_symbol_handler
{
public:
  upper_phy_rx_symbol_handler_printer_decorator(std::unique_ptr<upper_phy_rx_symbol_handler> handler_,
                                                srslog::basic_logger&                        logger_,
                                                const std::string&                           filename,
                                                unsigned                                     nof_rb) :
    handler(std::move(handler_)),
    logger(logger_),
    worker("rx_symbol_handler_printer", 40),
    temp_buffer(nof_rb * NRE),
    nof_symbols(MAX_NSYMB_PER_SLOT)
  {
    if (!logger.info.enabled()) {
      logger.error("Receive symbol enabled but logger level not enabled. No symbols will be printed.");
      return;
    }

    file = std::ofstream(filename, std::ios_base::binary | std::ios_base::trunc);
    if (!file.is_open()) {
      logger.error("RX_SYMBOL: failed to open file {}.", filename);
    }
  }

  void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const resource_grid_reader& grid) override
  {
    // Handle Rx symbol.
    handler->handle_rx_symbol(context, grid);

    // Early return if the number of symbols does not reach the configuredor the file is not open.
    if ((context.symbol != (nof_symbols - 1)) || !file.is_open()) {
      return;
    }

    // Queue write request.
    worker.push_task([this, context, &grid]() {
      // Save the resource grid.
      for (unsigned symbol_idx = 0; symbol_idx != nof_symbols; ++symbol_idx) {
        grid.get(temp_buffer, 0, symbol_idx, 0);
        file.write(reinterpret_cast<const char*>(temp_buffer.data()), temp_buffer.size() * sizeof(cf_t));
      }

      // Log the resource grid information.
      logger.set_context(context.slot.sfn(), context.slot.slot_index());
      logger.info(
          "RX_SYMBOL: sector={} offset={} size={}", context.sector, file_offset, temp_buffer.size() * nof_symbols);

      // Advance file offset.
      file_offset += temp_buffer.size() * nof_symbols;
    });
  }

  void handle_rx_prach_window(const prach_buffer_context& context, const prach_buffer& buffer) override
  {
    handler->handle_rx_prach_window(context, buffer);

    // Queue write request.
    worker.push_task([this, context, &buffer]() {
      // Save the first PRACH symbol only.
      span<const cf_t> samples = buffer.get_symbol(0);
      file.write(reinterpret_cast<const char*>(samples.data()), samples.size() * sizeof(cf_t));

      // Log the resource grid information.
      logger.set_context(context.slot.sfn(), context.slot.slot_index());
      logger.info("RX_PRACH: sector={} offset={} size={}", context.sector, file_offset, samples.size());

      // Advance file offset.
      file_offset += samples.size();
    });
  }

  void handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context) override
  {
    handler->handle_rx_srs_symbol(context);
  }

private:
  size_t                                       file_offset = 0;
  std::unique_ptr<upper_phy_rx_symbol_handler> handler;
  srslog::basic_logger&                        logger;
  std::ofstream                                file;
  task_worker                                  worker;
  srsvec::aligned_vec<cf_t>                    temp_buffer;
  unsigned                                     nof_symbols;
};

} // namespace srsgnb