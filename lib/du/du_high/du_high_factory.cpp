/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/du/du_high/du_high_factory.h"
#include "du_high_impl.h"

using namespace srsran;

std::unique_ptr<du_high> srsran::make_du_high(const srs_du::du_high_configuration& du_hi_cfg)
{
  return std::make_unique<srs_du::du_high_impl>(du_hi_cfg);
}
