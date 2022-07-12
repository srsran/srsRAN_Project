/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_creation_procedure.h"
#include "../adapters/rrc_adapters.h"
#include "srsgnb/rrc/rrc_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

ue_creation_procedure::ue_creation_procedure(ue_index_t                     ue_index_candidate,
                                             du_cell_index_t                pcell_index,
                                             const f1ap_initial_ul_rrc_msg& init_ul_rrc_msg,
                                             const cu_cp_manager_config_t&  cfg_,
                                             ue_manager_ctrl_configurer&    ue_mng_) :
  cfg(cfg_), logger(cfg.logger), msg(init_ul_rrc_msg), ue_mng(ue_mng_)
{
  ue_ctx.ue_index               = ue_index_candidate;
  ue_ctx.pcell_index            = pcell_index;
  ue_ctx.c_rnti                 = to_rnti(init_ul_rrc_msg.msg->c_rnti.value);
  const auto& du_to_cu_rrc_cont = init_ul_rrc_msg.msg->duto_currc_container.value;
  ue_ctx.du_to_cu_rrc_container = byte_buffer{du_to_cu_rrc_cont.begin(), du_to_cu_rrc_cont.end()};
}

void ue_creation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  log_proc_started(logger, ue_ctx.ue_index, ue_ctx.c_rnti, "UE Create");

  // 1. Verify if UE index was successfully allocated and params are valid.
  if (ue_ctx.ue_index == MAX_NOF_UES) {
    log_proc_failure(logger, MAX_NOF_UES, ue_ctx.c_rnti, name(), "Failure to allocate UE index.");
    CORO_EARLY_RETURN();
  }
  if (ue_mng.find_rnti(ue_ctx.c_rnti) != nullptr) {
    log_proc_failure(logger, MAX_NOF_UES, ue_ctx.c_rnti, name(), "Repeated RNTI.");
    CORO_EARLY_RETURN();
  }

  // 2. Create RRC
  rrc_ue_cfg_t                   rrc_cfg; // TODO: fill or pass as param
  rrc_ue_entity_creation_message rrc_ue_msg(ue_ctx, rrc_cfg);
  ue_ctx.rrc = create_rrc_ue_entity(rrc_ue_msg);

  // 3. Add UE to manager
  if (ue_mng.add_ue(std::move(ue_ctx)) == nullptr) {
    log_proc_failure(logger, ue_ctx.ue_index, ue_ctx.c_rnti, "UE failed to be created.");
    clear_ue();
    CORO_EARLY_RETURN();
  }

  // 4. Create SRB0 bearer and notifier
  create_srb0();

  // 5. Pass container to RRC
  if (msg.msg->rrc_container_rrc_setup_complete_present) {
    // check that SRB1 is present
    if (ue_ctx.srbs.contains(LCID_SRB0)) {
      ue_ctx.srbs[LCID_SRB1].rx_notifier->on_new_pdu(
          byte_buffer_slice({msg.msg->rrc_container_rrc_setup_complete.value.begin(),
                             msg.msg->rrc_container_rrc_setup_complete.value.end()}));
    } else {
      cfg.logger.error("SRB1 not present - dropping PDU");
    }
  } else {
    // pass UL-CCCH to RRC
    ue_ctx.srbs[LCID_SRB0].rx_notifier->on_new_pdu(
        byte_buffer_slice({msg.msg->rrc_container.value.begin(), msg.msg->rrc_container.value.end()}));
  }

  log_proc_completed(logger, ue_ctx.ue_index, ue_ctx.c_rnti, "UE Create");
  CORO_RETURN();
}

void ue_creation_procedure::create_srb0()
{
  ue_ctx.srbs.emplace(LCID_SRB0);
  cu_srb_context& srb0 = ue_ctx.srbs[LCID_SRB0];
  srb0.lcid            = LCID_SRB0;

  // create UE manager to RRC adapter
  srb0.rx_notifier = std::make_unique<rrc_ul_ccch_adapter>(*ue_ctx.rrc->get_ul_ccch_pdu_handler());
}