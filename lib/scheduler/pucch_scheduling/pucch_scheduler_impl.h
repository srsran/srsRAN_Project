/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid.h"
#include "../ue_scheduling/ue_configuration.h"
#include "pucch_scheduler.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

class pucch_scheduler_impl final : public pucch_scheduler
{
public:
  explicit pucch_scheduler_impl(const cell_configuration& cell_cfg_);
  ~pucch_scheduler_impl() override;

  void slot_indication(slot_point sl_tx);

  ul_pucch_info* alloc_pucch_ue(cell_slot_resource_allocator& slot_alloc,
                                pdcch_dl_information&         dl_alloc_info,
                                rnti_t                        rnti,
                                const ue_cell_configuration&  user,
                                bwp_id_t                      bwpid) override;

private:
  const cell_configuration& cell_cfg;
};

} // namespace srsgnb
