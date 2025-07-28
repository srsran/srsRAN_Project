/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/phy/upper/channel_processors/ssb/formatters.h"
#include "srsran/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsvec/bit.h"

namespace srsran {

class logging_ssb_processor_decorator : public ssb_processor
{
  template <typename Func>
  static std::chrono::nanoseconds time_execution(Func&& func)
  {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  }

public:
  logging_ssb_processor_decorator(srslog::basic_logger& logger_, std::unique_ptr<ssb_processor> processor_) :
    logger(logger_), processor(std::move(processor_))
  {
    srsran_assert(processor, "Invalid processor.");
  }

  void process(resource_grid_writer& grid, const pdu_t& pdu) override
  {
    const auto&& func = [&]() { processor->process(grid, pdu); };

    std::chrono::nanoseconds time_ns = time_execution(func);

    static_bit_buffer<MIB_PAYLOAD_SIZE> data(pdu.mib_payload.size());
    srsvec::zero(data.get_buffer());
    srsvec::bit_pack(data, pdu.mib_payload);

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all SSB PDU fields.
      logger.debug(pdu.slot.sfn(),
                   pdu.slot.slot_index(),
                   data.get_buffer().data(),
                   divide_ceil(data.size(), 8),
                   "SSB: {:s} {}\n  {:n}\n  {}",
                   pdu,
                   time_ns,
                   pdu,
                   time_ns);

      return;
    }
    // Single line log entry.
    logger.info(pdu.slot.sfn(),
                pdu.slot.slot_index(),
                data.get_buffer().data(),
                divide_ceil(data.size(), 8),
                "SSB: {:s} {}",
                pdu,
                time_ns);
  }

private:
  srslog::basic_logger&          logger;
  std::unique_ptr<ssb_processor> processor;
};

} // namespace srsran
