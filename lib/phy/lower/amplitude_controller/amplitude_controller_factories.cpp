/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "amplitude_controller_impl.h"

using namespace srsgnb;

namespace {

class amplitude_controller_factory_generic : public amplitude_controller_factory
{
public:
  std::unique_ptr<amplitude_controller>
  create_amplitude_controller(const amplitude_controller_configuration& config) override
  {
    srsgnb_assert(
        config.ceiling_dBFS <= 0.0F, "The amplitude ceiling ({} dBFS) cannot be above Full Scale", config.ceiling_dBFS);
    srsgnb_assert(
        config.full_scale_lin > 0.0F, "The Full Scale amplitude ({}) must be positive", config.full_scale_lin);
    return std::make_unique<amplitude_controller_impl>(config);
  }
};

} // namespace

std::shared_ptr<amplitude_controller_factory> srsgnb::create_amplitude_controller_factory()
{
  return std::make_shared<amplitude_controller_factory_generic>();
}
