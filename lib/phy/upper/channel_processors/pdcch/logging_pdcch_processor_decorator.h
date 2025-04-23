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
#include "srsran/phy/upper/channel_processors/pdcch/formatters.h"
#include "srsran/ran/precoding/precoding_weight_matrix_formatters.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/format/delimited_formatter.h"

namespace srsran {

inline bool is_broadcast_rnti(uint16_t rnti)
{
  return ((rnti < to_value(rnti_t::MIN_CRNTI)) || (rnti > to_value(rnti_t::MAX_CRNTI)));
}

template <typename Func>
inline std::chrono::nanoseconds time_execution(Func&& func)
{
  auto start = std::chrono::steady_clock::now();
  func();
  auto end = std::chrono::steady_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

class logging_pdcch_processor_decorator : public pdcch_processor
{
public:
  logging_pdcch_processor_decorator(srslog::basic_logger&            logger_,
                                    bool                             enable_logging_broadcast_,
                                    std::unique_ptr<pdcch_processor> processor_) :
    logger(logger_), enable_logging_broadcast(enable_logging_broadcast_), processor(std::move(processor_))
  {
    srsran_assert(processor, "Invalid processor.");
  }

  void process(resource_grid_writer& grid, const pdu_t& pdu) override
  {
    const auto&& func = [this, &grid, &pdu]() { processor->process(grid, pdu); };

    if (!enable_logging_broadcast && is_broadcast_rnti(pdu.dci.rnti)) {
      func();
      return;
    }

    std::chrono::nanoseconds time_ns = time_execution(func);

    static_bit_buffer<pdcch_constants::MAX_DCI_PAYLOAD_SIZE> data(pdu.dci.payload.size());
    srsvec::zero(data.get_buffer());
    srsvec::bit_pack(data, pdu.dci.payload);

    if (logger.debug.enabled()) {
      // Detailed log information, including a list of all PDU fields.
      logger.debug(pdu.slot.sfn(),
                   pdu.slot.slot_index(),
                   data.get_buffer().data(),
                   divide_ceil(data.size(), 8),
                   "PDCCH: {:s} {}\n  {:n}\n  {}",
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
                "PDCCH: {:s} {}",
                pdu,
                time_ns);
  }

private:
  srslog::basic_logger&            logger;
  bool                             enable_logging_broadcast;
  std::unique_ptr<pdcch_processor> processor;
};

} // namespace srsran
