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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/variant.h"
#include <vector>

namespace srsran {

struct cell_selection_info {
  /// \brief \c q-RxLevMin, part of \c cellSelectionInfo, \c SIB1, TS 38.311, in dBm.
  /// Indicates the required minimum received RSRP level for cell selection/re-selection (see \c Q-RxLevMin, TS 38.311).
  bounded_integer<int, -70, -22> q_rx_lev_min = -70;
  /// \brief \c q-QualMin, part of \c cellSelectionInfo, \c SIB1, TS 38.311, in dB.
  /// Indicates the required minimum received RSRQ level for cell selection/re-selection (see \c Q-QualMin, TS 38.311).
  bounded_integer<int, -43, -12> q_qual_min = -20;
};

enum class sib_type { sib1 = 1, sib2 = 2, sib19 = 19, sib_invalid };

/// Configures a pattern of SSBs. See TS 38.331, \c SSB-ToMeasure.
/// Equates to longBitmap when size of bitset equals to 64.
/// Equates to mediumBitmap when size of bitset equals to 8.
/// Equates to shortBitmap when size of bitset equals to 4.
/// \remark The size of 64 is chosen for bitset in order account for all the bitmap sizes corresponding to longBitmap,
/// mediumBitmap and shortBitmap.
using ssb_to_measure = bounded_bitset<64>;

/// Defines configuration for RSSI measurements based on synchronization reference signals. See TS 38.331, \c
/// SS-RSSI-Measurement.
struct ss_rssi_measurement {
  /// Indicates the slots in which the UE can perform RSSI measurements. The length of the bitmap is equal to the
  /// number of slots in the configured SMTC window. The first (left-most / most significant) bit in the bitmap
  /// corresponds to the first slot in the SMTC window, the second bit in the bitmap corresponds to the second slot in
  /// the SMTC window, and so on. The UE measures in slots for which the corresponding bit in the bitmap is set to 1.
  bounded_bitset<80, true> measurement_slots;
  /// Within a slot that is configured for RSSI measurements the UE measures the RSSI from symbol 0 to symbol endSymbol.
  /// Values {0,...,3}.
  uint8_t end_symbol;
};

/// Used to configure measurement timing configurations, i.e., timing occasions at which the UE measures SSBs. See
/// TS 38.331, \c SSB-MTC.
struct ssb_mtc {
  /// Periodicity of the measurement window in which to receive SS/PBCH blocks. Values {5, 10, 20, 40, 80, 160}
  /// subframes.
  uint8_t periodicity_sf;
  /// Offset of the measurement window in which to receive SS/PBCH blocks. Values {0,...,periodicity - 1} subframes.
  uint8_t offset_sf;
  /// Duration of the measurement window in which to receive SS/PBCH blocks. Values {1, 2, 3, 4, 5} subframes.
  uint8_t duration_sf;
};

enum class t_evaluation { s30, s60, s120, s180, s240, spare3, spare2, spare1 };

enum class t_hyst_normal { s30, s60, s120, s180, s240, spare3, spare2, spare1 };

struct nr_ns_p_max_value {
  /// Used to limit the UE's uplink transmission power on a carrier frequency. See TS 38.101-1. Values {-30,...,33}.
  optional<int8_t> additional_p_max;
  /// Indicates emission requirements to be fulfilled by the UE. See TS 38.101-1, clause 6.2.3, and TS 38.101-2,
  /// clause 6.2.3. Values {0,...,7}.
  uint8_t additional_spectrum_emission;
};

struct nr_multi_band_info {
  /// Provides an NR frequency band number as defined in TS 38.101-1 and TS 38.101-2, table 5.2-1.
  /// \remark This field is absent for SIB2 and is mandatory present in SIB4 and frequencyInfoDL-SIB.
  optional<nr_band> freq_band_indicator_nr;
  /// Maximum of 8 entries.
  std::vector<nr_ns_p_max_value> nr_ns_p_max_list;
};

struct speed_state_reselection_params {
  /// The duration for evaluating criteria to enter mobility states. Corresponds to TCRmax in TS 38.304.
  t_evaluation t_eval;
  /// The additional duration for evaluating criteria to enter normal mobility state. Corresponds to TCRmaxHyst in
  /// TS 38.304.
  t_hyst_normal t_hyst_norm;
  /// The number of cell changes to enter medium mobility state. Corresponds to NCR_M in TS 38.304. Values {1,...,16}.
  uint8_t n_cell_change_medium;
  /// The number of cell changes to enter high mobility state. Corresponds to NCR_H in TS 38.304. Values {1,...,16}.
  uint8_t n_cell_change_high;
  /// Additional hysteresis to be applied, in Medium Mobility state respectively, to Qhyst as defined in TS 38.304.
  /// Values {-6, -4, -2, 0}.
  int q_hyst_sf_medium_db;
  /// Additional hysteresis to be applied, in High Mobility state respectively, to Qhyst as defined in TS 38.304. Values
  /// {-6, -4, -2, 0}.
  int q_hyst_sf_high_db;
};

struct sib2_info {
  /// Number of SS blocks to average for cell measurement derivation. If the field is absent the UE uses the measurement
  /// quantity as specified in TS 38.304.
  optional<uint8_t> nof_ssbs_to_average;
  /// Threshold for consolidation of L1 measurements per RS index. If the \c abs_thres_ss_blocks_consolidation_rsrp, \c
  /// abs_thres_ss_blocks_consolidation_rsrq and \c abs_thres_ss_blocks_consolidation_sinr fields are absent, the UE
  /// uses the measurement quantity as specified in TS 38.304.
  optional<uint8_t> abs_thres_ss_blocks_consolidation_rsrp;
  optional<uint8_t> abs_thres_ss_blocks_consolidation_rsrq;
  optional<uint8_t> abs_thres_ss_blocks_consolidation_sinr;
  /// Used to indicate a cell, beam or measurement object specific offset to be applied when evaluating candidates for
  /// cell re-selection. Values {-24,...,24}.
  optional<int8_t> q_offset_range_to_best_cell_db;
  /// Parameter "Qhyst" in TS 38.304. Values {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24}.
  uint8_t                                  q_hyst_db;
  optional<speed_state_reselection_params> speed_state_resel_params;

