/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
