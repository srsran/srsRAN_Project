/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  /// Determines if bearer with LCID is part of this slice.
  bool contains(lcid_t lcid) const { return bearers.test(lcid); }

  /// Fetch DU cell index of UE's PCell.
  const ue_cell& get_pcell() const { return u.get_pcell(); }

  /// Add LCID to the bearers of the UE belonging to this slice.
  void add_logical_channel(lcid_t lcid);

  /// Remove LCID from the bearers of the UE belonging to this slice.
  void rem_logical_channel(lcid_t lcid);

  /// \brief Checks if there are DL pending bytes that are yet to be allocated in a DL HARQ.
  /// This method is faster than computing \c pending_dl_newtx_bytes() > 0.
  /// \remark Excludes SRB0 and UE Contention Resolution Identity CE.
  bool has_pending_dl_newtx_bytes() const;

  /// \brief Computes the number of DL pending bytes in SRBs that are not already allocated in a DL HARQ.
  /// \return The number of DL pending bytes in SRBs that are not already allocated in a DL HARQ.
  /// \remark Excludes SRB0 pending bytes.
  unsigned pending_dl_srb_newtx_bytes() const;

  /// \brief Computes the number of DL pending bytes that are not already allocated in a DL HARQ.
  /// param[in] lcid If the LCID is provided, the method will return the number of pending bytes for that LCID.
  ///           Otherwise it will return the sum of all LCIDs pending bytes, excluding SRB0.
  /// \return The number of DL pending bytes that are not already allocated in a DL HARQ.
  unsigned pending_dl_newtx_bytes(lcid_t lcid = lcid_t::INVALID_LCID) const;

  /// \brief Checks if there are DL pending bytes in SRBs that are not already allocated in a DL HARQ.
  /// \remark Excludes SRB0 pending bytes.
  bool has_pending_dl_srb_newtx_bytes() const;

  /// \brief Computes the number of UL pending bytes in bearers belonging to this slice that are not already allocated
  /// in a UL HARQ.
  unsigned pending_ul_newtx_bytes() const;

  /// \brief Returns whether a SR indication handling is pending.
  bool has_pending_sr() const;

  /// \brief Computes the number of UL pending bytes in SRBs. The value is used to derive the required transport block
  /// size for an UL grant.
  /// \return The number of UL pending bytes in SRBs.
  /// \remark The returned UL pending bytes does not exclude already allocated bytes in UL HARQ(s).
  unsigned pending_ul_srb_newtx_bytes() const;

private:
  const ue&                        u;
  bounded_bitset<MAX_NOF_RB_LCIDS> bearers;
};

/// Container that store all UEs belonging to a slice.
using slice_ue_repository = slotted_id_table<du_ue_index_t, slice_ue, MAX_NOF_DU_UES>;

} // namespace srsran
