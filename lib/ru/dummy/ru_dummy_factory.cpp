/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ru/dummy/ru_dummy_factory.h"
#include "ru_dummy_impl.h"
#include "srsran/ru/ru.h"
#include <memory>

using namespace srsran;

std::unique_ptr<radio_unit> srsran::create_dummy_ru(const ru_dummy_configuration& config,
                                                    ru_dummy_dependencies&        dependencies)
{
  return std::make_unique<ru_dummy_impl>(config, dependencies);
}
