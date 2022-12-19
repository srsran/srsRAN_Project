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
#include "srsgnb/f1ap/du/f1ap_ue_context_update.h"

namespace srsgnb {
namespace srs_du {

/// \brief This class manages (allocates + deallocates) the cell PHY, MAC and RLC resources for UE cells.
class du_ran_resource_allocator
{
public:
  virtual ~du_ran_resource_allocator() = default;

  /// \brief Updates the resources (PCell + SCells) used by a UE.
  ///
  /// \param ue_index Index of the DU UE.
  /// \param pcell_index DU Cell Index of the UE's PCell.
  /// \param upd_req UE Context Update Request for a given UE.
  /// \return cell group resource config allocated for the UE if configuration was successful. Nullptr, otherwise.
  virtual const cell_group_config* update_context(du_ue_index_t                         ue_index,
                                                  du_cell_index_t                       pcell_index,
                                                  const f1ap_ue_context_update_request& upd_req) = 0;
};

/// \brief This class manages the cell PHY and MAC resources of a UE.
class du_ue_resource_manager
{
public:
  du_ue_resource_manager(du_ue_index_t ue_index_, du_ran_resource_allocator& cell_res_mng_);
  du_ue_resource_manager(du_ue_resource_manager&&)                 = delete;
  du_ue_resource_manager(const du_ue_resource_manager&)            = delete;
  du_ue_resource_manager& operator=(du_ue_resource_manager&&)      = delete;
  du_ue_resource_manager& operator=(const du_ue_resource_manager&) = delete;
  ~du_ue_resource_manager();

  bool empty() const { return ue_res == nullptr; }

  /// \brief Update UE cell resources. This function handles both allocation of new resources for new or modified UE
  /// cells, and deallocation of resources of removed cells.
  ///
  /// \param upd_req UE Context Update Request for a given UE.
  /// \return True if allocation of resources was successful.
  bool update_context(du_cell_index_t pcell_index, const f1ap_ue_context_update_request& upd_req);

  bool contains_cell(du_cell_index_t cell_index) const
  {
    return du_cell_to_ue_cell_idx[cell_index] != SERVING_CELL_INVALID;
  }

  const cell_group_config& value() const { return *ue_res; }
  const cell_group_config& operator*() const { return *ue_res; }
  const cell_group_config* operator->() const { return ue_res; }

private:
  void update_du_cell_ue_cell_index_map(du_cell_index_t pcell_index, const f1ap_ue_context_update_request& upd_req);

  du_ue_index_t              ue_index;
  du_ran_resource_allocator* cell_res_mng;

  const cell_group_config* ue_res = nullptr;

  std::array<serv_cell_index_t, MAX_NOF_DU_CELLS> du_cell_to_ue_cell_idx;
};

} // namespace srs_du
} // namespace srsgnb