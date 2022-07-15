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
#include "demodulation_mapper_impl.h"
#include "modulation_mapper_impl.h"

using namespace srsgnb;

namespace {

class channel_modulation_sw_factory : public channel_modulation_factory
{
public:
  std::unique_ptr<modulation_mapper> create_modulation_mapper() override
  {
    return std::make_unique<modulation_mapper_impl>();
  }
  std::unique_ptr<demodulation_mapper> create_demodulation_mapper() override
  {
    return std::make_unique<demodulation_mapper_impl>();
  }
};

} // namespace

std::shared_ptr<channel_modulation_factory> srsgnb::create_channel_modulation_sw_factory()
{
  return std::make_shared<channel_modulation_sw_factory>();
}