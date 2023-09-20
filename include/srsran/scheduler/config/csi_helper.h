/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ran/csi_rs/csi_meas_config.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {
namespace csi_helper {

/// \brief Parameters used to generate CSI Meas Config.
struct csi_builder_params {
  /// PCI of the cell that will determine the scrambling.
  pci_t pci;
  /// \brief Number of RBs used for the CSI-RS. The csi config generators will find the closest number of RBs to
  /// the one provided that is a multiple of 4 as required by the TS 38.331, "CSI-FrequencyOccupation".
  unsigned nof_rbs;
  /// Number of ports set for the CSI-RS.
  unsigned nof_ports = 1;
  /// Period of the CSI-RS resources.
  csi_resource_periodicity csi_rs_period = csi_resource_periodicity::slots80;
  /// Slot offset for measurement CSI-RS resources. Note: Should avoid collisions with SSB and SIB1.
  unsigned meas_csi_slot_offset = 2;
  /// Slot offset of the first CSI-RS resource used for tracking. Note: Should avoid collisions with SSB and SIB1.
  unsigned tracking_csi_slot_offset = 12;
  /// Slot offset for zp-CSI-RS resources. Note: Should avoid collisions with SSB and SIB1.
  unsigned zp_csi_slot_offset = 2;
  /// Report slot offset.
  unsigned csi_report_slot_offset = 9;
};

/// \brief Compute default CSI-RS signalling period to use, while constrained by TS38.214, 5.1.6.1.1.
csi_resource_periodicity get_max_csi_rs_period(subcarrier_spacing pdsch_scs);

/// Checks whether a specified CSI-RS period is valid for a given TDD pattern.
SRSRAN_NODISCARD bool is_csi_rs_period_valid(csi_resource_periodicity       csi_rs_period,
                                             const tdd_ul_dl_config_common& tdd_cfg);

/// \brief Searches for a valid CSI-RS periodicity, while constrained by TDD pattern periodicity.
optional<csi_resource_periodicity> find_valid_csi_rs_period(const tdd_ul_dl_config_common& tdd_cfg);

/// \brief Search for valid CSI-RS slot offsets for measurement, tracking and interference management.
/// \remark TODO: This function assumes that the SSB and SIB1 configs are hardcoded to slot offsets 0 and 1.
///
/// \param csi_params [in/out] Parameters used to generate CSI Meas Config. This function assumes that the CSI-RS
/// period, is already set.
/// \param meas_csi_slot_offset [in] Slot offset for measurement CSI-RS resources. If passed as empty, a new value is
/// derived. If passed as non-empty, the function will check whether the value is valid.
/// \param tracking_csi_slot_offset [in] Slot offset for tracking CSI-RS resources. If passed as empty, a new value is
/// derived. If passed as non-empty, the function will check whether the value is valid.
/// \param zp_csi_slot_offset [in] Slot offset for IM CSI-RS resources. If passed as empty, a new value is derived.
/// If passed as non-empty, the function will check whether the value is valid.
/// \param tdd_cfg [in] TDD pattern.
SRSRAN_NODISCARD bool derive_valid_csi_rs_slot_offsets(csi_builder_params&            csi_params,
                                                       const optional<unsigned>&      meas_csi_slot_offset,
                                                       const optional<unsigned>&      tracking_csi_slot_offset,
                                                       const optional<unsigned>&      zp_csi_slot_offset,
                                                       const tdd_ul_dl_config_common& tdd_cfg);

/// \brief Generate list of zp-CSI-RS Resources.
std::vector<zp_csi_rs_resource> make_periodic_zp_csi_rs_resource_list(const csi_builder_params& params);

/// \brief Generate set of periodic zp-CSI-RS Resources.
zp_csi_rs_resource_set make_periodic_zp_csi_rs_resource_set(const csi_builder_params& params);

/// \brief Generate wideband NZP-CSI-RS Resource List, composed by nzp-CSI-RS resources for tracking and channel
/// measurements.
std::vector<nzp_csi_rs_resource> make_nzp_csi_rs_resource_list(const csi_builder_params& params);

/// \brief Generate CSI-MeasConfig.
csi_meas_config make_csi_meas_config(const csi_builder_params& params);

} // namespace csi_helper
} // namespace srsran
