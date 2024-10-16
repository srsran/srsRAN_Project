/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"

using namespace srsran;
using namespace hal;

SRSRAN_WEAK_SYMB std::shared_ptr<hw_accelerator_pusch_dec_factory>
srsran::hal::create_bbdev_pusch_dec_acc_factory(const bbdev_hwacc_pusch_dec_factory_configuration& accelerator_config)
{
  return nullptr;
}
