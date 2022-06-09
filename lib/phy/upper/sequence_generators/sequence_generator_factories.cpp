/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "pseudo_random_generator_impl.h"

using namespace srsgnb;

namespace {

class pseudo_random_generator_sw_factory : public pseudo_random_generator_factory
{
public:
  std::unique_ptr<pseudo_random_generator> create() override
  {
    return std::make_unique<pseudo_random_generator_impl>();
  }
};

} // namespace

std::shared_ptr<pseudo_random_generator_factory> srsgnb::create_pseudo_random_generator_sw_factory()
{
  return std::make_shared<pseudo_random_generator_sw_factory>();
}