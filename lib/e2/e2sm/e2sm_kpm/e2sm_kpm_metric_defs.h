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

#include "srsran/e2/e2sm/e2sm_kpm.h"

/*
 * In this file, we structure the metrics described in the 3GPP/ORAN technical specifications.
 */

namespace srsran {

enum e2sm_kpm_collection_method_t {
  CC    = 1, // Cumulative Counter
  GAUGE = 2, // Dynamic Variable
  DER   = 3, // Discrete Event Registration (n=1)
  SI    = 4, // Status Inspection
  OM    = 5  // ?
};

enum e2sm_kpm_measurement_object_t {
  E2_Node        = 0x0001, // ManagedElement
  GNBCUUP        = 0x0002, // GNBCUUPFunction
  NRCellCU       = 0x0004,
  NRCellDU       = 0x0008,
  NRCellRelation = 0x0010,
  EutranRelation = 0x0020,
  BEAM           = 0x0040,
  EP_F1U         = 0x0080,
  EP_XnU         = 0x0100,
  EP_X2U         = 0x0200,
  ANY            = 0xffff
};

enum e2sm_kpm_metric_dtype_t { INTEGER, REAL };

struct e2sm_kpm_metric_t {
  std::string                  name;
  uint32_t                     meas_obj;
  e2sm_kpm_collection_method_t collection_method;
  e2sm_kpm_metric_dtype_t      data_type;
  std::string                  units;
  uint32_t                     optional_labels;
  uint32_t                     optional_levels;
};

inline bool is_cell_id_required(const e2sm_kpm_metric_t& metric)
{
  // Cell ID is required if metric belongs to a measurement object class confined in a single cell.
  if (metric.meas_obj == NRCellCU) {
    return true;
  }

  if (metric.meas_obj == NRCellDU) {
    return true;
  }

  // Cell ID is not needed if metric is cell agnostic (e.g., GNBCUUPFunction).
  return false;
}

/// Number of E2SM-KPM metrics defined in 3GPP TS 28.552.
const size_t NOF_3GPP_TS_28_552_METRICS = 278;

/// Number of E2SM-KPM metrics defined in O-RAN.WG3.E2SM-KPM-R003-v3.00
const size_t NOF_ORAN_E2SM_KPM_METRICS = 9;

// Measurements defined in 3GPP TS 28.552 V18.23.0 (2023-0306)
inline span<const e2sm_kpm_metric_t> get_e2sm_kpm_28_552_metrics()
{
  // clang-format off
  static const std::array<e2sm_kpm_metric_t, NOF_3GPP_TS_28_552_METRICS> metrics =  {{
  // 5.1.1 Performance measurements valid for all gNB deployment scenarios
  // 5.1.1.1 Packet Delay
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  // 5.1.1.1.1 Average delay DL air-interface
  // Description: This measurement provides the average (arithmetic mean) time it takes for packet transmission
  //              over the air-interface in the downlink direction.
  {"DRB.AirIfDelayDl", NRCellDU, DER, REAL, "ms", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.1.2 Distribution of delay DL air-interface
  // Description: This measurement is an integer representing the number of RLC SDU packets measured with the delay
  //              within the range of the bin.
  // Note: bin width is vendor specific
  {"DRB.AirIfDelayDist", NRCellDU, DER, INTEGER, "-", DIST_BIN_X_LABEL | PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.1.3 Average delay UL on over-the-air interface
  // Description: This measurement provides the average (arithmetic mean) over-the-air packet delay on the uplink.
  {"DRB.AirIfDelayUl", NRCellDU, DER, REAL, "ms", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.1.4 Average RLC packet delay in the UL
  // Description: This measurement provides the average RLC packet delay on the uplink.
  {"DRB.RlcDelayUl", NRCellDU, DER, REAL, "ms", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.1.5 Average PDCP re-ordering delay in the UL
  // Description: This measurement provides the average (arithmetic mean) PDCP re-ordering delay on the uplink,
  //              i.e., the delay within the gNB-CU-UP.
  {"DRB.PdcpReordDelayUl", GNBCUUP, DER, REAL, "ms", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.1.6 Distribution of DL delay between NG-RAN and UE
  // Description: This measurement provides the distribution of DL packet delay between NG-RAN and UE, which
  //              is the delay incurred in NG-RAN (including the delay at gNB-CU-UP, on F1-U and on gNB-DU)
  //              and the delay over Uu interface.
  // Note: bin width is vendor specific
  // Note: source is NRCellCU (for non-split and 2-split scenario) or GNBCUUPFunction (for 3-split scenario)
  {"DRB.DelayDlNgranUeDist", NRCellCU | GNBCUUP, DER, INTEGER, "-", DIST_BIN_X_LABEL | PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.1.7 Distribution of UL delay between NG-RAN and UE
  // 5.1.1.1.7.1 Distribution of UL delay between NG-RAN and UE (excluding D1)
  // Description: This measurement provides the distribution of UL packet delay between NG-RAN and UE, which includes
  //              the delay occurred in NG-RAN (including the delay at gNB-CU-UP, on F1-U and on gNB-DU) and the delay
  //              over Uu interface (excluding the D1 UL PDCP delay occurred in the UE).
  // Note: bin width is vendor specific
  // Note: source is NRCellCU (for non-split and 2-split scenario) or GNBCUUPFunction (for 3-split scenario)
  {"DRB.DelayUlNgranUeDist", NRCellCU | GNBCUUP, DER, INTEGER, "-", DIST_BIN_X_LABEL | PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.1.7.2 Distribution of UL delay between NG-RAN and UE (including D1)
  // Description: This measurement provides the distribution of UL packet delay between NG-RAN and UE, which includes
  //              the delay occurred in NG-RAN (including the delay at gNB-CU-UP, on F1-U and on gNB-DU), the delay
  //              over Uu interface and the D1 UL PDCP delay occurred in the UE.
  // Note: bin width is vendor specific
  // Note: source is NRCellCU (for non-split and 2-split scenario) or GNBCUUPFunction (for 3-split scenario)
  {"DRB.DelayUlNgranUeIncD1Dist", NRCellCU | GNBCUUP, DER, INTEGER, "-", DIST_BIN_X_LABEL | PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.1.8 DL packet delay between NG-RAN and PSA UPF
  // 5.1.1.1.8.1 Average DL GTP packet delay between PSA UPF and NG-RAN
  // Description: This measurement provides the average DL GTP packet delay between PSA UPF and NG-RAN.
  // Note: source is EP_N3 (contained by GNBCUUPFunction)
  {"GTP.DelayDlPsaUpfNgranMean", GNBCUUP, DER, REAL, "us", FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.1.8.2 Distribution of DL GTP packet delay between PSA UPF and NG-RAN
  // Description: This measurement provides the distribution of DL GTP packet delay between PSA UPF and NG-RAN.
  // Note: bin width is vendor specific,
  // Note: source is EP_N3 (contained by GNBCUUPFunction)
  {"GTP.DelayDlPsaUpfNgranDist", GNBCUUP, DER, INTEGER, "-", DIST_BIN_X_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.2 Radio resource utilization
  // Note: Measurement definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 to allow E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.2.1 DL Total PRB Usage
  // Description: This measurement provides the total usage (in percentage) of physical resource blocks (PRBs)
  //              on the downlink for any purpose.
  {"RRU.PrbTotDl", NRCellDU, SI, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.2 UL Total PRB Usage
  // Description: This measurement provides the total usage (in percentage) of physical resource blocks (PRBs)
  //              on the uplink for any purpose.
  {"RRU.PrbTotUl", NRCellDU, SI, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.3 Distribution of DL Total PRB Usage
  // Description: This measurement provides the distribution of samples with total usage (in percentage) of physical
  //              resource blocks (PRBs) on the downlink in different ranges.
  {"RRU.PrbTotDlDist", NRCellDU, CC, INTEGER, "%", DIST_BIN_X_LABEL | NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.4 Distribution of UL total PRB usage
  // Description: This measurement provides the distribution of samples with total usage (in percentage) of physical
  //              resource blocks (PRBs) on the uplink in different usage ranges.
  {"RRU.PrbTotUlDist", NRCellDU, CC, INTEGER, "%", DIST_BIN_X_LABEL | NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.5 Mean DL PRB used for data traffic
  // Description: This measurement provides the number of physical resource blocks (PRBs) in average used in downlink
  //              for data traffic.
  {"RRU.PrbUsedDl", NRCellDU, SI, INTEGER, "-", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL | NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.6 DL total available PRB
  // Description: This measurement provides the total number of physical resource blocks (PRBs) in average available DL.
  {"RRU.PrbAvailDl", NRCellDU, SI, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.7 Mean UL PRB used for data traffic
  // Description: This measurement provides the number of physical resource blocks (PRBs) in average used in uplink
  //              for data traffic.
  {"RRU.PrbUsedUl", NRCellDU, SI, INTEGER, "-", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL | NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.8 UL total available PRB
  // Description: This measurement provides the total number of physical resource blocks (PRBs) available uplink.
  {"RRU.PrbAvailUl", NRCellDU, SI, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.9 Peak DL PRB used for data traffic
  // Description: This measurement provides the maximum number of PRBs used in downlink for data traffic.
  {"RRU.MaxPrbUsedDl", NRCellDU, SI, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL | NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.10 Peak UL PRB used for data traffic
  // Description: This measurement provides the number of PRBs used in uplink for data traffic.
  {"RRU.MaxPrbUsedUl", NRCellDU, SI, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL | NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.11	PDSCH PRB Usage per cell for MIMO
  // Description: This measurement provides the total usage (in percentage) of physical resource blocks (PRBs) per cell
  //              for MIMO with time domain averaged maximum scheduled layer number as spatial factor in the downlink.
  {"RRU.PrbTotDlMimo", NRCellDU, SI, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.12 PUSCH PRB Usage per cell for MIMO
  // Description: This measurement provides the total usage (in percentage) of physical resource blocks (PRBs) per cell
  //              for MIMO with time domain averaged maximum scheduled layer number as spatial factor in the uplink.
  {"RRU.PrbTotUlMimo", NRCellDU, SI, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.13 SDM PDSCH PRB Usage
  // Description: This measurement provides the total usage (in percentage) of PDSCH physical resource blocks (PRBs),
  //              based on statistical MIMO layers.
  {"RRU.PrbTotSdmDl", NRCellDU, SI, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.2.14 SDM PUSCH PRB Usage
  // Description: This measurement provides the total usage (in percentage) of PUSCH physical resource blocks (PRBs),
  //              based on statistical MIMO layers.
  {"RRU.PrbTotSdmUl", NRCellDU, SI, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.3 UE throughput
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  // 5.1.1.3.1 Average DL UE throughput in gNB
  // Description: This measurement provides the average UE throughput in downlink.
  // Note: seems that DRB.UEThpDl.BWP, where BWP identifies the Active BWP is not supported in KPM
  {"DRB.UEThpDl", NRCellDU, DER, REAL, "kbps", FIVE_QI_LABEL | SLICE_ID_LABEL | PLMN_ID_LABEL | NO_LABEL, ALL_LEVELS},

  // 5.1.1.3.2 Distribution of DL UE throughput in gNB
  // Description: This measurement provides the distribution of the UE throughput in downlink.
  {"DRB.UEThpDlDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL | PLMN_ID_LABEL, ALL_LEVELS},

  // 5.1.1.3.3 Average UL UE throughput in gNB
  // Description: This measurement provides the average UE throughput in uplink.
  // Note: seems that DRB.UEThpDl.BWP, where BWP identifies the Active BWP is not supported in KPM
  {"DRB.UEThpUl", NRCellDU, DER, REAL, "kbps", FIVE_QI_LABEL | SLICE_ID_LABEL | PLMN_ID_LABEL | NO_LABEL, ALL_LEVELS},

  // 5.1.1.3.4 Distribution of UL UE throughput in gNB
  // Description: This measurement provides the distribution of the UE throughput in uplink.
  {"DRB.UEThpUlDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL | PLMN_ID_LABEL, ALL_LEVELS},

  // 5.1.1.3.5 Percentage of unrestricted DL UE data volume in gNB
  // Description: This measurement provides the percentage of DL data volume for UEs in the cell that is classified
  //              as unrestricted, i.e., when the volume is so low that all data can be transferred in one slot and
  //              no UE throughput sample could be calculated.
  {"DRB.UEUnresVolDl", NRCellDU, SI, INTEGER, "%", FIVE_QI_LABEL | SLICE_ID_LABEL | PLMN_ID_LABEL, ALL_LEVELS},

  // 5.1.1.3.6 Percentage of unrestricted UL UE data volume in gNB
  // Description: This measurement provides the percentage of UL data volume for UEs in the cell that is classified
  //              as unrestricted, i.e., when the volume is so low that all data can be transferred in one slot and
  //              no UE throughput sample could be calculated.
  {"DRB.UEUnresVolUl", NRCellDU, SI, INTEGER, "%", FIVE_QI_LABEL | SLICE_ID_LABEL | PLMN_ID_LABEL, ALL_LEVELS},

  // 5.1.1.3.7 Average DL UE buffered Throughput per DRB
  // Description: This measurement provides the average downlink buffered UE throughput per DRB on NRCellCU.
  {"DRB.PDCP.UEThpDl", NRCellDU | GNBCUUP, DER, REAL, "kbps", FIVE_QI_LABEL, ALL_LEVELS},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.4 RRC connection number
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.4.1 Mean number of RRC Connections
  // Description: This measurement provides the mean number of users in RRC connected mode for each NR cell during
  //              each granularity period.
  {"RRC.ConnMean", NRCellCU, SI, INTEGER, "-", PLMN_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.4.2 Max number of RRC Connections
  // Description: This measurement provides the maximum number of users in RRC connected mode for each NR cell during
  //              each granularity period.
  {"RRC.ConnMax", NRCellCU, SI, INTEGER, "%", PLMN_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.4.3 Mean number of stored inactive RRC Connections
  // Description: This measurement provides the mean number of users in RRC inactive mode for each NR cell during
  //              each granularity period.
  {"RRC.InactiveConnMean", NRCellCU, SI, REAL, "-", PLMN_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.4.4 Max number of stored inactive RRC Connections
  // Description: This measurement provides the max number of users in RRC inactive mode during each granularity period.
  {"RRC.InactiveConnMax", NRCellCU, SI, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.5 PDU Session Management
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.5.1 Number of PDU Sessions requested to setup
  // Description: This measurement provides the number of PDU Sessions by the gNB.
  // Note: mandatory SLICE_ID_LABEL
  {"SM.PDUSessionSetupReq", NRCellCU, CC, INTEGER, "-", SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  //5.1.1.5.2 Number of PDU Sessions successfully setup
  // Description: This measurement provides the number of PDU Sessions successfully setup by the gNB from AMF.
  // Note: mandatory SLICE_ID_LABEL
  {"SM.PDUSessionSetupSucc", NRCellCU, CC, INTEGER, "-", SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.5.3 Number of PDU Sessions failed to setup
  // Description: This measurement provides the number of PDU Sessions failed to setup by the gNB.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"SM.PDUSessionSetupFail", NRCellCU, CC,  INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.5.4 Mean number of PDU sessions being allocated
  // Description: This measurement provides the mean number of PDU sessions that have been allocated in the NRCellCU.
  // Note: mandatory SLICE_ID_LABEL
  {"SM.MeanPDUSessionSetupReq", NRCellCU, SI, INTEGER, "-", SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.5.5 Peak number of PDU sessions being allocated
  // Description: This measurement provides the peak number of PDU sessions that have been allocated in the NRCellCU.
  // Note: mandatory SLICE_ID_LABEL
  {"SM.MaxPDUSessionSetupReq", NRCellCU, SI, INTEGER, "-", SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.6 Mobility Management
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.6.1 Inter-gNB handovers
  // 5.1.1.6.1.1 Number of requested legacy handover preparations
  // Description: This measurement provides the number of legacy handover preparations requested by the source gNB.
  {"MM.HoPrepInterReq", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.2 Number of successful legacy handover preparations
  // Description: This measurement provides the number of successful legacy handover preparations received by
  //              the source NR cell CU.
  {"MM.HoPrepInterSucc", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.3 Number of failed legacy handover preparations
  // Description: This measurement provides the number of failed legacy handover preparations received by
  //              the source NR cell CU.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.HoPrepInterFail", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.4 Number of requested legacy handover resource allocations
  // Description: This measurement provides the number of legacy handover resource allocation requests received
  //              by the target NR cell CU.
  {"MM.HoResAlloInterReq", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.5 Number of successful legacy handover resource allocations
  // Description: This measurement provides the number of successful legacy handover resource allocations at
  //              the target NR cell CU for the handover.
  {"MM.HoResAlloInterSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.6 Number of failed legacy handover resource allocations
  // Description: This measurement provides the number of failed legacy handover resource allocations at
  //              the target NR cell CU for the handover.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.HoResAlloInterFail", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.7 Number of requested legacy handover executions
  // Description: This inter gNB handover measurement provides the number of outgoing legacy handover executions
  //              requested by the source gNB.
  {"MM.HoExeInterReq", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.8 Number of successful legacy handover executions
  // Description: This inter gNB handover measurement provides the number of successful legacy handover executions
  //              received by the source gNB.
  {"MM.HoExeInterSucc", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.9 Number of failed legacy handover executions
  // Description: This inter gNB handover measurement provides the number of failed legacy handover executions for
  //              the source gNB.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  // possible causes: MM.HoExeInterFail.UeCtxtRelCmd.cause; MM.HoExeInterFail.RrcReestabReq;
  //                  MM.HoExeInterFail.HoExeSupTimer; MM.HoExeInterFail.RetrUeCtxtReq;
  {"MM.HoExeInterFail", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.10 Mean Time of requested legacy handover executions
  // Description: This measurement provides the mean time of inter gNB legacy handover executions during each
  //              granularity period.
  {"MM.HoExeInterReq.TimeMean", NRCellCU, DER, INTEGER, "ms", SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.11 Max Time of requested legacy handover executions
  // Description: This measurement provides the max time of inter gNB legacy handover executions during each
  //              granularity period.
  {"MM.HoExeInterReq.TimeMax", NRCellCU, DER, INTEGER, "ms", SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.12 Number of successful handover executions per beam pair
  // Description: This inter gNB handover measurement provides the number of successful handover executions received
  //              by the source gNB per beam pair, i.e. beam in the source and beam in the target cell.
  // Note: do we need SSB_IDX_LABEL?
  {"MM.HoExeInterSSBSucc", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.1.13 Number of failed handover executions per beam pair
  // Description: This inter gNB handover measurement provides the number of failed handover executions for
  //              source gNB per beam pair.
  // Note: do we need SSB_IDX_LABEL?
  // Note: mandatory Cause-label not supported in e2sm_kpm
  // possible causes: MM.HoExeInterSSBFail.UeCtxtRelCmd.cause; MM.HoExeInterSSBFail.RrcReestabReq;
  //                  MM.HoExeInterSSBFail.HoExeSupTimer; MM.HoExeInterSSBFail.RetrUeCtxtReq;
  {"MM.HoExeInterSSBFail", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.2 Intra-gNB handovers
  // 5.1.1.6.2.1 Number of requested legacy handover executions
  // Description: This measurement provides the number of outgoing intra gNB legacy handover executions requested
  //              by the source NRCellCU.
  {"MM.HoExeIntraReq", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.2.2 Number of successful legacy handover executions
  // Description: This measurement provides the number of successful intra gNB legacy handover executions received
  //              by the source NRCellCU.
  {"MM.HoExeIntraSucc", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3  Handovers between 5GS and EPS
  // 5.1.1.6.3.1	Number of requested preparations for handovers from 5GS to EPS
  // Description: This measurement provides the number of preparations requested by the source gNB for the outgoing
  //              handovers from 5GS to EPS.
  {"MM.HoOut5gsToEpsPrepReq", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.2 Number of successful preparations for handovers from 5GS to EPS
  // Description: This measurement provides the number of successful preparations received by the source gNB for
  //              the outgoing handovers from 5GS to EPS.
  {"MM.HoOut5gsToEpsPrepSucc", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.3 Number of failed preparations for handovers from 5GS to EPS
  // Description: This measurement provides the number of failed preparations received by the source gNB for
  //              the outgoing handovers from 5GS to EPS.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.HoOut5gsToEpsPrepFail", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.4 Number of requested resource allocations for handovers from EPS to 5GS
  // Description: This measurement provides the number of resource allocation requests received by the target gNB for
  //              handovers from EPS to 5GS.
  {"MM.HoIncEpsTo5gsResAlloReq", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.5 Number of successful resource allocations for handovers from EPS to 5GS
  // Description: This measurement provides the number of successful resource allocations at the target gNB for
  //              handovers from EPS to 5GS.
  {"MM.HoIncEpsTo5gsResAlloSucc", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.6 Number of failed resource allocations for handovers from EPS to 5GS
  // Description: This measurement provides the number of failed resource allocations at the target gNB for handovers
  //              from EPS to 5GS.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.HoIncEpsTo5gsResAlloFail", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.7 Number of requested executions for handovers from 5GS to EPS
  // Description: This measurement provides the number of executions requested by the source gNB for handovers
  //              from 5GS to EPS.
  {"MM.HoOutExe5gsToEpsReq", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.8 Number of successful executions for handovers from 5GS to EPS
  // Description: This measurement provides the number of successful executions at the source gNB for handovers
  //              from 5GS to EPS.
  {"MM.HoOutExe5gsToEpsSucc", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.9 Number of failed executions for handovers from 5GS to EPS
  // Description: This measurement provides the number of failed executions at the source gNB for handovers
  //              from 5GS to EPS.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.HoOutExe5gsToEpsFail", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.10 Number of requested preparations for EPS fallback handovers
  // Description: This measurement provides the number of EPS fallback preparations requested by the source gNB
  //              for the outgoing handovers from 5GS to EPS.
  {"MM.HoOut5gsToEpsFallbackPrepReq", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.11 Number of successful preparations for EPS fallback handovers
  // Description: This measurement provides the number of successful EPS fallback preparations received
  //              by the source gNB for the outgoing handovers from 5GS to EPS.
  {"MM.HoOut5gsToEpsFallbackPrepSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.12 Number of failed preparations for EPS fallback handovers
  // Description: This measurement provides the number of failed preparations received by the source gNB for
  //              the outgoing handovers from 5GS to EPS.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.HoOut5gsToEpsFallbackPrepFail", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.13 Number of successful executions for EPS fallback handovers
  // Description: This measurement provides the number of successful EPS fallback executions at the source gNB
  //              for handovers from 5GS to EPS.
  {"MM.HoOutExe5gsToEpsFallbackSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.14 Number of failed executions for EPS fallback handovers
  // Description: This measurement provides the number of failed EPS fallback executions at the source gNB for
  //              handovers from 5GS to EPS.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.HoOutExe5gsToEpsFallbackFail", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.15 Mean Time of EPS fallback handover
  // Description: This measurement provides the mean time of EPS fallback whole handover during each granularity period.
  {"MM.Ho5gsToEpsFallbackTimeMean", NRCellCU, DER, INTEGER, "ms", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.3.16 Mean Time of EPS fallback handover executions
  // Description: This measurement provides the mean time of EPS fallback handover executions during each
  //              granularity period.
  {"MM.HoExeHo5gsToEpsFallbackTimeMean", NRCellCU, DER, INTEGER, "ms", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.4 RRC redirection measurement
  // 5.1.1.6.4.1 Number of EPS fallback redirection
  // Description: This measurement provides the number of RRC release for EPS fallback redirection.
  {"MM.Redirection.5gsToEpsFallback", NRCellDU, SI, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.5 Intra/Inter-frequency Handover related measurements
  // 5.1.1.6.5.1 Number of requested intra-frequency handover executions
  // Description: This measurement provides the number of outgoing intra-frequency handover executions requested
  //              by the source NRCellCU.
  {"MM.HoExeIntraFreqReq", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.5.2 Number of successful intra-frequency handover executions
  // Description: This measurement provides the number of successful intra-frequency handover executions received
  //              by the source NRCellCU.
  {"MM.HoExeIntraFreqSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.5.3 Number of requested inter-frequency handover executions
  // Description: This measurement provides the number of outgoing inter-frequency handover executions requested
  //              by the source NRCellCU.
  {"MM.HoExeInterFreqReq", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.5.4 Number of successful inter-frequency handover executions
  // Description: This measurement provides the number of successful inter-frequency handover executions received
  //              by the source NRCellCU.
  {"MM.HoExeInterFreqSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6 Inter-gNB conditional handovers
  // 5.1.1.6.6.1 Number of requested conditional handover preparations
  // Description: This measurement provides the number of conditional handover preparations requested by the source gNB.
  {"MM.ChoPrepInterReq", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6.2 Number of successful conditional handover preparations
  // Description: This measurement provides the number of successful conditional handover preparations received
  //              by the source NR cell CU.
  {"MM.ChoPrepInterSucc", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6.3 Number of failed conditional handover preparations
  // Description: This measurement provides the number of failed conditional handover preparations received
  //              by the source NR cell CU.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.ChoPrepInterFail", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6.4 Number of requested conditional handover resource allocations
  // Description: This measurement provides the number of conditional handover resource allocation requests received
  //              by the target NR cell CU.
  {"MM.ChoResAlloInterReq", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6.5 Number of successful conditional handover resource allocations
  // Description: This measurement provides the number of successful conditional handover resource allocations
  //              at the target NR cell CU for the handover.
  {"MM.ChoResAlloInterSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6.6 Number of failed conditional handover resource allocations
  // Description: This measurement provides the number of failed conditional handover resource allocations
  //              at the target NR cell CU for the handover.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.ChoResAlloInterFail", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6.7 Number of configured conditional handover candidates
  // Description: This inter gNB handover measurement provides the number of outgoing conditional handover candidates
  //              requested by the source gNB.
  {"MM.ConfigInterReqCho", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6.8 Number of UEs configured with conditional handover.
  // Description: This inter-gNB handover measurement provides the number of UEs that has been configured with
  //              conditional handover by the source gNB.
  {"MM.ConfigInterReqChoUes", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6.9	Number of successful conditional handover executions
  // Description: This inter-gNB handover measurement provides the number of successful conditional handover executions
  //              received by the source gNB.
  {"MM.ChoExeInterSucc", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.6.6.10 Void
  // Note: empty in TS

  // 5.1.1.6.6.11 Mean Time of requested conditional handover executions
  // Description: This measurement provides the mean time of inter-gNB conditional handover executions during
  //              each granularity period.
  {"MM.ChoExeInterReq.TimeMean", NRCellCU, DER, INTEGER, "ms", SLICE_ID_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.6.12 Max Time of requested conditional handover executions
  // Description: This measurement provides the max time of inter-gNB conditional handover executions during
  //              each granularity period.
  {"MM.ChoExeInterReq.TimeMax", NRCellCU, DER, INTEGER, "ms", SLICE_ID_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.6.13 Number of UEs for which conditional handover preparations are requested
  // Description: This measurement provides the number of UEs for which conditional handover preparations
  //              were requested by the source gNB.
  {"MM.ChoPrepInterReqUes", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.6.14 Number of UEs for which conditional handover preparations were successful
  // Description: This measurement provides the number of UEs for which successful conditional handover preparations
  //              were received by the source NR cell CU.
  {"MM.ChoPrepInterSuccUes", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.6.15 Number of UEs for which conditional handover preparations failed
  // Description: This measurement provides the number of UEs for which conditional handover preparations failed,
  //              as received by the source NR cell CU.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.ChoPrepInterFailUes", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.7 Intra-gNB conditional handovers
  // 5.1.1.6.7.1 Number of configured conditional handover candidates
  // Description: This measurement provides the number of outgoing intra-gNB conditional handover candidates
  //              requested by the source NRCellCU.
  {"MM.ConfigIntraReqCho", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.7.2 Number of UEs configured with conditional handover
  // Description: This intra-gNB handover measurement provides the number of UEs that has been configured
  //              with conditional handover by the source cell.
  {"MM.ConfigIntraReqChoUes", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.7.3 Number of successful handover executions
  // Description: This measurement provides the number of successful intra-gNB handover executions received
  //              by the source NRCellCU.
  {"MM.ChoExeIntraSucc", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.8 Inter-gNB DAPS handovers
  // 5.1.1.6.8.1 Number of requested DAPS handover preparations
  // Description: This measurement provides the number of DAPS handover preparations requested by the source gNB.
  {"MM.DapsHoPrepInterReq", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.8.2 Number of successful DAPS handover preparations
  // Description: This measurement provides the number of successful DAPS handover preparations received
  //              by the source NR cell CU.
  {"MM.DapsHoPrepInterSucc", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.8.3 Number of failed DAPS handover preparations
  // Description: This measurement provides the number of failed DAPS handover preparations received
  //              by the source NR cell CU.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.DapsHoPrepInterFail", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.8.4 Number of requested DAPS handover resource allocations
  // Description: This measurement provides the number of DAPS handover resource allocation requests received
  //              by the target NR cell CU.
  {"MM.DapsHoResAlloInterReq", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.8.5 Number of successful DAPS handover resource allocations
  // Description: This measurement provides the number of successful DAPS handover resource allocations
  //              at the target NR cell CU for the handover.
  {"MM.DapsHoResAlloInterSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.8.6 Number of failed DAPS handover resource allocations
  // Description: This measurement provides the number of failed DAPS handover resource allocations
  //              at the target NR cell CU for the handover.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"MM.DapsHoResAlloInterFail", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.8.7 Number of requested DAPS handover executions
  // Description: This inter gNB handover measurement provides the number of outgoing DAPS handover executions
  //              requested by the source gNB.
  {"MM.DapsHoExeInterReq", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.8.8 Number of successful DAPS handover executions
  // Description: This inter gNB handover measurement provides the number of successful DAPS handover executions
  //              received by the source gNB.
  {"MM.DapsHoExeInterSucc", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.8.9 Number of failed DAPS handover executions
  // Description: This inter gNB handover measurement provides the number of failed DAPS handover executions.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  // Note: possible causes: MM.DapsHoExeInterFail.UeCtxtRelCmd.cause; MM.DapsHoExeInterFail.RrcReestabReq;
  //                        MM.DapsHoExeInterFail.HoExeSupTimer; MM.DapsHoExeInterFail.RetrUeCtxtReq;
  //                        MM.DapsHoExeInterFail.FailInfo.
  {"MM.DapsHoExeInterFail", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.9 Intra-gNB DAPS handovers
  // 5.1.1.6.9.1 Number of requested handovers
  // Description: This measurement provides the nof outgoing intra-gNB DAPS handovers requested by the source NRCellCU.
  {"MM.DapsHoExeIntraReq", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.6.9.2 Number of successful DAPS handovers
  // Description: This measurement provides the nof successful intra-gNB DAPS handovers received by the source NRCellCU.
  {"MM.DapsHoExeIntraSucc", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.7 TB related Measurements
  // 5.1.1.7.1 Total number of DL initial TBs
  // Description: This measurement provides the total number of initial TBs transmitted on the downlink in a cell.
  //              HARQ re-transmissions are excluded from this measurement.
  // Note: This measurement is optionally split into subcounters per modulation schema, e.g., TB.TotNbrDlInitial.64Qam
  // Note: seems there is no related label in kpm
  {"TB.TotNbrDlInitial", NRCellDU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.7.2 Intial error number of DL TBs
  // Description: This measurement provides the number of initial faulty TBs transmitted on the downlink in a cell.
  // Note: This measurement is optionally split into subcounters per modulation schema, e.g., TB.IntialErrNbrDl.64Qam
  // Note: seems there is no related label in kpm
  {"TB.IntialErrNbrDl", NRCellDU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.7.3 Total number of DL TBs
  // Description: This measurement provides the total number of TBs transmitted on the downlink in a cell.
  //              This measurement includes all transmitted TBs (including the successful and failed TBs during initial
  //              transmission and HARQ re-transmission).
  // Note: The measurement is split into subcounters per layer at MU-MIMO case.
  {"TB.TotNbrDl", NRCellDU, CC, INTEGER, "-", LAYER_MU_MIMO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.7.4	Total error number of DL TBs
  // Description: This measurement provides the number of total faulty TBs transmitted on the downlink in a cell.
  //              This measurement include all transmitted  faulty TBs  of initial transmission and re-transmission.
  // Note: The measurement is split into subcounters per layer at MU-MIMO case.
  {"TB.ErrTotNbrDl", NRCellDU, CC, INTEGER, "-", LAYER_MU_MIMO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.7.5 Residual error number of DL TBs
  // Description: This measurement provides the number of final faulty TBs transmitted on the downlink in a cell
  //              at last HARQ re-transmissions.
  {"TB.ResidualErrNbrDl", NRCellDU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.7.6 Total number of UL initial TBs
  // Description: This measurement provides the total number of initial TBs on the uplink in a cell.
  // Note: This measurement is optionally split into subcounters per modulation schema, e.g., TB.TotNbrUlInit.64Qam
  // Note: seems there is no related label in kpm
  {"TB.TotNbrUlInit", NRCellDU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.7.7 Error number of UL initial TBs
  // Description: This measurement provides the number of initial faulty TBs on the uplink in a cell.
  // Note: This measurement is optionally split into subcounters per modulation schema, e.g., TB.TotNbrUlInit.64Qam
  // Note: seems there is no related label in kpm
  {"TB.ErrNbrUlInitial", NRCellDU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.7.8 Total number of UL TBs
  // Description: This measurement provides the total number of TBs on the uplink in a cell.This measurement includes
  //              all transmitted TBs (including the successful and failed TBs during initial transmission and
  //              HARQ re-transmission).
  // Note: The measurement is split into subcounters per layer at MU-MIMO case.
  {"TB.TotNbrUl", NRCellDU, CC, INTEGER, "-", LAYER_MU_MIMO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.7.9 Total error number of UL TBs
  // Description: This measurement provides the number of total faulty TBs on the uplink in a cell.
  //              This measurement include all transmitted faulty TBs of initial  and re-transmission .
  // Note:  The measurement is split into subcounters per layer at MU-MIMO case.
  {"TB.ErrTotNbrUl", NRCellDU, CC, INTEGER, "-", LAYER_MU_MIMO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.7.10 Residual error number of UL TBs
  // Description: This measurement provides the number of final faulty TBs on the uplink in a cell.
  {"TB.ResidualErrNbrUl", NRCellDU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.8 Void - empty in TS
  // 5.1.1.9 Void - empty in TS

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.10 DRB related measurements
  // Note: Measurement definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE and UE levels.
  // 5.1.1.10.1	Number of DRBs attempted to setup
  // Description: This measurement provides the number of DRBs attempted to setup to support all requested QoS
  //              flows in the PDU sessions to be setup by the INITIAL CONTEXT SETUP REQUESTs,
  //              PDU SESSION RESOURCE SETUP REQUESTs and PDU SESSION RESOURCE MODIFY REQUEST message received
  //              by the gNB from AMF.
  {"DRB.EstabAtt", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.2 Number of DRBs successfully setup
  // Description: This measurement provides the number of DRBs successfully setup to support all requested QoS flows
  //              in the PDU sessions to be setup by the INITIAL CONTEXT SETUP REQUESTs, PDU SESSION RESOURCE SETUP
  //               REQUESTs and PDU SESSION RESOURCE MODIFY REQUEST message received by the gNB from AMF.
  {"DRB.EstabSucc", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.3 Number of released active DRBs
  // Description: This measurement provides the nof abnormally released DRBs that were active at the time of release.
  {"DRB.RelActNbr", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.4 In-session activity time for DRB
  // Description: This measurement provides the aggregated active session time for DRBs in a cell.
  // Note: number of "in session" seconds aggregated for DRBs with a certain mapped 5QI level or for a certain S-NSSAI
  // Note: Unit changed from s to ms in O-RAN.WG3.E2SM-KPM-R003-v3.00
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  {"DRB.SessionTime", NRCellCU, CC, INTEGER, "ms", FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.1.10.5 Number of Initial DRBs attempted to setup
  // Description: This measurement provides the number of initial DRBs attempted to setup to support all requested
  //              QoS flows in the PDU sessions to be setup by the INITIAL CONTEXT SETUP REQUEST messages received
  //              by the gNB from AMF.
  {"DRB.InitialEstabAtt", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.6 Number of Initial DRBs successfully setup
  // Description: This measurement provides the number of initial DRBs successfully setup to support all requested
  //              QoS flows in the PDU sessions to be setup by the INITIAL CONTEXT SETUP REQUEST messages received
  //              by the gNB from AMF.
  {"DRB.InitialEstabSucc", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.7 Number of DRBs attempted to be resumed
  // Description: This measurement provides the number of DRBs attempted to be resumed.
  {"DRB.ResumeAtt", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.8 Number of DRBs successfuly resumed
  // Description: This measurement provides the total successful number of DRBs successfuly resumed.
  {"DRB.ResumeSucc", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.9 Mean number of DRBs being allocated
  // Description: This measurement provides the mean number of DRBs that have been allocated.
  // Note: why not REAL if mean?
  {"DRB.MeanEstabSucc", NRCellCU, SI, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.10 Peak number of DRBs being allocated
  // Description: This measurement provides the peak number of DRBs that have been allocated.
  {"DRB.MaxEstabSucc", NRCellCU, SI, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.11 Mean number of DRBs undergoing from User Plane Path Failures
  // Description: This measurement provides the number of DRB’s prone to GTP-U Error Indication, the 5G CU-UP shall
  //              return a GTP-U Error Indication if it does not have a corresponding GTP-U context.
  // Note: why not REAL if mean?
  {"DRB.GTPUPathFailure", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.12 Number of DRBs attempted to setup in case of Dual Connectivity
  // Description: This measurement provides the number of DRBs attempted to setup to support all requested QoS flows
  //              in the PDU sessions to be setup by the S-NODE ADDITION REQUESTs and S-NODE MODIFICATION REQUESTs
  //              message received by the S-NG-RAN node from M-NG-RAN node .
  {"DRB.EstabAttDC", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.10.13 Number of DRBs successfully setup in case of Dual Connectivity
  // Description: This measurement provides the number of DRBs successfully setup to support all requested QoS flows
  //              in the PDU sessions to be setup by the S-NODE ADDITION REQUESTs and S-NODE MODIFICATION REQUESTs
  //              message received by the S-NG-RAN node from M-NG-RAN node .
  {"DRB.EstabSuccDC", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.11 CQI related measurements
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.11.1 Wideband CQI distribution
  // Description: This measurement provides the distribution of Wideband CQI (Channel Quality Indicator)
  //              reported by UEs in the cell.
  // Note: CARR.WBCQIDist.BinX.BinY.BinZ, where X represents the index of the CQI value (0 to 15).
  //       Y represents the index of rank value (1 to 8), Z represents the index of table value (1 to 4).
  {"CARR.WBCQIDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL | DIST_BIN_Y_LABEL | DIST_BIN_X_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.12 MCS related Measurements
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.12.1 MCS Distribution in PDSCH
  // Description: This measurement provides the distribution of the MCS scheduled for PDSCH RB by NG-RAN.
  // Note: CARR.PDSCHMCSDist.BinX.BinY.BinZ, where X represents the index of rank value (1 to 8),
  //       Y represents the index of table value (1 to 3), and Z represents the index of the MCS value (0 to 31).
  {"CARR.PDSCHMCSDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL | DIST_BIN_Y_LABEL | DIST_BIN_X_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.12.2 MCS Distribution in PUSCH
  // Description: This measurement provides the distribution of the MCS scheduled for PUSCH RB by NG-RAN.
  // Note: CARR.PUSCHMCSDist.BinX.BinY.BinZ, where X represents the index of rank value (1 to 8),
  //        Y represents the index of table value (1 to 2), and Z represents the index of the MCS value (0 to 31).
  {"CARR.PUSCHMCSDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL | DIST_BIN_Y_LABEL | DIST_BIN_X_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.12.3 PDSCH MCS Distribution for MU-MIMO
  // Description: This measurement provides the distribution of the MCS scheduled for PDSCH RB by NG-RAN
  //              in MU-MIMO scenario.
  // Note: CARR.MUPDSCHMCSDist.BinX, where X represents the index of the MCS value (0 to 31).
  {"CARR.MUPDSCHMCSDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.12.4 PUSCH MCS Distribution for MU-MIMO
  // Description: This measurement provides the distribution of the MCS scheduled for PUSCH RB by NG-RAN
  //              in MU-MIMO scenario.
  // Note: CARR. MUPUSCHMCSDist.BinX, where X represents the index of the MCS value (0 to 31).
  {"CARR.MUPUSCHMCSDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.13 QoS flow related measurements
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.13.1 QoS flow release
  // 5.1.1.13.1.1 Number of released active QoS flows
  // Description: This measurement provides the number of released QoS flows that were active at the time of release.
  // Note: The nof measurements is equal to the nof QoS flows plus a possible sum value identified by the .sum suffix.
  {"QF.RelActNbr", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SUM_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.1.2 Number of QoS flows attempted to release
  // Description: This measurement provides the number of QoS flows attempted to release.
  {"QF.ReleaseAttNbr", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.2 QoS flow activity
  // 5.1.1.13.2.1 In-session activity time for QoS flow
  // Description: This measurement provides the aggregated active session time for QoS flow in a cell.
  // Note: The nof measurements is equal to the nof QoS levels plus a possible sum value identified by the .sum suffix.
  // Note: Unit changed from s to ms in O-RAN.WG3.E2SM-KPM-R003-v3.00
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  {"QF.SessionTimeQoS", NRCellCU, CC, INTEGER, "ms", FIVE_QI_LABEL | SUM_LABEL, ALL_LEVELS},

  // 5.1.1.13.2.2 In-session activity time for UE
  // Description: This measurement provides the aggregated active session time for UEs in a cell.
  // Note: Unit changed from s to ms in O-RAN.WG3.E2SM-KPM-R003-v3.00
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  {"QF.SessionTimeUE", NRCellCU, CC, INTEGER, "ms", NO_LABEL, ALL_LEVELS},

  // 5.1.1.13.3 QoS flow setup
  // 5.1.1.13.3.1 Number of QoS flow attempted to setup
  // Description: This measurement provides the number of QoS flows attempted to setup.
  // Note: The nof measurements is equal to the nof QoS levels plus the number of S-NSSAIs, plus a possible sum value
  //       identified by the .sum suffix.
  {"QF.EstabAttNbr", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL | SUM_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.3.2 Number of QoS flow successfully established
  // Description: This measurement provides the number of QoS flows successfully established.
  // Note: The nof measurements is equal to the nof QoS levels plus a possible sum value identified by the .sum suffix.
  {"QF.EstabSuccNbr", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL | SUM_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.3.3 Number of QoS flow failed to setup
  // Description: This measurement provides the number of QoS flows failed to setup.
  // Note: The nof measurements is equal to the nof causes plus a possible sum value identified by the .sum suffix.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"QF.EstabFailNbr", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.3.4 Number of Initial QoS flow attempted to setup
  // Description: This measurement provides the number of Initial QoS flows attempted to setup.
  // Note: The nof measurements is equal to the nof QoS levels plus the number of S-NSSAIs, plus a possible sum value
  //       identified by the .sum suffix.
  {"QF.InitialEstabAttNbr", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL | SUM_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.3.5 Number of Initial QoS flow successfully established
  // Description: This measurement provides the number of Initial QoS flows successfully established.
  // Note: The number of measurements is equal to the number of QoS levels plus a possible sum
  //       value identified by the .sum suffix.
  {"QF.InitialEstabSuccNbr", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL | SUM_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.3.6 Number of Initial QoS flow failed to setup
  // Description: a)	This measurement provides the number of Initial QoS flows failed to setup.
  // Note: The nof measurements is equal to the nof causes plus a possible sum value identified by the .sum suffix.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"QF.InitialEstabFailNbr", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.4 QoS flow modification
  // 5.1.1.13.4.1 Number of QoS flows attempted to modify
  // Description: This measurement provides the number of QoS flows attempted to modify.
  {"QF.ModNbrAtt", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.4.2 Number of QoS flows successfully modified
  // Description: This measurement provides the number of QoS flows successfully modified.
  {"QF.ModNbrSucc", NRCellCU, CC, INTEGER, "-", FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.13.4.3 Number of QoS flows failed to modify
  // Description: This measurement provides the number of QoS flows failed to modify.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"QF.ModNbrFail", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.14 Void - empty in TS

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.15  RRC connection establishment related measurements
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.15.1 Attempted RRC connection establishments
  // Description: This measurement provides the nof RRC connection establishment attempts for each establishment cause.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"RRC.ConnEstabAtt", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.15.2 Successful RRC connection establishments
  // Description: This measurement provides the nof successful RRC establishments for each establishment cause.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"RRC.ConnEstabSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.15.3 Failed RRC connection establishments
  // Description: This measurement provides the nof failed RRC establishments, this measurmenet is split into
  //              subcounters per failure cause.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  // Note: cause labels: RRC.ConnEstabFailCause.NetworkReject, RRC.ConnEstabFailCause.NoReply,
  //                     RRC.ConnEstabFailCause.Other
  {"RRC.ConnEstabFailCause", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.16 UE-associated logical NG-connection related measurements
  // 5.1.1.16.1 Attempted UE-associated logical NG-connection establishment from gNB to AMF
  // Description: This measurement provides the number of attempted UE-associated logical NG-connection establishments
  //              from gNB to AMF, for each RRCSetupRequest establishment cause.
  // Note: The number of measurements is equal to the number of establishment causes.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"UECNTX.ConnEstabAtt", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.16.2 Successful UE-associated logical NG-connection establishment from gNB to AMF
  // Description: This measurement provides the number of successful UE-associated logical NG-connection establishments
  //              from gNB to AMF, for each RRCSetupRequest establishment cause.
  // Note: The number of measurements is equal to the number of establishment causes.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"UECNTX.ConnEstabSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.17 RRC Connection Re-establishment
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.17.1 Number of RRC connection re-establishment attempts
  // Description: This measurement provides the number of RRC connection re-establishment attempts.
  {"RRC.ReEstabAtt", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.17.2 Successful RRC connection re-establishment with UE context
  // Description: This measurement provides the successful number of RRC connection re-establishment when UE context
  //              can be retrieved.
  {"RRC.ReEstabSuccWithUeContext", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.17.3 Successful RRC connection re-establishment without UE context
  // Description: This measurement provides the successful number of RRC connection re-establishment when UE context
  //              can not be retrieved.
  {"RRC.ReEstabSuccWithoutUeContext", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.17.4 Number of RRC connection re-establishment attempts followed by RRC Setup
  // Description: This measurement provides the number of RRC connection re-establishment attempts where no UE context
  //              could be retrieved and therefore fallback to RRC Setup procedure was attempted.
  {"RRC.ReEstabFallbackToSetupAtt", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.18 RRC Connection Resuming
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.18.1 Number of RRC connection resuming attempts
  // Description: This measurement provides the number of RRC connection resuming attempts.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"RRC.ResumeAtt", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.18.2 Successful RRC connection resuming
  // Description: This measurement provides the total successful number of RRC connection resuming.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"RRC.ResumeSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.18.3 Successful RRC connection resuming with fallback
  // Description: This measurement provides the successful number of RRC connection resuming by fallback
  //              to RRC connection establishment.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"RRC.ResumeSuccByFallback", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.18.4 RRC connection resuming followed by network release
  // Description: This measurement provides the number of RRC connection resuming followed by network release.
  {"RRC.ResumeFollowedbyNetworkRelease", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.18.5 RRC connection resuming followed by network suspension
  // Description: This measurement provides the number of RRC connection resuming followed by network suspension.
  {"RRC.ResumeFollowedbySuspension", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.18.6 Number of RRC connection resuming attempts followed by RRC Setup
  // Description: This measurement provides the number of RRC connection resuming attempts where no UE context
  //              could be retrieved and therefore fallback to RRC Setup procedure was attempted.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"RRC.ResumeFallbackToSetupAtt", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.19 Power, Energy and Environmental (PEE) measurements
  // 5.1.1.19.1 Applicability of measurements -> no metric here

  // 5.1.1.19.2 PNF Power Consumption
  // 5.1.1.19.2.1 Average Power
  // Description: This measurement provides the average power consumed over the measurement period.
  {"PEE.AvgPower", E2_Node, SI, REAL, "W", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.119.2.2 Minimum Power
  // Description: This measurement provides the minimum power consumed during the measurement period
  {"PEE.MinPower", E2_Node, SI, REAL, "W", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.19.2.3 Maximum Power
  // Description: This measurement provides the maximum power consumed during the measurement period.
  {"PEE.MaxPower", E2_Node, SI, REAL, "W", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.19.3 PNF Energy consumption
  // Description: This measurement provides the energy consumed.
  {"PEE.Energy", E2_Node, SI, REAL, "kWh", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.19.4 PNF Temperature
  // 5.1.1.19.4.1 Average Temperature
  // Description: This measurement provides the average temperature over the measurement period.
  {"PEE.AvgTemperature", E2_Node, SI, REAL, "C", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.19.4.2 Minimum Temperature
  // Description: This measurement provides the minimum temperature during the measurement period.
  {"PEE.MinTemperature", E2_Node, SI, REAL, "C", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.19.4.3 Maximum Temperature
  // Description: This measurement provides the maximum temperature during the measurement period.
  {"PEE.MaxTemperature", E2_Node, SI, REAL, "C", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.19.5 PNF Voltage
  // Description: This measurement provides the voltage.
  {"PEE.Voltage", E2_Node, SI, REAL, "V", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.19.6 PNF Current
  // Description: This measurement provides the current.
  {"PEE.Current", E2_Node, SI, REAL, "A", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.19.7 PNF Humidity
  // Description: This measurement provides the percentage of humidity during the measurement period
  {"PEE.Humidity", E2_Node, SI, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.20 Received Random Access Preambles
  // 5.1.1.20.1 Received Random Access Preambles per cell
  // Description: This measurement provides the average (arithmetic mean) number of RACH preambles received in a cell.
  // Note: The number of measurements is equal to three.
  {"RACH.PreambleDedCell", NRCellDU, DER, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},
  {"RACH.PreambleACell", NRCellDU, DER, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},
  {"RACH.PreambleBCell", NRCellDU, DER, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.20.2 Received Random Access Preambles per SSB
  // Description: This measurement provides the average number of RACH preambles received in a cell per SSB.
  // Note: The number of measurements is equal to three times the number of SSB beams defined in the cell.
  {"RACH.PreambleDed", NRCellDU, DER, INTEGER, "-", SSB_IDX_LABEL, E2_NODE_LEVEL},
  {"RACH.PreambleA", NRCellDU, DER, INTEGER, "-", SSB_IDX_LABEL, E2_NODE_LEVEL},
  {"RACH.PreambleB", NRCellDU, DER, INTEGER, "-", SSB_IDX_LABEL, E2_NODE_LEVEL},

  // 5.1.1.20.3 Distribution of number of RACH preambles per cell
  // Description: This measurement provides the distribution of the number of RACH preambles sent by the UE when
  //              successfully accessing the network, as reported by the UEs inside the RA-ReportList-r16 IE
  //              in the UEInformationResponse-r16 message.
  // Note: RACH.PreambleDist.Bin where Bin is to identify the bins associated with the number of preambles sent.
  {"RACH.PreambleDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.20.4	Distribution of RACH access delay
  // Description: This measurement provides an estimate of the distribution of the RACH access delay, that is the
  //              interval from the time a UE sends its first RACH preamble until the UE is connected to the network.
  // Note: RACH.AccessDelayDist.Bin where Bin is to identify the bins associated with the RACH access delay.
  {"RACH.AccessDelayDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.21 Intra-NRCell SSB Beam switch Measurement
  // 5.1.1.21.1 Number of requested Intra-NRCell SSB Beam switch executions
  // Description: This measurement provides the number of outgoing intra-NRCell SSB Beam switch executions requested
  //              by the source SSB Beam in an NRCell in case the beam switch function is enabled (see TS 38.331[20]).
  {"MR.IntraCellSSBSwitchReq", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.21.2 Number of successful Intra-NRCell SSB  Beam switch executions
  // Description: This measurement provides the number of successful intra-NRcell SSB Beam switch executions received
  //              by the source SSB Beam in case the beam switch function is enabled (see TS 38.331[20]).
  {"MR.IntrCellSuccSSBSwitch", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.22 RSRP Measurement
  // 5.1.1.22.1 SS-RSRP distribution per SSB
  // Description: This measurement provides the distribution of SS-RSRP per SSB (see TS 38.215 [34]) received by gNB
  //              from UEs in the cell when SS-RSRP is used for L1-RSRP as configured by reporting configurations
  //              as defined in TS 38.214 [33], in case  the L1-RSRP report function is enabled.
  // Note: L1M.SS-RSRP.Bin, where Bin represents the range of reported SS-RSRP value (0 to 127 dBm)
  // Note: Number of bins and the range for each bin is left to implementation.
  {"L1M.SS-RSRP", BEAM, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.22.2 SS-RSRP distribution per SSB of neighbor NR cell
  // Description: This measurement provides the distribution of SS-RSRP per SSB (see TS 38.215 [34]) of a neighbour
  //              NR cell received by gNB from UEs when SS-RSRP is used for L1-RSRP as configured by reporting
  //              configurations as defined in TS 38.214 [33], in case the L1-RSRP report function is enabled.
  // Note: L1M.SS-RSRPNrNbr.SSBIndex.Bin where SSBIndex identifies the SSB beam of the neighbor NR cell; and
  //       the Bin represents the range of reported SS-RSRP value (0 to 127).
  {"L1M.SS-RSRPNrNbr", NRCellCU | NRCellRelation, CC, INTEGER, "-", SSB_IDX_LABEL | DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.22.3 RSRP distribution per neighbor E-UTRAN cell
  // Description: This measurement provides the distribution of RSRP per neighbour E-UTRA cell received by gNB from UEs.
  // Note: L1M.RSRPEutraNbr.Bin, where the Bin represents the range of reported RSRP value to 97).
  // Note: Number of bins and the range for each bin is left to implementation.
  {"L1M.RSRPEutraNbr", NRCellCU | EutranRelation, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.22.4 SRS-RSRP measurement
  // Description: This measurement provides the distribution of SRS-RSRP received by gNB from UEs in the cell.
  // Note: MR.NRScSRSRSRP.Bin, where a Bin represents a range from the measured SRS-RSRP value (-140 to -44 dBm)
  // Note: Number of bins and the range for each bin is left to implementation.
  {"MR.NRScSRSRSRP", NRCellCU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.23 Number of Active Ues
  // Note: Measurement definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE and UE levels.
  // 5.1.1.23.1 Mean number of Active UEs in the DL per cell
  // Description: This measurement provides the mean number of active DRBs for UEs in an NRCellDU.
  // Note: why INTEGER if mean value?
  {"DRB.MeanActiveUeDl", NRCellDU, DER, INTEGER, "-", PLMN_ID_LABEL | SLICE_ID_LABEL | FIVE_QI_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.23.2 Max number of Active UEs in the DL per cell
  // Description: This measurement provides the max number of active DRBs for UEs in an NRCellDU.
  // Note: type not specified in TS
  {"DRB.MaxActiveUeDl", NRCellDU, DER, INTEGER, "-", PLMN_ID_LABEL | SLICE_ID_LABEL | FIVE_QI_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.23.3 Mean number of Active UEs in the UL per cell
  // Description: This measurement provides the mean number of active DRBs for UEs in an NRCellDU.
  // Note: why INTEGER if mean value?
  {"DRB.MeanActiveUeUl", NRCellDU, DER, INTEGER, "-", PLMN_ID_LABEL | SLICE_ID_LABEL | FIVE_QI_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.23.4 Max number of Active UEs in the UL per cell
  // Description: This measurement provides the max number of active DRBs for UEs in an NRCellDU.
  // Note: type not specified in TS
  {"DRB.MaxActiveUeUl", NRCellDU, DER, INTEGER, "-", PLMN_ID_LABEL | SLICE_ID_LABEL | FIVE_QI_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.24 5QI 1 QoS Flow Duration Monitoring
  // Note: Definitions were changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE_LEVEL and UE_LEVEL
  // 5.1.1.24.1 Average Normally Released Call (5QI 1 QoS Flow) Duration
  // Description: This measurement provides the average value of normally released call (5QI 1 QoS Flow) duration.
  // Note: why INTEGER is mean value?
  {"5QI1QoSflow.Rel.Average.NormCallDuration", NRCellCU, CC, INTEGER, "ms", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.24.2 Average Abnormally Released Call (5QI 1 QoS Flow) Duration
  // Description: This measurement provides the average value of abnormally released call (5QI 1 QoS Flow) duration.
  // Note: why INTEGER is mean value?
  {"5QI1QoSflow.Rel.Average.AbnormCallDuration", NRCellCU, CC, INTEGER, "ms", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.24.3 Distribution of Normally Released Call (5QI 1 QoS Flow) Duration
  // Description: This measurement provides the histogram result of the samples related to normally released call
  //             (5QI 1 QoS Flow) duration collected during measurement period duration.
  // Note: 5QI1QoSflow.Rel.NormCallDurationBinX where X denotes the X-th bin from total number of N configured bins.
  //       X-th bin stands for the normal call duration which is within the range from t{x-1} to t{x}.
  {"5QI1QoSflow.Rel.NormCallDuration", NRCellCU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.1.24.4 Distribution of Abnormally Released Call (5QI 1 QoS Flow) Duration
  // Description: This measurement provides the histogram result of the samples related to abnormally released call
  //              (5QI 1 QoS Flow) duration collected during measurement period duration.
  // Note: 5QI1QoSflow.Rel.AbnormCallDurationBinX where X denotes the X-th bin from total number of N configured bins.
  //       X-th bin stands for the abnormal call duration which is within the range from t{x-1} to t{x}.
  {"5QI1QoSflow.Rel.AbnormCallDuration", NRCellCU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.25 Measurements related to MRO
  // 5.1.1.25.1 Handover failures related to MRO for intra-system mobility
  // Description: This measurement provides the number of handover failure events related to MRO detected
  //              during the intra-system mobility within 5GS, see TS 38.300 [49] clause 15.5.2.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  // Note: available cause labels: HO.IntraSys.TooEarly, HO.IntraSys.TooLate, HO.IntraSys.ToWrongCell
  {"HO.IntraSys.TooEarly", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},
  {"HO.IntraSys.TooLate", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},
  {"HO.IntraSys.ToWrongCell", NRCellCU | NRCellRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.25.2 Handover failures related to MRO for inter-system mobility
  // Description: This measurement provides the number of handover failure events delated to MRO detected
  //              during the inter-system mobility between NG-RAN and E-UTRAN, limited to the scenarios defined
  //              in TS 38.300 [49] clause 15.5.2.2.3.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  // Note: available cause labels: HO.InterSys.TooEarly, HO.InterSys.TooLate
  {"HO.InterSys.TooEarly", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},
  {"HO.InterSys.TooLate", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.25.3 Unnecessary handovers for inter-system mobility
  // Description: This measurement provides the number of unnecessary handover events detected during
  //              the inter-system mobility from NG-RAN to E-UTRAN, see TS 38.300 [49] clause 15.5.2.3.
  {"HO.InterSys.Unnecessary", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.25.4 Handover ping-pong for inter-system mobility
  // Description: This measurement provides the number of handover ping-pong events detected during
  //              the inter-system mobility between NG-RAN and E-UTRAN, see TS 38.300 [49] clause 15.5.2.4.
  {"HO.InterSys.PingPong", NRCellCU | EutranRelation, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.25.5 Handover failures per beam-cell pair related to MRO for intra-system mobility
  // Description: This measurement provides the number of handover failure events per beam-cell pair
  //              (source beam, i.e., the last beam before failure, and target cell) related to MRO
  //              detected during the intra-system mobility within 5GS.
  // Note: NR Cell Identity (NCI) label (filter) not available, but cell_scope
  {"HO.IntraSys.bTooEarly.NCI", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},
  {"HO.IntraSys.bTooLate.NCI", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},
  {"HO.IntraSys.bToWrongCell.NCI", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.25.6 Handover failures per beam-cell pair related to MRO for inter-system mobility
  // Description: This measurement provides the number of handover failure events per beam-cell pair
  //              (source beam, i.e., the last beam before failure, and target cell) related to MRO
  //              detected during the inter-system mobility from 5GS to EPS.
  // Note: E-UTRAN Cell Global Identifier (ECGI) label (filter) not available, but cell_scope
  {"HO.InterSys.bTooLate.ECGI", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.25.7 Unnecessary handovers per beam-cell pair for inter-system mobility
  // Description: This measurement provides the number of unnecessary handover events per beam-cell pair
  //              (source beam, i.e., the last beam before handover, and target cell) detected during
  //              the inter-system mobility from 5GS to EPS.
  // Note: E-UTRAN Cell Global Identifier (ECGI) label (filter) not available, but cell_scope
  {"HO.InterSys.bUnnecessary.ECGI", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.25.8 Handover ping-pong per beam-cell pair for inter-system mobility
  // Description: This measurement provides the number of handover ping-pong events per beam-cell pair
  //              (source beam, i.e., the last beam before failure, and target cell) detected during
  //              the inter-system mobility from 5GS to EPS.
  // Note: NR Cell Identity (NCI) label (filter) not available, but cell_scope
  {"HO.InterSys.bPingPong.NCI", BEAM, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.26 PHR Measurement
  // 5.1.1.26.1 Type 1 power headroom distribution
  // Description: This measurement provides a bin distribution (histogram) of Type 1 power headroom measurements.
  // Note: L1M.PHR1.BinX, where X represents the range of PHR value (-32 ...+38 dB) (See in TS 38.133 [32])
  // Note: Number of bins and the range for each bin is left to implementation.
  {"L1M.PHR1", NRCellDU , CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.27 Paging Measurement
  // 5.1.1.27.1 Number of CN Initiated paging records received by the gNB-CU
  // Description: This measurement provides number of CN Initiated paging records received by the gNB-CU.
  {"PAG.ReceivedNbrCnInitiated", GNBCUUP, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.27.2 Number of NG-RAN Initiated paging records received by the gNB-CU
  // Description: This measurement provides numbe of NR RAN Initiated paging records received by the gNB-CU.
  {"PAG.ReceivedNbrRanIntiated", GNBCUUP, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.27.3 Number of paging records received by the NRCellDU
  // Description: This measurement provides number of paging records received by gNB-DU which shall perform paging
  //              of the UE in cells which belong to cells as indicated in the Paging Cell List IE.
  {"PAG.ReceivedNbr", NRCellDU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.27.4 Number of CN Initiated paging records discarded at the gNB-CU
  // Description: This measurement provides number of CN Initiated paging records discarded at the gNB-CU.
  {"PAG.DiscardedNbrCnInitiated", GNBCUUP, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.27.5 Number of NG-RAN Initiated paging records discarded at the gNB-CU
  // Description: This measurement provides number of NG-RAN Initiated paging records discarded at the gNB-CU.
  {"PAG.DiscardedNbrRanInitiated", GNBCUUP, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.27.6 Number of paging records discarded at the NRCellDU
  // Description: This measurement provides number of paging records discarded at gNB-DU in cells
  //              as indicated in the Paging Cell List IE.
  {"PAG.DiscardedNbr", NRCellDU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.28 SSB beam related Measurement
  // 5.1.1.28.1 Number of UE related the SSB beam Index (mean)
  // Description: This measurement provides number of UE related the SSB beam index.
  {"L1M.SSBBeamRelatedUeNbr", BEAM, CC, INTEGER, "-", SSB_IDX_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.29 Transmit power utilization measurements
  // 5.1.1.29.1 Maximum transmit power of NR cell
  // Description: This measurement provides the maximum carrier transmit power in the measurement granularity interval.
  {"CARR.MaxTxPwr", NRCellDU, SI, REAL, "dBm", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.29.2 Mean transmit power of NR cell
  // Description: This measurement provides the mean carrier transmit power in the measurement granularity interval.
  {"CARR.NRCellDU", NRCellDU, SI, REAL, "dBm", NO_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.30 MU-MIMO related measurements
  // 5.1.1.30.1 Scheduled PDSCH RBs per layer of MU-MIMO
  // Description: This measurement provides the distribution of the scheduled PDSCH RBs
  //              per MU-MIMO layer by NG-RAN in MU-MIMO scenario.
  // Note: CARR.MUPDSCHRB.BINX, where X represents the MU-MIMO layer value (2 to n).
  // Note: why not LAYER_MU_MIMO_LABEL?
  {"CARR.MUPDSCHRB", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.30.2 Scheduled PUSCH RBs per layer of MU-MIMO
  // Description: This measurement provides the distribution of the scheduled PUSCH RBs
  //              per MU-MIMO layer by NG-RAN in   MU-MIMO scenario.
  // Note: CARR.MUPUSCHRB.BINX, where X represents the MU-MIMO layer value (2 to n).
  // Note: why not LAYER_MU_MIMO_LABEL?
  {"CARR.MUPUSCHRB", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.30.3 PDSCH Time-domain average Maximum Scheduled Layer Number of cell for MIMO scenario
  // Description: This measurement provides the Time-domain average maximum scheduled layer number
  //              for PDSCH under MIMO scenario in the downlink.
  {"RRU.MaxLayerDlMimo", NRCellDU, SI, REAL, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.30.4 PUSCH Time-domain average Maximum Scheduled Layer Number of cell for MIMO scenario
  // Description: This measurement provides the Time-domain average maximum scheduled layer number
  //              for PUSCH under MIMO scenario in the uplink.
  {"RRU.MaxLayerUlMimo", NRCellDU, SI, REAL, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.30.5 Average value of scheduled MIMO layers per PRB on the DL
  // Description: This measurement provides the average value of allocated MIMO layers on the downlink
  //              per PRB per cell, for MIMO scenario within the measurement period.
  {"CARR.AverageLayersDl", NRCellDU, SI, REAL, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.30.6 Average value of scheduled MIMO layers per PRB on the UL
  // Description: This measurement provides the average value of allocated MIMO layers on the uplink per PRB per cell,
  //              for MIMO scenario within the measurement period.
  {"CARR.AverageLayersUl", NRCellDU, SI, REAL, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.30.7 Available MIMO Layers Coverage Map per UE and per PRB on the DL
  // Description: This measurement provides the sub-cell radius within the observed cell up to where a UE
  //              with the given number of MIMO layers in DL per PRB can be localized.
  // Note: MIMOLayersDLy where y ranges from 1 to maximum possible MIMO layers that can be assigned per UE and PRB in DL
  // Note: wrong name format in TS?
  // Note: meter label (filter) not available in e2sm_kpm
  {"MIMOLayersDLy", NRCellDU, SI, REAL, "m", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.30.8 Available MIMO Layers Coverage Map per UE and per PRB on the UL
  // Description: This measurement provides the sub-cell radius within the observed cell up to
  //              where an UE with the given number of MIMO layers in UL per PRB can be localized.
  // Note: MIMOLayersULy where y ranges from 1 to maximum possible MIMO layers that can be assigned per UE and PRB in UL
  // Note: wrong name format in TS?
  // Note: meter label (filter) not available in e2sm_kpm
  {"MIMOLayersULy", NRCellDU, SI, REAL, "m", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.1.30.9 Distribution of Scheduled PDSCH PRBs based on MIMO Layers Coverage Map in DL
  // Description: This measurement provides Distribution of Scheduled PDSCH PRBs based on MIMO Layers Coverage Map
  //              per UE and per PRB on the DL.
  // Note: PDSCHPRBsLayer.BINy where y represents the number of layers that can be assigned per PRB on PDSCH to the UE
  //       on the distance from base station within the following interval: (MIMOLayersDL(y-1), MIMOLayersDLy>,
  //       where MIMOLayersDLy represents measurement defined in the clause 5.1.1.30.7
  {"PDSCHPRBsLayer", NRCellDU, SI, REAL, "-", DIST_BIN_Y_LABEL, E2_NODE_LEVEL},

  // 5.1.1.30.10 Distribution of Scheduled PUSCH PRBs based on MIMO Layers Coverage Map in UL
  // Description: This measurement provides Distribution of Scheduled PUSCH PRBs based on MIMO Layers Coverage Map
  //              per UE and per PRB on the UL.
  // Note: PUSCHPRBsLayer.BINy where y represents the number of layers that can be assigned per PRB on PUSCH to the UE
  //       on the distance from base station within the following interval: (MIMOLayersUL(y-1), MIMOLayersULy>,
  //       where MIMOLayersULy represents measurement defined in the clause 5.1.1.30.8
  {"PUSCHPRBsLayer", NRCellDU, SI, REAL, "-", DIST_BIN_Y_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.31 RSRQ measurement
  // 5.1.1.31.1 SS-RSRQ distribution in gNB
  // Description:  This measurement provides the distribution of SS-RSRQ received by gNB from UEs in the cell.
  // Note: MR.NRScSSRSRQ.BinX, where X represents  the range of  Measured quantity  SS-RSRQ value (-43 to 20 dB)
  // Note: Number of bins and the range for each bin is left to implementation.
  {"MR.NRScSSRSRQ", NRCellCU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.31.2 SS-RSRQ distribution per SSB
  // Description: This measurement provides the distribution of SS-RSRQ per SSB received by the gNB of a serving cell
  //              from UEs in the measResults IEs in MeasurementReport messages that is triggered by the gNB sending
  //              the measConfig messages to request UEs to send the UE measurement reports.
  // Note: MR.SS-RSRQPerSSB.Bin, where Bin represents the range of reported SS-RSRQ value (0 .. 127)
  //       mapping to -43 dB to 20 dB with 0.5 dB resolution (See Table 10.1.11.1-1 in TS 38.133 [35]).
  // Note: Number of bins and the range for each bin is left to implementation.
  {"MR.SS-RSRQPerSSB", BEAM, CC, INTEGER, "-", SSB_IDX_LABEL | DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.31.3 SS-RSRQ distribution per SSB of neighbor NR cell
  // Description: This measurement provides the distribution of SS-RSRQ per SSB received by the gNB of a neighbour cell
  //              from UEs in the measResults IEs in MeasurementReport messages that is triggered by the gNB sending
  //              the measConfig messages to request UEs to send the UE measurement reports.
  // Note: MR.SS-RSRQ.SSBIndex.Bin, where SSBIndex identifies the SSB beam of the neighbor NR cell. Bin represents
  //       the range of reported SS-RSRQ value (0 .. 127) mapping to -43 dB to 20 dB with 0.5 dB resolution
  // Note: Number of bins and the range for each bin is left to implementation.
  {"MR.SS-RSRQ", NRCellCU | NRCellRelation, CC, INTEGER, "-", SSB_IDX_LABEL | DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.32 SINR measurement
  // 5.1.1.32.1 SS-SINR distribution in gNB
  // Description: This measurement provides the distribution of SS-SINR received by gNB from UEs in the cell.
  // Note: MR.NRScSSSINR.BinX, where X represents  the range of  Measured quantity  SS-SINR value (-23 to 40 dB)
  // Note: Number of bins and the range for each bin is left to implementation.
  {"MR.NRScSSSINR", NRCellCU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.32.2 SS-SINR distribution per SSB
  // Description: This measurement provides the distribution of SS-SINR per SSB received by the gNB of a serving cell
  //              from UEs in the measResults IEs in MeasurementReport messages that is triggered by the gNB sending
  //              the measConfig messages to request UEs to send the UE measurement reports.
  // Note: MR.SS-SINRPerSSB.Bin, where Bin represents the range of reported SS-SINR value (0 .. 127) mapping
  //       to -23 dB to 40 dB with 0.5 dB resolution (See Table 10.1.16.1-1 in TS 38.133 [35]).
  // Note: Number of bins and the range for each bin is left to implementation.
  {"MR.SS-SINRPerSSB", BEAM, CC, INTEGER, "-", SSB_IDX_LABEL | DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // 5.1.1.32.3 SS-SINR distribution per SSB of neighbor NR cell
  // Description: This measurement provides the distribution of SS-SINR per SSB received by the gNB of a neighbour cell
  //              from UEs in the measResults IEs in MeasurementReport messages that is triggered by the gNB sending
  //              the measConfig messages to request UEs to send the UE measurement reports.
  // Note: MR.SS-SINR.SSBIndex.Bin, where SSBIndex identifies the SSB beam of the neighbor NR cell. Bin represents
  //       the range of reported SS-RSRQ value (0 .. 127) mapping to -23 dB to 40 dB with 0.5 dB resolution.
  // Note: Number of bins and the range for each bin is left to implementation.
  {"MR.SS-SINR", NRCellCU | NRCellRelation, CC, INTEGER, "-", SSB_IDX_LABEL | DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.33 Timing Advance
  // 5.1.1.33.1 Timing Advance distribution for NR Cell
  // Description: This measurement provides the distribution of the Absolute Timing Advance (TA) values transmitted
  //              by the gNB to UEs in the cell.
  // Note: L1M.ATADist.Bin, where Bin represents the range of absolute TA value (0 to 4095).
  // Note: Number of bins and the range for each bin is left to implementation.
  {"L1M.ATADist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.34 Incoming GTP Data Packet Loss in gNB over N3
  // Description: This measurement provides the number of GTP data packets which are not successfully received
  //              at gNB over N3 after being sent by UPF.
  // Note: source is EP_NgU (contained by GNBCUUPFunction)
  {"GTP.InDataPktPacketLossN3gNB", GNBCUUP, CC, INTEGER, "-", NO_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.1.35 DL Packet Loss rate on Uu
  // Description: This measurement provides the DL Packet (i.e., RLC SDU) Loss rate on Uu interface for an NR cell.
  // Note: Measurement definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE and UE levels.
  {"DRB.PacketLossRateUu", NRCellDU, CC, INTEGER, "-", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.2 Performance measurements valid only for non-split gNB deployment scenario
  // 5.1.2.1 PDCP Data Volume
  // Note: Unit changed from Mbit to kbit in O-RAN.WG3.E2SM-KPM-R003-v3.00
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  // 5.1.2.1.1 DL PDCP SDU Data Volume Measurements
  // 5.1.2.1.1.1 DL Cell PDCP SDU Data Volume
  // Description: This measurement provides the Data Volume (PDCP SDU bits) in the downlink delivered to PDCP layer.
  {"DRB.PdcpSduVolumeDL", NRCellCU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.2.1.1.2 DL Cell PDCP SDU Data Volume on X2 Interface
  // Description: This measurement provides the Data Volume (amount of PDCP SDU bits) in the downlink
  //              delivered on X2 interface in DC-scenarios.
  {"DRB.PdcpSduVolumeX2DL", NRCellCU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.2.1.1.3 DL Cell PDCP SDU Data Volume on Xn Interface
  // Description: This measurement provides the Data Volume (PDCP SDU bits) in the downlink delivered on Xn interface.
  {"DRB.PdcpSduVolumeXnDL", NRCellCU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.2.1.2 UL PDCP SDU Data Volume Measurements
  // 5.1.2.1.2.1 UL Cell PDCP SDU Data Volume
  // Description: This measurement provides the Data Volume (amount of PDCP SDU bits) in the uplink delivered
  //              from PDCP layer to higher layers.
  {"DRB.PdcpSduVolumeUL", NRCellCU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.2.1.2.2 UL Cell PDCP SDU Data Volume on X2 Interface
  // Description: This measurement provides the Data Volume (amount of PDCP SDU bits) in the uplink delivered
  //              on X2 interface in NSA scenarios.
  {"DRB.PdcpSduVolumeX2UL", NRCellCU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.2.1.2.3 UL Cell PDCP SDU Data Volume on Xn Interface
  // Description: This measurement provides the Data Volume (amount of PDCP SDU bits) in the uplink delivered
  //              on Xn interface in SA scenarios.
  {"DRB.PdcpSduVolumeXnUL", NRCellCU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.2.2 Packet Success Rate
  // 5.1.2.2.1 UL PDCP SDU Success Rate
  // Description: This measurement provides the fraction of PDCP SDU packets which are successfully received at gNB.
  // Note: Measurement definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE and UE levels.
  {"DRB.PacketSuccessRateUlgNBUu", NRCellCU, SI, INTEGER, "-", NO_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.2.3 QoS flow release
  // 5.1.2.3.1 Mean interruption time interval for 5QI 1 QoS Flow released due to double NG (double UE context)
  // Description: This measurement provides the average interruption time interval for 5QI 1 QoS Flow released
  //              due to double NG (double UE context).
  // Note: no family prefix in TS
  {"MeanTime5QI1Flow.RelDoubleNG", NRCellCU, CC, INTEGER, "ms", NO_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.3 Performance measurements valid for split gNB deployment scenario
  // 5.1.3.1 Packet Loss Rate
  // Note: Measurement definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable E2_NODE and UE levels.
  // 5.1.3.1.1 UL PDCP SDU Loss Rate
  // Description: This measurement provides the fraction of PDCP SDU packets which are not successfully received
  //              at gNB-CU-UP.
  // Note: integer value representing the loss rate multiplied by 1e6
  {"DRB.PacketLossRateUl", NRCellCU | GNBCUUP, SI, INTEGER, "-", NO_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.3.1.2 UL F1-U Packet Loss Rate
  // Description: This measurement provides the fraction of PDCP SDU packets which are not successfully received
  //              at gNB-CU-UP.
  // Note: integer value representing the loss rate multiplied by 1e6
  {"DRB.F1UpacketLossRateUl", GNBCUUP, SI, INTEGER, "-", NO_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 5.1.3.1.3 DL F1-U Packet Loss Rate
  // Description: This measurement provides the fraction of PDCP SDU packets which are not successfully received
  //              at the gNB-DU).
  // Note: integer value representing the loss rate multiplied by 1e6
  {"DRB.F1UpacketLossRateDl", NRCellDU, SI, INTEGER, "-", NO_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.3.2 Packet Drop Rate
  // 5.1.3.2.1 DL PDCP SDU Drop rate in gNB-CU-UP
  // Description: This measurement provides the fraction of PDCP SDU packets which are dropped on the downlink,
  //              due to high traffic load, traffic management etc in the gNB-CU-UP.
  // Note: integer value representing the loss rate multiplied by 1e6
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  {"DRB.PdcpPacketDropRateDl", NRCellCU | GNBCUUP, SI, INTEGER, "-", NO_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.2.2 DL Packet Drop Rate in gNB-DU
  // Description: This measurement provides the fraction of RLC SDU packets which are dropped on the downlink,
  //              due to high traffic load, traffic management etc in the gNB-DU.
  // Note: integer value representing the loss rate multiplied by 1e6
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  {"DRB.RlcPacketDropRateDl", NRCellDU, SI, INTEGER, "-", NO_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.3.3 Packet delay
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  // 5.1.3.3.1 Average delay DL in CU-UP
  // Description: This measurement provides the average (arithmetic mean) PDCP SDU delay on the downlink within
  //              the gNB-CU-UP, for all PDCP packets.
  {"DRB.PdcpSduDelayDl", GNBCUUP, DER, REAL, "0.1ms", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.3.2 Average delay DL on F1-U
  // Description: This measurement provides the average (arithmetic mean) GTP packet delay DL on the F1-U interface.
  {"DRB.PdcpF1DelayDl", GNBCUUP, DER, REAL, "0.1ms", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.3.3 Average delay DL in gNB-DU
  // Description: This measurement provides the average (arithmetic mean) RLC SDU delay on the downlink within
  //              the gNB-DU, for initial transmission of all RLC packets.
  {"DRB.RlcSduDelayDl", NRCellDU, DER, REAL, "0.1ms", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.3.4 Distribution of delay DL in CU-UP
  // Description: This measurement provides the distribution of PDCP SDU delay on the downlink within the gNB-CU-UP,
  //              for all PDCP packets.
  // Note: DRB.PdcpSduDelayDlDist.Bin_Filter, where Bin indicates a delay range which is vendor specific
  {"DRB.PdcpSduDelayDlDist", GNBCUUP, DER, INTEGER, "-", DIST_BIN_X_LABEL | PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.3.5 Distribution of delay DL on F1-U
  // Description: This measurement provides the distribution of GTP packet delay DL on the F1-U interface.
  // Note: DRB.GtpF1DelayDlDist.Bin_Filter, where Bin indicates a delay range which is vendor specific
  {"DRB.GtpF1DelayDlDist", GNBCUUP, DER, INTEGER, "-", DIST_BIN_X_LABEL | PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.3.6 Distribution of delay DL in gNB-DU
  // Description: This measurement provides the distribution of RLC SDU delay on the downlink within the gNB-DU,
  //              for initial transmission of all RLC packets.
  // Note: DRB.RlcSduDelayDlDist.Bin_Filter, where Bin indicates a delay range which is vendor specific
  {"DRB.RlcSduDelayDlDist", NRCellDU, DER, INTEGER, "-", DIST_BIN_X_LABEL | PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.3.4 IP Latency measurements
  // 5.1.3.4.1 General information -> no metric defined
  // 5.1.3.4.2 Average IP Latency DL in gNB-DU
  // Description: This measurement provides the average IP Latency in DL (arithmetic mean) within the gNB-DU,
  //              when there is no other prior data to be transmitted to the same UE in the gNB-DU.
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  {"DRB.RlcSduLatencyDl", NRCellDU, DER, REAL, "0.1ms", NO_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.4.3 Distribution of IP Latency DL in gNB-DU
  // Description: This measurement provides the distribution of IP Latency in DL within the gNB-DU,
  //              when there is no other prior data to be transmitted to the same UE in the gNB-DU.
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  {"DRB.RlcSduLatencyDlDist", NRCellDU, DER, INTEGER, "-", DIST_BIN_X_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.3.5 UE Context Release
  // 5.1.3.5.1 UE Context Release Request (gNB-DU initiated)
  // Description: This measurement provides the number of UE CONTEXT Release initiated by gNB-DU for each release cause.
  // Note: The nof measurements is equal to the nof causes plus a possible sum value identified by the .sum suffix.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"UECNTX.RelReq", NRCellDU | BEAM, SI, INTEGER, "-", NO_LABEL | SUM_LABEL, E2_NODE_LEVEL},

  // 5.1.3.5.2 Number of UE Context Release Requests (gNB-CU initiated)
  // Description: This measurement provides the number of UE CONTEXT RELEASE initiated by gNB-CU for each release cause.
  // Note: The nof measurements is equal to the nof causes plus a possible sum value identified by the .sum suffix.
  // Note: mandatory Cause-label not supported in e2sm_kpm
  {"UECNTX.RelCmd", NRCellCU | BEAM, SI, INTEGER, "-", NO_LABEL | SUM_LABEL, E2_NODE_LEVEL},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.3.6 PDCP data volume measurements
  // Note: Definition changed in O-RAN.WG3.E2SM-KPM-R003-v3.00 (Sec. 7.9) to enable ALL_LEVELS (NODE, UE and QOS)
  // 5.1.3.6.1 PDCP PDU data volume Measurement
  // 5.1.3.6.1.1 DL PDCP PDU Data Volume
  // Description: This measurement provides the Data Volume (amount of PDCP PDU bits) in the downlink delivered
  //              from GNB-CU to GNB-DU.
  // Note: Unit changed from Mbit to kbit in O-RAN.WG3.E2SM-KPM-R003-v3.00
  {"QosFlow.PdcpPduVolumeDL", NRCellCU | GNBCUUP, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.6.1.2 UL PDCP PDU Data Volume
  // Description: This measurement provides the Data Volume (amount of PDCP PDU bits) in the uplink delievered
  //              from GNB-DU to GNB-CU.
  // Note: Unit changed from Mbit to kbit in O-RAN.WG3.E2SM-KPM-R003-v3.00
  {"QosFlow.PdcpPduVolumeUL", NRCellCU | GNBCUUP, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.6.2 PDCP SDU data volume Measurement
  // 5.1.3.6.2.1 DL PDCP SDU Data Volume
  // Description: This measurement provides the Data Volume (amount of PDCP SDU bits) in the downlink
  //              delivered to PDCP layer.
  // Note: Unit changed from Mbit to kbit in O-RAN.WG3.E2SM-KPM-R003-v3.00
  {"QosFlow.PdcpSduVolumeDl", NRCellCU | GNBCUUP, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.6.2.2 UL PDCP SDU Data Volume
  // Description: This measurement provides the Data Volume (amount of PDCP SDU bits) in the uplink delivered from
  //              PDCP layer to SDAP layer or UPF.
  // Note: Unit changed from Mbit to kbit in O-RAN.WG3.E2SM-KPM-R003-v3.00
  {"QosFlow.PdcpSduVolumeUl", NRCellCU | GNBCUUP, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 5.1.3.6.2.3 DL PDCP SDU Data Volume per interface
  // Description: This measurement provides the Data Volume (amount of PDCP SDU bits) in the downlink delivered
  //              from GNB-CU-UP to GNB-DU (F1-U interface), to external gNB-CU-UP (Xn-U interface)
  //              and to external eNB (X2-U interface).
  // Note: different filters (labels) set for each entity
  // Note: Unit changed from Mbit to kbit in O-RAN.WG3.E2SM-KPM-R003-v3.00
  // Note: X2uPdcpSduVolumeDl and XnuPdcpSduVolumeDl defined in O-RAN.WG3.E2SM-KPM-R003-v3.00 Section 7.9.1
  {"DRB.F1uPdcpSduVolumeDL", EP_F1U, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},
  {"DRB.X2uPdcpSduVolumeDl", EP_X2U, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL, ALL_LEVELS},
  {"DRB.XnuPdcpSduVolumeDl", EP_XnU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},


  // 5.1.3.6.2.4 UL PDCP SDU Data Volume per interface
  // Description: This measurement provides the Data Volume (amount of PDCP SDU bits) in the uplink delivered
  //              to GNB-CU-UP from GNB-DU (F1-U interface), from external gNB-CU-UP (Xn-U interface)
  //              and from external eNB (X2-U interface).
  // Note: different filters (labels) set for each entity
  // Note: Unit changed from Mbit to kbit in O-RAN.WG3.E2SM-KPM-R003-v3.00
  // Note: X2uPdcpSduVolumeUl and XnuPdcpSduVolumeDl defined in O-RAN.WG3.E2SM-KPM-R003-v3.00 Section 7.9.1
  {"DRB.F1uPdcpSduVolumeUL", EP_F1U, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},
  {"DRB.X2uPdcpSduVolumeUl", EP_X2U, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL, ALL_LEVELS},
  {"DRB.XnuPdcpSduVolumeUl", EP_XnU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.1.3.7 Handovers measurements
  // 5.1.3.7.1 Intra-gNB handovers
  // 5.1.3.7.1.1 Number of requested legacy handover preparations
  // Description: This measurement provides the number of outgoing intra-gNB legacy handover preparations
  //              requested by the source NRCellCU for split gNB deployment.
  {"MM.HoPrepIntraReq", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.3.7.1.2 Number of successful legacy handover preparations
  // Description: This measurement provides the number of successful intra-gNB legacy handover preparations
  //              received by the source NRCellCU, for split gNB deployment.
  {"MM.HoPrepIntraSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.3.7.1.3 Number of requested conditional handover preparations
  // Description: This measurement provides the number of outgoing intra-gNB conditional handover preparations
  //              requested by the source NRCellCU for a split gNB deployment.
  {"MM.ChoPrepIntraReq", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.3.7.1.4 Number of successful conditional handover preparations
  // Description: This measurement provides the number of successful intra-gNB conditional handover preparations
  //              received by the source NRCellCU, for a split gNB deployment.
  {"MM.ChoPrepIntraSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.3.7.1.5 Number of requested DAPS handover preparations
  // Description: This measurement provides the number of outgoing intra-gNB DAPS handover preparations requested
  //              by the source NRCellCU for a split gNB deployment.
  {"MM.DapsHoPrepIntraReq", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.3.7.1.6 Number of successful DAPS handover preparations
  // Description: This measurement provides the number of successful intra-gNB DAPS handover preparations received
  //              by the source NRCellCU, for a split gNB deployment.
  {"MM.DapsHoPrepIntraSucc", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.3.7.1.7 Number of UEs for which conditional handover preparations are requested
  // Description: This measurement provides the number of UEs for which outgoing intra-gNB conditional handover
  //              preparations are requested by the source NRCellCU for a split gNB deployment.
  {"MM.ChoPrepIntraReqUes", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.3.7.1.8 Number of UEs for which conditional handover preparations are successful
  // Description: This measurement provides the number of UEs for which intra-gNB conditional handover preparations
  //              received by the source NRCellCU are successful, for a split gNB deployment.
  {"MM.ChoPrepIntraSuccUes", NRCellCU, CC, INTEGER, "-", NO_LABEL, E2_NODE_LEVEL},

  // 5.1.3.8 Void -> no metric defined
  // 5.1.3.9 Void -> no metric defined

  // --------------------------------------------------------------------------------------------------------------- //
  // 5.7 Common performance measurements for NFs
  // 5.7.1 VR usage of NF
  // 5.7.1.1 Virtual CPU usage
  // 5.7.1.1.1 Mean virtual CPU usage
  // Description: This measurement provides the mean usage of the underlying virtualized CPUs for a virtualized
  //              3GPP NF.
  // Note: source can be any NF
  {"VR.VCpuUsageMean", ANY, OM, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL},

  // 5.7.1.2 Virtual memory usage
  // 5.7.1.2.1 Mean virtual memory usage
  // Description: This measurement provides the mean usage of the underlying virtualized memories for a virtualized
  //              3GPP NF.
  // Note: source can be any NF
  {"VR.VMemoryUsageMean", ANY, OM, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL},

  // 5.7.1.3 Virtual disk usage
  // 5.7.1.3.1 Mean virtual disk usage
  // Description: This measurement provides the mean usage of the underlying virtualized disks for a virtualized
  //              3GPP NF.
  // Note: source can be any NF
  {"VR.VDiskUsageMean", ANY, OM, INTEGER, "%", NO_LABEL, E2_NODE_LEVEL}}};
  // clang-format on
  return metrics;
};

// E2SM_KPM O-RAN specific Measurements defined in O-RAN.WG3.E2SM-KPM-R003-v3.00
inline span<const e2sm_kpm_metric_t> get_e2sm_kpm_oran_metrics()
{
  // clang-format off
  static const std::array<e2sm_kpm_metric_t, NOF_ORAN_E2SM_KPM_METRICS> metrics =  {{
  // 7.10 O-RAN specific Performance Measurement
  // 7.10.1 DL Transmitted Data Volume
  // Description: This measurement provides the transmitted data volume in the downlink in a measurement time.
  {"DRB.RlcSduTransmittedVolumeDL", NRCellDU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 7.10.2 UL Transmitted Data Volume
  // Description: This measurement provides the transmitted data volume in the uplink in a certain period.
  {"DRB.RlcSduTransmittedVolumeUL", NRCellDU, CC, INTEGER, "kbit", PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 7.10.3 Distribution of Percentage of DL Transmitted Data Volume to Incoming Data Volume
  // Description: This measurement provides the distribution of the percentage of successfully transmitted data
  //              volume to incoming data volume in downlink for UEs.
  // Note: DRB.PerDataVolumeDLDist.Bin where Bin represents the bin width
  {"DRB.PerDataVolumeDLDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL | PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 7.10.4 Distribution of Percentage of UL Transmitted Data Volume to Incoming Data Volume
  // Description: This measurement provides the distribution of the percentage of successfully transmitted data
  //              volume to incoming data volume in uplink for UEs.
  // Note: DRB.PerDataVolumeULDist.Bin where Bin represents the bin width
  {"DRB.PerDataVolumeULDist", NRCellDU, CC, INTEGER, "-", DIST_BIN_X_LABEL | PLMN_ID_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 7.10.5 Distribution of DL Packet Drop Rate
  // Description: This measurement provides the fraction of RLC SDU packets which are dropped on the downlink,
  //              due to high traffic load, traffic management etc in the gNB-DU.
  // Note: integer value representing the drop rate multiplied by 1e6 of each UE within the range of the bin
  // Note: drop rate multiplied by 1e6?
  {"DRB.RlcPacketDropRateDLDist", NRCellDU, SI, INTEGER, "-", DIST_BIN_X_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 7.10.6 Distribution of UL Packet Loss Rate
  // Description: This measurement provides the distribution of the fraction of PDCP SDU packets
  //              which are not successfully received at gNB-CU-UP.
  // Note: integer value representing the loss rate multiplied by 1E6 of each UE within the range of the bin
  // Note: loss rate multiplied by 1e6?
  {"DRB.PacketLossRateULDist", NRCellCU | GNBCUUP, SI, INTEGER, "-", DIST_BIN_X_LABEL | FIVE_QI_LABEL | SLICE_ID_LABEL, ALL_LEVELS},

  // 7.10.7 DL Synchronization Signal based Reference Signal Received Power (SS-RSRP)
  // Description: This measurement provides the average of the DL SS-RSRP (see TS 38.215 [16]) values reported
  //              from UEs in the cell when SS-RSRP is used for L1-RSRP as configured by reporting configurations
  //              as defined in TS 38.214 [18], in case the L1-RSRP report function is enabled.
  // Note: Separate counters are maintained for each SSB in the cell.
  {"L1M.DL-SS-RSRP", NRCellDU, DER, REAL, "-", SSB_IDX_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 7.10.8 DL Synchronization Signal based Signal to Noise and Interference Ratio (SS-SINR)
  // Description: This measurement provides the average of the DL SS-SINR (see TS 38.215 [16]) values reported
  //              from UEs in the cell when SS-SINR is used for L1-SINR as configured by reporting configurations
  //              as defined in TS 38.214 [18], in case the L1-SINR report function is enabled.
  // Note: Separate counters are maintained for each SSB in the cell.
  {"L1M.DL-SS-SINR", NRCellDU, DER, REAL, "-", SSB_IDX_LABEL, E2_NODE_LEVEL | UE_LEVEL},

  // 7.10.9 UL Sounding Reference Signal based Reference Signal Received Power (SRS-RSRP)
  // Description: This measurement provides the average of the UL SRS-RSRP (see TS 38.215 [16])
  //              values measured for UEs in the cell.
  {"L1M.UL-SRS-RSRP", NRCellDU, DER, REAL, "W", NO_LABEL, E2_NODE_LEVEL | UE_LEVEL}}};
  // clang-format on
  return metrics;
}

} // namespace srsran
