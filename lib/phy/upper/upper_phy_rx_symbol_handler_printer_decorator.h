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

#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/conversion.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/executors/task_worker.h"
#include <fstream>

namespace srsran {

class upper_phy_rx_symbol_handler_printer_decorator : public upper_phy_rx_symbol_handler
{
public:
  upper_phy_rx_symbol_handler_printer_decorator(std::unique_ptr<upper_phy_rx_symbol_handler> handler_,
                                                srslog::basic_logger&                        logger_,
                                                const std::string&                           filename,
                                                unsigned                                     nof_rb,
                                                interval<unsigned>                           ul_print_ports,
                                                bool                                         print_prach_) :
    handler(std::move(handler_)),
    logger(logger_),
    worker("rx_symb_print", 40),
    temp_buffer(nof_rb * NRE),
    temp_prach_buffer(prach_constants::LONG_SEQUENCE_LENGTH),
    nof_symbols(MAX_NSYMB_PER_SLOT),
    start_port(ul_print_ports.start()),
    end_port(ul_print_ports.stop()),
    print_prach(print_prach_)
  {
    if (!logger.info.enabled()) {
      logger.error("Receive symbol enabled but logger level not enabled. No symbols will be printed.");
      return;
    }

    file = std::ofstream(filename, std::ios_base::binary | std::ios_base::trunc);
    if (!file.is_open()) {
      logger.error("RX_SYMBOL: failed to open file {}.", filename);
    }

    if (end_port <= start_port) {
      logger.error("End port {} is not larger than start port {}.", end_port, start_port);
    }
  }

  void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const resource_grid_reader& grid) override
  {
    // Handle Rx symbol.
    handler->handle_rx_symbol(context, grid);

    // Early return if the number of symbols does not reach the configured one or the file is not open.
    if ((context.symbol != (nof_symbols - 1)) || !file.is_open()) {
      return;
    }

    // Queue write request.
    if (not worker.push_task([this, context, &grid]() {
          // Save the resource grid.
          for (unsigned i_port = start_port; i_port != end_port; ++i_port) {
            for (unsigned symbol_idx = 0; symbol_idx != nof_symbols; ++symbol_idx) {
              grid.get(temp_buffer, i_port, symbol_idx, 0);
              file.write(reinterpret_cast<const char*>(temp_buffer.data()), temp_buffer.size() * sizeof(cf_t));
            }
          }

          // Log the resource grid information.
          unsigned nof_complex_floats = temp_buffer.size() * nof_symbols * (end_port - start_port);
          logger.info(context.slot.sfn(),
                      context.slot.slot_index(),
                      "RX_SYMBOL: sector={} offset={} size={}",
                      context.sector,
                      file_offset,
                      nof_complex_floats);

          // Advance file offset.
          file_offset += nof_complex_floats;
        })) {
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "RX_SYMBOL: Failed to write UL samples. Cause: task worker queue is full");
    }
  }

  void handle_rx_prach_window(const prach_buffer_context& context, const prach_buffer& buffer) override
  {
    handler->handle_rx_prach_window(context, buffer);

    // Queue write request.
    if (print_prach && !worker.push_task([this, context, &buffer]() {
          unsigned nof_replicas = buffer.get_max_nof_symbols();
          unsigned prach_start  = 0;
          unsigned prach_stop   = buffer.get_max_nof_ports();
          for (unsigned i_port = prach_start; i_port != prach_stop; ++i_port) {
            for (unsigned i_replica = 0; i_replica != nof_replicas; ++i_replica) {
              // Select view of the replica.
              span<const cbf16_t> samples = buffer.get_symbol(i_port, 0, 0, i_replica);

              // Convert samples to complex float.
              span<cf_t> samples_cf = temp_prach_buffer.first(samples.size());
              srsvec::convert(samples_cf, samples);

              // Write file.
              file.write(reinterpret_cast<const char*>(samples_cf.data()), samples.size() * sizeof(cf_t));
            }
          }

          unsigned nof_complex_floats = buffer.get_sequence_length() * nof_replicas * (prach_stop - prach_start);
          // Log the resource grid information.
          logger.info(context.slot.sfn(),
                      context.slot.slot_index(),
                      "RX_PRACH: sector={} offset={} size={}",
                      context.sector,
                      file_offset,
                      nof_complex_floats);

          // Advance file offset.
          file_offset += nof_complex_floats;
        })) {
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "RX_PRACH: Failed to write PRACH samples. Cause: task worker queue is full");
    }
  }

private:
  size_t                                       file_offset = 0;
  std::unique_ptr<upper_phy_rx_symbol_handler> handler;
  srslog::basic_logger&                        logger;
  std::ofstream                                file;
  task_worker                                  worker;
  srsvec::aligned_vec<cf_t>                    temp_buffer;
  srsvec::aligned_vec<cf_t>                    temp_prach_buffer;
  unsigned                                     nof_symbols;
  unsigned                                     start_port;
  unsigned                                     end_port;
  bool                                         print_prach;
};

} // namespace srsran
