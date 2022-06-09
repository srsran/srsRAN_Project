/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_CSI_RS_PROCESSOR_TEST_DOUBLES_H
#define SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_CSI_RS_PROCESSOR_TEST_DOUBLES_H

#include "srsgnb/phy/upper/signal_processors/csi_rs_processor.h"

namespace srsgnb {

/// Implementation of a CSI-RS processor for test only. It sets a variable to true when the process method is called.
class csi_rs_processor_fto : public csi_rs_processor
{
public:
  bool process_method_called = false;
  void map(resource_grid_writer& grid, const config_t& config) override { process_method_called = true; }
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_CSI_RS_PROCESSOR_TEST_DOUBLES_H
