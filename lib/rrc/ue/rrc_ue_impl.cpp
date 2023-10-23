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

#include "rrc_ue_impl.h"
#include "rrc_ue_helpers.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::rrc_nr;

rrc_ue_impl::rrc_ue_impl(up_resource_manager&             up_resource_mng_,
                         rrc_ue_du_processor_notifier&    du_proc_notif_,
                         rrc_pdu_f1ap_notifier&           f1ap_pdu_notifier_,
                         rrc_ue_nas_notifier&             nas_notif_,
                         rrc_ue_control_notifier&         ngap_ctrl_notif_,
                         rrc_ue_reestablishment_notifier& cu_cp_notif_,
                         cell_meas_manager&               cell_meas_mng_,
                         const ue_index_t                 ue_index_,
                         const rnti_t                     c_rnti_,
                         const rrc_cell_context           cell_,
                         const rrc_ue_cfg_t&              cfg_,
                         const byte_buffer                du_to_cu_container_,
                         rrc_ue_task_scheduler&           task_sched_,
                         bool&                            reject_users_,
                         bool                             is_inter_cu_handover_) :
  context(ue_index_, c_rnti_, cell_, cfg_, is_inter_cu_handover_),
  up_resource_mng(up_resource_mng_),
  du_processor_notifier(du_proc_notif_),
  f1ap_pdu_notifier(f1ap_pdu_notifier_),
  nas_notifier(nas_notif_),
  ngap_ctrl_notifier(ngap_ctrl_notif_),
  cu_cp_notifier(cu_cp_notif_),
  cell_meas_mng(cell_meas_mng_),
  du_to_cu_container(du_to_cu_container_),
  task_sched(task_sched_),
  reject_users(reject_users_),
  logger(cfg_.logger),
  event_mng(std::make_unique<rrc_ue_event_manager>(task_sched_.get_timer_factory()))
{
  // TODO: Use task_sched to schedule RRC procedures.
  (void)task_sched;
  srsran_assert(context.cell.bands.empty() == false, "Band must be present in RRC cell configuration.");
  srsran_assert(context.cfg.rrc_procedure_timeout_ms > 0, "RRC procedure timeout cannot be zero.");
}

void rrc_ue_impl::create_srb(const srb_creation_message& msg)
{
  logger.debug("ue={} Creating {}.", msg.ue_index, msg.srb_id);

  // create adapter objects and PDCP bearer as needed
  if (msg.srb_id == srb_id_t::srb0) {
    // SRB0 is already set up
    return;
  } else if (msg.srb_id <= srb_id_t::srb2) {
    // create PDCP entity for this SRB
    context.srbs.emplace(std::piecewise_construct,
                         std::forward_as_tuple(msg.srb_id),
                         std::forward_as_tuple(msg.ue_index, msg.srb_id, task_sched.get_timer_factory()));
    auto& srb_context = context.srbs.at(msg.srb_id);

    if (msg.srb_id == srb_id_t::srb2) {
      security::sec_as_config sec_cfg = context.sec_context.get_as_config(security::sec_domain::rrc);
      srb_context.enable_security(security::truncate_config(sec_cfg));
    }

  } else {
    logger.error("Couldn't create SRB{}.", msg.srb_id);
  }
}

static_vector<srb_id_t, MAX_NOF_SRBS> rrc_ue_impl::get_srbs()
{
  static_vector<srb_id_t, MAX_NOF_SRBS> srb_ids;
  for (const auto& srb_ctxt : context.srbs) {
    srb_ids.push_back(srb_ctxt.first);
  }

  return srb_ids;
}

void rrc_ue_impl::on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg)
{
  send_dl_ccch(dl_ccch_msg);
}

void rrc_ue_impl::on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg)
{
  send_dl_dcch(srb_id, dl_dcch_msg);
}

void rrc_ue_impl::on_new_as_security_context()
{
  srsran_sanity_check(context.srbs.find(srb_id_t::srb1) != context.srbs.end(),
                      "Attempted to configure security, but there is no interface to PDCP");

  context.srbs.at(srb_id_t::srb1).enable_security(context.sec_context.get_128_as_config(security::sec_domain::rrc));
  context.security_enabled = true;
}

async_task<bool> rrc_ue_impl::handle_init_security_context(const security::security_context& sec_ctx)
{
  context.sec_context = sec_ctx;
  //  Launch RRC security mode procedure
  return launch_async<rrc_security_mode_command_procedure>(context, sec_ctx, *this, *event_mng, logger);
}

byte_buffer rrc_ue_impl::get_packed_handover_preparation_message()
{
  struct ho_prep_info_s ho_prep;
  ho_prep_info_ies_s&   ies = ho_prep.crit_exts.set_c1().set_ho_prep_info();

  if (not context.capabilities_list.has_value()) {
    return {}; // no capabilities present, return empty buffer.
  }
  ies.ue_cap_rat_list = *context.capabilities_list;

  // TODO fill source and as configs.
  return pack_into_pdu(ho_prep, "handover preparation info");
}

void rrc_ue_impl::on_ue_release_required(const cause_t& cause)
{
  // FIXME: this enqueues a new CORO on top of an existing one.
  cu_cp_ue_context_release_command msg = {};
  msg.ue_index                         = context.ue_index;
  msg.cause                            = cause;

  task_sched.schedule_async_task(launch_async([this, msg](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT_VALUE(cu_cp_ue_context_release_complete ignore,
                     du_processor_notifier.on_ue_context_release_command(msg));

    CORO_RETURN();
  }));
}
