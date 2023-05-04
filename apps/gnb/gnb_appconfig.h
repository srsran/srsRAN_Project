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

#include "srsran/ran/band_helper.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/ran/five_qi.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <string>
#include <thread>
#include <vector>

namespace srsran {

/// RF cell driver configuration.
struct rf_driver_appconfig {
  /// Sampling frequency in MHz.
  double srate_MHz = 61.44;
  /// RF driver name.
  std::string device_driver = "uhd";
  /// RF driver arguments.
  std::string device_arguments = "";
  /// All transmit channel gain in decibels.
  double tx_gain_dB = 50.0;
  /// All receive channel gain in decibels.
  double rx_gain_dB = 60.0;
  /// Center frequency offset in hertz applied to all radio channels.
  double center_freq_offset_Hz = 0.0;
  /// Clock calibration in Parts Per Million (PPM). It is applied to the carrier frequency.
  double calibrate_clock_ppm = 0.0;
  /// LO Offset in MHz. It shifts the LO from the center frequency for moving the LO leakage out of the channel.
  double lo_offset_MHz = 0.0;
  /// \brief Rx to Tx radio time alignment calibration in samples.
  ///
  /// Compensates for the reception and transmission time misalignment inherent to the RF device. Setting this parameter
  /// overrides the default calibration, which is dependent on the selected RF device driver. Positive values reduce the
  /// RF transmission delay with respect to the RF reception. Since the UE receives the DL signal earlier, an incoming
  /// PRACH will also be detected earlier within the reception window. Negative values have the opposite effect, for
  /// example, a value of -1000 at a sample rate of 61.44 MHz increases the transmission delay and causes an incoming
  /// PRACH to be detected 16.3 us later within the reception window.
  optional<int> time_alignment_calibration;
  /// Synchronization source.
  std::string synch_source = "default";
  /// Clock source.
  std::string clock_source = "default";
  /// Over-the wire format. Determines the format in which samples are transported from the radio to the host.
  std::string otw_format = "default";
};

/// PRACH application configuration.
struct prach_appconfig {
  /// PRACH configuration index.
  unsigned prach_config_index = 1;
  /// PRACH root sequence index.
  unsigned prach_root_sequence_index = 1;
  /// Zero correlation zone
  unsigned zero_correlation_zone = 0;
  unsigned fixed_msg3_mcs        = 0;
  unsigned max_msg3_harq_retx    = 4;
  /// Total number of PRACH preambles used for contention based and contention free 4-step or 2-step random access.
  optional<unsigned> total_nof_ra_preambles;
};

/// TDD pattern configuration. See TS 38.331, \c TDD-UL-DL-Pattern.
struct tdd_ul_dl_pattern_config {
  /// Periodicity of the DL-UL pattern in Milliseconds. Values {0.5, 0.625, 1, 1.25, 2, 2.5, 5, 10}.
  float dl_ul_tx_period = 5.0F;
  /// Values: {0,...,maxNrofSlots=80}.
  unsigned nof_dl_slots = 6;
  /// Values: {0,...,maxNrofSymbols-1=13}.
  unsigned nof_dl_symbols = 0;
  /// Values: {0,...,maxNrofSlots=80}.
  unsigned nof_ul_slots = 3;
  /// Values: {0,...,maxNrofSymbols-1=13}.
  unsigned nof_ul_symbols = 0;
};

/// TDD configuration. See TS 38.331, \c TDD-UL-DL-ConfigCommon.
struct tdd_ul_dl_config {
  tdd_ul_dl_pattern_config           pattern1;
  optional<tdd_ul_dl_pattern_config> pattern2;
};

/// PDCCH application configuration.
struct pdcch_appconfig {
  /// Use an UE-dedicated or Common Search Space.
  search_space_configuration::type_t ue_ss_type = search_space_configuration::type_t::ue_dedicated;
};

/// PDSCH application configuration.
struct pdsch_appconfig {
  /// Minimum modulation and coding scheme index for C-RNTI PDSCH allocations. Note that setting a high minimum MCS
  /// may lead to a high BLER if the SINR is low.
  uint8_t min_ue_mcs = 0;
  /// Maximum modulation and coding scheme index for C-RNTI PDSCH allocations. To set a fixed MCS, set \c min_ue_mcs
  /// equal to the \c max_ue_mcs.
  uint8_t max_ue_mcs = 28;
  /// RAR modulation and coding scheme index.
  unsigned fixed_rar_mcs = 0;
  /// SI modulation and coding scheme index.
  unsigned fixed_sib1_mcs = 5;
  /// Number of UE DL HARQ processes.
  unsigned nof_harqs = 16;
};

/// PUSCH application configuration.
struct pusch_appconfig {
  /// Minimum modulation and coding scheme index for C-RNTI PUSCH allocations. Note that setting a high minimum MCS
  /// may lead to a high BLER if the SINR is low.
  uint8_t min_ue_mcs = 0;
  /// Maximum modulation and coding scheme index for C-RNTI PUSCH allocations. To set a fixed MCS, set \c min_ue_mcs
  /// equal to the \c max_ue_mcs.
  uint8_t max_ue_mcs = 28;
};

/// Amplitude control application configuration.
struct amplitude_control_appconfig {
  /// Baseband gain back-off. This accounts for the signal PAPR and is applied regardless of clipping settings.
  float gain_backoff_dB = 12.0F;
  /// Power ceiling in dB, relative to the full scale amplitude of the radio.
  float power_ceiling_dBFS = -0.1F;
  /// Clipping of the baseband samples. If enabled, the samples that exceed the power ceiling are clipped.
  bool enable_clipping = false;
};

/// Base cell configuration.
struct base_cell_appconfig {
  /// Physical cell identifier.
  pci_t pci = 1;
  /// Downlink arfcn.
  unsigned dl_arfcn = 536020;
  /// NR band.
  optional<nr_band> band;
  /// Channel bandwidth in MHz.
  bs_channel_bandwidth_fr1 channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;
  /// Number of antennas in downlink.
  unsigned nof_antennas_dl = 1;
  /// Number of antennas in uplink.
  unsigned nof_antennas_ul = 1;
  /// Human readable full PLMN (without possible filler digit).
  std::string plmn = "00101";
  /// TAC.
  unsigned tac = 7;
  /// PDCCH configuration.
  pdcch_appconfig pdcch_cfg;
  /// PDSCH configuration.
  pdsch_appconfig pdsch_cfg;
  /// PRACH configuration.
  prach_appconfig prach_cfg;
  /// PUSCH configuration.
  pusch_appconfig pusch_cfg;
  /// Common subcarrier spacing for the entire resource grid. It must be supported by the band SS raster.
  subcarrier_spacing common_scs = subcarrier_spacing::kHz15;
  /// Amplitude control configuration.
  amplitude_control_appconfig amplitude_cfg;
  /// TDD slot configuration.
  optional<tdd_ul_dl_config> tdd_pattern_cfg;
};

/// Cell configuration
struct cell_appconfig {
  /// Cell configuration.
  base_cell_appconfig cell;
};

/// RLC UM TX configuration
struct rlc_tx_um_appconfig {
  uint16_t sn_field_length; ///< Number of bits used for sequence number
  int32_t  t_reassembly;    ///< Timer used by rx to detect PDU loss (ms)
};

/// RLC UM RX configuration
struct rlc_rx_um_appconfig {
  uint16_t sn_field_length; ///< Number of bits used for sequence number
  int32_t  t_reassembly;    ///< Timer used by rx to detect PDU loss (ms)
};

/// RLC UM configuration
struct rlc_um_appconfig {
  rlc_tx_um_appconfig tx;
  rlc_rx_um_appconfig rx;
};

/// RLC UM TX configuration
struct rlc_tx_am_appconfig {
  uint16_t sn_field_length; ///< Number of bits used for sequence number
  int32_t  t_poll_retx;     ///< Poll retx timeout (ms)
  uint32_t max_retx_thresh; ///< Max retx threshold
  int32_t  poll_pdu;        ///< Insert poll bit after this many PDUs
  int32_t  poll_byte;       ///< Insert poll bit after this much data (bytes)
};

/// RLC UM RX configuration
struct rlc_rx_am_appconfig {
  uint16_t sn_field_length;   ///< Number of bits used for sequence number
  int32_t  t_reassembly;      ///< Timer used by rx to detect PDU loss (ms)
  int32_t  t_status_prohibit; ///< Timer used by rx to prohibit tx of status PDU (ms)
};

/// RLC AM configuration
struct rlc_am_appconfig {
  rlc_tx_am_appconfig tx;
  rlc_rx_am_appconfig rx;
};

/// RLC configuration
struct rlc_appconfig {
  std::string      mode = "am";
  rlc_um_appconfig um;
  rlc_am_appconfig am;
};

struct pdcp_rx_appconfig {
  uint16_t sn_field_length;       ///< Number of bits used for sequence number
  int32_t  t_reordering;          ///< Timer used to detect PDUs losses (ms)
  bool     out_of_order_delivery; ///< Whether out-of-order delivery to upper layers is enabled
};

struct pdcp_tx_appconfig {
  uint16_t sn_field_length;        ///< Number of bits used for sequence number
  int32_t  discard_timer;          ///< Timer used to notify lower layers to discard PDUs (ms)
  bool     status_report_required; ///< Whether PDCP status report is required
};

struct pdcp_appconfig {
  bool              integrity_protection_required; ///< Whether DRB integrity is required
  pdcp_tx_appconfig tx;
  pdcp_rx_appconfig rx;
};
/// QoS configuration
struct qos_appconfig {
  five_qi_t      five_qi = uint_to_five_qi(9);
  rlc_appconfig  rlc;
  pdcp_appconfig pdcp;
};

struct amf_appconfig {
  std::string ip_addr                = "127.0.0.1";
  uint16_t    port                   = 38412;
  std::string bind_addr              = "127.0.0.1";
  int         sctp_rto_initial       = 120;
  int         sctp_rto_min           = 120;
  int         sctp_rto_max           = 500;
  int         sctp_init_max_attempts = 3;
  int         sctp_max_init_timeo    = 500;
};

struct cu_cp_appconfig {
  int inactivity_timer = 7200; // in seconds
};

struct log_appconfig {
  std::string filename    = "/tmp/gnb.log"; // Path to write log file or "stdout" to print to console.
  std::string all_level   = "warning";      // Default log level for all layers.
  std::string lib_level   = "warning"; // Generic log level assigned to library components without layer-specific level.
  std::string du_level    = "warning";
  std::string cu_level    = "warning";
  std::string phy_level   = "warning";
  std::string radio_level = "info";
  std::string mac_level   = "warning";
  std::string rlc_level   = "warning";
  std::string f1u_level   = "warning";
  std::string pdcp_level  = "warning";
  std::string rrc_level   = "warning";
  std::string ngap_level  = "warning";
  std::string sdap_level  = "warning";
  std::string gtpu_level  = "warning";
  std::string fapi_level  = "warning";
  int         hex_max_size      = 0;     // Maximum number of bytes to write when dumping hex arrays.
  bool        broadcast_enabled = false; // Set to true to log broadcasting messages and all PRACH opportunities.
  std::string phy_rx_symbols_filename;   // Set to a valid file path to print the received symbols.
};

struct pcap_appconfig {
  struct {
    std::string filename = "/tmp/gnb_ngap.pcap";
    bool        enabled  = false;
  } ngap;
  struct {
    std::string filename = "/tmp/gnb_e1ap.pcap";
    bool        enabled  = false;
  } e1ap;
  struct {
    std::string filename = "/tmp/gnb_mac.pcap";
    bool        enabled  = false;
  } mac;
};

/// Lower physical layer thread profiles.
enum class lower_phy_thread_profile {
  /// Same task worker as the rest of the PHY (ZMQ only).
  blocking = 0,
  /// Single task worker for all the lower physical layer task executors.
  single,
  /// Two task workers - one for the downlink and one for the uplink.
  dual,
  /// Dedicated task workers for each of the subtasks (downlink processing, uplink processing, reception and
  /// transmission).
  quad
};

/// Expert physical layer configuration.
struct expert_phy_appconfig {
  /// Lower physical layer thread profile.
  lower_phy_thread_profile lphy_executor_profile = lower_phy_thread_profile::dual;
  /// Number of threads for processing PUSCH and PUCCH. It is set to 4 by default unless the available hardware
  /// concurrency is limited, in which case the most suitable number of threads between one and three will be selected.
  unsigned nof_ul_threads = std::min(4U, std::max(std::thread::hardware_concurrency(), 4U) - 3U);
  /// Number of PUSCH LDPC decoder iterations.
  unsigned pusch_decoder_max_iterations = 6;
  /// Set to true to enable the PUSCH LDPC decoder early stop.
  bool pusch_decoder_early_stop = true;
};

struct test_mode_ue_appconfig {
  /// C-RNTI to assign to the test UE.
  rnti_t rnti = INVALID_RNTI;
  /// Whether PDSCH grants are automatically assigned to the test UE.
  bool pdsch_active = true;
  /// Whether PUSCH grants are automatically assigned to the test UE.
  bool pusch_active = true;
};

/// gNB app Test Mode configuration.
struct test_mode_appconfig {
  /// Creates a UE with the given the given params for testing purposes.
  test_mode_ue_appconfig test_ue;
};

/// Monolithic gnb application configuration.
struct gnb_appconfig {
  /// Logging configuration.
  log_appconfig log_cfg;
  /// PCAP configuration.
  pcap_appconfig pcap_cfg;
  /// gNodeB identifier.
  uint32_t gnb_id = 411;
  /// Length of gNB identity in bits. Values {22,...,32}.
  uint8_t gnb_id_bit_length = 32;
  /// Node name.
  std::string ran_node_name = "srsgnb01";
  /// AMF configuration.
  amf_appconfig amf_cfg;
  /// CU-CP configuration.
  cu_cp_appconfig cu_cp_cfg;
  /// RF driver configuration.
  rf_driver_appconfig rf_driver_cfg;
  /// \brief Base cell application configuration.
  ///
  /// \note When a cell is added, it will use the values of this base cell as default values for its base cell
  /// configuration. This parameter usage is restricted for filling cell information in the \remark cell_cfg variable.
  base_cell_appconfig common_cell_cfg;
  /// \brief Cell configuration.
  ///
  /// \note Add one cell by default.
  std::vector<cell_appconfig> cells_cfg = {{}};

  /// \brief QoS configuration.
  std::vector<qos_appconfig> qos_cfg;

  /// Expert physical layer configuration.
  expert_phy_appconfig expert_phy_cfg;

  /// Configuration for testing purposes.
  test_mode_appconfig test_mode_cfg = {};
};

} // namespace srsran
