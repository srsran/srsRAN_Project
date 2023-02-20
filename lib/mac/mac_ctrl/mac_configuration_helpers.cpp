/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
