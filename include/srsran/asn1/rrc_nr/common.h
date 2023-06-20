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

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "srsran/asn1/asn1_utils.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_RRC_NR_MAX_ADD_RACH_R17 256
#define ASN1_RRC_NR_MAX_AI_DCI_PAYLOAD_SIZE_R16 128
#define ASN1_RRC_NR_MAX_AI_DCI_PAYLOAD_SIZE_1_R16 127
#define ASN1_RRC_NR_MAX_BAND_COMB 65536
#define ASN1_RRC_NR_MAX_BANDS_UTRA_FDD_R16 64
#define ASN1_RRC_NR_MAX_BH_RLC_CH_ID_R16 65536
#define ASN1_RRC_NR_MAX_BT_ID_REPORT_R16 32
#define ASN1_RRC_NR_MAX_BT_NAME_R16 4
#define ASN1_RRC_NR_MAX_CAG_CELL_R16 16
#define ASN1_RRC_NR_MAX_TWO_PUCCH_GRP_CFG_LIST_R16 32
#define ASN1_RRC_NR_MAX_TWO_PUCCH_GRP_CFG_LIST_R17 16
#define ASN1_RRC_NR_MAX_CBR_CFG_R16 8
#define ASN1_RRC_NR_MAX_CBR_CFG_1_R16 7
#define ASN1_RRC_NR_MAX_CBR_LEVEL_R16 16
#define ASN1_RRC_NR_MAX_CBR_LEVEL_1_R16 15
#define ASN1_RRC_NR_MAX_CELL_EXCLUDED 16
#define ASN1_RRC_NR_MAX_CELL_GROUPINGS_R16 32
#define ASN1_RRC_NR_MAX_CELL_HISTORY_R16 16
#define ASN1_RRC_NR_MAX_PSCELL_HISTORY_R17 16
#define ASN1_RRC_NR_MAX_CELL_INTER 16
#define ASN1_RRC_NR_MAX_CELL_INTRA 16
#define ASN1_RRC_NR_MAX_CELL_MEAS_EUTRA 32
#define ASN1_RRC_NR_MAX_CELL_MEAS_IDLE_R16 8
#define ASN1_RRC_NR_MAX_CELL_MEAS_UTRA_FDD_R16 32
#define ASN1_RRC_NR_MAX_CELL_NTN_R17 4
#define ASN1_RRC_NR_MAX_CARRIER_TYPE_PAIR_LIST_R16 16
#define ASN1_RRC_NR_MAX_CELL_ALLOWED 16
#define ASN1_RRC_NR_MAX_EARFCN 262143
#define ASN1_RRC_NR_MAX_EUTRA_CELL_EXCLUDED 16
#define ASN1_RRC_NR_MAX_EUTRA_NS_PMAX 8
#define ASN1_RRC_NR_MAX_FEATURE_COMB_PREAMBS_PER_RACH_RES_R17 256
#define ASN1_RRC_NR_MAX_LOG_MEAS_REPORT_R16 520
#define ASN1_RRC_NR_MAX_MULTI_BANDS 8
#define ASN1_RRC_NR_MAX_NARFCN 3279165
#define ASN1_RRC_NR_MAX_NR_NS_PMAX 8
#define ASN1_RRC_NR_MAX_FREQ_IDLE_R16 8
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS 32
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS_1 31
#define ASN1_RRC_NR_MAX_NROF_AGGR_CELLS_PER_CELL_GROUP 16
#define ASN1_RRC_NR_MAX_NROF_AGGR_CELLS_PER_CELL_GROUP_MINUS4_R16 12
#define ASN1_RRC_NR_MAX_NROF_DU_CELLS_R16 512
#define ASN1_RRC_NR_MAX_NROF_APP_LAYER_MEAS_R17 16
#define ASN1_RRC_NR_MAX_NROF_APP_LAYER_MEAS_1_R17 15
#define ASN1_RRC_NR_MAX_NROF_AVAILABILITY_COMBINATIONS_PER_SET_R16 512
#define ASN1_RRC_NR_MAX_NROF_AVAILABILITY_COMBINATIONS_PER_SET_1_R16 511
#define ASN1_RRC_NR_MAX_NROF_IAB_RES_CFG_R17 65536
#define ASN1_RRC_NR_MAX_NROF_IAB_RES_CFG_1_R17 65535
#define ASN1_RRC_NR_MAX_NROF_SCELL_ACT_RS_R17 255
#define ASN1_RRC_NR_MAX_NROF_SCELLS 31
#define ASN1_RRC_NR_MAX_NROF_CELL_MEAS 32
#define ASN1_RRC_NR_MAX_NROF_CRS_IM_INTERF_CELL_R17 8
#define ASN1_RRC_NR_MAX_NROF_RELAY_MEAS_R17 32
#define ASN1_RRC_NR_MAX_NROF_CG_SL_R16 8
#define ASN1_RRC_NR_MAX_NROF_CG_SL_1_R16 7
#define ASN1_RRC_NR_MAX_SL_GC_BC_DRX_QOS_R17 16
#define ASN1_RRC_NR_MAX_NROF_SL_RX_INFO_SET_R17 4
#define ASN1_RRC_NR_MAX_NROF_SS_BLOCKS_TO_AVERAGE 16
#define ASN1_RRC_NR_MAX_NROF_COND_CELLS_R16 8
#define ASN1_RRC_NR_MAX_NROF_COND_CELLS_1_R17 7
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_TO_AVERAGE 16
#define ASN1_RRC_NR_MAX_NROF_DL_ALLOCS 16
#define ASN1_RRC_NR_MAX_NROF_DL_ALLOCS_EXT_R17 64
#define ASN1_RRC_NR_MAX_NROF_PDU_SESSIONS_R17 256
#define ASN1_RRC_NR_MAX_NROF_SR_CFG_PER_CELL_GROUP 8
#define ASN1_RRC_NR_MAX_LCG_ID 7
#define ASN1_RRC_NR_MAX_LCG_ID_IAB_R17 255
#define ASN1_RRC_NR_MAX_LC_ID 32
#define ASN1_RRC_NR_MAX_LC_ID_IAB_R16 65855
#define ASN1_RRC_NR_MAX_LTE_CRS_PATTERNS_R16 3
#define ASN1_RRC_NR_MAX_NROF_TAGS 4
#define ASN1_RRC_NR_MAX_NROF_TAGS_1 3
#define ASN1_RRC_NR_MAX_NROF_BWPS 4
#define ASN1_RRC_NR_MAX_NROF_COMB_IDC 128
#define ASN1_RRC_NR_MAX_NROF_SYMBOLS_1 13
#define ASN1_RRC_NR_MAX_NROF_SLOTS 320
#define ASN1_RRC_NR_MAX_NROF_SLOTS_1 319
#define ASN1_RRC_NR_MAX_NROF_PHYS_RES_BLOCKS 275
#define ASN1_RRC_NR_MAX_NROF_PHYS_RES_BLOCKS_1 274
#define ASN1_RRC_NR_MAX_NROF_PHYS_RES_BLOCKS_PLUS1 276
#define ASN1_RRC_NR_MAX_NROF_CORESETS 12
#define ASN1_RRC_NR_MAX_NROF_CORESETS_1 11
#define ASN1_RRC_NR_MAX_NROF_CORESETS_1_R16 15
#define ASN1_RRC_NR_MAX_NROF_CORESET_POOLS_R16 2
#define ASN1_RRC_NR_MAX_CO_RE_SET_DUR 3
#define ASN1_RRC_NR_MAX_NROF_SEARCH_SPACES_1 39
#define ASN1_RRC_NR_MAX_NROF_SEARCH_SPACES_LINKS_1_R17 39
#define ASN1_RRC_NR_MAX_NROF_BFD_RES_PER_SET_R17 64
#define ASN1_RRC_NR_MAX_SFI_DCI_PAYLOAD_SIZE 128
#define ASN1_RRC_NR_MAX_SFI_DCI_PAYLOAD_SIZE_1 127
#define ASN1_RRC_NR_MAX_IAB_IP_ADDRESS_R16 32
#define ASN1_RRC_NR_MAX_INT_DCI_PAYLOAD_SIZE 126
#define ASN1_RRC_NR_MAX_INT_DCI_PAYLOAD_SIZE_1 125
#define ASN1_RRC_NR_MAX_NROF_RATE_MATCH_PATTERNS 4
#define ASN1_RRC_NR_MAX_NROF_RATE_MATCH_PATTERNS_1 3
#define ASN1_RRC_NR_MAX_NROF_RATE_MATCH_PATTERNS_PER_GROUP 8
#define ASN1_RRC_NR_MAX_NROF_CSI_REPORT_CFGS 48
#define ASN1_RRC_NR_MAX_NROF_CSI_REPORT_CFGS_1 47
#define ASN1_RRC_NR_MAX_NROF_CSI_RES_CFGS 112
#define ASN1_RRC_NR_MAX_NROF_CSI_RES_CFGS_1 111
#define ASN1_RRC_NR_MAX_NROF_AP_CSI_RS_RES_PER_SET 16
#define ASN1_RRC_NR_MAX_NR_OF_CSI_APERIODIC_TRIGGERS 128
#define ASN1_RRC_NR_MAX_NROF_REPORT_CFG_PER_APERIODIC_TRIGGER 16
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES 192
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_1 191
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_PER_SET 64
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_SETS 64
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_SETS_1 63
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_SETS_PER_CFG 16
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_PER_CFG 128
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES 32
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_1 31
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_SETS_1 15
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_PER_SET 16
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_SETS 16
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES 32
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_1 31
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_PER_SET 8
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_SETS 64
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_SETS_1 63
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_SETS_PER_CFG 16
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_PER_SET 64
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS 64
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS_1 63
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS_PER_CFG 1
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS_PER_CFG_EXT 2
#define ASN1_RRC_NR_MAX_NROF_FAIL_DETECTION_RES 10
#define ASN1_RRC_NR_MAX_NROF_FAIL_DETECTION_RES_1 9
#define ASN1_RRC_NR_MAX_NROF_FAIL_DETECTION_RES_1_R17 63
#define ASN1_RRC_NR_MAX_NROF_FREQ_SL_R16 8
#define ASN1_RRC_NR_MAX_NROF_SL_BWPS_R16 4
#define ASN1_RRC_NR_MAX_FREQ_SL_EUTRA_R16 8
#define ASN1_RRC_NR_MAX_NROF_SL_MEAS_ID_R16 64
#define ASN1_RRC_NR_MAX_NROF_SL_OBJ_ID_R16 64
#define ASN1_RRC_NR_MAX_NROF_SL_REPORT_CFG_ID_R16 64
#define ASN1_RRC_NR_MAX_NROF_SL_POOL_TO_MEASURE_NR_R16 8
#define ASN1_RRC_NR_MAX_FREQ_SL_NR_R16 8
#define ASN1_RRC_NR_MAX_NROF_SL_QFIS_R16 2048
#define ASN1_RRC_NR_MAX_NROF_SL_QFIS_PER_DEST_R16 64
#define ASN1_RRC_NR_MAX_NROF_OBJ_ID 64
#define ASN1_RRC_NR_MAX_NROF_PAGE_REC 32
#define ASN1_RRC_NR_MAX_NROF_PCI_RANGES 8
#define ASN1_RRC_NR_MAX_PLMN 12
#define ASN1_RRC_NR_MAX_TAC_R17 12
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_RRM 96
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_RRM_1 95
#define ASN1_RRC_NR_MAX_NROF_MEAS_ID 64
#define ASN1_RRC_NR_MAX_NROF_QUANT_CFG 2
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_CELLS_RRM 96
#define ASN1_RRC_NR_MAX_NROF_SL_DEST_R16 32
#define ASN1_RRC_NR_MAX_NROF_SL_DEST_1_R16 31
#define ASN1_RRC_NR_MAX_NROF_SLRB_R16 512
#define ASN1_RRC_NR_MAX_SL_LCID_R16 512
#define ASN1_RRC_NR_MAX_SL_SYNC_CFG_R16 16
#define ASN1_RRC_NR_MAX_NROF_RX_POOL_R16 16
#define ASN1_RRC_NR_MAX_NROF_TX_POOL_R16 8
#define ASN1_RRC_NR_MAX_NROF_POOL_ID_R16 16
#define ASN1_RRC_NR_MAX_NROF_SRS_PATHLOSS_REF_RS_R16 64
#define ASN1_RRC_NR_MAX_NROF_SRS_PATHLOSS_REF_RS_1_R16 63
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_SETS 16
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_SETS_1 15
#define ASN1_RRC_NR_MAX_NROF_SRS_POS_RES_SETS_R16 16
#define ASN1_RRC_NR_MAX_NROF_SRS_POS_RES_SETS_1_R16 15
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_FEATURES 64
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_1 63
#define ASN1_RRC_NR_MAX_NROF_SRS_POS_RES_R16 64
#define ASN1_RRC_NR_MAX_NROF_SRS_POS_RES_1_R16 63
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_PER_SET 16
#define ASN1_RRC_NR_MAX_NROF_SRS_TRIGGER_STATES_1 3
#define ASN1_RRC_NR_MAX_NROF_SRS_TRIGGER_STATES_2 2
#define ASN1_RRC_NR_MAX_RAT_CAP_CONTAINERS 8
#define ASN1_RRC_NR_MAX_SIMUL_BANDS 32
#define ASN1_RRC_NR_MAX_UL_TX_SWITCHING_BAND_PAIRS 32
#define ASN1_RRC_NR_MAX_NROF_SLOT_FORMAT_COMBINATIONS_PER_SET 512
#define ASN1_RRC_NR_MAX_NROF_SLOT_FORMAT_COMBINATIONS_PER_SET_1 511
#define ASN1_RRC_NR_MAX_NROF_TRAFFIC_PATTERN_R16 8
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES 128
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_1 127
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_SETS 4
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_SETS_1 3
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_PER_SET 32
#define ASN1_RRC_NR_MAX_NROF_PUCCH_P0_PER_SET 8
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS 4
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS_1 3
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS_R16 64
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS_1_R16 63
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS_1_R17 7
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS_DIFF_R16 60
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_GROUPS_R16 4
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_PER_GROUP_R16 128
#define ASN1_RRC_NR_MAX_NROF_PWR_CTRL_SET_INFOS_R17 8
#define ASN1_RRC_NR_MAX_NROF_MULTIPLE_PUSC_HS_R16 8
#define ASN1_RRC_NR_MAX_NROF_P0_PUSCH_ALPHA_SETS 30
#define ASN1_RRC_NR_MAX_NROF_P0_PUSCH_ALPHA_SETS_1 29
#define ASN1_RRC_NR_MAX_NROF_PUSCH_PATHLOSS_REF_RSS 4
#define ASN1_RRC_NR_MAX_NROF_PUSCH_PATHLOSS_REF_RSS_1 3
#define ASN1_RRC_NR_MAX_NROF_PUSCH_PATHLOSS_REF_RSS_R16 64
#define ASN1_RRC_NR_MAX_NROF_PUSCH_PATHLOSS_REF_RSS_1_R16 63
#define ASN1_RRC_NR_MAX_NROF_PUSCH_PATHLOSS_REF_RSS_DIFF_R16 60
#define ASN1_RRC_NR_MAX_NROF_PATHLOSS_REF_RSS_R17 64
#define ASN1_RRC_NR_MAX_NROF_PATHLOSS_REF_RSS_1_R17 63
#define ASN1_RRC_NR_MAX_NROF_NAICS_ENTRIES 8
#define ASN1_RRC_NR_MAX_BANDS 1024
#define ASN1_RRC_NR_MAX_BANDS_MRDC 1280
#define ASN1_RRC_NR_MAX_BANDS_EUTRA 256
#define ASN1_RRC_NR_MAX_CELL_REPORT 8
#define ASN1_RRC_NR_MAX_DRB 29
#define ASN1_RRC_NR_MAX_FREQ 8
#define ASN1_RRC_NR_MAX_FREQ_LAYERS 4
#define ASN1_RRC_NR_MAX_FREQ_PLUS1 9
#define ASN1_RRC_NR_MAX_FREQ_IDC_R16 128
#define ASN1_RRC_NR_MAX_COMB_IDC_R16 128
#define ASN1_RRC_NR_MAX_FREQ_IDC_MRDC 32
#define ASN1_RRC_NR_MAX_NROF_CANDIDATE_BEAMS 16
#define ASN1_RRC_NR_MAX_NROF_CANDIDATE_BEAMS_R16 64
#define ASN1_RRC_NR_MAX_NROF_CANDIDATE_BEAMS_EXT_R16 48
#define ASN1_RRC_NR_MAX_NROF_PCIS_PER_SMTC 64
#define ASN1_RRC_NR_MAX_NROF_QFIS 64
#define ASN1_RRC_NR_MAX_NROF_RES_AVAILABILITY_PER_COMBINATION_R16 256
#define ASN1_RRC_NR_MAX_NR_OF_SEMI_PERSISTENT_PUSCH_TRIGGERS 64
#define ASN1_RRC_NR_MAX_NROF_SR_RES 8
#define ASN1_RRC_NR_MAX_NROF_SLOT_FORMATS_PER_COMBINATION 256
#define ASN1_RRC_NR_MAX_NROF_SPATIAL_RELATION_INFOS 8
#define ASN1_RRC_NR_MAX_NROF_SPATIAL_RELATION_INFOS_PLUS_1 9
#define ASN1_RRC_NR_MAX_NROF_SPATIAL_RELATION_INFOS_R16 64
#define ASN1_RRC_NR_MAX_NROF_SPATIAL_RELATION_INFOS_DIFF_R16 56
#define ASN1_RRC_NR_MAX_NROF_IDXES_TO_REPORT 32
#define ASN1_RRC_NR_MAX_NROF_IDXES_TO_REPORT2 64
#define ASN1_RRC_NR_MAX_NROF_SSBS_R16 64
#define ASN1_RRC_NR_MAX_NROF_SSBS_1 63
#define ASN1_RRC_NR_MAX_NROF_S_NSSAI 8
#define ASN1_RRC_NR_MAX_NROF_TCI_STATES_PDCCH 64
#define ASN1_RRC_NR_MAX_NROF_TCI_STATES 128
#define ASN1_RRC_NR_MAX_NROF_TCI_STATES_1 127
#define ASN1_RRC_NR_MAX_UL_TCI_R17 64
#define ASN1_RRC_NR_MAX_UL_TCI_1_R17 63
#define ASN1_RRC_NR_MAX_NROF_ADD_PCI_R17 7
#define ASN1_RRC_NR_MAX_MPE_RES_R17 64
#define ASN1_RRC_NR_MAX_NROF_UL_ALLOCS 16
#define ASN1_RRC_NR_MAX_QFI 63
#define ASN1_RRC_NR_MAX_RA_CSI_RS_RES 96
#define ASN1_RRC_NR_MAX_RA_OCCASIONS_PER_CSI_RS 64
#define ASN1_RRC_NR_MAX_RA_OCCASIONS_1 511
#define ASN1_RRC_NR_MAX_RA_SSB_RES 64
#define ASN1_RRC_NR_MAX_SCSS 5
#define ASN1_RRC_NR_MAX_SECONDARY_CELL_GROUPS 3
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS_EUTRA 32
#define ASN1_RRC_NR_MAX_MBSFN_ALLOCS 8
#define ASN1_RRC_NR_MAX_NROF_MULTI_BANDS 8
#define ASN1_RRC_NR_MAX_CELL_SFTD 3
#define ASN1_RRC_NR_MAX_REPORT_CFG_ID 64
#define ASN1_RRC_NR_MAX_NROF_CODEBOOKS 16
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_EXT_R16 16
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_EXT_R17 8
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES 7
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_ALT_R16 512
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_ALT_1_R16 511
#define ASN1_RRC_NR_MAX_NROF_SRI_PUSCH_MAPS 16
#define ASN1_RRC_NR_MAX_NROF_SRI_PUSCH_MAPS_1 15
#define ASN1_RRC_NR_MAX_SIB 32
#define ASN1_RRC_NR_MAX_SI_MSG 32
#define ASN1_RRC_NR_MAX_SIB_MSG_PLUS1_R17 33
#define ASN1_RRC_NR_MAX_PO_PER_PF 4
#define ASN1_RRC_NR_MAX_PEI_PER_PF_R17 4
#define ASN1_RRC_NR_MAX_ACCESS_CAT_1 63
#define ASN1_RRC_NR_MAX_BARR_INFO_SET 8
#define ASN1_RRC_NR_MAX_CELL_EUTRA 8
#define ASN1_RRC_NR_MAX_EUTRA_CARRIER 8
#define ASN1_RRC_NR_MAX_PLMN_IDENTITIES 8
#define ASN1_RRC_NR_MAX_DL_FEATURE_SETS 1024
#define ASN1_RRC_NR_MAX_UL_FEATURE_SETS 1024
#define ASN1_RRC_NR_MAX_EUTRA_DL_FEATURE_SETS 256
#define ASN1_RRC_NR_MAX_EUTRA_UL_FEATURE_SETS 256
#define ASN1_RRC_NR_MAX_FEATURE_SETS_PER_BAND 128
#define ASN1_RRC_NR_MAX_PER_CC_FEATURE_SETS 1024
#define ASN1_RRC_NR_MAX_FEATURE_SET_COMBINATIONS 1024
#define ASN1_RRC_NR_MAX_INTER_RAT_RSTD_FREQ 3
#define ASN1_RRC_NR_MAX_GIN_R17 24
#define ASN1_RRC_NR_MAX_HRNN_LEN_R16 48
#define ASN1_RRC_NR_MAX_NPN_R16 12
#define ASN1_RRC_NR_MAX_NR_OF_MIN_SCHED_OFFSET_VALUES_R16 2
#define ASN1_RRC_NR_MAX_K0_SCHED_OFFSET_R16 16
#define ASN1_RRC_NR_MAX_K2_SCHED_OFFSET_R16 16
#define ASN1_RRC_NR_MAX_K0_SCHED_OFFSET_R17 64
#define ASN1_RRC_NR_MAX_K2_SCHED_OFFSET_R17 64
#define ASN1_RRC_NR_MAX_DCI_2_6_SIZE_R16 140
#define ASN1_RRC_NR_MAX_DCI_2_7_SIZE_R17 43
#define ASN1_RRC_NR_MAX_DCI_2_6_SIZE_1_R16 139
#define ASN1_RRC_NR_MAX_NROF_UL_ALLOCS_R16 64
#define ASN1_RRC_NR_MAX_NROF_P0_PUSCH_SET_R16 2
#define ASN1_RRC_NR_MAX_ON_DEMAND_SIB_R16 8
#define ASN1_RRC_NR_MAX_ON_DEMAND_POS_SIB_R16 32
#define ASN1_RRC_NR_MAX_CI_DCI_PAYLOAD_SIZE_R16 126
#define ASN1_RRC_NR_MAX_CI_DCI_PAYLOAD_SIZE_1_R16 125
#define ASN1_RRC_NR_MAX_UU_RELAY_RLC_CH_ID_R17 32
#define ASN1_RRC_NR_MAX_WLAN_ID_REPORT_R16 32
#define ASN1_RRC_NR_MAX_WLAN_NAME_R16 4
#define ASN1_RRC_NR_MAX_RA_REPORT_R16 8
#define ASN1_RRC_NR_MAX_TX_CFG_R16 64
#define ASN1_RRC_NR_MAX_TX_CFG_1_R16 63
#define ASN1_RRC_NR_MAX_PSSCH_TX_CFG_R16 16
#define ASN1_RRC_NR_MAX_NROF_CLI_RSSI_RES_R16 64
#define ASN1_RRC_NR_MAX_NROF_CLI_RSSI_RES_1_R16 63
#define ASN1_RRC_NR_MAX_NROF_CLI_SRS_RES_R16 32
#define ASN1_RRC_NR_MAX_CLI_REPORT_R16 8
#define ASN1_RRC_NR_MAX_NROF_CC_GROUP_R17 16
#define ASN1_RRC_NR_MAX_NROF_CFG_GRANT_CFG_R16 12
#define ASN1_RRC_NR_MAX_NROF_CFG_GRANT_CFG_1_R16 11
#define ASN1_RRC_NR_MAX_NROF_CG_TYPE2_DEACTIVATION_STATE 16
#define ASN1_RRC_NR_MAX_NROF_CFG_GRANT_CFG_MAC_1_R16 31
#define ASN1_RRC_NR_MAX_NROF_SPS_CFG_R16 8
#define ASN1_RRC_NR_MAX_NROF_SPS_CFG_1_R16 7
#define ASN1_RRC_NR_MAX_NROF_SPS_DEACTIVATION_STATE 16
#define ASN1_RRC_NR_MAX_NROF_PPW_CFG_R17 4
#define ASN1_RRC_NR_MAX_NROF_PPW_ID_1_R17 15
#define ASN1_RRC_NR_MAX_NR_OF_TX_TEG_REPORT_R17 256
#define ASN1_RRC_NR_MAX_NR_OF_TX_TEG_ID_1_R17 7
#define ASN1_RRC_NR_MAX_NROF_PAGING_SUBGROUPS_R17 8
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_GROUPS_1_R16 3
#define ASN1_RRC_NR_MAX_NROF_REQ_COM_DC_LOCATION_R17 128
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS_TCI_R16 32
#define ASN1_RRC_NR_MAX_NROF_TX_DC_TWO_CARRIER_R16 64
#define ASN1_RRC_NR_MAX_NROF_RB_SET_GROUPS_R17 8
#define ASN1_RRC_NR_MAX_NROF_RB_SETS_R17 8
#define ASN1_RRC_NR_MAX_NROF_ENH_TYPE3_HARQ_ACK_R17 8
#define ASN1_RRC_NR_MAX_NROF_ENH_TYPE3_HARQ_ACK_1_R17 7
#define ASN1_RRC_NR_MAX_NROF_PRS_RES_PER_SET_R17 64
#define ASN1_RRC_NR_MAX_NROF_PRS_RES_PER_SET_1_R17 63
#define ASN1_RRC_NR_MAX_NROF_PRS_RES_OFFSET_VALUE_1_R17 511
#define ASN1_RRC_NR_MAX_NROF_GAP_ID_R17 8
#define ASN1_RRC_NR_MAX_NROF_PRE_CFG_POS_GAP_ID_R17 16
#define ASN1_RRC_NR_MAX_NR_OF_GAP_PRI_R17 16
#define ASN1_RRC_NR_MAX_CEF_REPORT_R17 4
#define ASN1_RRC_NR_MAX_NROF_MULTIPLE_PDSC_HS_R17 8
#define ASN1_RRC_NR_MAX_SLICE_INFO_R17 8
#define ASN1_RRC_NR_MAX_CELL_SLICE_R17 16
#define ASN1_RRC_NR_MAX_NROF_TRS_RES_SETS_R17 64
#define ASN1_RRC_NR_MAX_NROF_SEARCH_SPACE_GROUPS_1_R17 2
#define ASN1_RRC_NR_MAX_NROF_REMOTE_UE_R17 32
#define ASN1_RRC_NR_MAX_DCI_4_2_SIZE_R17 140
#define ASN1_RRC_NR_MAX_FREQ_MBS_R17 16
#define ASN1_RRC_NR_MAX_NROF_DRX_CFG_PTM_R17 64
#define ASN1_RRC_NR_MAX_NROF_DRX_CFG_PTM_1_R17 63
#define ASN1_RRC_NR_MAX_NROF_MBS_SERVICE_LIST_PER_UE_R17 16
#define ASN1_RRC_NR_MAX_NROF_MBS_SESSION_R17 1024
#define ASN1_RRC_NR_MAX_NROF_MTCH_SSB_MAP_WIN_R17 16
#define ASN1_RRC_NR_MAX_NROF_MTCH_SSB_MAP_WIN_1_R17 15
#define ASN1_RRC_NR_MAX_NROF_MRB_BROADCAST_R17 4
#define ASN1_RRC_NR_MAX_NROF_PAGE_GROUP_R17 32
#define ASN1_RRC_NR_MAX_NROF_PDSCH_CFG_PTM_R17 16
#define ASN1_RRC_NR_MAX_NROF_PDSCH_CFG_PTM_1_R17 15
#define ASN1_RRC_NR_MAX_G_RNTI_R17 16
#define ASN1_RRC_NR_MAX_G_RNTI_1_R17 15
#define ASN1_RRC_NR_MAX_G_CS_RNTI_R17 8
#define ASN1_RRC_NR_MAX_G_CS_RNTI_1_R17 7
#define ASN1_RRC_NR_MAX_MRB_R17 32
#define ASN1_RRC_NR_MAX_FSAI_MBS_R17 64
#define ASN1_RRC_NR_MAX_NEIGH_CELL_MBS_R17 8
#define ASN1_RRC_NR_MAX_NROF_PDCCH_BLIND_DETECTION_MIXED_1_R16 7
#define ASN1_RRC_NR_MAX_NROF_PDCCH_BLIND_DETECTION_R17 16
#define ASN1_RRC_NR_MAX_MEAS_FREQS_MN 32
#define ASN1_RRC_NR_MAX_MEAS_FREQS_SN 32
#define ASN1_RRC_NR_MAX_MEAS_IDENTITIES_MN 62
#define ASN1_RRC_NR_MAX_CELL_PREP 32

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// SubcarrierSpacing ::= ENUMERATED
struct subcarrier_spacing_opts {
  enum options { khz15, khz30, khz60, khz120, khz240, khz480_v1700, khz960_v1700, spare1, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using subcarrier_spacing_e = enumerated<subcarrier_spacing_opts>;

// MCC ::= SEQUENCE (SIZE (3)) OF INTEGER (0..9)
using mcc_l = std::array<uint8_t, 3>;

// MNC ::= SEQUENCE (SIZE (2..3)) OF INTEGER (0..9)
using mnc_l = bounded_array<uint8_t, 3>;

// PLMN-Identity ::= SEQUENCE
struct plmn_id_s {
  bool  mcc_present = false;
  mcc_l mcc;
  mnc_l mnc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-PeriodicityAndOffset ::= CHOICE
struct srs_periodicity_and_offset_c {
  struct types_opts {
    enum options {
      sl1,
      sl2,
      sl4,
      sl5,
      sl8,
      sl10,
      sl16,
      sl20,
      sl32,
      sl40,
      sl64,
      sl80,
      sl160,
      sl320,
      sl640,
      sl1280,
      sl2560,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  srs_periodicity_and_offset_c() = default;
  srs_periodicity_and_offset_c(const srs_periodicity_and_offset_c& other);
  srs_periodicity_and_offset_c& operator=(const srs_periodicity_and_offset_c& other);
  ~srs_periodicity_and_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& sl2()
  {
    assert_choice_type(types::sl2, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl4()
  {
    assert_choice_type(types::sl4, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl5()
  {
    assert_choice_type(types::sl5, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl8()
  {
    assert_choice_type(types::sl8, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl10()
  {
    assert_choice_type(types::sl10, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl16()
  {
    assert_choice_type(types::sl16, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl20()
  {
    assert_choice_type(types::sl20, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl32()
  {
    assert_choice_type(types::sl32, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl40()
  {
    assert_choice_type(types::sl40, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl64()
  {
    assert_choice_type(types::sl64, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl80()
  {
    assert_choice_type(types::sl80, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint8_t& sl160()
  {
    assert_choice_type(types::sl160, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  uint16_t& sl320()
  {
    assert_choice_type(types::sl320, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint16_t& sl640()
  {
    assert_choice_type(types::sl640, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint16_t& sl1280()
  {
    assert_choice_type(types::sl1280, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  uint16_t& sl2560()
  {
    assert_choice_type(types::sl2560, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint8_t& sl2() const
  {
    assert_choice_type(types::sl2, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl4() const
  {
    assert_choice_type(types::sl4, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl5() const
  {
    assert_choice_type(types::sl5, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl8() const
  {
    assert_choice_type(types::sl8, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl10() const
  {
    assert_choice_type(types::sl10, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl16() const
  {
    assert_choice_type(types::sl16, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl20() const
  {
    assert_choice_type(types::sl20, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl32() const
  {
    assert_choice_type(types::sl32, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl40() const
  {
    assert_choice_type(types::sl40, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl64() const
  {
    assert_choice_type(types::sl64, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl80() const
  {
    assert_choice_type(types::sl80, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint8_t& sl160() const
  {
    assert_choice_type(types::sl160, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint8_t>();
  }
  const uint16_t& sl320() const
  {
    assert_choice_type(types::sl320, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint16_t& sl640() const
  {
    assert_choice_type(types::sl640, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint16_t& sl1280() const
  {
    assert_choice_type(types::sl1280, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  const uint16_t& sl2560() const
  {
    assert_choice_type(types::sl2560, type_, "SRS-PeriodicityAndOffset");
    return c.get<uint16_t>();
  }
  void      set_sl1();
  uint8_t&  set_sl2();
  uint8_t&  set_sl4();
  uint8_t&  set_sl5();
  uint8_t&  set_sl8();
  uint8_t&  set_sl10();
  uint8_t&  set_sl16();
  uint8_t&  set_sl20();
  uint8_t&  set_sl32();
  uint8_t&  set_sl40();
  uint8_t&  set_sl64();
  uint8_t&  set_sl80();
  uint8_t&  set_sl160();
  uint16_t& set_sl320();
  uint16_t& set_sl640();
  uint16_t& set_sl1280();
  uint16_t& set_sl2560();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// SRS-SpatialRelationInfo ::= SEQUENCE
struct srs_spatial_relation_info_s {
  struct ref_sig_c_ {
    struct srs_s_ {
      uint8_t res_id = 0;
      uint8_t ul_bwp = 0;
    };
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, srs, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    srs_s_& srs()
    {
      assert_choice_type(types::srs, type_, "referenceSignal");
      return c.get<srs_s_>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const srs_s_& srs() const
    {
      assert_choice_type(types::srs, type_, "referenceSignal");
      return c.get<srs_s_>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();
    srs_s_&  set_srs();

  private:
    types                   type_;
    choice_buffer_t<srs_s_> c;

    void destroy_();
  };

  // member variables
  bool       serving_cell_id_present = false;
  uint8_t    serving_cell_id         = 0;
  ref_sig_c_ ref_sig;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellAndBWP-Id-r17 ::= SEQUENCE
struct serving_cell_and_bwp_id_r17_s {
  uint8_t servingcell_r17 = 0;
  uint8_t bwp_r17         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpatialRelationInfo-PDC-r17 ::= SEQUENCE
struct spatial_relation_info_pdc_r17_s {
  struct ref_sig_c_ {
    struct srs_s_ {
      uint8_t res_id = 0;
      uint8_t ul_bwp = 0;
    };
    struct types_opts {
      enum options { ssb_idx, csi_rs_idx, dl_prs_pdc, srs, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    ref_sig_c_() = default;
    ref_sig_c_(const ref_sig_c_& other);
    ref_sig_c_& operator=(const ref_sig_c_& other);
    ~ref_sig_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_idx()
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rs_idx()
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    uint8_t& dl_prs_pdc()
    {
      assert_choice_type(types::dl_prs_pdc, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    srs_s_& srs()
    {
      assert_choice_type(types::srs, type_, "referenceSignal");
      return c.get<srs_s_>();
    }
    const uint8_t& ssb_idx() const
    {
      assert_choice_type(types::ssb_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rs_idx() const
    {
      assert_choice_type(types::csi_rs_idx, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const uint8_t& dl_prs_pdc() const
    {
      assert_choice_type(types::dl_prs_pdc, type_, "referenceSignal");
      return c.get<uint8_t>();
    }
    const srs_s_& srs() const
    {
      assert_choice_type(types::srs, type_, "referenceSignal");
      return c.get<srs_s_>();
    }
    uint8_t& set_ssb_idx();
    uint8_t& set_csi_rs_idx();
    uint8_t& set_dl_prs_pdc();
    srs_s_&  set_srs();

  private:
    types                   type_;
    choice_buffer_t<srs_s_> c;

    void destroy_();
  };

  // member variables
  bool       ext = false;
  ref_sig_c_ ref_sig;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-Resource ::= SEQUENCE
struct srs_res_s {
  struct nrof_srs_ports_opts {
    enum options { port1, ports2, ports4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_srs_ports_e_ = enumerated<nrof_srs_ports_opts>;
  struct ptrs_port_idx_opts {
    enum options { n0, n1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ptrs_port_idx_e_ = enumerated<ptrs_port_idx_opts>;
  struct tx_comb_c_ {
    struct n2_s_ {
      uint8_t comb_offset_n2  = 0;
      uint8_t cyclic_shift_n2 = 0;
    };
    struct n4_s_ {
      uint8_t comb_offset_n4  = 0;
      uint8_t cyclic_shift_n4 = 0;
    };
    struct types_opts {
      enum options { n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    tx_comb_c_() = default;
    tx_comb_c_(const tx_comb_c_& other);
    tx_comb_c_& operator=(const tx_comb_c_& other);
    ~tx_comb_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    n2_s_& n2()
    {
      assert_choice_type(types::n2, type_, "transmissionComb");
      return c.get<n2_s_>();
    }
    n4_s_& n4()
    {
      assert_choice_type(types::n4, type_, "transmissionComb");
      return c.get<n4_s_>();
    }
    const n2_s_& n2() const
    {
      assert_choice_type(types::n2, type_, "transmissionComb");
      return c.get<n2_s_>();
    }
    const n4_s_& n4() const
    {
      assert_choice_type(types::n4, type_, "transmissionComb");
      return c.get<n4_s_>();
    }
    n2_s_& set_n2();
    n4_s_& set_n4();

  private:
    types                         type_;
    choice_buffer_t<n2_s_, n4_s_> c;

    void destroy_();
  };
  struct res_map_s_ {
    struct nrof_symbols_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using nrof_symbols_e_ = enumerated<nrof_symbols_opts>;
    struct repeat_factor_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using repeat_factor_e_ = enumerated<repeat_factor_opts>;

    // member variables
    uint8_t          start_position = 0;
    nrof_symbols_e_  nrof_symbols;
    repeat_factor_e_ repeat_factor;
  };
  struct freq_hop_s_ {
    uint8_t c_srs = 0;
    uint8_t b_srs = 0;
    uint8_t b_hop = 0;
  };
  struct group_or_seq_hop_opts {
    enum options { neither, group_hop, seq_hop, nulltype } value;

    const char* to_string() const;
  };
  using group_or_seq_hop_e_ = enumerated<group_or_seq_hop_opts>;
  struct res_type_c_ {
    struct aperiodic_s_ {
      bool ext = false;
      // ...
    };
    struct semi_persistent_s_ {
      bool                         ext = false;
      srs_periodicity_and_offset_c periodicity_and_offset_sp;
      // ...
    };
    struct periodic_s_ {
      bool                         ext = false;
      srs_periodicity_and_offset_c periodicity_and_offset_p;
      // ...
    };
    struct types_opts {
      enum options { aperiodic, semi_persistent, periodic, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    res_type_c_() = default;
    res_type_c_(const res_type_c_& other);
    res_type_c_& operator=(const res_type_c_& other);
    ~res_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    aperiodic_s_& aperiodic()
    {
      assert_choice_type(types::aperiodic, type_, "resourceType");
      return c.get<aperiodic_s_>();
    }
    semi_persistent_s_& semi_persistent()
    {
      assert_choice_type(types::semi_persistent, type_, "resourceType");
      return c.get<semi_persistent_s_>();
    }
    periodic_s_& periodic()
    {
      assert_choice_type(types::periodic, type_, "resourceType");
      return c.get<periodic_s_>();
    }
    const aperiodic_s_& aperiodic() const
    {
      assert_choice_type(types::aperiodic, type_, "resourceType");
      return c.get<aperiodic_s_>();
    }
    const semi_persistent_s_& semi_persistent() const
    {
      assert_choice_type(types::semi_persistent, type_, "resourceType");
      return c.get<semi_persistent_s_>();
    }
    const periodic_s_& periodic() const
    {
      assert_choice_type(types::periodic, type_, "resourceType");
      return c.get<periodic_s_>();
    }
    aperiodic_s_&       set_aperiodic();
    semi_persistent_s_& set_semi_persistent();
    periodic_s_&        set_periodic();

  private:
    types                                                          type_;
    choice_buffer_t<aperiodic_s_, periodic_s_, semi_persistent_s_> c;

    void destroy_();
  };
  struct res_map_r16_s_ {
    struct nrof_symbols_r16_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using nrof_symbols_r16_e_ = enumerated<nrof_symbols_r16_opts>;
    struct repeat_factor_r16_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using repeat_factor_r16_e_ = enumerated<repeat_factor_r16_opts>;

    // member variables
    uint8_t              start_position_r16 = 0;
    nrof_symbols_r16_e_  nrof_symbols_r16;
    repeat_factor_r16_e_ repeat_factor_r16;
  };
  struct res_map_r17_s_ {
    struct nrof_symbols_r17_opts {
      enum options { n1, n2, n4, n8, n10, n12, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using nrof_symbols_r17_e_ = enumerated<nrof_symbols_r17_opts>;
    struct repeat_factor_r17_opts {
      enum options { n1, n2, n4, n5, n6, n7, n8, n10, n12, n14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using repeat_factor_r17_e_ = enumerated<repeat_factor_r17_opts>;

    // member variables
    uint8_t              start_position_r17 = 0;
    nrof_symbols_r17_e_  nrof_symbols_r17;
    repeat_factor_r17_e_ repeat_factor_r17;
  };
  struct partial_freq_sr17_s_ {
    struct start_rb_idx_f_scaling_r17_c_ {
      struct types_opts {
        enum options {
          start_rb_idx_and_freq_scaling_factor2_r17,
          start_rb_idx_and_freq_scaling_factor4_r17,
          nulltype
        } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      start_rb_idx_f_scaling_r17_c_() = default;
      start_rb_idx_f_scaling_r17_c_(const start_rb_idx_f_scaling_r17_c_& other);
      start_rb_idx_f_scaling_r17_c_& operator=(const start_rb_idx_f_scaling_r17_c_& other);
      ~start_rb_idx_f_scaling_r17_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint8_t& start_rb_idx_and_freq_scaling_factor2_r17()
      {
        assert_choice_type(types::start_rb_idx_and_freq_scaling_factor2_r17, type_, "startRBIndexFScaling-r17");
        return c.get<uint8_t>();
      }
      uint8_t& start_rb_idx_and_freq_scaling_factor4_r17()
      {
        assert_choice_type(types::start_rb_idx_and_freq_scaling_factor4_r17, type_, "startRBIndexFScaling-r17");
        return c.get<uint8_t>();
      }
      const uint8_t& start_rb_idx_and_freq_scaling_factor2_r17() const
      {
        assert_choice_type(types::start_rb_idx_and_freq_scaling_factor2_r17, type_, "startRBIndexFScaling-r17");
        return c.get<uint8_t>();
      }
      const uint8_t& start_rb_idx_and_freq_scaling_factor4_r17() const
      {
        assert_choice_type(types::start_rb_idx_and_freq_scaling_factor4_r17, type_, "startRBIndexFScaling-r17");
        return c.get<uint8_t>();
      }
      uint8_t& set_start_rb_idx_and_freq_scaling_factor2_r17();
      uint8_t& set_start_rb_idx_and_freq_scaling_factor4_r17();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };

    // member variables
    bool                          enable_start_rb_hop_r17_present = false;
    start_rb_idx_f_scaling_r17_c_ start_rb_idx_f_scaling_r17;
  };
  struct tx_comb_n8_r17_s_ {
    uint8_t comb_offset_n8_r17  = 0;
    uint8_t cyclic_shift_n8_r17 = 0;
  };
  struct srs_tci_state_r17_c_ {
    struct types_opts {
      enum options { srs_ul_tci_state, srs_dl_or_joint_tci_state, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    srs_tci_state_r17_c_() = default;
    srs_tci_state_r17_c_(const srs_tci_state_r17_c_& other);
    srs_tci_state_r17_c_& operator=(const srs_tci_state_r17_c_& other);
    ~srs_tci_state_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& srs_ul_tci_state()
    {
      assert_choice_type(types::srs_ul_tci_state, type_, "srs-TCI-State-r17");
      return c.get<uint8_t>();
    }
    uint8_t& srs_dl_or_joint_tci_state()
    {
      assert_choice_type(types::srs_dl_or_joint_tci_state, type_, "srs-TCI-State-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& srs_ul_tci_state() const
    {
      assert_choice_type(types::srs_ul_tci_state, type_, "srs-TCI-State-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& srs_dl_or_joint_tci_state() const
    {
      assert_choice_type(types::srs_dl_or_joint_tci_state, type_, "srs-TCI-State-r17");
      return c.get<uint8_t>();
    }
    uint8_t& set_srs_ul_tci_state();
    uint8_t& set_srs_dl_or_joint_tci_state();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct srs_dl_or_joint_tci_state_v1730_s_ {
    serving_cell_and_bwp_id_r17_s cell_and_bwp_r17;
  };

  // member variables
  bool                        ext                           = false;
  bool                        ptrs_port_idx_present         = false;
  bool                        spatial_relation_info_present = false;
  uint8_t                     srs_res_id                    = 0;
  nrof_srs_ports_e_           nrof_srs_ports;
  ptrs_port_idx_e_            ptrs_port_idx;
  tx_comb_c_                  tx_comb;
  res_map_s_                  res_map;
  uint8_t                     freq_domain_position = 0;
  uint16_t                    freq_domain_shift    = 0;
  freq_hop_s_                 freq_hop;
  group_or_seq_hop_e_         group_or_seq_hop;
  res_type_c_                 res_type;
  uint16_t                    seq_id = 0;
  srs_spatial_relation_info_s spatial_relation_info;
  // ...
  // group 0
  copy_ptr<res_map_r16_s_> res_map_r16;
  // group 1
  copy_ptr<setup_release_c<spatial_relation_info_pdc_r17_s>> spatial_relation_info_pdc_r17;
  copy_ptr<res_map_r17_s_>                                   res_map_r17;
  copy_ptr<partial_freq_sr17_s_>                             partial_freq_sr17;
  copy_ptr<tx_comb_n8_r17_s_>                                tx_comb_n8_r17;
  copy_ptr<srs_tci_state_r17_c_>                             srs_tci_state_r17;
  // group 2
  bool                                         repeat_factor_v1730_present = false;
  copy_ptr<srs_dl_or_joint_tci_state_v1730_s_> srs_dl_or_joint_tci_state_v1730;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-Configuration-r16 ::= SEQUENCE
struct ssb_cfg_r16_s {
  struct half_frame_idx_r16_opts {
    enum options { zero, one, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using half_frame_idx_r16_e_ = enumerated<half_frame_idx_r16_opts>;
  struct ssb_periodicity_r16_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ssb_periodicity_r16_e_ = enumerated<ssb_periodicity_r16_opts>;
  struct sfn0_offset_r16_s_ {
    bool     integer_sf_offset_r16_present = false;
    uint16_t sfn_offset_r16                = 0;
    uint8_t  integer_sf_offset_r16         = 0;
  };

  // member variables
  bool                   ssb_periodicity_r16_present   = false;
  bool                   sfn0_offset_r16_present       = false;
  bool                   ss_pbch_block_pwr_r16_present = false;
  uint32_t               ssb_freq_r16                  = 0;
  half_frame_idx_r16_e_  half_frame_idx_r16;
  subcarrier_spacing_e   ssb_subcarrier_spacing_r16;
  ssb_periodicity_r16_e_ ssb_periodicity_r16;
  sfn0_offset_r16_s_     sfn0_offset_r16;
  uint8_t                sfn_ssb_offset_r16    = 0;
  int8_t                 ss_pbch_block_pwr_r16 = -60;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-PRS-Info-r16 ::= SEQUENCE
struct dl_prs_info_r16_s {
  bool     dl_prs_res_id_r16_present = false;
  uint16_t dl_prs_id_r16             = 0;
  uint8_t  dl_prs_res_set_id_r16     = 0;
  uint8_t  dl_prs_res_id_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-InfoNcell-r16 ::= SEQUENCE
struct ssb_info_ncell_r16_s {
  bool          ssb_idx_ncell_r16_present = false;
  bool          ssb_cfg_r16_present       = false;
  uint16_t      phys_cell_id_r16          = 0;
  uint8_t       ssb_idx_ncell_r16         = 0;
  ssb_cfg_r16_s ssb_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-PeriodicityAndOffset-r16 ::= CHOICE
struct srs_periodicity_and_offset_r16_c {
  struct types_opts {
    enum options {
      sl1,
      sl2,
      sl4,
      sl5,
      sl8,
      sl10,
      sl16,
      sl20,
      sl32,
      sl40,
      sl64,
      sl80,
      sl160,
      sl320,
      sl640,
      sl1280,
      sl2560,
      sl5120,
      sl10240,
      sl40960,
      sl81920,
      // ...
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  srs_periodicity_and_offset_r16_c() = default;
  srs_periodicity_and_offset_r16_c(const srs_periodicity_and_offset_r16_c& other);
  srs_periodicity_and_offset_r16_c& operator=(const srs_periodicity_and_offset_r16_c& other);
  ~srs_periodicity_and_offset_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& sl2()
  {
    assert_choice_type(types::sl2, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl4()
  {
    assert_choice_type(types::sl4, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl5()
  {
    assert_choice_type(types::sl5, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl8()
  {
    assert_choice_type(types::sl8, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl10()
  {
    assert_choice_type(types::sl10, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl16()
  {
    assert_choice_type(types::sl16, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl20()
  {
    assert_choice_type(types::sl20, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl32()
  {
    assert_choice_type(types::sl32, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl40()
  {
    assert_choice_type(types::sl40, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl64()
  {
    assert_choice_type(types::sl64, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl80()
  {
    assert_choice_type(types::sl80, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl160()
  {
    assert_choice_type(types::sl160, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint16_t& sl320()
  {
    assert_choice_type(types::sl320, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& sl640()
  {
    assert_choice_type(types::sl640, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& sl1280()
  {
    assert_choice_type(types::sl1280, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& sl2560()
  {
    assert_choice_type(types::sl2560, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& sl5120()
  {
    assert_choice_type(types::sl5120, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& sl10240()
  {
    assert_choice_type(types::sl10240, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& sl40960()
  {
    assert_choice_type(types::sl40960, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint32_t& sl81920()
  {
    assert_choice_type(types::sl81920, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint32_t>();
  }
  const uint8_t& sl2() const
  {
    assert_choice_type(types::sl2, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl4() const
  {
    assert_choice_type(types::sl4, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl5() const
  {
    assert_choice_type(types::sl5, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl8() const
  {
    assert_choice_type(types::sl8, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl10() const
  {
    assert_choice_type(types::sl10, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl16() const
  {
    assert_choice_type(types::sl16, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl20() const
  {
    assert_choice_type(types::sl20, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl32() const
  {
    assert_choice_type(types::sl32, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl40() const
  {
    assert_choice_type(types::sl40, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl64() const
  {
    assert_choice_type(types::sl64, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl80() const
  {
    assert_choice_type(types::sl80, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl160() const
  {
    assert_choice_type(types::sl160, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint16_t& sl320() const
  {
    assert_choice_type(types::sl320, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& sl640() const
  {
    assert_choice_type(types::sl640, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& sl1280() const
  {
    assert_choice_type(types::sl1280, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& sl2560() const
  {
    assert_choice_type(types::sl2560, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& sl5120() const
  {
    assert_choice_type(types::sl5120, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& sl10240() const
  {
    assert_choice_type(types::sl10240, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& sl40960() const
  {
    assert_choice_type(types::sl40960, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint32_t& sl81920() const
  {
    assert_choice_type(types::sl81920, type_, "SRS-PeriodicityAndOffset-r16");
    return c.get<uint32_t>();
  }
  void      set_sl1();
  uint8_t&  set_sl2();
  uint8_t&  set_sl4();
  uint8_t&  set_sl5();
  uint8_t&  set_sl8();
  uint8_t&  set_sl10();
  uint8_t&  set_sl16();
  uint8_t&  set_sl20();
  uint8_t&  set_sl32();
  uint8_t&  set_sl40();
  uint8_t&  set_sl64();
  uint8_t&  set_sl80();
  uint8_t&  set_sl160();
  uint16_t& set_sl320();
  uint16_t& set_sl640();
  uint16_t& set_sl1280();
  uint16_t& set_sl2560();
  uint16_t& set_sl5120();
  uint16_t& set_sl10240();
  uint16_t& set_sl40960();
  uint32_t& set_sl81920();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// SRS-PeriodicityAndOffsetExt-r16 ::= CHOICE
struct srs_periodicity_and_offset_ext_r16_c {
  struct types_opts {
    enum options { sl128, sl256, sl512, sl20480, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  srs_periodicity_and_offset_ext_r16_c() = default;
  srs_periodicity_and_offset_ext_r16_c(const srs_periodicity_and_offset_ext_r16_c& other);
  srs_periodicity_and_offset_ext_r16_c& operator=(const srs_periodicity_and_offset_ext_r16_c& other);
  ~srs_periodicity_and_offset_ext_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& sl128()
  {
    assert_choice_type(types::sl128, type_, "SRS-PeriodicityAndOffsetExt-r16");
    return c.get<uint8_t>();
  }
  uint16_t& sl256()
  {
    assert_choice_type(types::sl256, type_, "SRS-PeriodicityAndOffsetExt-r16");
    return c.get<uint16_t>();
  }
  uint16_t& sl512()
  {
    assert_choice_type(types::sl512, type_, "SRS-PeriodicityAndOffsetExt-r16");
    return c.get<uint16_t>();
  }
  uint16_t& sl20480()
  {
    assert_choice_type(types::sl20480, type_, "SRS-PeriodicityAndOffsetExt-r16");
    return c.get<uint16_t>();
  }
  const uint8_t& sl128() const
  {
    assert_choice_type(types::sl128, type_, "SRS-PeriodicityAndOffsetExt-r16");
    return c.get<uint8_t>();
  }
  const uint16_t& sl256() const
  {
    assert_choice_type(types::sl256, type_, "SRS-PeriodicityAndOffsetExt-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& sl512() const
  {
    assert_choice_type(types::sl512, type_, "SRS-PeriodicityAndOffsetExt-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& sl20480() const
  {
    assert_choice_type(types::sl20480, type_, "SRS-PeriodicityAndOffsetExt-r16");
    return c.get<uint16_t>();
  }
  uint8_t&  set_sl128();
  uint16_t& set_sl256();
  uint16_t& set_sl512();
  uint16_t& set_sl20480();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// SRS-SpatialRelationInfoPos-r16 ::= CHOICE
struct srs_spatial_relation_info_pos_r16_c {
  struct serving_rs_r16_s_ {
    struct ref_sig_r16_c_ {
      struct srs_spatial_relation_r16_s_ {
        struct res_sel_r16_c_ {
          struct types_opts {
            enum options { srs_res_id_r16, srs_pos_res_id_r16, nulltype } value;

            const char* to_string() const;
          };
          using types = enumerated<types_opts>;

          // choice methods
          res_sel_r16_c_() = default;
          res_sel_r16_c_(const res_sel_r16_c_& other);
          res_sel_r16_c_& operator=(const res_sel_r16_c_& other);
          ~res_sel_r16_c_() { destroy_(); }
          void        set(types::options e = types::nulltype);
          types       type() const { return type_; }
          SRSASN_CODE pack(bit_ref& bref) const;
          SRSASN_CODE unpack(cbit_ref& bref);
          void        to_json(json_writer& j) const;
          // getters
          uint8_t& srs_res_id_r16()
          {
            assert_choice_type(types::srs_res_id_r16, type_, "resourceSelection-r16");
            return c.get<uint8_t>();
          }
          uint8_t& srs_pos_res_id_r16()
          {
            assert_choice_type(types::srs_pos_res_id_r16, type_, "resourceSelection-r16");
            return c.get<uint8_t>();
          }
          const uint8_t& srs_res_id_r16() const
          {
            assert_choice_type(types::srs_res_id_r16, type_, "resourceSelection-r16");
            return c.get<uint8_t>();
          }
          const uint8_t& srs_pos_res_id_r16() const
          {
            assert_choice_type(types::srs_pos_res_id_r16, type_, "resourceSelection-r16");
            return c.get<uint8_t>();
          }
          uint8_t& set_srs_res_id_r16();
          uint8_t& set_srs_pos_res_id_r16();

        private:
          types               type_;
          pod_choice_buffer_t c;

          void destroy_();
        };

        // member variables
        res_sel_r16_c_ res_sel_r16;
        uint8_t        ul_bwp_r16 = 0;
      };
      struct types_opts {
        enum options { ssb_idx_serving_r16, csi_rs_idx_serving_r16, srs_spatial_relation_r16, nulltype } value;

        const char* to_string() const;
      };
      using types = enumerated<types_opts>;

      // choice methods
      ref_sig_r16_c_() = default;
      ref_sig_r16_c_(const ref_sig_r16_c_& other);
      ref_sig_r16_c_& operator=(const ref_sig_r16_c_& other);
      ~ref_sig_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint8_t& ssb_idx_serving_r16()
      {
        assert_choice_type(types::ssb_idx_serving_r16, type_, "referenceSignal-r16");
        return c.get<uint8_t>();
      }
      uint8_t& csi_rs_idx_serving_r16()
      {
        assert_choice_type(types::csi_rs_idx_serving_r16, type_, "referenceSignal-r16");
        return c.get<uint8_t>();
      }
      srs_spatial_relation_r16_s_& srs_spatial_relation_r16()
      {
        assert_choice_type(types::srs_spatial_relation_r16, type_, "referenceSignal-r16");
        return c.get<srs_spatial_relation_r16_s_>();
      }
      const uint8_t& ssb_idx_serving_r16() const
      {
        assert_choice_type(types::ssb_idx_serving_r16, type_, "referenceSignal-r16");
        return c.get<uint8_t>();
      }
      const uint8_t& csi_rs_idx_serving_r16() const
      {
        assert_choice_type(types::csi_rs_idx_serving_r16, type_, "referenceSignal-r16");
        return c.get<uint8_t>();
      }
      const srs_spatial_relation_r16_s_& srs_spatial_relation_r16() const
      {
        assert_choice_type(types::srs_spatial_relation_r16, type_, "referenceSignal-r16");
        return c.get<srs_spatial_relation_r16_s_>();
      }
      uint8_t&                     set_ssb_idx_serving_r16();
      uint8_t&                     set_csi_rs_idx_serving_r16();
      srs_spatial_relation_r16_s_& set_srs_spatial_relation_r16();

    private:
      types                                        type_;
      choice_buffer_t<srs_spatial_relation_r16_s_> c;

      void destroy_();
    };

    // member variables
    bool           serving_cell_id_present = false;
    uint8_t        serving_cell_id         = 0;
    ref_sig_r16_c_ ref_sig_r16;
  };
  struct types_opts {
    enum options { serving_rs_r16, ssb_ncell_r16, dl_prs_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  srs_spatial_relation_info_pos_r16_c() = default;
  srs_spatial_relation_info_pos_r16_c(const srs_spatial_relation_info_pos_r16_c& other);
  srs_spatial_relation_info_pos_r16_c& operator=(const srs_spatial_relation_info_pos_r16_c& other);
  ~srs_spatial_relation_info_pos_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  serving_rs_r16_s_& serving_rs_r16()
  {
    assert_choice_type(types::serving_rs_r16, type_, "SRS-SpatialRelationInfoPos-r16");
    return c.get<serving_rs_r16_s_>();
  }
  ssb_info_ncell_r16_s& ssb_ncell_r16()
  {
    assert_choice_type(types::ssb_ncell_r16, type_, "SRS-SpatialRelationInfoPos-r16");
    return c.get<ssb_info_ncell_r16_s>();
  }
  dl_prs_info_r16_s& dl_prs_r16()
  {
    assert_choice_type(types::dl_prs_r16, type_, "SRS-SpatialRelationInfoPos-r16");
    return c.get<dl_prs_info_r16_s>();
  }
  const serving_rs_r16_s_& serving_rs_r16() const
  {
    assert_choice_type(types::serving_rs_r16, type_, "SRS-SpatialRelationInfoPos-r16");
    return c.get<serving_rs_r16_s_>();
  }
  const ssb_info_ncell_r16_s& ssb_ncell_r16() const
  {
    assert_choice_type(types::ssb_ncell_r16, type_, "SRS-SpatialRelationInfoPos-r16");
    return c.get<ssb_info_ncell_r16_s>();
  }
  const dl_prs_info_r16_s& dl_prs_r16() const
  {
    assert_choice_type(types::dl_prs_r16, type_, "SRS-SpatialRelationInfoPos-r16");
    return c.get<dl_prs_info_r16_s>();
  }
  serving_rs_r16_s_&    set_serving_rs_r16();
  ssb_info_ncell_r16_s& set_ssb_ncell_r16();
  dl_prs_info_r16_s&    set_dl_prs_r16();

private:
  types                                                                       type_;
  choice_buffer_t<dl_prs_info_r16_s, serving_rs_r16_s_, ssb_info_ncell_r16_s> c;

  void destroy_();
};

// SRS-PosResource-r16 ::= SEQUENCE
struct srs_pos_res_r16_s {
  struct tx_comb_r16_c_ {
    struct n2_r16_s_ {
      uint8_t comb_offset_n2_r16  = 0;
      uint8_t cyclic_shift_n2_r16 = 0;
    };
    struct n4_r16_s_ {
      uint8_t comb_offset_n4_r16  = 0;
      uint8_t cyclic_shift_n4_r16 = 0;
    };
    struct n8_r16_s_ {
      uint8_t comb_offset_n8_r16  = 0;
      uint8_t cyclic_shift_n8_r16 = 0;
    };
    struct types_opts {
      enum options { n2_r16, n4_r16, n8_r16, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    tx_comb_r16_c_() = default;
    tx_comb_r16_c_(const tx_comb_r16_c_& other);
    tx_comb_r16_c_& operator=(const tx_comb_r16_c_& other);
    ~tx_comb_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    n2_r16_s_& n2_r16()
    {
      assert_choice_type(types::n2_r16, type_, "transmissionComb-r16");
      return c.get<n2_r16_s_>();
    }
    n4_r16_s_& n4_r16()
    {
      assert_choice_type(types::n4_r16, type_, "transmissionComb-r16");
      return c.get<n4_r16_s_>();
    }
    n8_r16_s_& n8_r16()
    {
      assert_choice_type(types::n8_r16, type_, "transmissionComb-r16");
      return c.get<n8_r16_s_>();
    }
    const n2_r16_s_& n2_r16() const
    {
      assert_choice_type(types::n2_r16, type_, "transmissionComb-r16");
      return c.get<n2_r16_s_>();
    }
    const n4_r16_s_& n4_r16() const
    {
      assert_choice_type(types::n4_r16, type_, "transmissionComb-r16");
      return c.get<n4_r16_s_>();
    }
    const n8_r16_s_& n8_r16() const
    {
      assert_choice_type(types::n8_r16, type_, "transmissionComb-r16");
      return c.get<n8_r16_s_>();
    }
    n2_r16_s_& set_n2_r16();
    n4_r16_s_& set_n4_r16();
    n8_r16_s_& set_n8_r16();

  private:
    types                                            type_;
    choice_buffer_t<n2_r16_s_, n4_r16_s_, n8_r16_s_> c;

    void destroy_();
  };
  struct res_map_r16_s_ {
    struct nrof_symbols_r16_opts {
      enum options { n1, n2, n4, n8, n12, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using nrof_symbols_r16_e_ = enumerated<nrof_symbols_r16_opts>;

    // member variables
    uint8_t             start_position_r16 = 0;
    nrof_symbols_r16_e_ nrof_symbols_r16;
  };
  struct freq_hop_r16_s_ {
    bool    ext       = false;
    uint8_t c_srs_r16 = 0;
    // ...
  };
  struct group_or_seq_hop_r16_opts {
    enum options { neither, group_hop, seq_hop, nulltype } value;

    const char* to_string() const;
  };
  using group_or_seq_hop_r16_e_ = enumerated<group_or_seq_hop_r16_opts>;
  struct res_type_r16_c_ {
    struct aperiodic_r16_s_ {
      bool    ext                     = false;
      bool    slot_offset_r16_present = false;
      uint8_t slot_offset_r16         = 1;
      // ...
    };
    struct semi_persistent_r16_s_ {
      bool                             ext = false;
      srs_periodicity_and_offset_r16_c periodicity_and_offset_sp_r16;
      // ...
      // group 0
      copy_ptr<srs_periodicity_and_offset_ext_r16_c> periodicity_and_offset_sp_ext_r16;

      // sequence methods
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
    };
    struct periodic_r16_s_ {
      bool                             ext = false;
      srs_periodicity_and_offset_r16_c periodicity_and_offset_p_r16;
      // ...
      // group 0
      copy_ptr<srs_periodicity_and_offset_ext_r16_c> periodicity_and_offset_p_ext_r16;

      // sequence methods
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
    };
    struct types_opts {
      enum options { aperiodic_r16, semi_persistent_r16, periodic_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    res_type_r16_c_() = default;
    res_type_r16_c_(const res_type_r16_c_& other);
    res_type_r16_c_& operator=(const res_type_r16_c_& other);
    ~res_type_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    aperiodic_r16_s_& aperiodic_r16()
    {
      assert_choice_type(types::aperiodic_r16, type_, "resourceType-r16");
      return c.get<aperiodic_r16_s_>();
    }
    semi_persistent_r16_s_& semi_persistent_r16()
    {
      assert_choice_type(types::semi_persistent_r16, type_, "resourceType-r16");
      return c.get<semi_persistent_r16_s_>();
    }
    periodic_r16_s_& periodic_r16()
    {
      assert_choice_type(types::periodic_r16, type_, "resourceType-r16");
      return c.get<periodic_r16_s_>();
    }
    const aperiodic_r16_s_& aperiodic_r16() const
    {
      assert_choice_type(types::aperiodic_r16, type_, "resourceType-r16");
      return c.get<aperiodic_r16_s_>();
    }
    const semi_persistent_r16_s_& semi_persistent_r16() const
    {
      assert_choice_type(types::semi_persistent_r16, type_, "resourceType-r16");
      return c.get<semi_persistent_r16_s_>();
    }
    const periodic_r16_s_& periodic_r16() const
    {
      assert_choice_type(types::periodic_r16, type_, "resourceType-r16");
      return c.get<periodic_r16_s_>();
    }
    aperiodic_r16_s_&       set_aperiodic_r16();
    semi_persistent_r16_s_& set_semi_persistent_r16();
    periodic_r16_s_&        set_periodic_r16();

  private:
    types                                                                      type_;
    choice_buffer_t<aperiodic_r16_s_, periodic_r16_s_, semi_persistent_r16_s_> c;

    void destroy_();
  };

  // member variables
  bool                                ext                                   = false;
  bool                                spatial_relation_info_pos_r16_present = false;
  uint8_t                             srs_pos_res_id_r16                    = 0;
  tx_comb_r16_c_                      tx_comb_r16;
  res_map_r16_s_                      res_map_r16;
  uint16_t                            freq_domain_shift_r16 = 0;
  freq_hop_r16_s_                     freq_hop_r16;
  group_or_seq_hop_r16_e_             group_or_seq_hop_r16;
  res_type_r16_c_                     res_type_r16;
  uint32_t                            seq_id_r16 = 0;
  srs_spatial_relation_info_pos_r16_c spatial_relation_info_pos_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-PosResourceAP-r16 ::= SEQUENCE
struct srs_pos_res_ap_r16_s {
  struct max_num_ap_srs_pos_res_per_bwp_r16_opts {
    enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_ap_srs_pos_res_per_bwp_r16_e_ = enumerated<max_num_ap_srs_pos_res_per_bwp_r16_opts>;
  struct max_num_ap_srs_pos_res_per_bwp_per_slot_r16_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, n12, n14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_ap_srs_pos_res_per_bwp_per_slot_r16_e_ = enumerated<max_num_ap_srs_pos_res_per_bwp_per_slot_r16_opts>;

  // member variables
  max_num_ap_srs_pos_res_per_bwp_r16_e_          max_num_ap_srs_pos_res_per_bwp_r16;
  max_num_ap_srs_pos_res_per_bwp_per_slot_r16_e_ max_num_ap_srs_pos_res_per_bwp_per_slot_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-PosResourceSP-r16 ::= SEQUENCE
struct srs_pos_res_sp_r16_s {
  struct max_num_sp_srs_pos_res_per_bwp_r16_opts {
    enum options { n1, n2, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_sp_srs_pos_res_per_bwp_r16_e_ = enumerated<max_num_sp_srs_pos_res_per_bwp_r16_opts>;
  struct max_num_sp_srs_pos_res_per_bwp_per_slot_r16_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, n12, n14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_num_sp_srs_pos_res_per_bwp_per_slot_r16_e_ = enumerated<max_num_sp_srs_pos_res_per_bwp_per_slot_r16_opts>;

  // member variables
  max_num_sp_srs_pos_res_per_bwp_r16_e_          max_num_sp_srs_pos_res_per_bwp_r16;
  max_num_sp_srs_pos_res_per_bwp_per_slot_r16_e_ max_num_sp_srs_pos_res_per_bwp_per_slot_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-AllPosResources-r16 ::= SEQUENCE
struct srs_all_pos_res_r16_s {
  bool                 srs_pos_res_ap_r16_present = false;
  bool                 srs_pos_res_sp_r16_present = false;
  srs_pos_res_r16_s    srs_pos_res_r16;
  srs_pos_res_ap_r16_s srs_pos_res_ap_r16;
  srs_pos_res_sp_r16_s srs_pos_res_sp_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
