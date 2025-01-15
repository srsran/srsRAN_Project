/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "channel_equalizer_generic_impl.h"

using namespace srsran;

namespace {

class channel_equalizer_generic_factory : public channel_equalizer_factory
{
public:
  channel_equalizer_generic_factory(channel_equalizer_algorithm_type type_) : type(type_) {}

  std::unique_ptr<channel_equalizer> create() override
  {
    return std::make_unique<channel_equalizer_generic_impl>(type);
  }

private:
  channel_equalizer_algorithm_type type;
};

} // namespace

std::shared_ptr<channel_equalizer_factory>
srsran::create_channel_equalizer_generic_factory(channel_equalizer_algorithm_type type)
{
  return std::make_shared<channel_equalizer_generic_factory>(type);
}
