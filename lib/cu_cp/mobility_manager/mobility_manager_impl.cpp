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

mobility_manager_impl::mobility_manager_impl(const mobility_manager_cfg&      cfg_,
                                             mobility_manager_cu_cp_notifier& cu_cp_notifier_) :
  cfg(cfg_), cu_cp_notifier(cu_cp_notifier_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void mobility_manager_impl::handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci)
{
  if (!cfg.trigger_handover_from_measurements) {
    logger.debug("ue_index={} - Ignoring better neighbor pci={}", ue_index, neighbor_pci);
    return;
  }

  // TODO: Lookup and verify PCI is valid in DU.
  cu_cp_notifier.on_inter_du_handover_request(ue_index, neighbor_pci);
}
