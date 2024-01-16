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

// Constructs a factory of spy components.
#define PHY_SPY_FACTORY_TEMPLATE(COMPONENT)                                                                            \
  class COMPONENT##_factory_spy : public COMPONENT##_factory                                                           \
  {                                                                                                                    \
  public:                                                                                                              \
    std::unique_ptr<COMPONENT> create() override                                                                       \
    {                                                                                                                  \
      std::unique_ptr<COMPONENT##_spy> spy = std::make_unique<COMPONENT##_spy>();                                      \
      entries.push_back(spy.get());                                                                                    \
      return spy;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    std::vector<COMPONENT##_spy*>& get_entries() { return entries; }                                                   \
                                                                                                                       \
  private:                                                                                                             \
    std::vector<COMPONENT##_spy*> entries;                                                                             \
  }
