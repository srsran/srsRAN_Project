
#include "mac_controller.h"
#include "ue_creation_procedure.h"
#include "ue_delete_procedure.h"
#include "ue_reconfiguration_procedure.h"

using namespace srsgnb;

mac_controller::mac_controller(mac_common_config_t& cfg_, mac_ul_configurer& ul_unit_, mac_dl_configurer& dl_unit_) :
  cfg(cfg_), logger(cfg.logger), ul_unit(ul_unit_), dl_unit(dl_unit_)
{
  std::fill(ue_index_to_rnti_map.begin(), ue_index_to_rnti_map.end(), INVALID_RNTI);
}

async_task<mac_ue_create_response_message> mac_controller::ue_create_request(const mac_ue_create_request_message& msg)
{
  // Launch UE create request procedure
  return launch_async<mac_ue_create_request_procedure>(msg, cfg, *this, ul_unit, dl_unit);
}

async_task<mac_ue_delete_response_message> mac_controller::ue_delete_request(const mac_ue_delete_request_message& msg)
{
  // Enqueue UE delete procedure
  return launch_async<mac_ue_delete_procedure>(msg, cfg, *this, ul_unit, dl_unit);
}

async_task<mac_ue_reconfiguration_response_message>
mac_controller::ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg)
{
  return launch_async<mac_ue_reconfiguration_procedure>(msg, cfg, ul_unit, dl_unit);
}

bool mac_controller::add_ue(rnti_t crnti, du_ue_index_t ue_index, du_cell_index_t cell_index)
{
  srsran_assert(crnti != INVALID_RNTI, "Invalid RNTI");
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);

  if (ue_index_to_rnti_map[ue_index] != INVALID_RNTI) {
    // ue_index already exists.
    return false;
  }

  if (ue_db.contains(crnti)) {
    // UE already existed with same rnti.
    return false;
  }

  // Create UE object
  ue_db.emplace(crnti);
  mac_ue_context& u = ue_db[crnti];
  u.rnti            = crnti;
  u.du_ue_index     = ue_index;
  u.pcell_idx       = cell_index;

  // Update UE index -> CRNTI map
  ue_index_to_rnti_map[ue_index] = crnti;
  return true;
}

void mac_controller::remove_ue(rnti_t rnti)
{
  if (not ue_db.contains(rnti)) {
    logger.warning("Failed to find rnti={:#x}", rnti);
    return;
  }
  logger.debug("Removing rnti={:#x}", rnti);
  ue_index_to_rnti_map[ue_db[rnti].du_ue_index] = INVALID_RNTI;
  ue_db.erase(rnti);
}

mac_ue_context* mac_controller::find_ue(du_ue_index_t ue_index)
{
  rnti_t rnti = ue_index_to_rnti_map[ue_index];
  return rnti == INVALID_RNTI ? nullptr : &ue_db[rnti];
}

mac_ue_context* mac_controller::find_by_rnti(rnti_t rnti)
{
  return ue_db.contains(rnti) ? &ue_db[rnti] : nullptr;
}
