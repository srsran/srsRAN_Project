/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_PROCESSOR_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_PROCESSOR_IMPL_H

#include "srsgnb/phy/upper/channel_processors/pusch_decoder.h"
#include "srsgnb/phy/upper/channel_processors/pusch_demodulator.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include <memory>

namespace srsgnb {

class pusch_processor_impl : public pusch_processor
{
public:
  /// Describes the necessary parameters for creating a PUSCH processor.
  struct configuration {
    /// Channel estimator instance. Ownership is transferred to the processor.
    std::unique_ptr<dmrs_pusch_estimator> estimator;
    /// Demodulator instance. Ownership is transferred to the processor.
    std::unique_ptr<pusch_demodulator> demodulator;
    /// Decoder instance. Ownership is transferred to the processor.
    std::unique_ptr<pusch_decoder> decoder;
  };

  pusch_processor_impl(configuration& config);

  void
  process(span<uint8_t> data, rx_softbuffer& softbuffer, const resource_grid_reader& grid, const pdu_t& pdu) override;
};

} // namespace srsgnb
#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_PROCESSOR_IMPL_H
