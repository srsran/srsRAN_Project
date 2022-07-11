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

#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {

class pseudo_random_generator_factory
{
public:
  virtual ~pseudo_random_generator_factory()                = default;
  virtual std::unique_ptr<pseudo_random_generator> create() = 0;
};

std::shared_ptr<pseudo_random_generator_factory> create_pseudo_random_generator_sw_factory();

} // namespace srsgnb
