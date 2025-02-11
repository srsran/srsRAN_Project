/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
