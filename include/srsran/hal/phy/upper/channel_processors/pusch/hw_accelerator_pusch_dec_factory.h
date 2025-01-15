/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"

namespace srsran {
namespace hal {

/// PUSCH decoder hardware accelerator factory.
class hw_accelerator_pusch_dec_factory
{
public:
  virtual ~hw_accelerator_pusch_dec_factory()                = default;
  virtual std::unique_ptr<hw_accelerator_pusch_dec> create() = 0;
};

} // namespace hal
} // namespace srsran
