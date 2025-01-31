/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "demodulation_mapper_impl.h"
#include "evm_calculator_generic_impl.h"
#include "modulation_mapper_lut_impl.h"
#include "srsran/support/cpu_features.h"

#ifdef __x86_64__
#include "modulation_mapper_avx512_impl.h"
#endif // __x86_64__
#ifdef __aarch64__
#include "modulation_mapper_neon_impl.h"
#endif // __aarch64__

using namespace srsran;

namespace {

class modulation_mapper_factory_impl : public modulation_mapper_factory
{
public:
  std::unique_ptr<modulation_mapper> create() override
  {
#ifdef __x86_64__
    if (cpu_supports_feature(cpu_feature::avx512f) && cpu_supports_feature(cpu_feature::avx512bw) &&
        cpu_supports_feature(cpu_feature::avx512vbmi)) {
      return std::make_unique<modulation_mapper_avx512_impl>();
    }
#endif // __x86_64__
#ifdef __ARM_NEON
    if (cpu_supports_feature(cpu_feature::neon)) {
      return std::make_unique<modulation_mapper_neon_impl>();
    }
#endif // __ARM_NEON

    return std::make_unique<modulation_mapper_lut_impl>();
  }
};

class demodulation_mapper_factory_impl : public demodulation_mapper_factory
{
public:
  std::unique_ptr<demodulation_mapper> create() override { return std::make_unique<demodulation_mapper_impl>(); }
};

class evm_calculator_factory_impl : public evm_calculator_factory
{
public:
  evm_calculator_factory_impl(std::shared_ptr<modulation_mapper_factory> modulator_factory_) :
    modulator_factory(std::move(modulator_factory_))
  {
    srsran_assert(modulator_factory, "Invalid modulator mapper factory.");
  }

  std::unique_ptr<evm_calculator> create() override
  {
    return std::make_unique<evm_calculator_generic_impl>(modulator_factory->create());
  }

private:
  std::shared_ptr<modulation_mapper_factory> modulator_factory;
};

} // namespace

std::shared_ptr<modulation_mapper_factory> srsran::create_modulation_mapper_factory()
{
  return std::make_shared<modulation_mapper_factory_impl>();
}

std::shared_ptr<demodulation_mapper_factory> srsran::create_demodulation_mapper_factory()
{
  return std::make_shared<demodulation_mapper_factory_impl>();
}

std::shared_ptr<evm_calculator_factory> srsran::create_evm_calculator_factory()
{
  return std::make_shared<evm_calculator_factory_impl>(create_modulation_mapper_factory());
}
