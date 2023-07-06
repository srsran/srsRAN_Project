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

mobility_manager_impl::mobility_manager_impl(const mobility_manager_cfg& cfg_) :
  cfg(cfg_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void mobility_manager_impl::handle_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) {}
