/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "modulation_mapper_impl.h"

using namespace srsgnb;

namespace {

class modulation_mapper_sw_factory : public modulation_mapper_factory
{
public:
  std::unique_ptr<modulation_mapper> create() override { return std::make_unique<modulation_mapper_impl>(); }
};

} // namespace

std::shared_ptr<modulation_mapper_factory> srsgnb::create_modulation_mapper_sw_factory()
{
  return std::make_shared<modulation_mapper_sw_factory>();
}