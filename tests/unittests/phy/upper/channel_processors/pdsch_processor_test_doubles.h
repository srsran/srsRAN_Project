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

#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include <thread>

namespace srsran {

/// Spy implementation of a PDSCH processor.
class pdsch_processor_spy : public pdsch_processor
{
  bool process_method_called = false;

public:
  void process(resource_grid_mapper&                                        mapper,
               pdsch_processor_notifier&                                    notifier,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                 pdu) override
  {
    process_method_called = true;
    notifier.on_finish_processing();
  }

  /// Returns true if the process method has been called, false otherwise.
  bool is_process_called() const { return process_method_called; }
};

class pdsch_processor_notifier_spy : public pdsch_processor_notifier
{
public:
  void on_finish_processing() override { finished = true; }

  void wait_for_finished()
  {
    while (!finished) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

  void reset() { finished = false; }

private:
  std::atomic<bool> finished = {};
};

} // namespace srsran
