/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "low_papr_sequence_collection_impl.h"
#include "low_papr_sequence_generator_impl.h"
#include "pseudo_random_generator_impl.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

namespace {

class low_papr_sequence_collection_sw_factory : public low_papr_sequence_collection_factory
{
public:
  low_papr_sequence_collection_sw_factory(std::shared_ptr<low_papr_sequence_generator_factory>& lpg_factory_) :
    lpg_factory(std::move(lpg_factory_))
  {
    srsgnb_assert(lpg_factory, "Invalid generator factory.");
  }

  std::unique_ptr<low_papr_sequence_collection> create(unsigned m, unsigned delta, span<const float> alphas) override
  {
    std::unique_ptr<low_papr_sequence_generator> generator = lpg_factory->create();
    srsgnb_assert(generator, "Invalid generator.");

    return std::make_unique<low_papr_sequence_collection_impl>(*generator, m, delta, alphas);
  }

private:
  std::shared_ptr<low_papr_sequence_generator_factory> lpg_factory;
};

class low_papr_sequence_generator_sw_factory : public low_papr_sequence_generator_factory
{
public:
  std::unique_ptr<low_papr_sequence_generator> create() override
  {
    return std::make_unique<low_papr_sequence_generator_impl>();
  }
};

class pseudo_random_generator_sw_factory : public pseudo_random_generator_factory
{
public:
  std::unique_ptr<pseudo_random_generator> create() override
  {
    return std::make_unique<pseudo_random_generator_impl>();
  }
};

} // namespace

std::shared_ptr<low_papr_sequence_collection_factory>
srsran::create_low_papr_sequence_collection_sw_factory(std::shared_ptr<low_papr_sequence_generator_factory> lpg_factory)
{
  return std::make_shared<low_papr_sequence_collection_sw_factory>(lpg_factory);
}

std::shared_ptr<low_papr_sequence_generator_factory> srsran::create_low_papr_sequence_generator_sw_factory()
{
  return std::make_shared<low_papr_sequence_generator_sw_factory>();
}

std::shared_ptr<pseudo_random_generator_factory> srsran::create_pseudo_random_generator_sw_factory()
{
  return std::make_shared<pseudo_random_generator_sw_factory>();
}
