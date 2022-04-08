
#include "mac_controller.h"
#include "../rach_handler.h"
#include "ue_creation_procedure.h"
#include "ue_delete_procedure.h"
#include "ue_reconfiguration_procedure.h"

using namespace srsgnb;

mac_controller::mac_controller(mac_common_config_t& cfg_,
                               mac_ul_configurer&   ul_unit_,
                               mac_dl_configurer&   dl_unit_,
                               du_rnti_table&       rnti_table_) :
  cfg(cfg_), logger(cfg.logger), ul_unit(ul_unit_), dl_unit(dl_unit_), rnti_table(rnti_table_)
{}

lazy_task<mac_ue_create_response_message> mac_controller::ue_create_request(const mac_ue_create_request_message& msg)
{
  // Launch UE create request procedure
  return launch_async<mac_ue_create_request_procedure>(msg, cfg, *this, ul_unit, dl_unit);
}

lazy_task<mac_ue_delete_response_message> mac_controller::ue_delete_request(const mac_ue_delete_request_message& msg)
{
  // Enqueue UE delete procedure
  return launch_async<mac_ue_delete_procedure>(msg, cfg, *this, ul_unit, dl_unit);
}

lazy_task<mac_ue_reconfiguration_response_message>
mac_controller::ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg)
{
  return launch_async<mac_ue_reconfiguration_procedure>(msg, cfg, ul_unit, dl_unit);
}

bool mac_controller::add_ue(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t cell_index)
{
  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue_index={}", ue_index);

  if (ue_db.contains(ue_index)) {
    // UE already existed with same rnti.
    return false;
  }

  // Create UE object.
  ue_db.emplace(ue_index);
  mac_ue_context& u = ue_db[ue_index];
  u.rnti            = crnti;
  u.du_ue_index     = ue_index;
  u.pcell_idx       = cell_index;

  // Update RNTI -> UE index map.
  rnti_table.add_ue(crnti, ue_index);

  return true;
}

void mac_controller::remove_ue(du_ue_index_t ue_index)
{
  if (not ue_db.contains(ue_index)) {
    logger.warning("Failed to find ue_index={:#x}", ue_index);
    return;
  }
  logger.debug("Removing ue_index={:#x}", ue_index);

  rnti_table.rem_ue(ue_db[ue_index].rnti);

  ue_db.erase(ue_index);
}

mac_ue_context* mac_controller::find_ue(du_ue_index_t ue_index)
{
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

mac_ue_context* mac_controller::find_by_rnti(rnti_t rnti)
{
  return find_ue(rnti_table[rnti]);
}
