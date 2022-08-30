/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_scheduler_impl.h"

namespace srsgnb {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pucch_scheduler_impl::pucch_scheduler_impl(const cell_configuration& cell_cfg_) : cell_cfg(cell_cfg_) {}

ul_pucch_info* pucch_scheduler_impl::alloc_pucch_ue(cell_slot_resource_allocator& slot_alloc,
                                                    pdcch_dl_information&         dl_alloc_info,
                                                    rnti_t                        rnti,
                                                    const ue_cell_configuration&  user,
                                                    bwp_id_t                      bwpid)
{
  return NULL;
}

} // namespace srsgnb