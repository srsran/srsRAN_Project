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

#include "srsgnb/phy/lower/processors/prach/prach_processor_notifier.h"
#include "srsgnb/phy/support/prach_buffer.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/srslog/srslog.h"
#include <vector>

namespace srsgnb {

class prach_processor_notifier_spy : public prach_processor_notifier
{
private:
  struct rx_prach_window_entry {
    prach_buffer_context context;
    const prach_buffer*  buffer;
  };

  srslog::basic_logger&              logger;
  std::vector<prach_buffer_context>  request_late_entries;
  std::vector<prach_buffer_context>  request_overflow_entries;
  std::vector<rx_prach_window_entry> rx_prach_window_entries;

public:
  prach_processor_notifier_spy(const std::string& log_level = "info") :
    logger(srslog::fetch_basic_logger("Notifier", false))
  {
    srslog::init();
    logger.set_level(srslog::str_to_basic_level(log_level));
  }

  void on_prach_request_late(const prach_buffer_context& context) override
  {
    logger.info("PRACH request is late. Sector/Port {}/{}. Slot/Symbol {}/{}.",
                context.sector,
                context.port,
                context.slot,
                context.start_symbol);
    request_late_entries.push_back(context);
  }

  void on_prach_request_overflow(const prach_buffer_context& context) override
  {
    logger.info("PRACH request discarded. Sector/Port {}/{}. Slot/Symbol {}/{}.",
                context.sector,
                context.port,
                context.slot,
                context.start_symbol);
    request_overflow_entries.push_back(context);
  }

  void on_rx_prach_window(const prach_buffer& buffer, const prach_buffer_context& context) override
  {
    logger.info("PRACH Rx Window Processed. Sector/Port {}/{}. Slot/Symbol {}/{}.",
                context.sector,
                context.port,
                context.slot,
                context.start_symbol);
    rx_prach_window_entries.emplace_back();
    rx_prach_window_entry& entry = rx_prach_window_entries.back();
    entry.context                = context;
    entry.buffer                 = &buffer;
  }

  unsigned get_nof_notifications() const
  {
    return request_late_entries.size() + request_overflow_entries.size() + rx_prach_window_entries.size();
  }

  void clear_notifications()
  {
    request_late_entries.clear();
    request_overflow_entries.clear();
    rx_prach_window_entries.clear();
  }

  const std::vector<prach_buffer_context>& get_request_late_entries() const { return request_late_entries; }

  const std::vector<prach_buffer_context>& get_request_overflow_entries() const { return request_overflow_entries; }

  const std::vector<rx_prach_window_entry>& get_rx_prach_window_entries() const { return rx_prach_window_entries; }
};

} // namespace srsgnb