  /// Parameter "SnonIntraSearchP" in TS 38.304. If this field is not present, the UE applies the (default) value of
  /// infinity. Values {0,...,31}.
  optional<uint8_t> s_non_intra_search_p;
  /// Parameter "SnonIntraSearchQ" in TS 38.304. If the field is not present, the UE applies the (default) value of 0
  /// dB. Values {0,...,31}.
  optional<uint8_t> s_non_intra_search_q;
  /// Parameter "ThreshServing, LowP" in TS 38.304. Values {0,...,31}.
  uint8_t thresh_serving_low_p;
  /// Parameter "ThreshServing, LowQ" in TS 38.304. Values {0,...,31}.
  optional<uint8_t> thresh_serving_low_q;
  /// Defines the absolute priority of the concerned carrier frequency, as used by the cell reselection procedure. Value
  /// 0 means: lowest priority. Values {0,...,7}.
  uint8_t cell_reselection_priority;
  /// Indicates a fractional value to be added to the value of \c cell_reselection_priority to obtain the absolute
  /// priority of the concerned carrier frequency for E-UTRA and NR.Values {0.2, 0.4, 0.6, 0.8}.
  optional<float> cell_reselection_sub_priority;

  /// Parameter "Qrxlevmin" in TS 38.304, applicable for intra-frequency neighbour cells. Values {-70,...,-22}.
  int8_t q_rx_lev_min;
  /// Parameter "QrxlevminSUL" in TS 38.304, applicable for intra-frequency neighbour cells. Values {-70,...,-22}.
  optional<int8_t> q_rx_lev_min_sul;
  /// Parameter "Qqualmin" in TS 38.304, applicable for intra-frequency neighbour cells. If the field is not
  /// present, the UE applies the (default) value of negative infinity. Value {-43,...,-12}.
  optional<int8_t> q_qual_min;
  /// Parameter "SIntraSearchP" in TS 38.304. If this field is not present, the UE applies the (default) value of
  /// infinity. Values {0,...,31}.
  uint8_t s_intra_search_p;
  /// Parameter "SIntraSearchQ2 in TS 38.304. If the field is not present, the UE applies the (default) value of 0
  /// dB. Values {0,...,31}.
  optional<uint8_t> s_intra_search_q;
  /// Parameter "TreselectionNR" in TS 38.304. Values {0,...,7}.
  uint8_t t_reselection_nr;
  /// Indicates the list of frequency bands for which the NR cell reselection parameters apply. Maximum of 8 entries.
  std::vector<nr_multi_band_info> freq_band_lst;
  /// Indicates the list of frequency bands for which the NR cell reselection parameters apply. Maximum of 8 entries.
  std::vector<nr_multi_band_info> freq_band_lst_sul;
  /// Used to limit the UE's uplink transmission power on a carrier frequency. See TS 38.101-1. Values {-30,...,33}.
  optional<int8_t>              p_max;
  optional<ssb_mtc>             smtc;
  optional<ss_rssi_measurement> ss_rssi_meas;
  optional<ssb_to_measure>      ssb_to_meas;
  /// This field indicates whether the UE can utilize serving cell timing to derive the index of SS block transmitted by
  /// neighbour cell.
  bool derive_ssb_index_from_cell;
};

struct sib19_info {
  optional<uint16_t> distance_thres;
  // TODO
};

/// \brief Variant type that can hold different types of SIBs that go in a SI message.
using sib_info = variant<sib2_info, sib19_info>;

inline sib_type get_sib_info_type(const sib_info& sib)
{
  if (variant_holds_alternative<sib2_info>(sib)) {
    return sib_type::sib2;
  }
  if (variant_holds_alternative<sib19_info>(sib)) {
    return sib_type::sib19;
  }
  return sib_type::sib_invalid;
}

/// \brief This struct contains the information required for the scheduling of the SI messages by the network.
struct si_message_sched_info {
  /// List of SIBs (sib2, sib3, ...) included in this SI message. The list has at most 32 elements.
  std::vector<sib_type> sib_mapping_info;
  /// Periodicity of the SI-message in radio frames. Values: {8, 16, 32, 64, 128, 256, 512}.
  unsigned si_period_radio_frames = 32;
};

/// This struct contains the information required for the generation of the SI messages sent by the network and the
/// generation of the SIB1 "SI-SchedulingInfo" field of the SIB1. See TS 38.331, "SystemInformation" and
/// "SI-SchedulingInfo".
struct si_scheduling_info_config {
  /// The length of the SI scheduling window, in slots. It is always shorter or equal to the period of the SI message.
  /// Values: {5, 10, 20, 40, 80, 160, 320, 640, 1280}.
  unsigned si_window_len_slots;
  /// List of SI-messages and associated scheduling information.
  std::vector<si_message_sched_info> si_sched_info;
  /// Information included in each SIB that is scheduled as part of one of the SI-messages.
  std::vector<sib_info> sibs;
};

/// This struct contains the information required for the generation of the SIB1 "UE-TimersAndConstants" field of the
/// SIB1. See TS 38.331 section 7.
struct ue_timers_and_constants_config {
  /// t300
  /// Values (in ms): {100, 200, 300, 400, 600, 1000, 1500, 2000}
  std::chrono::milliseconds t300;
  /// t301
  /// Values (in ms): {100, 200, 300, 400, 600, 1000, 1500, 2000}
  std::chrono::milliseconds t301;
  /// \brief Timer triggered by UE upon detection of N310 consecutive out-of-sync indications from lower layers, as
  /// per TS 38.331, 7.1.1. Values: {ms0, ms50, ms100, ms200, ms500, ms1000, ms2000}.
  std::chrono::milliseconds t310;
  /// n310
  /// Values: {1, 2, 3, 4, 6, 8, 10, 20}
  unsigned n310;
  /// \brief Timer triggered by UE upon initiating RRC connection reestablishment procedure, as per TS 38.331 7.1.1.
  /// Values: {ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000}.
  std::chrono::milliseconds t311;
  /// n311
  /// Values: {1, 2, 3, 4, 5, 6, 8, 10}
  unsigned n311;
  /// t319
  /// Values (in ms): {100, 200, 300, 400, 600, 1000, 1500, 2000}
  std::chrono::milliseconds t319;
};

} // namespace srsran
