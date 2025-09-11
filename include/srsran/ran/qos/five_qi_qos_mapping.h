/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/qos/five_qi.h"
#include "srsran/ran/qos/packet_error_rate.h"
#include "srsran/ran/qos/qos_prio_level.h"
#include <optional>

namespace srsran {

/// \brief Resource Type determines whether QoS Flow is of type Guaranteed Bit Rate (GBR), Delay-critical GBR or
/// non-GBR. See TS 23.501, clause 5.7.3.2 Resource Type.
enum class qos_flow_resource_type { gbr, non_gbr, delay_critical_gbr };

/// Maximum QoS Priority Level as per TS 38.471.
constexpr uint8_t MAX_QOS_PRIORITY_LEVEL = 127;

/// \brief Represents 5G QoS characteristics associated with a standardized 5QI, as per TS 23.501 5.7.4-1.
struct standardized_qos_characteristics {
  qos_flow_resource_type res_type;
  /// The Priority Level associated with 5G QoS characteristics indicates a priority in scheduling resources among QoS
  /// Flows. The lowest Priority Level value corresponds to the highest priority. See TS 23.501, clause 5.7.3.3.
  qos_prio_level_t priority;
  /// The Packet Delay Budget (PDB) defines an upper bound for the time that a packet may be delayed between the UE and
  /// the UPF that terminates the N6 interface. For a certain 5QI the value of the PDB is the same in UL and DL. See
  /// TS 23.501, clause 5.7.3.4.
  unsigned packet_delay_budget_ms;
  /// The Packet Error Rate (PER) as defined in TS 23.501, 5.7.3.5.
  packet_error_rate_t per;
  /// Each GBR QoS Flow shall be associated with an Averaging window. The Averaging window represents the duration over
  /// which the GFBR and MFBR shall be calculated (e.g. in the (R)AN, UPF, UE). See TS 23.501, clause 5.7.3.6.
  std::optional<unsigned> average_window_ms;
  /// Each GBR QoS Flow with Delay-critical resource type shall be associated with a Maximum Data Burst Volume (MDBV).
  /// MDBV denotes the largest amount of data that the 5G-AN is required to serve within a period of 5G-AN PDB (i.e.
  /// 5G-AN part of the PDB). See TS 23.501, clause 5.7.3.7.
  std::optional<uint16_t> max_data_burst_volume;

  bool operator==(const standardized_qos_characteristics& rhs) const
  {
    return res_type == rhs.res_type && priority == rhs.priority &&
           packet_delay_budget_ms == rhs.packet_delay_budget_ms && per == rhs.per &&
           average_window_ms == rhs.average_window_ms && max_data_burst_volume == rhs.max_data_burst_volume;
  }
};

/// \brief Returns the standardized 5QI to QoS characteristics mapping from TS 23.501, table 5.7.4-1 based on given 5QI.
/// \param[in] nof_layers Number of layers.
/// \return The standardized 5QI to QoS characteristics mapping from TS 23.501, table 5.7.4-1. Returns nullptr if the
/// 5QI is not present in the table.
const standardized_qos_characteristics* get_5qi_to_qos_characteristics_mapping(five_qi_t five_qi);

} // namespace srsran
