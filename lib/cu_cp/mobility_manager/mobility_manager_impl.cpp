/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mobility_manager_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

mobility_manager_impl::mobility_manager_impl(const mobility_manager_cfg& cfg_,
                                             du_repository&              du_db_,
                                             du_processor_ue_manager&    ue_mng_) :
  cfg(cfg_), du_db(du_db_), ue_mng(ue_mng_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void mobility_manager_impl::handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci)
{
  if (!cfg.trigger_handover_from_measurements) {
    logger.debug("ue_index={} - Ignoring better neighbor pci={}", ue_index, neighbor_pci);
    return;
  }

  // Try to find target DU.
  du_index_t target_du = du_db.find_du(neighbor_pci);
  if (target_du == du_index_t::invalid) {
    logger.debug("No local DU/cell with pci={} found. Requesting Inter CU handover.");
    handle_inter_cu_handover(ue_index, neighbor_pci);
    return;
  }

  du_index_t source_du = ue_mng.find_du_ue(ue_index)->get_du_index();
  if (target_du == source_du) {
    logger.info("Trigger IntraDU handover.");
    // TODO: Prepare request and call notifier.
    return;
  } else {
    logger.info(
        "Trigger Inter DU handover for ue_index={} from source_du={} to target_du={}", ue_index, source_du, target_du);
    handle_inter_du_handover(ue_index, neighbor_pci, source_du, target_du);
  }
}

void mobility_manager_impl::handle_inter_du_handover(ue_index_t ue_index,
                                                     pci_t      neighbor_pci,
                                                     du_index_t source_du,
                                                     du_index_t target_du)
{
  cu_cp_inter_du_handover_request request = {};
  request.ue_index                        = ue_index;
  request.neighbor_pci                    = neighbor_pci;

  // Lookup F1AP notifier of target DU.
  du_processor_f1ap_ue_context_notifier& target_du_f1ap_notifier =
      du_db.get_du(target_du).get_f1ap_ue_context_notifier();

  // Trigger Inter DU handover routine on the DU processor of the source DU.
  du_db.get_du(source_du).get_mobility_interface().handle_inter_du_handover_request(request, target_du_f1ap_notifier);
}

void mobility_manager_impl::handle_intra_du_handover(ue_index_t ue_index, pci_t neighbor_pci, du_index_t du)
{
  // TODO: prepare call
}

void mobility_manager_impl::handle_inter_cu_handover(ue_index_t ue_index, pci_t neighbor_pci)
{
  // TODO: prepare NGAP call
}
