/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/du/du_high/du_high_factory.h"
#include "du_high_impl.h"

using namespace srsran;
using namespace srs_du;

std::unique_ptr<du_high> srsran::srs_du::make_du_high(const du_high_configuration& du_hi_cfg,
                                                      const du_high_dependencies&  dependencies)
{
  return std::make_unique<du_high_impl>(du_hi_cfg, dependencies);
}
