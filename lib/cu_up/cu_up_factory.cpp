/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cu_up/cu_up_factory.h"
#include "cu_up_impl.h"

using namespace srsran;

std::unique_ptr<srs_cu_up::cu_up_interface> srsran::create_cu_up(const srs_cu_up::cu_up_configuration& cfg_)
{
  return std::make_unique<srs_cu_up::cu_up>(cfg_);
}