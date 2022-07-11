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

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include <memory>

namespace srsgnb {

class modulation_mapper_factory
{
public:
  virtual ~modulation_mapper_factory()                = default;
  virtual std::unique_ptr<modulation_mapper> create() = 0;
};

std::shared_ptr<modulation_mapper_factory> create_modulation_mapper_sw_factory();

} // namespace srsgnb
