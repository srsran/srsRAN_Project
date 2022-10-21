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

#include "srsgnb/phy/upper/channel_processors/ulsch_demultiplex.h"

namespace srsgnb {

class ulsch_demultiplex_impl : public ulsch_demultiplex
{
public:
  // See interface for documentation.
  void demultiplex(span<log_likelihood_ratio>       sch_data,
                   span<log_likelihood_ratio>       harq_ack,
                   span<log_likelihood_ratio>       csi_part1,
                   span<log_likelihood_ratio>       csi_part2,
                   span<const log_likelihood_ratio> input,
                   const configuration&             config) override;
};

} // namespace srsgnb
