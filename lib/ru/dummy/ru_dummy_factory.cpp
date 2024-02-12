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

#include "srsran/ru/ru_dummy_factory.h"
#include "ru_dummy_impl.h"
#include "srsran/ru/ru.h"
#include <memory>

using namespace srsran;

std::unique_ptr<radio_unit> srsran::create_dummy_ru(const ru_dummy_configuration& config,
                                                    ru_dummy_dependencies&        dependencies)
{
  return std::make_unique<ru_dummy_impl>(config, dependencies);
}