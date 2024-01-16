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

#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "channel_equalizer_zf_impl.h"

using namespace srsran;

namespace {

class channel_equalizer_factory_zf : public channel_equalizer_factory
{
public:
  channel_equalizer_factory_zf() = default;

  std::unique_ptr<channel_equalizer> create() override { return std::make_unique<channel_equalizer_zf_impl>(); }
};
} // namespace

std::shared_ptr<channel_equalizer_factory> srsran::create_channel_equalizer_factory_zf()
{
  return std::make_shared<channel_equalizer_factory_zf>();
}
