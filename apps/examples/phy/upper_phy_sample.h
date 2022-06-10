/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_APPS_EXAMPLES_PHY_UPPER_PHY_SAMPLE_H_
#define SRSGNB_APPS_EXAMPLES_PHY_UPPER_PHY_SAMPLE_H_

#include "srsgnb/phy/upper/channel_processors/pdcch_processor.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_processor.h"
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"
#include "srsgnb/phy/upper/signal_processors/csi_rs_processor.h"

namespace srsgnb {

/// Sample implementation of a upper PHY processor.
class upper_phy_sample : public csi_rs_processor, public pdcch_processor, public pdsch_processor, public ssb_processor
{};

/// Groups upper PHY sample configuration parameters.
struct upper_phy_sample_configuration {
  /// General upper PHY logging level.
  std::string log_level;
  /// Specifies the LDPC encoder type.
  std::string ldpc_encoder_type;
};

/// Creates an upper PHY sample.
std::unique_ptr<upper_phy_sample> create_upper_phy_sample(const upper_phy_sample_configuration& config);

} // namespace srsgnb
#endif // SRSGNB_APPS_EXAMPLES_PHY_UPPER_PHY_SAMPLE_H_
