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

#include "srsran/adt/static_vector.h"
#include "srsran/fapi/messages/base_message.h"
#include "srsran/fapi/messages/srs_pdu_report_type.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"
#include "srsran/ran/srs/srs_channel_matrix.h"
#include "srsran/ran/srs/srs_configuration.h"
#include <optional>

namespace srsran {
namespace fapi {

/// Encodes the PRGs.
struct srs_prg {
  uint8_t rb_snr;
};

/// Encodes the reported symbols for the beamforming report.
struct srs_reported_symbols {
  /// Maximum number of PRGs.
  static constexpr unsigned MAX_NUM_PRGS = 273;

  uint16_t                          num_prg;
  std::array<srs_prg, MAX_NUM_PRGS> prgs;
};

/// Encodes the beamforming report.
struct srs_beamforming_report {
  /// Maximum number of reported symbols.
  static constexpr unsigned MAX_NUM_REP_SYMBOLS = 4;

  uint16_t                                              prg_size;
  uint8_t                                               num_symbols;
  uint8_t                                               wideband_snr;
  uint8_t                                               num_reported_symbols;
  std::array<srs_reported_symbols, MAX_NUM_REP_SYMBOLS> rep_symbols;
};

/// Encodes the normalized IQ representation.
enum class normalized_iq_representation : uint8_t { normal, extended };

/// Encodes the normalized channel IQ matrix report.
struct srs_normalized_channel_iq_matrix {
  // :TODO: The real value is way too big to represent it ~4M.
  static constexpr unsigned MAX_MATRIX_H_SIZE = 1024;

  normalized_iq_representation           iq_representation;
  uint16_t                               num_gnb_antenna_elements;
  uint16_t                               num_ue_srs_ports;
  uint16_t                               prg_size;
  uint16_t                               num_prgs;
  std::array<uint8_t, MAX_MATRIX_H_SIZE> channel_matrix_h;
};

/// Encodes the SVD array for each PRG.
struct srs_svd_prg {
  static constexpr unsigned MAX_SIZE_LEFT_EIGENVECTOR  = 256;
  static constexpr unsigned MAX_SIZE_SINGULAR_MATRIX   = 32;
  static constexpr unsigned MAX_SIZE_RIGHT_EIGENVECTOR = 8736;

  std::array<uint8_t, MAX_SIZE_LEFT_EIGENVECTOR>  left_eigenvectors;
  std::array<uint8_t, MAX_SIZE_SINGULAR_MATRIX>   singular_matrix;
  std::array<uint8_t, MAX_SIZE_RIGHT_EIGENVECTOR> right_eigenvectors;
};

/// Encodes the normalized singular representation.
enum class normalized_singular_representation : uint8_t { normal, extended };

/// Encodes the channel svd representation.
struct srs_channel_svd_representation {
  /// Maximum number of PRGs.
  static constexpr unsigned NUM_MAX_PRG = 273;

  normalized_iq_representation         iq_representation;
  normalized_singular_representation   singular_representation;
  int8_t                               singular_value_scaling;
  uint16_t                             num_gnb_antenna_elements;
  uint8_t                              num_ue_srs_ports;
  uint16_t                             prg_size;
  uint16_t                             num_prgs;
  std::array<srs_svd_prg, NUM_MAX_PRG> svd_prg;
};

/// Describes the coordinate system for uplink Angle Of Arrival.
enum class srs_coordinate_system_ul_aoa : uint8_t { local = 0, global = 1 };

/// Encodes SRS positioning report.
struct srs_positioning_report {
  /// TUL-RTOA as recorded in TS 38.215 on section 5.1.
  std::optional<phy_time_unit> ul_relative_toa;
  std::optional<uint32_t>      gnb_rx_tx_difference;
  srs_coordinate_system_ul_aoa coordinate_system_aoa;
  std::optional<float>         ul_aoa;
  std::optional<float>         rsrp;
};

/// SRS indication pdu.
struct srs_indication_pdu {
  uint32_t handle;
  rnti_t   rnti;
  uint16_t timing_advance_offset;
  int16_t  timing_advance_offset_ns;
  /// \remark The enum doesn't contain the \c reserved value defined in the FAPI spec. This is because the value is
  /// currently not used anywhere.
  srs_usage              usage;
  srs_report_type        report_type;
  srs_channel_matrix     matrix;
  srs_positioning_report positioning;
};

/// SRS indication message.
struct srs_indication_message : public base_message {
  uint16_t                                                    sfn;
  uint16_t                                                    slot;
  uint16_t                                                    control_length;
  static_vector<srs_indication_pdu, MAX_SRS_PDUS_PER_SRS_IND> pdus;
};

} // namespace fapi
} // namespace srsran
