/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/signal_processors/srs/srs_estimator.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"

namespace srsran {

class srs_estimator_dummy : public srs_estimator
{
public:
  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override
  {
    return srs_estimator_result();
  }
};

} // namespace srsran
