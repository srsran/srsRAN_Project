/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "amplitude_controller_clipping_impl.h"
#include "amplitude_controller_scaling_impl.h"

using namespace srsran;

namespace {

class amplitude_controller_clipping_factory : public amplitude_controller_factory
{
public:
  std::unique_ptr<amplitude_controller> create() override
  {
    return std::make_unique<amplitude_controller_clipping_impl>(amplitude_controller_config.enable_clipping,
                                                                amplitude_controller_config.input_gain_dB,
                                                                amplitude_controller_config.full_scale_lin,
                                                                amplitude_controller_config.ceiling_dBFS);
  }

  explicit amplitude_controller_clipping_factory(const amplitude_controller_clipping_config& config) :
    amplitude_controller_config(config)
  {
    srsran_assert(
        config.ceiling_dBFS <= 0.0F, "The amplitude ceiling ({} dBFS) cannot be above Full Scale", config.ceiling_dBFS);
    srsran_assert(
        config.full_scale_lin > 0.0F, "The Full Scale amplitude ({}) must be positive", config.full_scale_lin);
  }

private:
  amplitude_controller_clipping_config amplitude_controller_config;
};

class amplitude_controller_scaling_factory : public amplitude_controller_factory
{
public:
  std::unique_ptr<amplitude_controller> create() override
  {
    return std::make_unique<amplitude_controller_scaling_impl>(gain_dB);
  }

  explicit amplitude_controller_scaling_factory(float gain_dB_) : gain_dB(gain_dB_) {}

private:
  float gain_dB;
};

} // namespace

std::shared_ptr<amplitude_controller_factory>
srsran::create_amplitude_controller_clipping_factory(const amplitude_controller_clipping_config& config)
{
  return std::make_shared<amplitude_controller_clipping_factory>(config);
}

std::shared_ptr<amplitude_controller_factory> srsran::create_amplitude_controller_scaling_factory(float gain_dB_)
{
  return std::make_shared<amplitude_controller_scaling_factory>(gain_dB_);
}
