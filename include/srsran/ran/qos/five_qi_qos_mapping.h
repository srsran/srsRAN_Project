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

#include "five_qi.h"
#include "srsran/adt/optional.h"

namespace srsran {

/// \brief Resource Type determines whether QoS Flow is of type Guaranteed Bit Rate (GBR), Delay-critical GBR or
/// non-GBR. See TS 23.501, clause 5.7.3.2 Resource Type.
enum class qos_flow_resource_type { gbr, non_gbr, delay_critical_gbr };

/// \brief Represents 5G QoS characteristics associated with 5QI.
struct qos_characteristics {
  qos_flow_resource_type res_type;
  /// The Priority Level associated with 5G QoS characteristics indicates a priority in scheduling resources among QoS
  /// Flows. The lowest Priority Level value corresponds to the highest priority. See TS 23.501, clause 5.7.3.3.
  uint8_t qos_priority_level;
  /// The Packet Delay Budget (PDB) defines an upper bound for the time that a packet may be delayed between the UE and
  /// the UPF that terminates the N6 interface. For a certain 5QI the value of the PDB is the same in UL and DL. See
  /// TS 23.501, clause 5.7.3.4.
  unsigned packet_delay_budget_ms;
  /// The Packet Error Rate (PER) defines an upper bound for the rate of PDUs (e.g. IP packets) that have been processed
  /// by the sender of a link layer protocol (e.g. RLC in RAN of a 3GPP access) but that are not successfully delivered
  /// by the corresponding receiver to the upper layer (e.g. PDCP in RAN of a 3GPP access). Thus, the PER defines an
  /// upper bound for a rate of non-congestion related packet losses. See TS 23.501, clause 5.7.3.5.
  double packet_error_rate;
  /// Each GBR QoS Flow shall be associated with an Averaging window. The Averaging window represents the duration over
  /// which the GFBR and MFBR shall be calculated (e.g. in the (R)AN, UPF, UE). See TS 23.501, clause 5.7.3.6.
  std::optional<unsigned> average_window_ms;
  /// Each GBR QoS Flow with Delay-critical resource type shall be associated with a Maximum Data Burst Volume (MDBV).
  /// MDBV denotes the largest amount of data that the 5G-AN is required to serve within a period of 5G-AN PDB (i.e.
  /// 5G-AN part of the PDB). See TS 23.501, clause 5.7.3.7.
  std::optional<unsigned> max_data_burst_volume_bytes;
};

/// \brief Returns the standardized 5QI to QoS characteristics mapping from TS 23.501, table 5.7.4-1 based on given 5QI.
/// \param[in] nof_layers Number of layers.
/// \return The standardized 5QI to QoS characteristics mapping from TS 23.501, table 5.7.4-1.
qos_characteristics get_5qi_to_qos_characteristics_mapping(five_qi_t five_qi);

} // namespace srsran
