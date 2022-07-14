/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_manager.h"
#include "adapters/ue_manager_adapters.h"
#include "srsgnb/rrc/rrc_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

ue_manager::ue_manager(cu_cp_manager_config_t& cfg_) : cfg(cfg_), logger(cfg.logger)
{
  std::fill(rnti_to_ue_index.begin(), rnti_to_ue_index.end(), -1);
}

ue_context* ue_manager::find_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_index);
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

ue_context* ue_manager::find_rnti(rnti_t rnti)
{
  if (rnti_to_ue_index[rnti % MAX_NOF_UES] < 0) {
    return nullptr;
  }
  return &ue_db[rnti_to_ue_index[rnti % MAX_NOF_UES]];
}

ue_context* ue_manager::add_ue(ue_context ue_ctx)
{
  srsran_assert(ue_ctx.ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_ctx.ue_index);

  if (ue_db.contains(ue_ctx.ue_index)) {
    // UE already existed with same ue_index
    return nullptr;
  }

  // Create UE object
  ue_index_t ue_index = ue_ctx.ue_index;
  ue_db.emplace(ue_index, std::move(ue_ctx));
  auto& u = ue_db[ue_index];

  // Update RNTI -> UE index map
  srsran_sanity_check(rnti_to_ue_index[u.c_rnti % MAX_NOF_UES] < 0, "Invalid RNTI=0x{:x}", u.c_rnti);
  rnti_to_ue_index[u.c_rnti % MAX_NOF_UES] = ue_index;

  return &u;
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ueId={}", ue_index);
  logger.debug("Scheduling ueId={} deletion", ue_index);

  srsran_assert(ue_db.contains(ue_index), "Remove UE called for inexistent ueId={}", ue_index);
  ue_db.erase(ue_index);
  logger.info("Removed ueId={}", ue_index);
  return;
}

ue_index_t ue_manager::get_next_ue_index()
{
  // Search unallocated UE index
  for (int i = 0; i < MAX_NOF_UES; i++) {
    if (not ue_db.contains(i)) {
      return int_to_ue_index(i);
      break;
    }
  }
  return INVALID_UE_INDEX;
}

size_t ue_manager::get_nof_ues()
{
  return ue_db.size();
}

ue_index_t ue_manager::handle_initial_ul_rrc_message_transfer(const ue_manager_initial_ul_rrc_message& msg)
{
  ue_context ue_ctx{};
  ue_ctx.ue_index               = get_next_ue_index();
  ue_ctx.pcell_index            = msg.pcell_index;
  ue_ctx.c_rnti                 = msg.c_rnti;
  const auto& du_to_cu_rrc_cont = msg.du_to_cu_rrc_container;
  ue_ctx.du_to_cu_rrc_container = byte_buffer{du_to_cu_rrc_cont.begin(), du_to_cu_rrc_cont.end()};

  // 1. Verify if UE index was successfully allocated and params are valid.
  if (ue_ctx.ue_index == INVALID_UE_INDEX) {
    logger.error("Failure to allocate UE index.");
    return INVALID_UE_INDEX;
  }
  if (find_rnti(ue_ctx.c_rnti) != nullptr) {
    logger.error("Repeated RNTI.");
    return INVALID_UE_INDEX;
  }

  // 2. Create RRC
  rrc_ue_cfg_t                   rrc_cfg; // TODO: fill or pass as param
  rrc_ue_entity_creation_message rrc_ue_msg(ue_ctx, rrc_cfg);
  ue_ctx.rrc = create_rrc_ue_entity(rrc_ue_msg);

  // 3. Add UE to manager
  if (add_ue(std::move(ue_ctx)) == nullptr) {
    logger.error("UE failed to be created.");
    clear_ue();
    return INVALID_UE_INDEX;
  }

  // 4. Create SRB0 bearer and notifier
  create_srb0(ue_ctx);

  // 5. Pass container to RRC
  if (msg.rrc_container_rrc_setup_complete.has_value()) {
    // check that SRB1 is present
    if (ue_ctx.srbs.contains(LCID_SRB0)) {
      ue_ctx.srbs[LCID_SRB1].rx_notifier->on_new_pdu(byte_buffer_slice(
          {msg.rrc_container_rrc_setup_complete.value().begin(), msg.rrc_container_rrc_setup_complete.value().end()}));
    } else {
      cfg.logger.error("SRB1 not present - dropping PDU");
    }
  } else {
    // pass UL-CCCH to RRC
    ue_ctx.srbs[LCID_SRB0].rx_notifier->on_new_pdu(
        byte_buffer_slice({msg.rrc_container.begin(), msg.rrc_container.end()}));
  }

  logger.info("UE Created (ue_index={}, c-rnti={})", ue_ctx.ue_index, ue_ctx.c_rnti);
  return ue_ctx.ue_index;
}

void ue_manager::handle_ul_rrc_message_transfer(const ue_manager_ul_rrc_message& msg)
{
  logger.info("Handling UL RRC Message transfer.");

  // Convert RRCContainer to byte_buffer
  byte_buffer pdcp_pdu = make_byte_buffer(msg.rrc_container.to_string());
  // TODO: Send pdcp_pdu to PDCP
}

void ue_manager::create_srb0(ue_context& ue_ctx)
{
  ue_ctx.srbs.emplace(LCID_SRB0);
  cu_srb_context& srb0 = ue_ctx.srbs[LCID_SRB0];
  srb0.lcid            = LCID_SRB0;

  // create UE manager to RRC adapter
  srb0.rx_notifier = std::make_unique<rrc_ul_ccch_adapter>(*ue_ctx.rrc->get_ul_ccch_pdu_handler());
}