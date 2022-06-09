/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_SSB_PROCESSOR_TEST_DOUBLES_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_SSB_PROCESSOR_TEST_DOUBLES_H

#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"

namespace srsgnb {

/// Implementation of a SSB processor for test only. It sets a variable to true when the process method is called.
class ssb_processor_fto : public ssb_processor
{
public:
  bool process_method_called = false;
  void process(const pdu_t& pdu, resource_grid_writer& grid) override
  {
    process_method_called = true;
  }
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_SSB_PROCESSOR_TEST_DOUBLES_H
