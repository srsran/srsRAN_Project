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

#include "srsran/phy/support/support_formatters.h"
#include "srsran/phy/upper/channel_processors/ssb/formatters.h"
#include "srsran/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsvec/bit.h"

template <typename Func>
static std::chrono::nanoseconds time_execution(Func&& func)
{
  auto start = std::chrono::steady_clock::now();
  func();
  auto end = std::chrono::steady_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

namespace srsran {

class logging_ssb_processor_decorator : public ssb_processor
{
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

    static_bit_buffer<BCH_PAYLOAD_SIZE> data(pdu.bch_payload.size());
    srsvec::zero(data.get_buffer());
    srsvec::bit_pack(data, pdu.bch_payload);

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
    logger.info(data.get_buffer().data(), divide_ceil(data.size(), 8), "SSB: {:s} {}", pdu, time_ns);
  }

private:
  srslog::basic_logger&          logger;
  std::unique_ptr<ssb_processor> processor;
};

} // namespace srsran
