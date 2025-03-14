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

#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/unique_rx_buffer.h"

namespace srsran {

class pusch_processor_spy : public pusch_processor
{
public:
  void process(span<uint8_t>                    data,
               unique_rx_buffer                 buffer,
               pusch_processor_result_notifier& notifier,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu) override
  {
    processed_method_been_called = true;

    // Notify completion of PUSCH UCI.
    if ((pdu.uci.nof_harq_ack != 0) || (pdu.uci.nof_csi_part1 != 0)) {
      notifier.on_uci({});
    }

    // Notify completion of PUSCH data.
    notifier.on_sch({});
  }

  bool has_process_method_been_called() const { return processed_method_been_called; }

private:
  bool processed_method_been_called = false;
};

} // namespace srsran
