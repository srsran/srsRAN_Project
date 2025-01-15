/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cu_up/cu_up_factory.h"
#include "cu_up_impl.h"

using namespace srsran;
using namespace srs_cu_up;

std::unique_ptr<cu_up_interface> srsran::srs_cu_up::create_cu_up(const cu_up_config&       cfg,
                                                                 const cu_up_dependencies& dependencies)
{
  return std::make_unique<cu_up>(cfg, dependencies);
}