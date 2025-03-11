/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_phy_capabilities.h"
#include "srsran/support/compiler.h"

using namespace srsran;

#ifndef PUSCH_PROCESSOR_MAX_NOF_LAYERS
#define PUSCH_PROCESSOR_MAX_NOF_LAYERS 1
#endif // PUSCH_PROCESSOR_MAX_NOF_LAYERS

pusch_processor_phy_capabilities srsran::get_pusch_processor_phy_capabilities()
{
  return {.max_nof_layers = PUSCH_PROCESSOR_MAX_NOF_LAYERS};
}
