/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/e2/e2sm/e2sm_factory.h"
#include "e2sm_kpm_asn1_packer.h"

using namespace srsran;

std::unique_ptr<e2sm_handler> srsran::create_e2sm(std::string e2sm_version)
{
  if (e2sm_version == "E2SM-KPM") {
    return std::make_unique<e2sm_kpm_asn1_packer>();
  }
  return nullptr;
}