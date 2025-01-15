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

#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"

namespace srsran {
namespace hal {

/// PDSCH encoder hardware accelerator factory.
class hw_accelerator_pdsch_enc_factory
{
public:
  virtual ~hw_accelerator_pdsch_enc_factory()                = default;
  virtual std::unique_ptr<hw_accelerator_pdsch_enc> create() = 0;
};

} // namespace hal
} // namespace srsran
