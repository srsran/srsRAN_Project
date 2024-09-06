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

#include "du_bearer.h"

namespace srsran {
namespace srs_du {

/// \brief Bearer container for a UE object in the DU manager.
class du_ue_bearer_manager
{
public:
  /// Add new SRB with specified SRB-Id and configuration.
  du_ue_srb& add_srb(srb_id_t srb_id);

  /// Add new bearer to list of bearers.
  void add_drb(std::unique_ptr<du_ue_drb> drb);

  std::unique_ptr<du_ue_drb> remove_drb(drb_id_t drb_id);

  const slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS>& srbs() const { return srbs_; }
  slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS>&       srbs() { return srbs_; }
  const std::map<drb_id_t, std::unique_ptr<du_ue_drb>>&      drbs() const { return drbs_; };

  std::optional<lcid_t> allocate_lcid() const;

private:
  slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS> srbs_;
  std::map<drb_id_t, std::unique_ptr<du_ue_drb>>      drbs_;
};

} // namespace srs_du
} // namespace srsran
