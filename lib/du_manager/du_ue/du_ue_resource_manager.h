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

#include "cell_group_config.h"

namespace srsgnb {
namespace srs_du {

struct ue_cell_resource {
  static_vector<pucch_resource, MAX_NOF_PUCCH_RESOURCES>                  pucch_res_list;
  static_vector<scheduling_request_resource_config, MAX_NOF_SR_RESOURCES> sr_res_list;
};

void fill_cell_group_resources(cell_group_config& cell_group_cfg, const ue_cell_resource& res);

/// \brief List of Cell Resources, indexed by UE cell index, associated with a given UE.
using ue_cell_resource_list = static_vector<const ue_cell_resource*, MAX_NOF_DU_CELLS>;

/// \brief This class manages (allocates + deallocates) the cell PHY and MAC resources for UE cells.
class du_cell_resource_allocator
{
public:
  virtual ~du_cell_resource_allocator()                                                                  = default;
  virtual ue_cell_resource_list update_resources(du_ue_index_t ue_index, span<du_cell_index_t> ue_cells) = 0;
};

/// \brief This class manages the cell PHY and MAC resources of a UE.
class du_ue_resource_manager
{
public:
  du_ue_resource_manager(du_ue_index_t ue_index_, du_cell_resource_allocator& cell_res_mng_) :
    ue_index(ue_index_), cell_res_mng(&cell_res_mng_)
  {
  }
  ~du_ue_resource_manager() { cell_res_mng->update_resources(ue_index, {}); }

  /// \brief Update UE cell resources. This function handles both allocation of new resources for new or modified UE
  /// cells, and deallocation of resources of removed cells.
  ///
  /// \param cell_indexes list of UE cells indexed by ue_cell_index (PCell is index 0).
  /// \return True if all Cells were allocated successfully.
  bool set_cells(span<du_cell_index_t> cell_indexes);

  bool contains(du_cell_index_t cell_index) const
  {
    return du_cell_to_ue_cell_idx[cell_index] != INVALID_UE_CELL_INDEX;
  }
  bool contains(ue_cell_index_t ue_cell_index) const { return ue_cells[ue_cell_index] != nullptr; }

  const ue_cell_resource& operator[](du_cell_index_t cell_index) const
  {
    return *ue_cells[du_cell_to_ue_cell_idx[cell_index]];
  }
  const ue_cell_resource& operator[](ue_cell_index_t ue_cell_idx) const { return *ue_cells[ue_cell_idx]; }

private:
  void update_du_cell_ue_cell_index_map(span<du_cell_index_t> cell_indexes);

  du_ue_index_t                                 ue_index;
  du_cell_resource_allocator*                   cell_res_mng;
  ue_cell_resource_list                         ue_cells;
  std::array<ue_cell_index_t, MAX_NOF_DU_CELLS> du_cell_to_ue_cell_idx;
};

} // namespace srs_du
} // namespace srsgnb