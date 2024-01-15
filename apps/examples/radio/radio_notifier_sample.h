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

#include "srsran/radio/radio_factory.h"

namespace srsran {

class radio_notifier_spy : public radio_notification_handler
{
private:
  srslog::basic_logger& logger;
  unsigned              count_tx_overflow  = 0;
  unsigned              count_tx_underflow = 0;
  unsigned              count_tx_late      = 0;
  unsigned              count_tx_other     = 0;
  unsigned              count_rx_overflow  = 0;
  unsigned              count_rx_underflow = 0;
  unsigned              count_rx_late      = 0;
  unsigned              count_rx_other     = 0;

public:
  radio_notifier_spy(std::string log_level_) : logger(srslog::fetch_basic_logger("Radio notification"))
  {
    srslog::init();
    logger.set_level(srslog::str_to_basic_level(log_level_));
  }

  void on_radio_rt_event(const event_description& description) override
  {
    logger.warning("stream_id={} channel_id={} source={} type={}",
                   description.stream_id == UNKNOWN_ID ? "na" : std::to_string(description.stream_id),
                   description.channel_id == UNKNOWN_ID ? "na" : std::to_string(description.channel_id),
                   description.source.to_string(),
                   description.type.to_string());
    switch (description.type) {
      case event_type::UNDEFINED:
        // Ignore.
        break;
      case event_type::LATE:
        if (description.source == event_source::TRANSMIT) {
          count_tx_late++;
        } else {
          count_rx_late++;
        }
        break;
      case event_type::UNDERFLOW:
        if (description.source == event_source::TRANSMIT) {
          count_tx_underflow++;
        } else {
          count_rx_underflow++;
        }
        break;
      case event_type::OVERFLOW:
        if (description.source == event_source::TRANSMIT) {
          count_tx_overflow++;
        } else {
          count_rx_overflow++;
        }
        break;
      case event_type::OTHER:
        if (description.source == event_source::TRANSMIT) {
          count_tx_other++;
        } else {
          count_rx_other++;
        }
        break;
      case event_type::START_OF_BURST:
      case event_type::END_OF_BURST:
        // Ignore cases.
        break;
    }
  }

  void print()
  {
    printf("[TX] Overflow: %d; Late: %d Underflow: %d Other: %d\n",
           count_tx_overflow,
           count_tx_late,
           count_tx_underflow,
           count_tx_other);
    printf("[RX] Overflow: %d; Late: %d Underflow: %d Other: %d\n",
           count_rx_overflow,
           count_rx_late,
           count_rx_underflow,
           count_rx_other);
  }
};

} // namespace srsran
