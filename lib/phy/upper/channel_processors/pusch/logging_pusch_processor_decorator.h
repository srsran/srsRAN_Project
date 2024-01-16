/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/phy/support/support_formatters.h"
#include "srsran/phy/upper/channel_processors/pusch/formatters.h"
#include "srsran/phy/upper/unique_rx_buffer.h"

namespace fmt {

struct pusch_results_wrapper {
  srsran::optional<srsran::pusch_processor_result_control> uci;
  srsran::optional<srsran::pusch_processor_result_data>    sch;
};

/// \brief Custom formatter for \c pusch_results_wrapper.
template <>
struct formatter<pusch_results_wrapper> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const pusch_results_wrapper& result, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    // Format SCH message.
    if (result.sch.has_value()) {
      helper.format_always(ctx, result.sch.value());
    }

    // Format UCI message.
    if (result.uci.has_value()) {
      helper.format_always(ctx, result.uci.value());
    }

    // Format channel state information.
    if (result.sch.has_value()) {
      helper.format_always(ctx, result.sch.value().csi);
    } else if (result.uci.has_value()) {
      helper.format_always(ctx, result.uci.value().csi);
    }

    return ctx.out();
  }
};

} // namespace fmt

namespace srsran {

class logging_pusch_processor_decorator : public pusch_processor, private pusch_processor_result_notifier
{
public:
  logging_pusch_processor_decorator(srslog::basic_logger& logger_, std::unique_ptr<pusch_processor> processor_) :
    logger(logger_), processor(std::move(processor_))
  {
    srsran_assert(processor, "Invalid processor.");
  }

  void process(span<uint8_t>                    data_,
               unique_rx_buffer                 rm_buffer,
               pusch_processor_result_notifier& notifier_,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu_) override
  {
    notifier    = &notifier_;
    data        = data_;
    pdu         = pdu_;
    time_start  = std::chrono::steady_clock::now();
    time_uci    = std::chrono::time_point<std::chrono::steady_clock>();
    time_return = std::chrono::time_point<std::chrono::steady_clock>();

    // Clear processor results.
    results.sch.reset();
    results.uci.reset();

    processor->process(data, std::move(rm_buffer), *this, grid, pdu);
    time_return = std::chrono::steady_clock::now();
  }

private:
  void on_uci(const pusch_processor_result_control& uci) override
  {
    srsran_assert(notifier, "Invalid notifier");
    time_uci    = std::chrono::steady_clock::now();
    results.uci = uci;
    notifier->on_uci(uci);
  }

  void on_sch(const pusch_processor_result_data& sch) override
  {
    srsran_assert(notifier, "Invalid notifier");

    // Data size in bytes for printing hex dump only if SCH is present and CRC is passed.
    unsigned data_size = 0;
    if (results.sch.has_value() && results.sch->data.tb_crc_ok) {
      data_size = data.size();
    }

    // Save SCH results.
    results.sch = sch;

    std::chrono::time_point<std::chrono::steady_clock> time_end = std::chrono::steady_clock::now();

    // Calculate the UCI report latency if available.
    std::chrono::nanoseconds time_uci_ns(0);
    if (time_uci != std::chrono::time_point<std::chrono::steady_clock>()) {
      time_uci_ns = time_uci - time_start;
    }

    // Calculate the return latency if available.
    std::chrono::nanoseconds time_return_ns(0);
    if (time_return != std::chrono::time_point<std::chrono::steady_clock>()) {
      time_return_ns = time_return - time_start;
    }

    // Calculate the final time.
    std::chrono::nanoseconds time_ns = time_end - time_start;

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all PDU fields.
      logger.debug(pdu.slot.sfn(),
                   pdu.slot.slot_index(),
                   data.data(),
                   data_size,
                   "PUSCH: {:s} tbs={} {:s} {} uci_{} ret_{}\n  {:n}\n  {:n}",
                   pdu,
                   data.size(),
                   results,
                   time_ns,
                   time_uci_ns,
                   time_return_ns,
                   pdu,
                   results);
    } else {
      // Single line log entry.
      logger.info(pdu.slot.sfn(),
                  pdu.slot.slot_index(),
                  data.data(),
                  data_size,
                  "PUSCH: {:s} tbs={} {:s} {} uci_{} ret_{}",
                  pdu,
                  data.size(),
                  results,
                  time_ns,
                  time_uci_ns,
                  time_return_ns);
    }

    // Exchanges the notifier before notifying the reception of SCH.
    pusch_processor_result_notifier* notifier_ = nullptr;
    std::exchange(notifier_, notifier);

    // Notify the SCH reception.
    notifier_->on_sch(sch);
  }

  srslog::basic_logger&                              logger;
  std::unique_ptr<pusch_processor>                   processor;
  span<uint8_t>                                      data;
  pdu_t                                              pdu;
  pusch_processor_result_notifier*                   notifier;
  std::chrono::time_point<std::chrono::steady_clock> time_start;
  std::chrono::time_point<std::chrono::steady_clock> time_uci;
  std::chrono::time_point<std::chrono::steady_clock> time_return;
  fmt::pusch_results_wrapper                         results;
};

} // namespace srsran
