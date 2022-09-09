/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_reconfiguration_procedure.h"
#include "../../ran/gnb_format.h"
#include "srsgnb/mac/mac_ue_configurator.h"

using namespace srsgnb::srs_du;

ue_reconfiguration_procedure::ue_reconfiguration_procedure(const du_ue_config_update_request& request_,
                                                           ue_manager_ctrl_configurer&        ue_mng_) :
  request(request_), ue_mng(ue_mng_), ue(ue_mng.find_ue(request.ue_index))
{
  srsgnb_assert(ue != nullptr, "ueId={} not found", request.ue_index);
}

void ue_reconfiguration_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  log_proc_started(logger, request.ue_index, ue->rnti, "UE Modification");

  add_bearers_in_ue_context();

  update_mac_bearers();

  log_proc_completed(logger, request.ue_index, ue->rnti, "UE Modification");

  CORO_RETURN();
}

void ue_reconfiguration_procedure::add_bearers_in_ue_context()
{
  for (srb_id_t srbid : request.srbs_to_addmod) {
    lcid_t lcid = (lcid_t)srbid;
    srsgnb_assert(not ue->bearers.contains(lcid), "Reconfigurations of bearers not supported");
    ue->bearers.emplace(lcid);
    du_logical_channel_context& srb = ue->bearers[lcid];
    srb.lcid                        = lcid;
  }
}

void ue_reconfiguration_procedure::update_mac_bearers() {}
