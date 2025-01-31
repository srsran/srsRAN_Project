/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsran/phy/upper/channel_modulation/evm_calculator.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include <memory>

namespace srsran {

class modulation_mapper_factory
{
public:
  virtual ~modulation_mapper_factory()                = default;
  virtual std::unique_ptr<modulation_mapper> create() = 0;
};

std::shared_ptr<modulation_mapper_factory> create_modulation_mapper_factory();

class demodulation_mapper_factory
{
public:
  virtual ~demodulation_mapper_factory()                = default;
  virtual std::unique_ptr<demodulation_mapper> create() = 0;
};

std::shared_ptr<demodulation_mapper_factory> create_demodulation_mapper_factory();

class evm_calculator_factory
{
public:
  virtual ~evm_calculator_factory()                = default;
  virtual std::unique_ptr<evm_calculator> create() = 0;
};

std::shared_ptr<evm_calculator_factory> create_evm_calculator_factory();

} // namespace srsran
