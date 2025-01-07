/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

class channel_modulation_sw_factory : public channel_modulation_factory
{
public:
  std::unique_ptr<modulation_mapper> create_modulation_mapper() override
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
  std::unique_ptr<demodulation_mapper> create_demodulation_mapper() override
  {
    return std::make_unique<demodulation_mapper_impl>();
  }
  std::unique_ptr<evm_calculator> create_evm_calculator() override
  {
    return std::make_unique<evm_calculator_generic_impl>(create_modulation_mapper());
  }
};

} // namespace

std::shared_ptr<channel_modulation_factory> srsran::create_channel_modulation_sw_factory()
{
  return std::make_shared<channel_modulation_sw_factory>();
}