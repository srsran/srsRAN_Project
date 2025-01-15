/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/upper/signal_processors/prs/factories.h"
#include "prs_generator_impl.h"
#include "prs_generator_validator_impl.h"

using namespace srsran;

namespace {

/// Implements a generic PRS generator factory.
class prs_generator_factory_generic_impl : public prs_generator_factory
{
public:
  prs_generator_factory_generic_impl(std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                     std::shared_ptr<channel_precoder_factory>        precoder_factory_) :
    prg_factory(std::move(prg_factory_)), precoder_factory(std::move(precoder_factory_))
  {
    srsran_assert(prg_factory, "Invalid pseudo-random sequence generator factory.");
    srsran_assert(precoder_factory, "Invalid channel precoder factory.");
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator> create() override
  {
    return std::make_unique<prs_generator_impl>(prg_factory->create(), precoder_factory->create());
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator_validator> create_validator() override
  {
    return std::make_unique<prs_generator_validator_impl>();
  }

private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<channel_precoder_factory>        precoder_factory;
};

} // namespace

std::shared_ptr<prs_generator_factory>
srsran::create_prs_generator_generic_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                             std::shared_ptr<channel_precoder_factory>        precoder_factory)
{
  return std::make_shared<prs_generator_factory_generic_impl>(std::move(prg_factory), std::move(precoder_factory));
}
