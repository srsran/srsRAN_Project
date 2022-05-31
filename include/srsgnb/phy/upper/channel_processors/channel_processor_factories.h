/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_CHANNEL_PROCESSOR_FACTORIES_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_CHANNEL_PROCESSOR_FACTORIES_H

#include "srsgnb/phy/upper/channel_processors/pusch_decoder.h"
#include "srsgnb/phy/upper/channel_processors/pusch_demodulator.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"

namespace srsgnb {

struct pusch_decoder_configuration {
  // TBD.
};

std::unique_ptr<pusch_decoder> create_pusch_decoder(pusch_decoder_configuration& config);

/// Describes the necessary parameters for creating a PUSCH processor.
struct pusch_processor_configuration {
  /// Channel estimator instance. Ownership is transferred to the processor.
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  /// Demodulator instance. Ownership is transferred to the processor.
  std::unique_ptr<pusch_demodulator> demodulator;
  /// Decoder instance. Ownership is transferred to the processor.
  std::unique_ptr<pusch_decoder> decoder;
};

/// Creates a generic PUSCH processor.
std::unique_ptr<pusch_processor> create_pusch_processor(pusch_processor_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_CHANNEL_PROCESSOR_FACTORIES_H
