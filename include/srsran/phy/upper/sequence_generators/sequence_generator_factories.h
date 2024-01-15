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

#pragma once

#include "srsran/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include <memory>

namespace srsran {

class low_papr_sequence_generator_factory;

class low_papr_sequence_collection_factory
{
public:
  virtual ~low_papr_sequence_collection_factory() = default;
  virtual std::unique_ptr<low_papr_sequence_collection>
  create(unsigned m, unsigned delta, span<const float> alphas) = 0;
};

std::shared_ptr<low_papr_sequence_collection_factory>
create_low_papr_sequence_collection_sw_factory(std::shared_ptr<low_papr_sequence_generator_factory> lpg_factory);

class low_papr_sequence_generator_factory
{
public:
  virtual ~low_papr_sequence_generator_factory()                = default;
  virtual std::unique_ptr<low_papr_sequence_generator> create() = 0;
};

std::shared_ptr<low_papr_sequence_generator_factory> create_low_papr_sequence_generator_sw_factory();

class pseudo_random_generator_factory
{
public:
  virtual ~pseudo_random_generator_factory()                = default;
  virtual std::unique_ptr<pseudo_random_generator> create() = 0;
};

std::shared_ptr<pseudo_random_generator_factory> create_pseudo_random_generator_sw_factory();

} // namespace srsran
