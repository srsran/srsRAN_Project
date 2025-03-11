/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cu_cp/cu_cp_factory.h"
#include "cu_cp_impl.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

std::unique_ptr<srs_cu_cp::cu_cp> srsran::create_cu_cp(const srs_cu_cp::cu_cp_configuration& cfg_)
{
  return std::make_unique<srs_cu_cp::cu_cp_impl>(cfg_);
}
