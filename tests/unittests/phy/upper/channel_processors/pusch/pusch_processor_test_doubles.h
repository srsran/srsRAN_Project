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
