/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/e2/e2sm/e2sm_factory.h"
#include "e2sm_kpm/e2sm_kpm_asn1_packer.h"

using namespace srsran;

std::unique_ptr<e2sm_handler> srsran::create_e2sm(std::string e2sm_version, e2sm_kpm_meas_provider& meas_provider)
{
  if (e2sm_version == "E2SM-KPM") {
    return std::make_unique<e2sm_kpm_asn1_packer>(meas_provider);
  }
  return nullptr;
}