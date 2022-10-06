/*
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/resource_grid.h"
#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "../ue_scheduling/ue.h"
#include "../ue_scheduling/ue_scheduler.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

struct ue_pdsch_grant {
  const ue*         user;
  du_cell_index_t   cell_index;
  harq_id_t         h_id;
  search_space_id   ss_id;
  unsigned          time_res_index;
  crb_interval      crbs;
  dci_dl_format     dci_fmt;
  aggregation_level aggr_lvl = aggregation_level::n4;
};

struct ue_pusch_grant {
  const ue*         user;
  du_cell_index_t   cell_index;
  harq_id_t         h_id;
  crb_interval      crbs;
  ofdm_symbol_range symbols;
  unsigned          k2;
  search_space_id   ss_id    = to_search_space_id(1);
  aggregation_level aggr_lvl = aggregation_level::n4;
};

/// Allocator of PDSCH grants for UE RLC data.
class ue_pdsch_allocator
{
public:
  virtual ~ue_pdsch_allocator() = default;

  virtual bool allocate_pdsch(const ue_pdsch_grant& grant) = 0;

  virtual const cell_slot_resource_grid& dl_resource_grid(du_cell_index_t cell_index, unsigned k0) const = 0;
};

/// Allocator of PUSCH grants for UE RLC data.
class ue_pusch_allocator
{
public:
  virtual ~ue_pusch_allocator() = default;

  virtual bool allocate_pusch(const ue_pusch_grant& grant) = 0;

  virtual const cell_slot_resource_grid& ul_resource_grid(du_cell_index_t cell_index, unsigned k2) const = 0;
};

} // namespace srsgnb
