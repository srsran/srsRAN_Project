/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/bs_channel_bandwidth.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/pdcch/aggregation_level.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include <string>
#include <vector>

namespace srsgnb {

/// RF cell driver configuration.
struct rf_driver_appconfig {
  /// Sampling frequency in MHz.
  double srate_MHz = 61.44;
  /// RF driver name.
  std::string device_driver = "zmq";
  /// RF driver arguments.
  std::string device_arguments = "tx_port=tcp://*:5000,rx_port=tcp://localhost:6000";
  /// All transmit channel gain in decibels.
  double tx_gain_dB = 50.0;
  /// All receive channel gain in decibels.
  double rx_gain_dB = 60.0;
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
};

/// PDCCH application configuration.
struct pdcch_appconfig {
  /// Aggregation level for the UE.
  aggregation_level ue_aggregation_level_index = aggregation_level::n4;
  /// Aggregation level for the RAR.
  aggregation_level rar_aggregation_level_index = aggregation_level::n4;
  /// Aggregation level for the SI.
  aggregation_level si_aggregation_level_index = aggregation_level::n4;
};

/// PDSCH application configuration.
struct pdsch_appconfig {
  /// UE modulation and coding scheme index.
  unsigned fixed_ue_mcs = 10;
  /// RAR modulation and coding scheme index.
  unsigned fixed_rar_mcs = 0;
  /// SI modulation and coding scheme index.
  unsigned fixed_si_mcs = 5;
};

/// PUSCH application configuration.
struct pusch_appconfig {
  /// UE modulation and coding scheme index.
  unsigned fixed_ue_mcs = std::numeric_limits<unsigned>::max();
};

/// PUCCH application configuration.
struct pucch_appconfig {};

/// CSI-RS application configuration.
struct csi_appconfig {};

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
  /// Downlink arfcn.
  unsigned dl_arfcn = 536020;
  /// NR band.
  nr_band band = nr_band::n7;
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
  /// CSI-RS configuration.
  csi_appconfig csi_cfg;
  /// PRACH configuration.
  prach_appconfig prach_cfg;
  /// PUCCH configuration.
  pucch_appconfig pucch_cfg;
  /// PUSCH configuration.
  pusch_appconfig pusch_cfg;
  /// Common subcarrier spacing for the entire resource grid. It must be supported by the band SS raster.
  subcarrier_spacing common_scs = subcarrier_spacing::kHz15;
  /// Amplitude control configuration.
  amplitude_control_appconfig amplitude_cfg;
};

/// Cell configuration
struct cell_appconfig {
  /// Physical cell identifier.
  pci_t pci = 1;
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

/// QoS configuration
struct qos_appconfig {
  uint8_t       five_qi = 9;
  rlc_appconfig rlc;
};

struct amf_appconfig {
  std::string ip_addr   = "127.0.0.1";
  uint16_t    port      = 38412;
  std::string bind_addr = "127.0.0.1";
};

struct log_appconfig {
  std::string filename      = "/tmp/gnb.log"; // Path to write log file or "stdout" to print to console.
  std::string app_level     = "info";         // Generic log level assigned to all layers without layer-specific level.
  std::string du_level      = "info";
  std::string cu_level      = "info";
  std::string phy_level     = "info";
  std::string radio_level   = "info";
  std::string mac_level     = "info";
  std::string rlc_level     = "info";
  std::string f1u_level     = "info";
  std::string pdcp_level    = "info";
  std::string rrc_level     = "info";
  std::string gtpu_level    = "info";
  std::string fapi_level    = "info";
  uint32_t    hex_max_size  = 32;           // Maximum number of bytes to write when dumping hex arrays.
  bool        phy_broadcast = false;        // Set to true to log broadcasting messages and all PRACH opportunities.
  std::string phy_rx_symbols_filename = ""; // Set to a valid file path to print the received symbols.
};

struct pcap_appconfig {
  struct {
    std::string filename = "/tmp/gnb_ngap.pcap";
    bool        enabled  = false;
  } ngap;
  struct {
    std::string filename = "/tmp/gnb_mac.pcap";
    bool        enabled  = false;
  } mac;
};

/// Expert physical layer configuration.
struct expert_phy_appconfig {
  /// Number of PUSCH LDPC decoder iterations.
  unsigned pusch_decoder_max_iterations = 6;
  /// Set to true to enable the PUSCH LDPC decoder early stop.
  bool pusch_decoder_early_stop = true;
};

/// Monolithic gnb application configuration.
struct gnb_appconfig {
  /// Logging configuration.
  log_appconfig log_cfg;
  /// PCAP configuration.
  pcap_appconfig pcap_cfg;
  /// gNodeB identifier.
  uint32_t gnb_id = 411;
  /// Node name.
  std::string ran_node_name = "srsgnb01";
  /// AMF configuration.
  amf_appconfig amf_cfg;
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
  std::vector<qos_appconfig> qos_cfg = {};

  /// Expert physical layer configuration.
  expert_phy_appconfig expert_phy_cfg;
};

} // namespace srsgnb
