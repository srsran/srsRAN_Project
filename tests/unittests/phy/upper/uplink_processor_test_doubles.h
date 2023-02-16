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

#include "srsgnb/phy/upper/uplink_processor.h"

namespace srsgnb {

class uplink_processor_spy : public uplink_processor
{
  bool has_process_prach_method_called = false;
  bool has_process_pusch_method_called = false;
  bool has_process_pucch_method_called = false;

public:
  void process_prach(upper_phy_rx_results_notifier& notifier,
                     const prach_buffer&            buffer,
                     const prach_buffer_context&    context) override
  {
    has_process_prach_method_called = true;
  }

  void process_pusch(span<uint8_t>                      data,
                     unique_rx_softbuffer               softbuffer,
                     upper_phy_rx_results_notifier&     notifier,
                     const resource_grid_reader&        grid,
                     const uplink_processor::pusch_pdu& pdu) override
  {
    has_process_pusch_method_called = true;
  }

  void process_pucch(upper_phy_rx_results_notifier& notifier,
                     const resource_grid_reader&    grid,
                     const pucch_pdu&               config) override
  {
    has_process_pucch_method_called = true;
  }

  bool is_process_prach_method_called() const { return has_process_prach_method_called; }

  bool is_process_pusch_method_called() const { return has_process_pusch_method_called; }

  bool is_process_pucch_method_called() const { return has_process_pucch_method_called; }
};

} // namespace srsgnb
