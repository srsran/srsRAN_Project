/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
    srsran_assert(lpg_factory, "Invalid generator factory.");
  }

  std::unique_ptr<low_papr_sequence_collection> create(unsigned m, unsigned delta, span<const float> alphas) override
  {
    std::unique_ptr<low_papr_sequence_generator> generator = lpg_factory->create();
    srsran_assert(generator, "Invalid generator.");

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
