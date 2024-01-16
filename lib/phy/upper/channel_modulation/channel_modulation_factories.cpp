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

#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "demodulation_mapper_impl.h"
#include "evm_calculator_generic_impl.h"
#include "modulation_mapper_impl.h"

using namespace srsran;

namespace {

class channel_modulation_sw_factory : public channel_modulation_factory
{
public:
  std::unique_ptr<modulation_mapper> create_modulation_mapper() override
  {
    return std::make_unique<modulation_mapper_impl>();
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