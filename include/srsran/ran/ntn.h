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

#include "nr_cgi.h"
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

namespace srsran {

/// Satellite ephemeris in format of position and velocity state vector in ECEF.
struct ecef_coordinates_t {
  /// X coordinate of position state vector. Unit is meter.
  double position_x;
  /// Y coordinate of position state vector. Unit is meter.
  double position_y;
  /// Z coordinate of position state vector. Unit is meter.
  double position_z;
  /// X coordinate of velocity state vector. Unit is meter/second.
  double velocity_vx;
  /// Y coordinate of velocity state vector. Unit is meter/second.
  double velocity_vy;
  /// Z coordinate of velocity state vector. Unit is meter/second.
  double velocity_vz;
};

/// Satellite ephemeris in format of orbital parameters in ECI. See NIMA TR 8350.2.
struct orbital_coordinates_t {
  /// Semi major axis. Unit is meter.
  double semi_major_axis;
  /// Eccentricity. Unitless.
  double eccentricity;
  /// Argument of periapsis. Unit is radian.
  double periapsis;
  /// Longitude of ascending node. Unit is radian.
  double longitude;
  /// Mean anomaly M at epoch time. Unit is radian.
  double mean_anomaly;
  /// Inclination. Unit is radian.
  double inclination;
};

/// Timing advance information for NTN.
struct ta_info_t {
  /// Network-controlled common timing advanced value and it may include any timing offset considered necessary by the
  /// network. ta-Common with value of 0 is supported. Unit is us.
  double ta_common;
  /// Indicate drift rate of the common TA. Unit is us/s.
  double ta_common_drift;
  /// Indicate drift rate variation of the common TA. Unit is us/s^2.
  double ta_common_drift_variant;
  /// Constant offset added to the NTA-common broadcast in SIB19 to model fixed system delays independent of satellite
  /// position. Unit is us.
  double ta_common_offset;
};

/// EpochTime is used to indicate the epoch time for the NTN assistance information, and it is defined as the starting
/// time of a DL sub-frame, indicated by a SFN and a sub-frame number signaled together with the assistance information.
struct epoch_time_t {
  /// For serving cell, it indicates the current SFN or the next upcoming SFN after the frame where the message
  /// indicating the epochTime is received. For neighbour cell, it indicates the SFN nearest to the frame where the
  /// message indicating the epochTime is received.
  unsigned sfn;
  /// Sub-frame number within the SFN.
  unsigned subframe_number;
};

/// Parameters of the feeder link used to compute the Doppler shifts.
struct feeder_link_info_t {
  /// Flag to enable/disable doppler compensation for the feeder link.
  bool enable_doppler_compensation;
  /// Downlink frequency of the feeder link. Unit is Hz.
  double dl_freq;
  /// Uplink frequency of the feeder link. Unit is Hz.
  double ul_freq;
};

/// Geodetic coordinates of the NTN Gateway location.
struct geodetic_coordinates_t {
  /// Latitude. Unit is degree.
  double latitude;
  /// Longitude. Unit is degree.
  double longitude;
  /// Altitude. Unit is meter.
  double altitude;
};

/// Indicates polarization information for downlink/uplink transmission on service link.
struct ntn_polarization_t {
  enum class polarization_type { rhcp, lhcp, linear };
  /// If present, this parameter indicates polarization information for downlink transmission on service link.
  std::optional<polarization_type> dl;
  /// If present, this parameter indicates Polarization information for uplink service link.
  std::optional<polarization_type> ul;
};

struct ntn_config {
  /// SIB 19 values
  /// Reference location of the serving cell in geodetic coordinates format (in degrees).
  std::optional<geodetic_coordinates_t> reference_location;
  /// Distance from the serving cell reference location, as defined in TS 38.304. Each step represents 50m.
  std::optional<unsigned> distance_threshold;
  /// Indicates the time information on when a cell provided via NTN is going to stop serving the area it is currently
  /// covering. It represents a timestamp in ms unit of Unix time (UTC time since 1970-01-01)
  std::optional<uint64_t> t_service;
  /// NTN-config values
  /// Indicate the epoch time for the NTN assistance information passed in the config file.
  /// It represents a timestamp in ms unit of Unix time (UTC time since 1970-01-01)
  std::optional<uint64_t> epoch_timestamp;
  /// Optional offset (in SFN) between the SIB19 transmission slot and the epoch time (EpochTime IE) of the NTN
  /// assistance info. Allows sending NTN assistance information that will become valid epoch_sfn_offset number of
  /// system frames after SIB19 Tx slot.
  std::optional<uint64_t> epoch_sfn_offset;
  /// If provided it will be used to fill the EpochTime section in SIB19.
  std::optional<epoch_time_t> epoch_time;
  /// Scheduling offset used for the timing relationships that are modified for NTN (see TS 38.213). The unit of the
  /// field K_offset is number of slots for a given subcarrier spacing of 15 kHz.
  unsigned cell_specific_koffset;
  /// Scheduling offset provided by network if downlink and uplink frame timing are not aligned at gNB.
  std::optional<unsigned> k_mac;
  /// A validity duration configured by the network for assistance information which indicates the maximum time duration
  /// (from epochTime) during which the UE can apply assistance information without having acquired new assistance
  /// information. Values {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900} seconds.
  std::optional<unsigned> ntn_ul_sync_validity_dur;
  /// Whether to broadcast Ephemeris information as ECEF state vectors (if true) or ECI Orbital parameters (if false).
  /// If not provided, the value is derived from the variant of ephemeris_info.
  /// If provided and does not match the variant of ephemeris_info, the ephemeris_info is converted accordingly.
  std::optional<bool> use_state_vector;
  /// This field provides satellite ephemeris either in format of position and velocity state vector or in format of
  /// orbital parameters.
  std::variant<ecef_coordinates_t, orbital_coordinates_t> ephemeris_info;
  /// Network-controlled common timing advanced value, and it may include any timing offset considered necessary by the
  /// network.
  std::optional<ta_info_t> ta_info;
  /// Parameters of the feeder link used to compute the Doppler shifts.
  std::optional<feeder_link_info_t> feeder_link_info;
  /// Geodetic coordinates (in degrees) of the NTN Gateway location.
  std::optional<geodetic_coordinates_t> ntn_gateway_location;
  /// Indicates polarization information for downlink/uplink transmission on service link.
  std::optional<ntn_polarization_t> polarization;
  /// When this field is included in SIB19, it indicates reporting of timing advanced is enabled.
  std::optional<bool> ta_report;
  /// SIB19 scheduling information.
  unsigned                si_msg_idx;
  unsigned                si_period_rf;
  unsigned                si_window_len_slots;
  std::optional<unsigned> si_window_position;
  // NR-CGI
  nr_cell_global_id_t nr_cgi;
};

} // namespace srsran
