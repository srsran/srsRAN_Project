/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../ue_scheduling/ue.h"

namespace srsran {

class slice_ue
{
public:
  explicit slice_ue(const ue& u);

  /// Returns DU UE index.
  du_ue_index_t ue_index() const { return u.ue_index; }

  /// Returns number of cells configured for the UE.
  unsigned nof_cells() const { return u.nof_cells(); }

  /// Returns UE C-RNTI.
  rnti_t crnti() const { return u.crnti; }

  /// \brief Fetch UE cell based on DU cell index.
  const ue_cell* find_cell(du_cell_index_t cell_index) const { return u.find_cell(cell_index); }

  /// \brief Fetch UE cell based on UE-specific cell identifier. E.g. PCell corresponds to ue_cell_index==0.
  const ue_cell& get_cell(ue_cell_index_t ue_cell_index) const { return u.get_cell(ue_cell_index); }

  /// Determines if at least one bearer of the UE is part of this slice.
  bool has_bearers_in_slice() const { return bearers.any(); }

  /// Determines if at least one SRB bearer of the UE is part of this slice.
  bool has_srb_bearers_in_slice() const
  {
    return contains(LCID_SRB0) or contains(LCID_SRB1) or contains(LCID_SRB2) or contains(LCID_SRB3);
  }

  /// Fetches the bitmap of bearers belonging to this slice.
  const bounded_bitset<MAX_NOF_RB_LCIDS>& get_bearers() const { return bearers; }

  /// Determines if bearer with LCID is part of this slice.
  bool contains(lcid_t lcid) const { return bearers.test(lcid); }

  /// Fetch DU cell index of UE's PCell.
  const ue_cell& get_pcell() const { return u.get_pcell(); }

  /// Add LCID to the bearers of the UE belonging to this slice.
  void add_logical_channel(lcid_t lcid, lcg_id_t lcg_id);

  /// Remove LCID from the bearers of the UE belonging to this slice.
  void rem_logical_channel(lcid_t lcid);

  /// \brief Checks if there are DL pending bytes that are yet to be allocated in a DL HARQ.
  /// This method is faster than computing \c pending_dl_newtx_bytes() > 0.
  /// \remark Excludes SRB0 and UE Contention Resolution Identity CE.
  bool has_pending_dl_newtx_bytes() const;

  /// \brief Computes the number of DL pending bytes that are not already allocated in a DL HARQ.
  /// \return The number of DL pending bytes that are not already allocated in a DL HARQ.
  unsigned pending_dl_newtx_bytes() const;

  /// \brief Computes the number of UL pending bytes in bearers belonging to this slice that are not already allocated
  /// in a UL HARQ.
  unsigned pending_ul_newtx_bytes() const;

  /// \brief Returns whether a SR indication handling is pending.
  bool has_pending_sr() const;

private:
  /// Helper function to get LCG ID of a bearer.
  lcg_id_t get_lcg_id_for_bearer(lcid_t lcid) const;

  const ue&                        u;
  bounded_bitset<MAX_NOF_RB_LCIDS> bearers;
  /// LCG IDs of bearers belonging to this slice.
  bounded_bitset<MAX_NOF_LCGS> lcg_ids;
};

/// Container that store all UEs belonging to a slice.
using slice_ue_repository = slotted_id_table<du_ue_index_t, slice_ue, MAX_NOF_DU_UES>;

} // namespace srsran
