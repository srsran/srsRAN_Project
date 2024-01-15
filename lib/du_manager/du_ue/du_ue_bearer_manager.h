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

#include "du_bearer.h"

namespace srsran {
namespace srs_du {

/// \brief Bearer container for a UE object in the DU manager.
class du_ue_bearer_manager
{
public:
  /// Add new SRB with specified SRB-Id and configuration.
  du_ue_srb& add_srb(srb_id_t srb_id, const rlc_config& rlc_cfg, optional<const mac_lc_config> mac_cfg = {});

  /// Add new bearer to list of bearers.
  void add_drb(std::unique_ptr<du_ue_drb> drb);

  std::unique_ptr<du_ue_drb> remove_drb(drb_id_t drb_id);

  const slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS>& srbs() const { return srbs_; }
  slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS>&       srbs() { return srbs_; }
  const std::map<drb_id_t, std::unique_ptr<du_ue_drb>>&      drbs() const { return drbs_; };

  optional<lcid_t> allocate_lcid() const;

private:
  slotted_id_table<srb_id_t, du_ue_srb, MAX_NOF_SRBS> srbs_;
  std::map<drb_id_t, std::unique_ptr<du_ue_drb>>      drbs_;
};

} // namespace srs_du
} // namespace srsran