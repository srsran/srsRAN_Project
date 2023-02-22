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

#include "srsran/mac/mac_configuration_helpers.h"

using namespace srsran;

sched_ue_creation_request_message
srsran::make_scheduler_ue_creation_request(const mac_ue_create_request_message& request)
{
  sched_ue_creation_request_message ret{};
  ret.ue_index = request.ue_index;
  ret.crnti    = request.crnti;
  ret.cfg      = request.sched_cfg;
  return ret;
}

sched_ue_reconfiguration_message
srsran::make_scheduler_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& request)
{
  sched_ue_reconfiguration_message ret{};
  ret.ue_index = request.ue_index;
  ret.crnti    = request.crnti;
  ret.cfg      = request.sched_cfg;
  return ret;
}
