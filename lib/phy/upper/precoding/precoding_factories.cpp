/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/upper/precoding/precoding_factories.h"
#include "channel_precoder_generic.h"

using namespace srsran;

namespace {

class channel_precoder_factory_impl : public channel_precoder_factory
{
public:
  channel_precoder_factory_impl() = default;

  std::unique_ptr<channel_precoder> create() override { return std::make_unique<channel_precoder_generic>(); }
};

} // namespace

std::shared_ptr<channel_precoder_factory> srsran::create_channel_precoder_factory()
{
  return std::make_shared<channel_precoder_factory_impl>();
}