/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_processors/pusch_decoder.h"
#include "srsgnb/phy/upper/channel_processors/pusch_demodulator.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include "srsgnb/support/srsgnb_test.h"

namespace srsgnb {
struct pusch_processor_configuration {
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  std::unique_ptr<pusch_demodulator>    demodulator;
  std::unique_ptr<pusch_decoder>        decoder;
};
std::unique_ptr<pusch_processor> create_pusch_processor(pusch_processor_configuration& config);
} // namespace srsgnb

using namespace srsgnb;

int main()
{
  pusch_processor_configuration processor_config = {};

  std::unique_ptr<pusch_processor> processor = create_pusch_processor(processor_config);
  TESTASSERT(processor);
}
