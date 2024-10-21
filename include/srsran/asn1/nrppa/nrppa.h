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

/*******************************************************************************
 *
 *                     3GPP TS ASN1 NRPPA v17.8.0 (2024-08)
 *
 ******************************************************************************/

#pragma once

#include "srsran/asn1/asn1_ap_utils.h"

namespace asn1 {
namespace nrppa {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_NRPPA_ID_ERROR_IND 0
#define ASN1_NRPPA_ID_PRIVATE_MSG 1
#define ASN1_NRPPA_ID_E_C_ID_MEAS_INITIATION 2
#define ASN1_NRPPA_ID_E_C_ID_MEAS_FAIL_IND 3
#define ASN1_NRPPA_ID_E_C_ID_MEAS_REPORT 4
#define ASN1_NRPPA_ID_E_C_ID_MEAS_TERMINATION 5
#define ASN1_NRPPA_ID_O_TDOA_INFO_EXCHANGE 6
#define ASN1_NRPPA_ID_ASSIST_INFO_CTRL 7
#define ASN1_NRPPA_ID_ASSIST_INFO_FEEDBACK 8
#define ASN1_NRPPA_ID_POSITIONING_INFO_EXCHANGE 9
#define ASN1_NRPPA_ID_POSITIONING_INFO_UPD 10
#define ASN1_NRPPA_ID_MEAS 11
#define ASN1_NRPPA_ID_MEAS_REPORT 12
#define ASN1_NRPPA_ID_MEAS_UPD 13
#define ASN1_NRPPA_ID_MEAS_ABORT 14
#define ASN1_NRPPA_ID_MEAS_FAIL_IND 15
#define ASN1_NRPPA_ID_T_RP_INFO_EXCHANGE 16
#define ASN1_NRPPA_ID_POSITIONING_ACTIVATION 17
#define ASN1_NRPPA_ID_POSITIONING_DEACTIVATION 18
#define ASN1_NRPPA_ID_P_RS_CFG_EXCHANGE 19
#define ASN1_NRPPA_ID_MEAS_PRECFG 20
#define ASN1_NRPPA_ID_MEAS_ACTIVATION 21
#define ASN1_NRPPA_MAX_NR_OF_ERRORS 256
#define ASN1_NRPPA_MAX_CELLIN_RA_NNODE 3840
#define ASN1_NRPPA_MAX_IDXES_REPORT 64
#define ASN1_NRPPA_MAX_NO_MEAS 64
#define ASN1_NRPPA_MAX_CELL_REPORT 9
#define ASN1_NRPPA_MAX_CELL_REPORT_NR 9
#define ASN1_NRPPA_MAXNO_OTDO_ATYPES 63
#define ASN1_NRPPA_MAX_SERV_CELL 5
#define ASN1_NRPPA_MAX_EUTRA_MEAS 8
#define ASN1_NRPPA_MAX_GERAN_MEAS 8
#define ASN1_NRPPA_MAX_NR_MEAS 8
#define ASN1_NRPPA_MAX_UTRAN_MEAS 8
#define ASN1_NRPPA_MAX_WLA_NCHS 16
#define ASN1_NRPPA_MAXNO_FREQ_HOP_BANDS_MINUS_ONE 7
#define ASN1_NRPPA_MAX_NO_PATH 2
#define ASN1_NRPPA_MAX_NR_OF_POS_SI_MSG 32
#define ASN1_NRPPA_MAXNO_ASSIST_INFO_FAIL_LIST_ITEMS 32
#define ASN1_NRPPA_MAX_NR_OF_SEGMENTS 64
#define ASN1_NRPPA_MAX_NR_OF_POS_SIBS 32
#define ASN1_NRPPA_MAX_NO_OF_MEAS_TR_PS 64
#define ASN1_NRPPA_MAXNO_TR_PS 65535
#define ASN1_NRPPA_MAXNO_TRP_INFO_TYPES 64
#define ASN1_NRPPA_MAXNOOF_ANGLE_INFO 65535
#define ASN1_NRPPA_MAXNOLCS_GCS_TRANSLATION 3
#define ASN1_NRPPA_MAXNO_BCAST_CELL 16384
#define ASN1_NRPPA_MAXNO_SRS_TRIGGER_STATES 3
#define ASN1_NRPPA_MAXNO_SPATIAL_RELATIONS 64
#define ASN1_NRPPA_MAXNO_POS_MEAS 16384
#define ASN1_NRPPA_MAXNO_SRS_CARRIERS 32
#define ASN1_NRPPA_MAXNO_SC_SS 5
#define ASN1_NRPPA_MAXNO_SRS_RES_FEATURES 64
#define ASN1_NRPPA_MAXNO_SRS_POS_RES 64
#define ASN1_NRPPA_MAXNO_SRS_RES_SETS 16
#define ASN1_NRPPA_MAXNO_SRS_RES_PER_SET 16
#define ASN1_NRPPA_MAXNO_SRS_POS_RES_SETS 16
#define ASN1_NRPPA_MAXNO_SRS_POS_RES_PER_SET 16
#define ASN1_NRPPA_MAX_PRS_RES_SETS 2
#define ASN1_NRPPA_MAX_PRS_RES_PER_SET 64
#define ASN1_NRPPA_MAX_NO_SS_BS 255
#define ASN1_NRPPA_MAXNOOF_PR_SRES_SET 8
#define ASN1_NRPPA_MAXNOOF_PR_SRES 64
#define ASN1_NRPPA_MAXNOOF_UL_AO_AS 8
#define ASN1_NRPPA_MAX_NO_PATH_EXTENDED 8
#define ASN1_NRPPA_MAXNO_AR_PS 16
#define ASN1_NRPPA_MAXNO_UE_TE_GS 256
#define ASN1_NRPPA_MAXNO_TRPTE_GS 8
#define ASN1_NRPPA_MAX_FREQ_LAYERS 4
#define ASN1_NRPPA_MAX_NUM_RES_PER_ANGLE 24
#define ASN1_NRPPA_MAXNO_AZIMUTH_ANGLES 3600
#define ASN1_NRPPA_MAXNO_ELEVATION_ANGLES 1801
#define ASN1_NRPPA_MAXNO_PRSTR_PS 256
#define ASN1_NRPPA_ID_CAUSE 0
#define ASN1_NRPPA_ID_CRIT_DIAGNOSTICS 1
#define ASN1_NRPPA_ID_LMF_UE_MEAS_ID 2
#define ASN1_NRPPA_ID_REPORT_CHARACTERISTICS 3
#define ASN1_NRPPA_ID_MEAS_PERIODICITY 4
#define ASN1_NRPPA_ID_MEAS_QUANTITIES 5
#define ASN1_NRPPA_ID_RAN_UE_MEAS_ID 6
#define ASN1_NRPPA_ID_E_C_ID_MEAS_RESULT 7
#define ASN1_NRPPA_ID_OTDOA_CELLS 8
#define ASN1_NRPPA_ID_OTDOA_INFO_TYPE_GROUP 9
#define ASN1_NRPPA_ID_OTDOA_INFO_TYPE_ITEM 10
#define ASN1_NRPPA_ID_MEAS_QUANTITIES_ITEM 11
#define ASN1_NRPPA_ID_REQUESTED_SRS_TX_CHARACTERISTICS 12
#define ASN1_NRPPA_ID_CELL_PORTION_ID 14
#define ASN1_NRPPA_ID_OTHER_RAT_MEAS_QUANTITIES 15
#define ASN1_NRPPA_ID_OTHER_RAT_MEAS_QUANTITIES_ITEM 16
#define ASN1_NRPPA_ID_OTHER_RAT_MEAS_RESULT 17
#define ASN1_NRPPA_ID_WLAN_MEAS_QUANTITIES 19
#define ASN1_NRPPA_ID_WLAN_MEAS_QUANTITIES_ITEM 20
#define ASN1_NRPPA_ID_WLAN_MEAS_RESULT 21
#define ASN1_NRPPA_ID_TDD_CFG_EUTRA_ITEM 22
#define ASN1_NRPPA_ID_ASSIST_INFO 23
#define ASN1_NRPPA_ID_BROADCAST 24
#define ASN1_NRPPA_ID_ASSIST_INFO_FAIL_LIST 25
#define ASN1_NRPPA_ID_SRSCFG 26
#define ASN1_NRPPA_ID_MEAS_RESULT 27
#define ASN1_NRPPA_ID_TRP_ID 28
#define ASN1_NRPPA_ID_TRP_INFO_TYPE_LIST_TRP_REQ 29
#define ASN1_NRPPA_ID_TRP_INFO_LIST_TRP_RESP 30
#define ASN1_NRPPA_ID_MEAS_BEAM_INFO_REQUEST 31
#define ASN1_NRPPA_ID_RESULT_SS_RSRP 32
#define ASN1_NRPPA_ID_RESULT_SS_RSRQ 33
#define ASN1_NRPPA_ID_RESULT_CSI_RSRP 34
#define ASN1_NRPPA_ID_RESULT_CSI_RSRQ 35
#define ASN1_NRPPA_ID_ANGLE_OF_ARRIVAL_NR 36
#define ASN1_NRPPA_ID_GEOGRAPHICAL_COORDINATES 37
#define ASN1_NRPPA_ID_POSITIONING_BROADCAST_CELLS 38
#define ASN1_NRPPA_ID_LMF_MEAS_ID 39
#define ASN1_NRPPA_ID_RAN_MEAS_ID 40
#define ASN1_NRPPA_ID_TRP_MEAS_REQUEST_LIST 41
#define ASN1_NRPPA_ID_TRP_MEAS_RESP_LIST 42
#define ASN1_NRPPA_ID_TRP_MEAS_REPORT_LIST 43
#define ASN1_NRPPA_ID_SRS_TYPE 44
#define ASN1_NRPPA_ID_ACTIVATION_TIME 45
#define ASN1_NRPPA_ID_SRS_RES_SET_ID 46
#define ASN1_NRPPA_ID_TRP_LIST 47
#define ASN1_NRPPA_ID_SRS_SPATIAL_RELATION 48
#define ASN1_NRPPA_ID_SYS_FRAME_NUM 49
#define ASN1_NRPPA_ID_SLOT_NUM 50
#define ASN1_NRPPA_ID_SRS_RES_TRIGGER 51
#define ASN1_NRPPA_ID_TRP_MEAS_QUANTITIES 52
#define ASN1_NRPPA_ID_ABORT_TX 53
#define ASN1_NRPPA_ID_SFN_INITISATION_TIME 54
#define ASN1_NRPPA_ID_RESULT_NR 55
#define ASN1_NRPPA_ID_RESULT_EUTRA 56
#define ASN1_NRPPA_ID_TRP_INFO_TYPE_ITEM 57
#define ASN1_NRPPA_ID_CGI_NR 58
#define ASN1_NRPPA_ID_SFN_INITISATION_TIME_NR 59
#define ASN1_NRPPA_ID_CELL_ID 60
#define ASN1_NRPPA_ID_SRS_FREQ 61
#define ASN1_NRPPA_ID_TRP_TYPE 62
#define ASN1_NRPPA_ID_SRS_SPATIAL_RELATION_PER_SRS_RES 63
#define ASN1_NRPPA_ID_MEAS_PERIODICITY_EXTENDED 64
#define ASN1_NRPPA_ID_PRS_RES_ID 65
#define ASN1_NRPPA_ID_PRSTRP_LIST 66
#define ASN1_NRPPA_ID_PRS_TX_TRP_LIST 67
#define ASN1_NRPPA_ID_ON_DEMAND_PRS 68
#define ASN1_NRPPA_ID_AO_A_SEARCH_WIN 69
#define ASN1_NRPPA_ID_TRP_MEAS_UPD_LIST 70
#define ASN1_NRPPA_ID_ZO_A 71
#define ASN1_NRPPA_ID_RESP_TIME 72
#define ASN1_NRPPA_ID_UE_REPORT_INFO 73
#define ASN1_NRPPA_ID_MULTIPLE_UL_AO_A 74
#define ASN1_NRPPA_ID_UL_SRS_RSRP_P 75
#define ASN1_NRPPA_ID_SRS_RESTYPE 76
#define ASN1_NRPPA_ID_EXTENDED_ADD_PATH_LIST 77
#define ASN1_NRPPA_ID_ARP_LOCATION_INFO 78
#define ASN1_NRPPA_ID_ARP_ID 79
#define ASN1_NRPPA_ID_LO_S_N_LO_SI_NFORMATION 80
#define ASN1_NRPPA_ID_UE_TX_TEG_ASSOC_LIST 81
#define ASN1_NRPPA_ID_NOF_TRP_RX_TEG 82
#define ASN1_NRPPA_ID_NOF_TRP_RX_TX_TEG 83
#define ASN1_NRPPA_ID_TRP_TX_TEG_ASSOC 84
#define ASN1_NRPPA_ID_TRPTEG_INFO 85
#define ASN1_NRPPA_ID_TRP_RX_TEG_INFO 86
#define ASN1_NRPPA_ID_TRP_PRS_INFO_LIST 87
#define ASN1_NRPPA_ID_PRS_MEASS_INFO_LIST 88
#define ASN1_NRPPA_ID_PRS_CFG_REQUEST_TYPE 89
#define ASN1_NRPPA_ID_UE_TEG_INFO_REQUEST 90
#define ASN1_NRPPA_ID_MEAS_TIME_OCCASION 91
#define ASN1_NRPPA_ID_MEAS_CHARACTERISTICS_REQUEST_IND 92
#define ASN1_NRPPA_ID_TRP_BEAM_ANT_INFO 93
#define ASN1_NRPPA_ID_NR_TADV 94
#define ASN1_NRPPA_ID_MEAS_AMOUNT 95
#define ASN1_NRPPA_ID_PATH_PWR 96
#define ASN1_NRPPA_ID_PRECFG_RESULT 97
#define ASN1_NRPPA_ID_REQUEST_TYPE 98
#define ASN1_NRPPA_ID_UE_TEG_REPORT_PERIODICITY 99
#define ASN1_NRPPA_ID_SRS_PORT_IDX 100
#define ASN1_NRPPA_ID_PROC_CODE_101_NOT_TO_BE_USED 101
#define ASN1_NRPPA_ID_PROC_CODE_102_NOT_TO_BE_USED 102
#define ASN1_NRPPA_ID_PROC_CODE_103_NOT_TO_BE_USED 103
#define ASN1_NRPPA_ID_UE_TX_TIMING_ERROR_MARGIN 104
#define ASN1_NRPPA_ID_MEAS_PERIODICITY_NR_AO_A 105
#define ASN1_NRPPA_ID_SRS_TX_STATUS 106
#define ASN1_NRPPA_MAX_PRIVATE_IES 65535
#define ASN1_NRPPA_MAX_PROTOCOL_EXTS 65535
#define ASN1_NRPPA_MAX_PROTOCOL_IES 65535

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// LocationUncertainty-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
using location_uncertainty_ext_ies_o = protocol_ext_empty_o;

using lcs_to_gcs_translation_ext_ies_container = protocol_ext_container_empty_l;

// LCS-to-GCS-Translation ::= SEQUENCE
struct lcs_to_gcs_translation_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint16_t                                 alpha           = 0;
  uint16_t                                 beta            = 0;
  uint16_t                                 gamma           = 0;
  lcs_to_gcs_translation_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultipleULAoA-Item-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using multiple_ul_ao_a_item_ext_ies_o = protocol_ies_empty_o;

using ul_ao_a_ext_ies_container = protocol_ext_container_empty_l;

using zo_a_ext_ies_container = protocol_ext_container_empty_l;

using trp_beam_pwr_item_ext_ies_container = protocol_ext_container_empty_l;

// UL-AoA ::= SEQUENCE
struct ul_ao_a_s {
  bool                      ext                            = false;
  bool                      zenith_ao_a_present            = false;
  bool                      lcs_to_gcs_translation_present = false;
  bool                      ie_exts_present                = false;
  uint16_t                  azimuth_ao_a                   = 0;
  uint16_t                  zenith_ao_a                    = 0;
  lcs_to_gcs_translation_s  lcs_to_gcs_translation;
  ul_ao_a_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ZoA ::= SEQUENCE
struct zo_a_s {
  bool                     ext                            = false;
  bool                     lcs_to_gcs_translation_present = false;
  bool                     ie_exts_present                = false;
  uint16_t                 zenith_ao_a                    = 0;
  lcs_to_gcs_translation_s lcs_to_gcs_translation;
  zo_a_ext_ies_container   ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using location_uncertainty_ext_ies_container = protocol_ext_container_empty_l;

// MultipleULAoA-Item ::= CHOICE
struct multiple_ul_ao_a_item_c {
  struct types_opts {
    enum options { ul_ao_a, ul_zo_a, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  multiple_ul_ao_a_item_c() = default;
  multiple_ul_ao_a_item_c(const multiple_ul_ao_a_item_c& other);
  multiple_ul_ao_a_item_c& operator=(const multiple_ul_ao_a_item_c& other);
  ~multiple_ul_ao_a_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ul_ao_a_s& ul_ao_a()
  {
    assert_choice_type(types::ul_ao_a, type_, "MultipleULAoA-Item");
    return c.get<ul_ao_a_s>();
  }
  zo_a_s& ul_zo_a()
  {
    assert_choice_type(types::ul_zo_a, type_, "MultipleULAoA-Item");
    return c.get<zo_a_s>();
  }
  protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "MultipleULAoA-Item");
    return c.get<protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>>();
  }
  const ul_ao_a_s& ul_ao_a() const
  {
    assert_choice_type(types::ul_ao_a, type_, "MultipleULAoA-Item");
    return c.get<ul_ao_a_s>();
  }
  const zo_a_s& ul_zo_a() const
  {
    assert_choice_type(types::ul_zo_a, type_, "MultipleULAoA-Item");
    return c.get<zo_a_s>();
  }
  const protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "MultipleULAoA-Item");
    return c.get<protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>>();
  }
  ul_ao_a_s&                                                       set_ul_ao_a();
  zo_a_s&                                                          set_ul_zo_a();
  protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>& set_choice_ext();

private:
  types                                                                                               type_;
  choice_buffer_t<protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>, ul_ao_a_s, zo_a_s> c;

  void destroy_();
};

// RelativePathDelay-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using relative_path_delay_ext_ies_o = protocol_ies_empty_o;

// TRP-Beam-Power-Item ::= SEQUENCE
struct trp_beam_pwr_item_s {
  bool                                ext                       = false;
  bool                                prs_res_set_id_present    = false;
  bool                                relative_pwr_fine_present = false;
  bool                                ie_exts_present           = false;
  uint8_t                             prs_res_set_id            = 0;
  uint8_t                             prs_res_id                = 0;
  uint8_t                             relative_pwr              = 0;
  uint8_t                             relative_pwr_fine         = 0;
  trp_beam_pwr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using trp_elevation_angle_list_item_ext_ies_container = protocol_ext_container_empty_l;

using trp_meas_angle_quality_ext_ies_container = protocol_ext_container_empty_l;

// TrpMeasurementQuality-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using trp_meas_quality_ext_ies_o = protocol_ies_empty_o;

using trp_meas_timing_quality_ext_ies_container = protocol_ext_container_empty_l;

// ARPLocationType-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using arp_location_type_ext_ies_o = protocol_ies_empty_o;

// DL-PRSMutingPattern-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using dl_prs_muting_pattern_ext_ies_o = protocol_ies_empty_o;

// LocationUncertainty ::= SEQUENCE
struct location_uncertainty_s {
  bool                                   ext                    = false;
  bool                                   ie_exts_present        = false;
  uint16_t                               horizontal_uncertainty = 0;
  uint8_t                                horizontal_confidence  = 0;
  uint16_t                               vertical_uncertainty   = 0;
  uint8_t                                vertical_confidence    = 0;
  location_uncertainty_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using multiple_ul_ao_a_ext_ies_container = protocol_ext_container_empty_l;

// MultipleULAoA-List ::= SEQUENCE (SIZE (1..8)) OF MultipleULAoA-Item
using multiple_ul_ao_a_list_l = dyn_array<multiple_ul_ao_a_item_c>;

// PRSResource-QCLInfo-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using prs_res_qcl_info_ext_ies_o = protocol_ies_empty_o;

using prs_res_qcl_source_prs_ext_ies_container = protocol_ext_container_empty_l;

using prs_res_qcl_source_ssb_ext_ies_container = protocol_ext_container_empty_l;

using relative_cartesian_location_ext_ies_container = protocol_ext_container_empty_l;

using relative_geodetic_location_ext_ies_container = protocol_ext_container_empty_l;

// RxTxTimingErrorMargin ::= ENUMERATED
struct rx_tx_timing_error_margin_opts {
  enum options {
    tc0dot5,
    tc1,
    tc2,
    tc4,
    tc8,
    tc12,
    tc16,
    tc20,
    tc24,
    tc32,
    tc40,
    tc48,
    tc64,
    tc80,
    tc96,
    tc128,
    // ...
    nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using rx_tx_timing_error_margin_e = enumerated<rx_tx_timing_error_margin_opts, true>;

// TimingErrorMargin ::= ENUMERATED
struct timing_error_margin_opts {
  enum options {
    tc0,
    tc2,
    tc4,
    tc6,
    tc8,
    tc12,
    tc16,
    tc20,
    tc24,
    tc32,
    tc40,
    tc48,
    tc56,
    tc64,
    tc72,
    tc80,
    /*...*/ nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using timing_error_margin_e = enumerated<timing_error_margin_opts, true>;

using trp_beam_ant_angles_list_item_ext_ies_container = protocol_ext_container_empty_l;

// TRP-ElevationAngleList-Item ::= SEQUENCE
struct trp_elevation_angle_list_item_s {
  using trp_beam_pwr_list_l_ = dyn_array<trp_beam_pwr_item_s>;

  // member variables
  bool                                            ext                              = false;
  bool                                            trp_elevation_angle_fine_present = false;
  bool                                            ie_exts_present                  = false;
  uint8_t                                         trp_elevation_angle              = 0;
  uint8_t                                         trp_elevation_angle_fine         = 0;
  trp_beam_pwr_list_l_                            trp_beam_pwr_list;
  trp_elevation_angle_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrpMeasurementAngleQuality ::= SEQUENCE
struct trp_meas_angle_quality_s {
  struct resolution_opts {
    enum options { deg0dot1, /*...*/ nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using resolution_e_ = enumerated<resolution_opts, true>;

  // member variables
  bool                                     ext                    = false;
  bool                                     zenith_quality_present = false;
  bool                                     ie_exts_present        = false;
  uint16_t                                 azimuth_quality        = 0;
  uint16_t                                 zenith_quality         = 0;
  resolution_e_                            resolution;
  trp_meas_angle_quality_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrpMeasurementTimingQuality ::= SEQUENCE
struct trp_meas_timing_quality_s {
  struct resolution_opts {
    enum options { m0dot1, m1, m10, m30, /*...*/ nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using resolution_e_ = enumerated<resolution_opts, true>;

  // member variables
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  uint8_t                                   meas_quality    = 0;
  resolution_e_                             resolution;
  trp_meas_timing_quality_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using trp_rx_teg_info_ext_ies_container = protocol_ext_container_empty_l;

using trp_rx_tx_teg_info_ext_ies_container = protocol_ext_container_empty_l;

using trp_tx_teg_info_ext_ies_container = protocol_ext_container_empty_l;

using ul_srs_rsrp_p_ext_ies_container = protocol_ext_container_empty_l;

// DL-PRSResourceARPLocation-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using dl_prs_res_arp_location_ext_ies_o = protocol_ies_empty_o;

using extended_add_path_list_item_ext_ies_container = protocol_ext_container_empty_l;

// MultipleULAoA ::= SEQUENCE
struct multiple_ul_ao_a_s {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  multiple_ul_ao_a_list_l            multiple_ul_ao_a;
  multiple_ul_ao_a_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSResource-QCLSourcePRS ::= SEQUENCE
struct prs_res_qcl_source_prs_s {
  bool                                     ext                           = false;
  bool                                     qcl_source_prs_res_id_present = false;
  bool                                     ie_exts_present               = false;
  uint8_t                                  qcl_source_prs_res_set_id     = 0;
  uint8_t                                  qcl_source_prs_res_id         = 0;
  prs_res_qcl_source_prs_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSResource-QCLSourceSSB ::= SEQUENCE
struct prs_res_qcl_source_ssb_s {
  bool                                     ext             = false;
  bool                                     ssb_idx_present = false;
  bool                                     ie_exts_present = false;
  uint16_t                                 pci_nr          = 0;
  uint8_t                                  ssb_idx         = 0;
  prs_res_qcl_source_ssb_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RelativeCartesianLocation ::= SEQUENCE
struct relative_cartesian_location_s {
  struct xy_zunit_opts {
    enum options { mm, cm, dm, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using xy_zunit_e_ = enumerated<xy_zunit_opts, true>;

  // member variables
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  xy_zunit_e_                                   xy_zunit;
  int32_t                                       xvalue = -65536;
  int32_t                                       yvalue = -65536;
  int32_t                                       zvalue = -32768;
  location_uncertainty_s                        location_uncertainty;
  relative_cartesian_location_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RelativeGeodeticLocation ::= SEQUENCE
struct relative_geodetic_location_s {
  struct milli_arc_second_units_opts {
    enum options { zerodot03, zerodot3, three, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using milli_arc_second_units_e_ = enumerated<milli_arc_second_units_opts, true>;
  struct height_units_opts {
    enum options { mm, cm, m, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using height_units_e_ = enumerated<height_units_opts, true>;

  // member variables
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  milli_arc_second_units_e_                    milli_arc_second_units;
  height_units_e_                              height_units;
  int16_t                                      delta_latitude  = -1024;
  int16_t                                      delta_longitude = -1024;
  int16_t                                      delta_height    = -1024;
  location_uncertainty_s                       location_uncertainty;
  relative_geodetic_location_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RelativePathDelay ::= CHOICE
struct relative_path_delay_c {
  struct types_opts {
    enum options { k0, k1, k2, k3, k4, k5, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  relative_path_delay_c() = default;
  relative_path_delay_c(const relative_path_delay_c& other);
  relative_path_delay_c& operator=(const relative_path_delay_c& other);
  ~relative_path_delay_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& k0()
  {
    assert_choice_type(types::k0, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  uint16_t& k1()
  {
    assert_choice_type(types::k1, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  uint16_t& k2()
  {
    assert_choice_type(types::k2, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  uint16_t& k3()
  {
    assert_choice_type(types::k3, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  uint16_t& k4()
  {
    assert_choice_type(types::k4, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  uint16_t& k5()
  {
    assert_choice_type(types::k5, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  protocol_ie_single_container_s<relative_path_delay_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "RelativePathDelay");
    return c.get<protocol_ie_single_container_s<relative_path_delay_ext_ies_o>>();
  }
  const uint16_t& k0() const
  {
    assert_choice_type(types::k0, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  const uint16_t& k1() const
  {
    assert_choice_type(types::k1, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  const uint16_t& k2() const
  {
    assert_choice_type(types::k2, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  const uint16_t& k3() const
  {
    assert_choice_type(types::k3, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  const uint16_t& k4() const
  {
    assert_choice_type(types::k4, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  const uint16_t& k5() const
  {
    assert_choice_type(types::k5, type_, "RelativePathDelay");
    return c.get<uint16_t>();
  }
  const protocol_ie_single_container_s<relative_path_delay_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "RelativePathDelay");
    return c.get<protocol_ie_single_container_s<relative_path_delay_ext_ies_o>>();
  }
  uint16_t&                                                      set_k0();
  uint16_t&                                                      set_k1();
  uint16_t&                                                      set_k2();
  uint16_t&                                                      set_k3();
  uint16_t&                                                      set_k4();
  uint16_t&                                                      set_k5();
  protocol_ie_single_container_s<relative_path_delay_ext_ies_o>& set_choice_ext();

private:
  types                                                                          type_;
  choice_buffer_t<protocol_ie_single_container_s<relative_path_delay_ext_ies_o>> c;

  void destroy_();
};

using rx_teg_ext_ies_container = protocol_ext_container_empty_l;

using rx_tx_teg_ext_ies_container = protocol_ext_container_empty_l;

// TRP-BeamAntennaAnglesList-Item ::= SEQUENCE
struct trp_beam_ant_angles_list_item_s {
  using trp_elevation_angle_list_l_ = dyn_array<trp_elevation_angle_list_item_s>;

  // member variables
  bool                                            ext                            = false;
  bool                                            trp_azimuth_angle_fine_present = false;
  bool                                            ie_exts_present                = false;
  uint16_t                                        trp_azimuth_angle              = 0;
  uint8_t                                         trp_azimuth_angle_fine         = 0;
  trp_elevation_angle_list_l_                     trp_elevation_angle_list;
  trp_beam_ant_angles_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrpMeasurementQuality ::= CHOICE
struct trp_meas_quality_c {
  struct types_opts {
    enum options { timing_meas_quality, angle_meas_quality, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  trp_meas_quality_c() = default;
  trp_meas_quality_c(const trp_meas_quality_c& other);
  trp_meas_quality_c& operator=(const trp_meas_quality_c& other);
  ~trp_meas_quality_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  trp_meas_timing_quality_s& timing_meas_quality()
  {
    assert_choice_type(types::timing_meas_quality, type_, "TrpMeasurementQuality");
    return c.get<trp_meas_timing_quality_s>();
  }
  trp_meas_angle_quality_s& angle_meas_quality()
  {
    assert_choice_type(types::angle_meas_quality, type_, "TrpMeasurementQuality");
    return c.get<trp_meas_angle_quality_s>();
  }
  protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TrpMeasurementQuality");
    return c.get<protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>>();
  }
  const trp_meas_timing_quality_s& timing_meas_quality() const
  {
    assert_choice_type(types::timing_meas_quality, type_, "TrpMeasurementQuality");
    return c.get<trp_meas_timing_quality_s>();
  }
  const trp_meas_angle_quality_s& angle_meas_quality() const
  {
    assert_choice_type(types::angle_meas_quality, type_, "TrpMeasurementQuality");
    return c.get<trp_meas_angle_quality_s>();
  }
  const protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TrpMeasurementQuality");
    return c.get<protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>>();
  }
  trp_meas_timing_quality_s&                                  set_timing_meas_quality();
  trp_meas_angle_quality_s&                                   set_angle_meas_quality();
  protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>,
                  trp_meas_angle_quality_s,
                  trp_meas_timing_quality_s>
      c;

  void destroy_();
};

// TRP-Rx-TEGInformation ::= SEQUENCE
struct trp_rx_teg_info_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  uint8_t                           trp_rx_teg_id   = 0;
  timing_error_margin_e             trp_rx_timing_error_margin;
  trp_rx_teg_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRP-RxTx-TEGInformation ::= SEQUENCE
struct trp_rx_tx_teg_info_s {
  bool                                 ext              = false;
  bool                                 ie_exts_present  = false;
  uint16_t                             trp_rx_tx_teg_id = 0;
  rx_tx_timing_error_margin_e          trp_rx_tx_timing_error_margin;
  trp_rx_tx_teg_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRP-Tx-TEGInformation ::= SEQUENCE
struct trp_tx_teg_info_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  uint8_t                           trp_tx_teg_id   = 0;
  timing_error_margin_e             trp_tx_timing_error_margin;
  trp_tx_teg_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPTEGInformation-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using trpteg_info_ext_ies_o = protocol_ies_empty_o;

// UL-SRS-RSRPP ::= SEQUENCE
struct ul_srs_rsrp_p_s {
  bool                            ext               = false;
  bool                            ie_exts_present   = false;
  uint8_t                         first_path_rsrp_p = 0;
  ul_srs_rsrp_p_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalPathListItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct add_path_list_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { multiple_ul_ao_a, path_pwr, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multiple_ul_ao_a_s&       multiple_ul_ao_a();
    ul_srs_rsrp_p_s&          path_pwr();
    const multiple_ul_ao_a_s& multiple_ul_ao_a() const;
    const ul_srs_rsrp_p_s&    path_pwr() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using arp_location_info_ext_ies_container = protocol_ext_container_empty_l;

// ARPLocationType ::= CHOICE
struct arp_location_type_c {
  struct types_opts {
    enum options { arp_position_relative_geodetic, arp_position_relative_cartesian, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  arp_location_type_c() = default;
  arp_location_type_c(const arp_location_type_c& other);
  arp_location_type_c& operator=(const arp_location_type_c& other);
  ~arp_location_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  relative_geodetic_location_s& arp_position_relative_geodetic()
  {
    assert_choice_type(types::arp_position_relative_geodetic, type_, "ARPLocationType");
    return c.get<relative_geodetic_location_s>();
  }
  relative_cartesian_location_s& arp_position_relative_cartesian()
  {
    assert_choice_type(types::arp_position_relative_cartesian, type_, "ARPLocationType");
    return c.get<relative_cartesian_location_s>();
  }
  protocol_ie_single_container_s<arp_location_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ARPLocationType");
    return c.get<protocol_ie_single_container_s<arp_location_type_ext_ies_o>>();
  }
  const relative_geodetic_location_s& arp_position_relative_geodetic() const
  {
    assert_choice_type(types::arp_position_relative_geodetic, type_, "ARPLocationType");
    return c.get<relative_geodetic_location_s>();
  }
  const relative_cartesian_location_s& arp_position_relative_cartesian() const
  {
    assert_choice_type(types::arp_position_relative_cartesian, type_, "ARPLocationType");
    return c.get<relative_cartesian_location_s>();
  }
  const protocol_ie_single_container_s<arp_location_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ARPLocationType");
    return c.get<protocol_ie_single_container_s<arp_location_type_ext_ies_o>>();
  }
  relative_geodetic_location_s&                                set_arp_position_relative_geodetic();
  relative_cartesian_location_s&                               set_arp_position_relative_cartesian();
  protocol_ie_single_container_s<arp_location_type_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<arp_location_type_ext_ies_o>,
                  relative_cartesian_location_s,
                  relative_geodetic_location_s>
      c;

  void destroy_();
};

// Choice-TRP-Beam-Info-Item-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using choice_trp_beam_info_item_ext_ies_o = protocol_ies_empty_o;

using dl_prs_ext_ies_container = protocol_ext_container_empty_l;

// DL-PRSMutingPattern ::= CHOICE
struct dl_prs_muting_pattern_c {
  struct types_opts {
    enum options { two, four, six, eight, sixteen, thirty_two, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  dl_prs_muting_pattern_c() = default;
  dl_prs_muting_pattern_c(const dl_prs_muting_pattern_c& other);
  dl_prs_muting_pattern_c& operator=(const dl_prs_muting_pattern_c& other);
  ~dl_prs_muting_pattern_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<2, false, true>& two()
  {
    assert_choice_type(types::two, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<2, false, true>>();
  }
  fixed_bitstring<4, false, true>& four()
  {
    assert_choice_type(types::four, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<4, false, true>>();
  }
  fixed_bitstring<6, false, true>& six()
  {
    assert_choice_type(types::six, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<6, false, true>>();
  }
  fixed_bitstring<8, false, true>& eight()
  {
    assert_choice_type(types::eight, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<8, false, true>>();
  }
  fixed_bitstring<16, false, true>& sixteen()
  {
    assert_choice_type(types::sixteen, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<16, false, true>>();
  }
  fixed_bitstring<32, false, true>& thirty_two()
  {
    assert_choice_type(types::thirty_two, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<32, false, true>>();
  }
  protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "DL-PRSMutingPattern");
    return c.get<protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>>();
  }
  const fixed_bitstring<2, false, true>& two() const
  {
    assert_choice_type(types::two, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<2, false, true>>();
  }
  const fixed_bitstring<4, false, true>& four() const
  {
    assert_choice_type(types::four, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<4, false, true>>();
  }
  const fixed_bitstring<6, false, true>& six() const
  {
    assert_choice_type(types::six, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<6, false, true>>();
  }
  const fixed_bitstring<8, false, true>& eight() const
  {
    assert_choice_type(types::eight, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<8, false, true>>();
  }
  const fixed_bitstring<16, false, true>& sixteen() const
  {
    assert_choice_type(types::sixteen, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<16, false, true>>();
  }
  const fixed_bitstring<32, false, true>& thirty_two() const
  {
    assert_choice_type(types::thirty_two, type_, "DL-PRSMutingPattern");
    return c.get<fixed_bitstring<32, false, true>>();
  }
  const protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "DL-PRSMutingPattern");
    return c.get<protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>>();
  }
  fixed_bitstring<2, false, true>&                                 set_two();
  fixed_bitstring<4, false, true>&                                 set_four();
  fixed_bitstring<6, false, true>&                                 set_six();
  fixed_bitstring<8, false, true>&                                 set_eight();
  fixed_bitstring<16, false, true>&                                set_sixteen();
  fixed_bitstring<32, false, true>&                                set_thirty_two();
  protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<fixed_bitstring<32, false, true>, protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>> c;

  void destroy_();
};

using dl_prs_res_item_ext_ies_container = protocol_ext_container_empty_l;

// DL-PRSResourceSetARPLocation-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using dl_prs_res_set_arp_location_ext_ies_o = protocol_ies_empty_o;

// ExtendedAdditionalPathList-Item ::= SEQUENCE
struct extended_add_path_list_item_s {
  bool                                          ext                      = false;
  bool                                          path_quality_present     = false;
  bool                                          multiple_ul_ao_a_present = false;
  bool                                          path_pwr_present         = false;
  bool                                          ie_exts_present          = false;
  relative_path_delay_c                         relative_time_of_path;
  trp_meas_quality_c                            path_quality;
  multiple_ul_ao_a_s                            multiple_ul_ao_a;
  ul_srs_rsrp_p_s                               path_pwr;
  extended_add_path_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ng_ran_access_point_position_ext_ies_container = protocol_ext_container_empty_l;

using ngran_high_accuracy_access_point_position_ext_ies_container = protocol_ext_container_empty_l;

// PRSAngleItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct prs_angle_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { prs_res_id, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::prs_res_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       prs_res_id() { return c; }
    const uint8_t& prs_res_id() const { return c; }

  private:
    uint8_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using prs_muting_option1_ext_ies_container = protocol_ext_container_empty_l;

using prs_muting_option2_ext_ies_container = protocol_ext_container_empty_l;

using prs_res_item_ext_ies_container = protocol_ext_container_empty_l;

// PRSResource-QCLInfo ::= CHOICE
struct prs_res_qcl_info_c {
  struct types_opts {
    enum options { qcl_source_ssb, qcl_source_prs, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  prs_res_qcl_info_c() = default;
  prs_res_qcl_info_c(const prs_res_qcl_info_c& other);
  prs_res_qcl_info_c& operator=(const prs_res_qcl_info_c& other);
  ~prs_res_qcl_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  prs_res_qcl_source_ssb_s& qcl_source_ssb()
  {
    assert_choice_type(types::qcl_source_ssb, type_, "PRSResource-QCLInfo");
    return c.get<prs_res_qcl_source_ssb_s>();
  }
  prs_res_qcl_source_prs_s& qcl_source_prs()
  {
    assert_choice_type(types::qcl_source_prs, type_, "PRSResource-QCLInfo");
    return c.get<prs_res_qcl_source_prs_s>();
  }
  protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "PRSResource-QCLInfo");
    return c.get<protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>>();
  }
  const prs_res_qcl_source_ssb_s& qcl_source_ssb() const
  {
    assert_choice_type(types::qcl_source_ssb, type_, "PRSResource-QCLInfo");
    return c.get<prs_res_qcl_source_ssb_s>();
  }
  const prs_res_qcl_source_prs_s& qcl_source_prs() const
  {
    assert_choice_type(types::qcl_source_prs, type_, "PRSResource-QCLInfo");
    return c.get<prs_res_qcl_source_prs_s>();
  }
  const protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "PRSResource-QCLInfo");
    return c.get<protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>>();
  }
  prs_res_qcl_source_ssb_s&                                   set_qcl_source_ssb();
  prs_res_qcl_source_prs_s&                                   set_qcl_source_prs();
  protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>,
                  prs_res_qcl_source_prs_s,
                  prs_res_qcl_source_ssb_s>
      c;

  void destroy_();
};

// ReferencePoint-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using ref_point_ext_ies_o = protocol_ies_empty_o;

// ReferenceSignal-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using ref_sig_ext_ie_o = protocol_ies_empty_o;

// RxTEG ::= SEQUENCE
struct rx_teg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  trp_rx_teg_info_s        trp_rx_teg_info;
  trp_tx_teg_info_s        trp_tx_teg_info;
  rx_teg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RxTxTEG ::= SEQUENCE
struct rx_tx_teg_s {
  bool                        ext                     = false;
  bool                        trp_tx_teg_info_present = false;
  bool                        ie_exts_present         = false;
  trp_rx_tx_teg_info_s        trp_rx_tx_teg_info;
  trp_tx_teg_info_s           trp_tx_teg_info;
  rx_tx_teg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRSPortIndex ::= ENUMERATED
struct srs_port_idx_opts {
  enum options { id1000, id1001, id1002, id1003, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using srs_port_idx_e = enumerated<srs_port_idx_opts, true>;

// SSBBurstPosition-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using ssb_burst_position_ext_ies_o = protocol_ies_empty_o;

using ssb_ext_ies_container = protocol_ext_container_empty_l;

// TRP-BeamAntennaAngles ::= SEQUENCE (SIZE (1..3600)) OF TRP-BeamAntennaAnglesList-Item
using trp_beam_ant_angles_l = dyn_array<trp_beam_ant_angles_list_item_s>;

using trp_beam_ant_explicit_info_ext_ies_container = protocol_ext_container_empty_l;

// TRPPositionDirectAccuracy-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using trp_position_direct_accuracy_ext_ies_o = protocol_ies_empty_o;

// TRPReferencePointType-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using trp_ref_point_type_ext_ies_o = protocol_ies_empty_o;

// ARPLocationInformation-Item ::= SEQUENCE
struct arp_location_info_item_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  uint8_t                             arp_id          = 1;
  arp_location_type_c                 arp_location_type;
  arp_location_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using dl_prs_res_arp_ext_ies_container = protocol_ext_container_empty_l;

// DL-PRSResourceARPLocation ::= CHOICE
struct dl_prs_res_arp_location_c {
  struct types_opts {
    enum options { relative_geodetic_location, relative_cartesian_location, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  dl_prs_res_arp_location_c() = default;
  dl_prs_res_arp_location_c(const dl_prs_res_arp_location_c& other);
  dl_prs_res_arp_location_c& operator=(const dl_prs_res_arp_location_c& other);
  ~dl_prs_res_arp_location_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  relative_geodetic_location_s& relative_geodetic_location()
  {
    assert_choice_type(types::relative_geodetic_location, type_, "DL-PRSResourceARPLocation");
    return c.get<relative_geodetic_location_s>();
  }
  relative_cartesian_location_s& relative_cartesian_location()
  {
    assert_choice_type(types::relative_cartesian_location, type_, "DL-PRSResourceARPLocation");
    return c.get<relative_cartesian_location_s>();
  }
  protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "DL-PRSResourceARPLocation");
    return c.get<protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>>();
  }
  const relative_geodetic_location_s& relative_geodetic_location() const
  {
    assert_choice_type(types::relative_geodetic_location, type_, "DL-PRSResourceARPLocation");
    return c.get<relative_geodetic_location_s>();
  }
  const relative_cartesian_location_s& relative_cartesian_location() const
  {
    assert_choice_type(types::relative_cartesian_location, type_, "DL-PRSResourceARPLocation");
    return c.get<relative_cartesian_location_s>();
  }
  const protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "DL-PRSResourceARPLocation");
    return c.get<protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>>();
  }
  relative_geodetic_location_s&                                      set_relative_geodetic_location();
  relative_cartesian_location_s&                                     set_relative_cartesian_location();
  protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>,
                  relative_cartesian_location_s,
                  relative_geodetic_location_s>
      c;

  void destroy_();
};

// DLPRSResourceID-Item ::= SEQUENCE
struct dl_prs_res_id_item_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  uint8_t                           dl_prs_res_id   = 0;
  dl_prs_res_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-PRS ::= SEQUENCE
struct dl_prs_s {
  bool                     ext                   = false;
  bool                     dl_prs_res_id_present = false;
  bool                     ie_exts_present       = false;
  uint16_t                 prsid                 = 0;
  uint8_t                  dl_prs_res_set_id     = 0;
  uint8_t                  dl_prs_res_id         = 0;
  dl_prs_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using expected_azimuth_ao_a_ext_ies_container = protocol_ext_container_empty_l;

using expected_zenith_ao_a_ext_ies_container = protocol_ext_container_empty_l;

// ExtendedAdditionalPathList ::= SEQUENCE (SIZE (1..8)) OF ExtendedAdditionalPathList-Item
using extended_add_path_list_l = dyn_array<extended_add_path_list_item_s>;

// LoS-NLoSInformation-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using lo_s_n_lo_si_nformation_ext_ies_o = protocol_ies_empty_o;

// NG-RANAccessPointPosition ::= SEQUENCE
struct ng_ran_access_point_position_s {
  struct latitude_sign_opts {
    enum options { north, south, nulltype } value;

    const char* to_string() const;
  };
  using latitude_sign_e_ = enumerated<latitude_sign_opts>;
  struct direction_of_altitude_opts {
    enum options { height, depth, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using direction_of_altitude_e_ = enumerated<direction_of_altitude_opts>;

  // member variables
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  latitude_sign_e_                               latitude_sign;
  uint32_t                                       latitude  = 0;
  int32_t                                        longitude = -8388608;
  direction_of_altitude_e_                       direction_of_altitude;
  uint16_t                                       altitude                  = 0;
  uint8_t                                        uncertainty_semi_major    = 0;
  uint8_t                                        uncertainty_semi_minor    = 0;
  uint8_t                                        orientation_of_major_axis = 0;
  uint8_t                                        uncertainty_altitude      = 0;
  uint8_t                                        confidence                = 0;
  ng_ran_access_point_position_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NGRANHighAccuracyAccessPointPosition ::= SEQUENCE
struct ngran_high_accuracy_access_point_position_s {
  bool                                                        ext                       = false;
  bool                                                        ie_exts_present           = false;
  int64_t                                                     latitude                  = -2147483648;
  int64_t                                                     longitude                 = -2147483648;
  int32_t                                                     altitude                  = -64000;
  uint16_t                                                    uncertainty_semi_major    = 0;
  uint16_t                                                    uncertainty_semi_minor    = 0;
  uint8_t                                                     orientation_of_major_axis = 0;
  uint8_t                                                     horizontal_confidence     = 0;
  uint16_t                                                    uncertainty_altitude      = 0;
  uint8_t                                                     vertical_confidence       = 0;
  ngran_high_accuracy_access_point_position_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pos_res_set_type_aperiodic_ext_ies_container = protocol_ext_container_empty_l;

// PosResourceSetType-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using pos_res_set_type_ext_ies_o = protocol_ies_empty_o;

using pos_res_set_type_periodic_ext_ies_container = protocol_ext_container_empty_l;

using pos_res_set_type_semi_persistent_ext_ies_container = protocol_ext_container_empty_l;

// PosSRSInfo ::= SEQUENCE
struct pos_srs_info_s {
  bool    ext            = false;
  uint8_t pos_srs_res_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using pr_si_nformation_pos_ext_ies_container = protocol_ext_container_empty_l;

using prs_muting_ext_ies_container = protocol_ext_container_empty_l;

// PRSMutingOption1 ::= SEQUENCE
struct prs_muting_option1_s {
  struct muting_bit_repeat_factor_opts {
    enum options { n1, n2, n4, n8, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using muting_bit_repeat_factor_e_ = enumerated<muting_bit_repeat_factor_opts, true>;

  // member variables
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  dl_prs_muting_pattern_c              muting_pattern;
  muting_bit_repeat_factor_e_          muting_bit_repeat_factor;
  prs_muting_option1_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSMutingOption2 ::= SEQUENCE
struct prs_muting_option2_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  dl_prs_muting_pattern_c              muting_pattern;
  prs_muting_option2_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSResource-Item ::= SEQUENCE
struct prs_res_item_s {
  bool                           ext               = false;
  bool                           qcl_info_present  = false;
  bool                           ie_exts_present   = false;
  uint8_t                        prs_res_id        = 0;
  uint16_t                       seq_id            = 0;
  uint8_t                        re_offset         = 0;
  uint16_t                       res_slot_offset   = 0;
  uint8_t                        res_symbol_offset = 0;
  prs_res_qcl_info_c             qcl_info;
  prs_res_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using res_set_type_aperiodic_ext_ies_container = protocol_ext_container_empty_l;

// ResourceSetType-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using res_set_type_ext_ies_o = protocol_ies_empty_o;

using res_set_type_periodic_ext_ies_container = protocol_ext_container_empty_l;

using res_set_type_semi_persistent_ext_ies_container = protocol_ext_container_empty_l;

using res_type_aperiodic_ext_ies_container = protocol_ext_container_empty_l;

using res_type_aperiodic_pos_ext_ies_container = protocol_ext_container_empty_l;

// ResourceType-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using res_type_ext_ies_o = protocol_ies_empty_o;

using res_type_periodic_ext_ies_container = protocol_ext_container_empty_l;

using res_type_periodic_pos_ext_ies_container = protocol_ext_container_empty_l;

// ResourceTypePos-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using res_type_pos_ext_ies_o = protocol_ies_empty_o;

using res_type_semi_persistent_ext_ies_container = protocol_ext_container_empty_l;

using res_type_semi_persistent_pos_ext_ies_container = protocol_ext_container_empty_l;

using result_csi_rsrp_per_csi_rs_item_ext_ies_container = protocol_ext_container_empty_l;

using result_csi_rsrq_per_csi_rs_item_ext_ies_container = protocol_ext_container_empty_l;

using result_ss_rsrp_per_ssb_item_ext_ies_container = protocol_ext_container_empty_l;

using result_ss_rsrq_per_ssb_item_ext_ies_container = protocol_ext_container_empty_l;

// SpatialInformationPos-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using spatial_info_pos_ext_ies_o = protocol_ies_empty_o;

// SRSInfo ::= SEQUENCE
struct srs_info_s {
  bool    ext     = false;
  uint8_t srs_res = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRSResourcetype-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct srs_restype_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { srs_port_idx, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srs_port_idx; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srs_port_idx_e&       srs_port_idx() { return c; }
    const srs_port_idx_e& srs_port_idx() const { return c; }

  private:
    srs_port_idx_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SSB ::= SEQUENCE
struct ssb_s {
  bool                  ext             = false;
  bool                  ssb_idx_present = false;
  bool                  ie_exts_present = false;
  uint16_t              pci_nr          = 0;
  uint8_t               ssb_idx         = 0;
  ssb_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRP-BeamAntennaExplicitInformation ::= SEQUENCE
struct trp_beam_ant_explicit_info_s {
  bool                                         ext                            = false;
  bool                                         lcs_to_gcs_translation_present = false;
  bool                                         ie_exts_present                = false;
  trp_beam_ant_angles_l                        trp_beam_ant_angles;
  lcs_to_gcs_translation_s                     lcs_to_gcs_translation;
  trp_beam_ant_explicit_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPTEGInformation ::= CHOICE
struct trpteg_info_c {
  struct types_opts {
    enum options { rx_tx_teg, rx_teg, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  trpteg_info_c() = default;
  trpteg_info_c(const trpteg_info_c& other);
  trpteg_info_c& operator=(const trpteg_info_c& other);
  ~trpteg_info_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  rx_tx_teg_s& rx_tx_teg()
  {
    assert_choice_type(types::rx_tx_teg, type_, "TRPTEGInformation");
    return c.get<rx_tx_teg_s>();
  }
  rx_teg_s& rx_teg()
  {
    assert_choice_type(types::rx_teg, type_, "TRPTEGInformation");
    return c.get<rx_teg_s>();
  }
  protocol_ie_single_container_s<trpteg_info_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TRPTEGInformation");
    return c.get<protocol_ie_single_container_s<trpteg_info_ext_ies_o>>();
  }
  const rx_tx_teg_s& rx_tx_teg() const
  {
    assert_choice_type(types::rx_tx_teg, type_, "TRPTEGInformation");
    return c.get<rx_tx_teg_s>();
  }
  const rx_teg_s& rx_teg() const
  {
    assert_choice_type(types::rx_teg, type_, "TRPTEGInformation");
    return c.get<rx_teg_s>();
  }
  const protocol_ie_single_container_s<trpteg_info_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TRPTEGInformation");
    return c.get<protocol_ie_single_container_s<trpteg_info_ext_ies_o>>();
  }
  rx_tx_teg_s&                                           set_rx_tx_teg();
  rx_teg_s&                                              set_rx_teg();
  protocol_ie_single_container_s<trpteg_info_ext_ies_o>& set_choice_ext();

private:
  types                                                                                         type_;
  choice_buffer_t<protocol_ie_single_container_s<trpteg_info_ext_ies_o>, rx_teg_s, rx_tx_teg_s> c;

  void destroy_();
};

using trpteg_item_ext_ies_container = protocol_ext_container_empty_l;

// TransmissionComb-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using tx_comb_ext_ies_o = protocol_ies_empty_o;

// TransmissionCombPos-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using tx_comb_pos_ext_ies_o = protocol_ies_empty_o;

struct add_path_list_item_ext_ies_container {
  bool               multiple_ul_ao_a_present = false;
  bool               path_pwr_present         = false;
  multiple_ul_ao_a_s multiple_ul_ao_a;
  ul_srs_rsrp_p_s    path_pwr;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AngleMeasurementType-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using angle_meas_type_ext_ies_o = protocol_ies_empty_o;

// ARPLocationInformation ::= SEQUENCE (SIZE (1..16)) OF ARPLocationInformation-Item
using arp_location_info_l = dyn_array<arp_location_info_item_s>;

using cgi_nr_ext_ies_container = protocol_ext_container_empty_l;

// Choice-TRP-Beam-Antenna-Info-Item ::= CHOICE
struct choice_trp_beam_ant_info_item_c {
  struct types_opts {
    enum options { ref, explicit_type, no_change, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  choice_trp_beam_ant_info_item_c() = default;
  choice_trp_beam_ant_info_item_c(const choice_trp_beam_ant_info_item_c& other);
  choice_trp_beam_ant_info_item_c& operator=(const choice_trp_beam_ant_info_item_c& other);
  ~choice_trp_beam_ant_info_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint32_t& ref()
  {
    assert_choice_type(types::ref, type_, "Choice-TRP-Beam-Antenna-Info-Item");
    return c.get<uint32_t>();
  }
  trp_beam_ant_explicit_info_s& explicit_type()
  {
    assert_choice_type(types::explicit_type, type_, "Choice-TRP-Beam-Antenna-Info-Item");
    return c.get<trp_beam_ant_explicit_info_s>();
  }
  protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "Choice-TRP-Beam-Antenna-Info-Item");
    return c.get<protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>>();
  }
  const uint32_t& ref() const
  {
    assert_choice_type(types::ref, type_, "Choice-TRP-Beam-Antenna-Info-Item");
    return c.get<uint32_t>();
  }
  const trp_beam_ant_explicit_info_s& explicit_type() const
  {
    assert_choice_type(types::explicit_type, type_, "Choice-TRP-Beam-Antenna-Info-Item");
    return c.get<trp_beam_ant_explicit_info_s>();
  }
  const protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "Choice-TRP-Beam-Antenna-Info-Item");
    return c.get<protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>>();
  }
  uint32_t&                                                            set_ref();
  trp_beam_ant_explicit_info_s&                                        set_explicit_type();
  void                                                                 set_no_change();
  protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>, trp_beam_ant_explicit_info_s> c;

  void destroy_();
};

// DLPRSResourceARP ::= SEQUENCE
struct dl_prs_res_arp_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  uint8_t                          dl_prs_res_id   = 0;
  dl_prs_res_arp_location_c        dl_prs_res_arp_location;
  dl_prs_res_arp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using dl_prs_res_set_arp_ext_ies_container = protocol_ext_container_empty_l;

// DL-PRSResourceSetARPLocation ::= CHOICE
struct dl_prs_res_set_arp_location_c {
  struct types_opts {
    enum options { relative_geodetic_location, relative_cartesian_location, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  dl_prs_res_set_arp_location_c() = default;
  dl_prs_res_set_arp_location_c(const dl_prs_res_set_arp_location_c& other);
  dl_prs_res_set_arp_location_c& operator=(const dl_prs_res_set_arp_location_c& other);
  ~dl_prs_res_set_arp_location_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  relative_geodetic_location_s& relative_geodetic_location()
  {
    assert_choice_type(types::relative_geodetic_location, type_, "DL-PRSResourceSetARPLocation");
    return c.get<relative_geodetic_location_s>();
  }
  relative_cartesian_location_s& relative_cartesian_location()
  {
    assert_choice_type(types::relative_cartesian_location, type_, "DL-PRSResourceSetARPLocation");
    return c.get<relative_cartesian_location_s>();
  }
  protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "DL-PRSResourceSetARPLocation");
    return c.get<protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>>();
  }
  const relative_geodetic_location_s& relative_geodetic_location() const
  {
    assert_choice_type(types::relative_geodetic_location, type_, "DL-PRSResourceSetARPLocation");
    return c.get<relative_geodetic_location_s>();
  }
  const relative_cartesian_location_s& relative_cartesian_location() const
  {
    assert_choice_type(types::relative_cartesian_location, type_, "DL-PRSResourceSetARPLocation");
    return c.get<relative_cartesian_location_s>();
  }
  const protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "DL-PRSResourceSetARPLocation");
    return c.get<protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>>();
  }
  relative_geodetic_location_s&                                          set_relative_geodetic_location();
  relative_cartesian_location_s&                                         set_relative_cartesian_location();
  protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>,
                  relative_cartesian_location_s,
                  relative_geodetic_location_s>
      c;

  void destroy_();
};

// Expected-Azimuth-AoA ::= SEQUENCE
struct expected_azimuth_ao_a_s {
  bool                                    ext                               = false;
  bool                                    ie_exts_present                   = false;
  uint16_t                                expected_azimuth_ao_a_value       = 0;
  uint16_t                                expected_azimuth_ao_a_uncertainty = 0;
  expected_azimuth_ao_a_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using expected_ul_ao_a_ext_ies_container = protocol_ext_container_empty_l;

// Expected-Zenith-AoA ::= SEQUENCE
struct expected_zenith_ao_a_s {
  bool                                   ext                              = false;
  bool                                   ie_exts_present                  = false;
  uint16_t                               expected_zenith_ao_a_value       = 0;
  uint16_t                               expected_zenith_ao_a_uncertainty = 0;
  expected_zenith_ao_a_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using expected_zo_a_only_ext_ies_container = protocol_ext_container_empty_l;

// GNB-RxTxTimeDiff-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct gnb_rx_tx_time_diff_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { extended_add_path_list, trpteg_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    extended_add_path_list_l&       extended_add_path_list();
    trpteg_info_c&                  trpteg_info();
    const extended_add_path_list_l& extended_add_path_list() const;
    const trpteg_info_c&            trpteg_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNBRxTxTimeDiffMeas-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using gnb_rx_tx_time_diff_meas_ext_ies_o = protocol_ies_empty_o;

using lcs_to_gcs_translation_item_ext_ies_container = protocol_ext_container_empty_l;

// LoS-NLoSIndicatorHard ::= ENUMERATED
struct lo_s_n_lo_si_ndicator_hard_opts {
  enum options { nlos, los, nulltype } value;

  const char* to_string() const;
};
using lo_s_n_lo_si_ndicator_hard_e = enumerated<lo_s_n_lo_si_ndicator_hard_opts>;

using nr_prs_beam_info_item_ext_ies_container = protocol_ext_container_empty_l;

using on_demand_prs_info_ext_ies_container = protocol_ext_container_empty_l;

// PosResourceSetTypeAperiodic ::= SEQUENCE
struct pos_res_set_type_aperiodic_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  uint8_t                                      srs_res_trigger = 1;
  pos_res_set_type_aperiodic_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosResourceSetTypePeriodic ::= SEQUENCE
struct pos_res_set_type_periodic_s {
  struct posperiodic_set_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using posperiodic_set_e_ = enumerated<posperiodic_set_opts, true>;

  // member variables
  bool                                        ext             = false;
  bool                                        ie_exts_present = false;
  posperiodic_set_e_                          posperiodic_set;
  pos_res_set_type_periodic_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosResourceSetTypeSemi-persistent ::= SEQUENCE
struct pos_res_set_type_semi_persistent_s {
  struct possemi_persistent_set_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using possemi_persistent_set_e_ = enumerated<possemi_persistent_set_opts, true>;

  // member variables
  bool                                               ext             = false;
  bool                                               ie_exts_present = false;
  possemi_persistent_set_e_                          possemi_persistent_set;
  pos_res_set_type_semi_persistent_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSInformationPos ::= SEQUENCE
struct pr_si_nformation_pos_s {
  bool                                   ext                    = false;
  bool                                   prs_res_id_pos_present = false;
  bool                                   ie_exts_present        = false;
  uint16_t                               prs_id_pos             = 0;
  uint8_t                                prs_res_set_id_pos     = 0;
  uint8_t                                prs_res_id_pos         = 0;
  pr_si_nformation_pos_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSAngleItem ::= SEQUENCE
struct prs_angle_item_s {
  bool                                               ext                           = false;
  bool                                               nr_prs_azimuth_fine_present   = false;
  bool                                               nr_prs_elevation_present      = false;
  bool                                               nr_prs_elevation_fine_present = false;
  uint16_t                                           nr_prs_azimuth                = 0;
  uint8_t                                            nr_prs_azimuth_fine           = 0;
  uint8_t                                            nr_prs_elevation              = 0;
  uint8_t                                            nr_prs_elevation_fine         = 0;
  protocol_ext_container_l<prs_angle_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSMuting ::= SEQUENCE
struct prs_muting_s {
  bool                         ext                        = false;
  bool                         prs_muting_option1_present = false;
  bool                         prs_muting_option2_present = false;
  bool                         ie_exts_present            = false;
  prs_muting_option1_s         prs_muting_option1;
  prs_muting_option2_s         prs_muting_option2;
  prs_muting_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSResource-List ::= SEQUENCE (SIZE (1..64)) OF PRSResource-Item
using prs_res_list_l = dyn_array<prs_res_item_s>;

using prs_res_set_item_ext_ies_container = protocol_ext_container_empty_l;

// ReferencePoint ::= CHOICE
struct ref_point_c {
  struct types_opts {
    enum options { relative_coordinate_id, ref_point_coordinate, ref_point_coordinate_ha, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ref_point_c() = default;
  ref_point_c(const ref_point_c& other);
  ref_point_c& operator=(const ref_point_c& other);
  ~ref_point_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& relative_coordinate_id()
  {
    assert_choice_type(types::relative_coordinate_id, type_, "ReferencePoint");
    return c.get<uint16_t>();
  }
  ng_ran_access_point_position_s& ref_point_coordinate()
  {
    assert_choice_type(types::ref_point_coordinate, type_, "ReferencePoint");
    return c.get<ng_ran_access_point_position_s>();
  }
  ngran_high_accuracy_access_point_position_s& ref_point_coordinate_ha()
  {
    assert_choice_type(types::ref_point_coordinate_ha, type_, "ReferencePoint");
    return c.get<ngran_high_accuracy_access_point_position_s>();
  }
  protocol_ie_single_container_s<ref_point_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ReferencePoint");
    return c.get<protocol_ie_single_container_s<ref_point_ext_ies_o>>();
  }
  const uint16_t& relative_coordinate_id() const
  {
    assert_choice_type(types::relative_coordinate_id, type_, "ReferencePoint");
    return c.get<uint16_t>();
  }
  const ng_ran_access_point_position_s& ref_point_coordinate() const
  {
    assert_choice_type(types::ref_point_coordinate, type_, "ReferencePoint");
    return c.get<ng_ran_access_point_position_s>();
  }
  const ngran_high_accuracy_access_point_position_s& ref_point_coordinate_ha() const
  {
    assert_choice_type(types::ref_point_coordinate_ha, type_, "ReferencePoint");
    return c.get<ngran_high_accuracy_access_point_position_s>();
  }
  const protocol_ie_single_container_s<ref_point_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ReferencePoint");
    return c.get<protocol_ie_single_container_s<ref_point_ext_ies_o>>();
  }
  uint16_t&                                            set_relative_coordinate_id();
  ng_ran_access_point_position_s&                      set_ref_point_coordinate();
  ngran_high_accuracy_access_point_position_s&         set_ref_point_coordinate_ha();
  protocol_ie_single_container_s<ref_point_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<ng_ran_access_point_position_s,
                  ngran_high_accuracy_access_point_position_s,
                  protocol_ie_single_container_s<ref_point_ext_ies_o>>
      c;

  void destroy_();
};

// ReferenceSignal ::= CHOICE
struct ref_sig_c {
  struct types_opts {
    enum options { nzp_csi_rs, ssb, srs, positioning_srs, dl_prs, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ref_sig_c() = default;
  ref_sig_c(const ref_sig_c& other);
  ref_sig_c& operator=(const ref_sig_c& other);
  ~ref_sig_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& nzp_csi_rs()
  {
    assert_choice_type(types::nzp_csi_rs, type_, "ReferenceSignal");
    return c.get<uint8_t>();
  }
  ssb_s& ssb()
  {
    assert_choice_type(types::ssb, type_, "ReferenceSignal");
    return c.get<ssb_s>();
  }
  uint8_t& srs()
  {
    assert_choice_type(types::srs, type_, "ReferenceSignal");
    return c.get<uint8_t>();
  }
  uint8_t& positioning_srs()
  {
    assert_choice_type(types::positioning_srs, type_, "ReferenceSignal");
    return c.get<uint8_t>();
  }
  dl_prs_s& dl_prs()
  {
    assert_choice_type(types::dl_prs, type_, "ReferenceSignal");
    return c.get<dl_prs_s>();
  }
  protocol_ie_single_container_s<ref_sig_ext_ie_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ReferenceSignal");
    return c.get<protocol_ie_single_container_s<ref_sig_ext_ie_o>>();
  }
  const uint8_t& nzp_csi_rs() const
  {
    assert_choice_type(types::nzp_csi_rs, type_, "ReferenceSignal");
    return c.get<uint8_t>();
  }
  const ssb_s& ssb() const
  {
    assert_choice_type(types::ssb, type_, "ReferenceSignal");
    return c.get<ssb_s>();
  }
  const uint8_t& srs() const
  {
    assert_choice_type(types::srs, type_, "ReferenceSignal");
    return c.get<uint8_t>();
  }
  const uint8_t& positioning_srs() const
  {
    assert_choice_type(types::positioning_srs, type_, "ReferenceSignal");
    return c.get<uint8_t>();
  }
  const dl_prs_s& dl_prs() const
  {
    assert_choice_type(types::dl_prs, type_, "ReferenceSignal");
    return c.get<dl_prs_s>();
  }
  const protocol_ie_single_container_s<ref_sig_ext_ie_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ReferenceSignal");
    return c.get<protocol_ie_single_container_s<ref_sig_ext_ie_o>>();
  }
  uint8_t&                                          set_nzp_csi_rs();
  ssb_s&                                            set_ssb();
  uint8_t&                                          set_srs();
  uint8_t&                                          set_positioning_srs();
  dl_prs_s&                                         set_dl_prs();
  protocol_ie_single_container_s<ref_sig_ext_ie_o>& set_choice_ext();

private:
  types                                                                              type_;
  choice_buffer_t<dl_prs_s, protocol_ie_single_container_s<ref_sig_ext_ie_o>, ssb_s> c;

  void destroy_();
};

// ResourceSetTypeAperiodic ::= SEQUENCE
struct res_set_type_aperiodic_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  srs_res_trigger = 1;
  uint8_t                                  slotoffset      = 0;
  res_set_type_aperiodic_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceSetTypePeriodic ::= SEQUENCE
struct res_set_type_periodic_s {
  struct periodic_set_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using periodic_set_e_ = enumerated<periodic_set_opts, true>;

  // member variables
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  periodic_set_e_                         periodic_set;
  res_set_type_periodic_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceSetTypeSemi-persistent ::= SEQUENCE
struct res_set_type_semi_persistent_s {
  struct semi_persistent_set_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using semi_persistent_set_e_ = enumerated<semi_persistent_set_opts, true>;

  // member variables
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  semi_persistent_set_e_                         semi_persistent_set;
  res_set_type_semi_persistent_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceTypeAperiodicPos ::= SEQUENCE
struct res_type_aperiodic_pos_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  uint8_t                                  slot_offset     = 0;
  res_type_aperiodic_pos_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceTypeAperiodic ::= SEQUENCE
struct res_type_aperiodic_s {
  struct aperiodic_res_type_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using aperiodic_res_type_e_ = enumerated<aperiodic_res_type_opts, true>;

  // member variables
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  aperiodic_res_type_e_                aperiodic_res_type;
  res_type_aperiodic_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceTypePeriodicPos ::= SEQUENCE
struct res_type_periodic_pos_s {
  struct periodicity_opts {
    enum options {
      slot1,
      slot2,
      slot4,
      slot5,
      slot8,
      slot10,
      slot16,
      slot20,
      slot32,
      slot40,
      slot64,
      slot80,
      slot160,
      slot320,
      slot640,
      slot1280,
      slot2560,
      slot5120,
      slot10240,
      slot40960,
      slot81920,
      // ...
      slot128,
      slot256,
      slot512,
      slot20480,
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts, true, 4>;

  // member variables
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  periodicity_e_                          periodicity;
  uint32_t                                offset = 0;
  res_type_periodic_pos_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceTypePeriodic ::= SEQUENCE
struct res_type_periodic_s {
  struct periodicity_opts {
    enum options {
      slot1,
      slot2,
      slot4,
      slot5,
      slot8,
      slot10,
      slot16,
      slot20,
      slot32,
      slot40,
      slot64,
      slot80,
      slot160,
      slot320,
      slot640,
      slot1280,
      slot2560,
      // ...
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts, true>;

  // member variables
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  periodicity_e_                      periodicity;
  uint16_t                            offset = 0;
  res_type_periodic_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceTypeSemi-persistentPos ::= SEQUENCE
struct res_type_semi_persistent_pos_s {
  struct periodicity_opts {
    enum options {
      slot1,
      slot2,
      slot4,
      slot5,
      slot8,
      slot10,
      slot16,
      slot20,
      slot32,
      slot40,
      slot64,
      slot80,
      slot160,
      slot320,
      slot640,
      slot1280,
      slot2560,
      slot5120,
      slot10240,
      slot40960,
      slot81920,
      // ...
      slot128,
      slot256,
      slot512,
      slot20480,
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts, true, 4>;

  // member variables
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  periodicity_e_                                 periodicity;
  uint32_t                                       offset = 0;
  res_type_semi_persistent_pos_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResourceTypeSemi-persistent ::= SEQUENCE
struct res_type_semi_persistent_s {
  struct periodicity_opts {
    enum options {
      slot1,
      slot2,
      slot4,
      slot5,
      slot8,
      slot10,
      slot16,
      slot20,
      slot32,
      slot40,
      slot64,
      slot80,
      slot160,
      slot320,
      slot640,
      slot1280,
      slot2560,
      // ...
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts, true>;

  // member variables
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  periodicity_e_                             periodicity;
  uint16_t                                   offset = 0;
  res_type_semi_persistent_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultCSI-RSRP-PerCSI-RS-Item ::= SEQUENCE
struct result_csi_rsrp_per_csi_rs_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           csi_rs_idx      = 0;
  uint8_t                                           value_csi_rsrp  = 0;
  result_csi_rsrp_per_csi_rs_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultCSI-RSRQ-PerCSI-RS-Item ::= SEQUENCE
struct result_csi_rsrq_per_csi_rs_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  uint8_t                                           csi_rs_idx      = 0;
  uint8_t                                           value_csi_rsrq  = 0;
  result_csi_rsrq_per_csi_rs_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultSS-RSRP-PerSSB-Item ::= SEQUENCE
struct result_ss_rsrp_per_ssb_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint8_t                                       ssb_idx         = 0;
  uint8_t                                       value_ss_rsrp   = 0;
  result_ss_rsrp_per_ssb_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultSS-RSRQ-PerSSB-Item ::= SEQUENCE
struct result_ss_rsrq_per_ssb_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint8_t                                       ssb_idx         = 0;
  uint8_t                                       value_ss_rsrq   = 0;
  result_ss_rsrq_per_ssb_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using spatial_relation_per_srs_res_item_ext_ies_container = protocol_ext_container_empty_l;

using spatial_relationfor_res_id_item_ext_ies_container = protocol_ext_container_empty_l;

// SRSResourceTypeChoice ::= CHOICE
struct srs_res_type_choice_c {
  struct types_opts {
    enum options { srs_res_info, pos_srs_res_info, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  srs_res_type_choice_c() = default;
  srs_res_type_choice_c(const srs_res_type_choice_c& other);
  srs_res_type_choice_c& operator=(const srs_res_type_choice_c& other);
  ~srs_res_type_choice_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  srs_info_s& srs_res_info()
  {
    assert_choice_type(types::srs_res_info, type_, "SRSResourceTypeChoice");
    return c.get<srs_info_s>();
  }
  pos_srs_info_s& pos_srs_res_info()
  {
    assert_choice_type(types::pos_srs_res_info, type_, "SRSResourceTypeChoice");
    return c.get<pos_srs_info_s>();
  }
  const srs_info_s& srs_res_info() const
  {
    assert_choice_type(types::srs_res_info, type_, "SRSResourceTypeChoice");
    return c.get<srs_info_s>();
  }
  const pos_srs_info_s& pos_srs_res_info() const
  {
    assert_choice_type(types::pos_srs_res_info, type_, "SRSResourceTypeChoice");
    return c.get<pos_srs_info_s>();
  }
  srs_info_s&     set_srs_res_info();
  pos_srs_info_s& set_pos_srs_res_info();

private:
  types                                       type_;
  choice_buffer_t<pos_srs_info_s, srs_info_s> c;

  void destroy_();
};

// SSBBurstPosition ::= CHOICE
struct ssb_burst_position_c {
  struct types_opts {
    enum options { short_bitmap, medium_bitmap, long_bitmap, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ssb_burst_position_c() = default;
  ssb_burst_position_c(const ssb_burst_position_c& other);
  ssb_burst_position_c& operator=(const ssb_burst_position_c& other);
  ~ssb_burst_position_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<4, false, true>& short_bitmap()
  {
    assert_choice_type(types::short_bitmap, type_, "SSBBurstPosition");
    return c.get<fixed_bitstring<4, false, true>>();
  }
  fixed_bitstring<8, false, true>& medium_bitmap()
  {
    assert_choice_type(types::medium_bitmap, type_, "SSBBurstPosition");
    return c.get<fixed_bitstring<8, false, true>>();
  }
  fixed_bitstring<64, false, true>& long_bitmap()
  {
    assert_choice_type(types::long_bitmap, type_, "SSBBurstPosition");
    return c.get<fixed_bitstring<64, false, true>>();
  }
  protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "SSBBurstPosition");
    return c.get<protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>>();
  }
  const fixed_bitstring<4, false, true>& short_bitmap() const
  {
    assert_choice_type(types::short_bitmap, type_, "SSBBurstPosition");
    return c.get<fixed_bitstring<4, false, true>>();
  }
  const fixed_bitstring<8, false, true>& medium_bitmap() const
  {
    assert_choice_type(types::medium_bitmap, type_, "SSBBurstPosition");
    return c.get<fixed_bitstring<8, false, true>>();
  }
  const fixed_bitstring<64, false, true>& long_bitmap() const
  {
    assert_choice_type(types::long_bitmap, type_, "SSBBurstPosition");
    return c.get<fixed_bitstring<64, false, true>>();
  }
  const protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "SSBBurstPosition");
    return c.get<protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>>();
  }
  fixed_bitstring<4, false, true>&                              set_short_bitmap();
  fixed_bitstring<8, false, true>&                              set_medium_bitmap();
  fixed_bitstring<64, false, true>&                             set_long_bitmap();
  protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                           type_;
  choice_buffer_t<fixed_bitstring<64, false, true>, protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>> c;

  void destroy_();
};

using tf_cfg_ext_ies_container = protocol_ext_container_empty_l;

using trp_beam_ant_info_ext_ies_container = protocol_ext_container_empty_l;

// TRPPositionDefinitionType-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using trp_position_definition_type_ext_ies_o = protocol_ies_empty_o;

// TRPPositionDirectAccuracy ::= CHOICE
struct trp_position_direct_accuracy_c {
  struct types_opts {
    enum options { trp_position, trph_aposition, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  trp_position_direct_accuracy_c() = default;
  trp_position_direct_accuracy_c(const trp_position_direct_accuracy_c& other);
  trp_position_direct_accuracy_c& operator=(const trp_position_direct_accuracy_c& other);
  ~trp_position_direct_accuracy_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ng_ran_access_point_position_s& trp_position()
  {
    assert_choice_type(types::trp_position, type_, "TRPPositionDirectAccuracy");
    return c.get<ng_ran_access_point_position_s>();
  }
  ngran_high_accuracy_access_point_position_s& trph_aposition()
  {
    assert_choice_type(types::trph_aposition, type_, "TRPPositionDirectAccuracy");
    return c.get<ngran_high_accuracy_access_point_position_s>();
  }
  protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TRPPositionDirectAccuracy");
    return c.get<protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>>();
  }
  const ng_ran_access_point_position_s& trp_position() const
  {
    assert_choice_type(types::trp_position, type_, "TRPPositionDirectAccuracy");
    return c.get<ng_ran_access_point_position_s>();
  }
  const ngran_high_accuracy_access_point_position_s& trph_aposition() const
  {
    assert_choice_type(types::trph_aposition, type_, "TRPPositionDirectAccuracy");
    return c.get<ngran_high_accuracy_access_point_position_s>();
  }
  const protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TRPPositionDirectAccuracy");
    return c.get<protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>>();
  }
  ng_ran_access_point_position_s&                                         set_trp_position();
  ngran_high_accuracy_access_point_position_s&                            set_trph_aposition();
  protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<ng_ran_access_point_position_s,
                  ngran_high_accuracy_access_point_position_s,
                  protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>>
      c;

  void destroy_();
};

using trp_position_direct_ext_ies_container = protocol_ext_container_empty_l;

using trp_position_refd_ext_ies_container = protocol_ext_container_empty_l;

// TRPReferencePointType ::= CHOICE
struct trp_ref_point_type_c {
  struct types_opts {
    enum options { trp_position_relative_geodetic, trp_position_relative_cartesian, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  trp_ref_point_type_c() = default;
  trp_ref_point_type_c(const trp_ref_point_type_c& other);
  trp_ref_point_type_c& operator=(const trp_ref_point_type_c& other);
  ~trp_ref_point_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  relative_geodetic_location_s& trp_position_relative_geodetic()
  {
    assert_choice_type(types::trp_position_relative_geodetic, type_, "TRPReferencePointType");
    return c.get<relative_geodetic_location_s>();
  }
  relative_cartesian_location_s& trp_position_relative_cartesian()
  {
    assert_choice_type(types::trp_position_relative_cartesian, type_, "TRPReferencePointType");
    return c.get<relative_cartesian_location_s>();
  }
  protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TRPReferencePointType");
    return c.get<protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>>();
  }
  const relative_geodetic_location_s& trp_position_relative_geodetic() const
  {
    assert_choice_type(types::trp_position_relative_geodetic, type_, "TRPReferencePointType");
    return c.get<relative_geodetic_location_s>();
  }
  const relative_cartesian_location_s& trp_position_relative_cartesian() const
  {
    assert_choice_type(types::trp_position_relative_cartesian, type_, "TRPReferencePointType");
    return c.get<relative_cartesian_location_s>();
  }
  const protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TRPReferencePointType");
    return c.get<protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>>();
  }
  relative_geodetic_location_s&                                 set_trp_position_relative_geodetic();
  relative_cartesian_location_s&                                set_trp_position_relative_cartesian();
  protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>,
                  relative_cartesian_location_s,
                  relative_geodetic_location_s>
      c;

  void destroy_();
};

// TRPTEGItem ::= SEQUENCE
struct trpteg_item_s {
  using dl_prs_res_id_list_l_ = dyn_array<dl_prs_res_id_item_s>;

  // member variables
  bool                          ext             = false;
  bool                          ie_exts_present = false;
  trp_tx_teg_info_s             trp_tx_teg_info;
  uint8_t                       dl_prs_res_set_id = 0;
  dl_prs_res_id_list_l_         dl_prs_res_id_list;
  trpteg_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULRTOAMeas-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using ul_rtoa_meas_ext_ies_o = protocol_ies_empty_o;

// UL-RTOAMeasurement-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct ul_rtoameas_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { extended_add_path_list, trp_rx_teg_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    extended_add_path_list_l&       extended_add_path_list();
    trp_rx_teg_info_s&              trp_rx_teg_info();
    const extended_add_path_list_l& extended_add_path_list() const;
    const trp_rx_teg_info_s&        trp_rx_teg_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// AdditionalPathListItem ::= SEQUENCE
struct add_path_list_item_s {
  bool                                 ext                  = false;
  bool                                 path_quality_present = false;
  bool                                 ie_exts_present      = false;
  relative_path_delay_c                relative_time_of_path;
  trp_meas_quality_c                   path_quality;
  add_path_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using cgi_eutra_ext_ies_container = protocol_ext_container_empty_l;

// CGI-NR ::= SEQUENCE
struct cgi_nr_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nr_cell_id;
  cgi_nr_ext_ies_container         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using dl_prs_res_coordinates_ext_ies_container = protocol_ext_container_empty_l;

// DLPRSResourceSetARP ::= SEQUENCE
struct dl_prs_res_set_arp_s {
  using listof_dl_prs_res_arp_l_ = dyn_array<dl_prs_res_arp_s>;

  // member variables
  bool                                 ext               = false;
  bool                                 ie_exts_present   = false;
  uint8_t                              dl_prs_res_set_id = 0;
  dl_prs_res_set_arp_location_c        dl_prs_res_set_arp_location;
  listof_dl_prs_res_arp_l_             listof_dl_prs_res_arp;
  dl_prs_res_set_arp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Expected-UL-AoA ::= SEQUENCE
struct expected_ul_ao_a_s {
  bool                               ext                          = false;
  bool                               expected_zenith_ao_a_present = false;
  bool                               ie_exts_present              = false;
  expected_azimuth_ao_a_s            expected_azimuth_ao_a;
  expected_zenith_ao_a_s             expected_zenith_ao_a;
  expected_ul_ao_a_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Expected-ZoA-only ::= SEQUENCE
struct expected_zo_a_only_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  expected_zenith_ao_a_s               expected_zo_a_only;
  expected_zo_a_only_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GeographicalCoordinates-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct geographical_coordinates_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { arp_location_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::arp_location_info; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    arp_location_info_l&       arp_location_info() { return c; }
    const arp_location_info_l& arp_location_info() const { return c; }

  private:
    arp_location_info_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// LCS-to-GCS-TranslationItem ::= SEQUENCE
struct lcs_to_gcs_translation_item_s {
  bool                                          ext                = false;
  bool                                          alpha_fine_present = false;
  bool                                          beta_fine_present  = false;
  bool                                          gamma_fine_present = false;
  bool                                          ie_exts_present    = false;
  uint16_t                                      alpha              = 0;
  uint8_t                                       alpha_fine         = 0;
  uint16_t                                      beta               = 0;
  uint8_t                                       beta_fine          = 0;
  uint16_t                                      gamma              = 0;
  uint8_t                                       gamma_fine         = 0;
  lcs_to_gcs_translation_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoS-NLoSInformation ::= CHOICE
struct lo_s_n_lo_si_nformation_c {
  struct types_opts {
    enum options { lo_s_n_lo_si_ndicator_soft, lo_s_n_lo_si_ndicator_hard, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  lo_s_n_lo_si_nformation_c() = default;
  lo_s_n_lo_si_nformation_c(const lo_s_n_lo_si_nformation_c& other);
  lo_s_n_lo_si_nformation_c& operator=(const lo_s_n_lo_si_nformation_c& other);
  ~lo_s_n_lo_si_nformation_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& lo_s_n_lo_si_ndicator_soft()
  {
    assert_choice_type(types::lo_s_n_lo_si_ndicator_soft, type_, "LoS-NLoSInformation");
    return c.get<uint8_t>();
  }
  lo_s_n_lo_si_ndicator_hard_e& lo_s_n_lo_si_ndicator_hard()
  {
    assert_choice_type(types::lo_s_n_lo_si_ndicator_hard, type_, "LoS-NLoSInformation");
    return c.get<lo_s_n_lo_si_ndicator_hard_e>();
  }
  protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "LoS-NLoSInformation");
    return c.get<protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>>();
  }
  const uint8_t& lo_s_n_lo_si_ndicator_soft() const
  {
    assert_choice_type(types::lo_s_n_lo_si_ndicator_soft, type_, "LoS-NLoSInformation");
    return c.get<uint8_t>();
  }
  const lo_s_n_lo_si_ndicator_hard_e& lo_s_n_lo_si_ndicator_hard() const
  {
    assert_choice_type(types::lo_s_n_lo_si_ndicator_hard, type_, "LoS-NLoSInformation");
    return c.get<lo_s_n_lo_si_ndicator_hard_e>();
  }
  const protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "LoS-NLoSInformation");
    return c.get<protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>>();
  }
  uint8_t&                                                           set_lo_s_n_lo_si_ndicator_soft();
  lo_s_n_lo_si_ndicator_hard_e&                                      set_lo_s_n_lo_si_ndicator_hard();
  protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>& set_choice_ext();

private:
  types                                                                              type_;
  choice_buffer_t<protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>> c;

  void destroy_();
};

using nr_prs_beam_info_ies_container = protocol_ext_container_empty_l;

// NR-PRS-Beam-InformationItem ::= SEQUENCE
struct nr_prs_beam_info_item_s {
  using prs_angle_l_ = dyn_array<prs_angle_item_s>;

  // member variables
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  uint8_t                                 pr_sres_set_id  = 0;
  prs_angle_l_                            prs_angle;
  nr_prs_beam_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OnDemandPRS-Info ::= SEQUENCE
struct on_demand_prs_info_s {
  bool                                 ext                                        = false;
  bool                                 allowed_res_set_periodicity_values_present = false;
  bool                                 allowed_prs_bw_values_present              = false;
  bool                                 allowed_res_repeat_factor_values_present   = false;
  bool                                 allowed_res_nof_symbols_values_present     = false;
  bool                                 allowed_comb_size_values_present           = false;
  bool                                 ie_exts_present                            = false;
  fixed_bitstring<16, false, true>     on_demand_prs_request_allowed;
  fixed_bitstring<24, false, true>     allowed_res_set_periodicity_values;
  fixed_bitstring<64, false, true>     allowed_prs_bw_values;
  fixed_bitstring<8, false, true>      allowed_res_repeat_factor_values;
  fixed_bitstring<8, false, true>      allowed_res_nof_symbols_values;
  fixed_bitstring<8, false, true>      allowed_comb_size_values;
  on_demand_prs_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosResourceSetType ::= CHOICE
struct pos_res_set_type_c {
  struct types_opts {
    enum options { periodic, semi_persistent, aperiodic, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  pos_res_set_type_c() = default;
  pos_res_set_type_c(const pos_res_set_type_c& other);
  pos_res_set_type_c& operator=(const pos_res_set_type_c& other);
  ~pos_res_set_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  pos_res_set_type_periodic_s& periodic()
  {
    assert_choice_type(types::periodic, type_, "PosResourceSetType");
    return c.get<pos_res_set_type_periodic_s>();
  }
  pos_res_set_type_semi_persistent_s& semi_persistent()
  {
    assert_choice_type(types::semi_persistent, type_, "PosResourceSetType");
    return c.get<pos_res_set_type_semi_persistent_s>();
  }
  pos_res_set_type_aperiodic_s& aperiodic()
  {
    assert_choice_type(types::aperiodic, type_, "PosResourceSetType");
    return c.get<pos_res_set_type_aperiodic_s>();
  }
  protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "PosResourceSetType");
    return c.get<protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>>();
  }
  const pos_res_set_type_periodic_s& periodic() const
  {
    assert_choice_type(types::periodic, type_, "PosResourceSetType");
    return c.get<pos_res_set_type_periodic_s>();
  }
  const pos_res_set_type_semi_persistent_s& semi_persistent() const
  {
    assert_choice_type(types::semi_persistent, type_, "PosResourceSetType");
    return c.get<pos_res_set_type_semi_persistent_s>();
  }
  const pos_res_set_type_aperiodic_s& aperiodic() const
  {
    assert_choice_type(types::aperiodic, type_, "PosResourceSetType");
    return c.get<pos_res_set_type_aperiodic_s>();
  }
  const protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "PosResourceSetType");
    return c.get<protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>>();
  }
  pos_res_set_type_periodic_s&                                set_periodic();
  pos_res_set_type_semi_persistent_s&                         set_semi_persistent();
  pos_res_set_type_aperiodic_s&                               set_aperiodic();
  protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<pos_res_set_type_aperiodic_s,
                  pos_res_set_type_periodic_s,
                  pos_res_set_type_semi_persistent_s,
                  protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>>
      c;

  void destroy_();
};

using pos_sib_segments_ext_ies_container = protocol_ext_container_empty_l;

// PosSRSResourceIDPerSet-List ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..63)
using pos_srs_res_id_per_set_list_l = bounded_array<uint8_t, 16>;

using pos_srs_res_item_ext_ies_container = protocol_ext_container_empty_l;

using pos_srs_res_set_item_ext_ies_container = protocol_ext_container_empty_l;

// PRSResourceSet-Item ::= SEQUENCE
struct prs_res_set_item_s {
  struct subcarrier_spacing_opts {
    enum options { khz15, khz30, khz60, khz120, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using subcarrier_spacing_e_ = enumerated<subcarrier_spacing_opts, true>;
  struct comb_size_opts {
    enum options { n2, n4, n6, n12, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using comb_size_e_ = enumerated<comb_size_opts, true>;
  struct cp_type_opts {
    enum options { normal, extended, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using cp_type_e_ = enumerated<cp_type_opts, true>;
  struct res_set_periodicity_opts {
    enum options {
      n4,
      n5,
      n8,
      n10,
      n16,
      n20,
      n32,
      n40,
      n64,
      n80,
      n160,
      n320,
      n640,
      n1280,
      n2560,
      n5120,
      n10240,
      n20480,
      n40960,
      n81920,
      // ...
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using res_set_periodicity_e_ = enumerated<res_set_periodicity_opts, true>;
  struct res_repeat_factor_opts {
    enum options { rf1, rf2, rf4, rf6, rf8, rf16, rf32, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_repeat_factor_e_ = enumerated<res_repeat_factor_opts, true>;
  struct res_time_gap_opts {
    enum options { tg1, tg2, tg4, tg8, tg16, tg32, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_time_gap_e_ = enumerated<res_time_gap_opts, true>;
  struct res_numof_symbols_opts {
    enum options { n2, n4, n6, n12, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_numof_symbols_e_ = enumerated<res_numof_symbols_opts, true>;

  // member variables
  bool                               ext                = false;
  bool                               prs_muting_present = false;
  bool                               ie_exts_present    = false;
  uint8_t                            prs_res_set_id     = 0;
  subcarrier_spacing_e_              subcarrier_spacing;
  uint8_t                            pr_sbw    = 1;
  uint16_t                           start_prb = 0;
  uint32_t                           point_a   = 0;
  comb_size_e_                       comb_size;
  cp_type_e_                         cp_type;
  res_set_periodicity_e_             res_set_periodicity;
  uint32_t                           res_set_slot_offset = 0;
  res_repeat_factor_e_               res_repeat_factor;
  res_time_gap_e_                    res_time_gap;
  res_numof_symbols_e_               res_numof_symbols;
  prs_muting_s                       prs_muting;
  int8_t                             prs_res_tx_pwr = -60;
  prs_res_list_l                     prs_res_list;
  prs_res_set_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using prs_tx_off_ind_per_res_item_ext_ies_container = protocol_ext_container_empty_l;

using requested_dl_prs_res_item_ext_ies_container = protocol_ext_container_empty_l;

// ResourceSetType ::= CHOICE
struct res_set_type_c {
  struct types_opts {
    enum options { periodic, semi_persistent, aperiodic, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  res_set_type_c() = default;
  res_set_type_c(const res_set_type_c& other);
  res_set_type_c& operator=(const res_set_type_c& other);
  ~res_set_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  res_set_type_periodic_s& periodic()
  {
    assert_choice_type(types::periodic, type_, "ResourceSetType");
    return c.get<res_set_type_periodic_s>();
  }
  res_set_type_semi_persistent_s& semi_persistent()
  {
    assert_choice_type(types::semi_persistent, type_, "ResourceSetType");
    return c.get<res_set_type_semi_persistent_s>();
  }
  res_set_type_aperiodic_s& aperiodic()
  {
    assert_choice_type(types::aperiodic, type_, "ResourceSetType");
    return c.get<res_set_type_aperiodic_s>();
  }
  protocol_ie_single_container_s<res_set_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ResourceSetType");
    return c.get<protocol_ie_single_container_s<res_set_type_ext_ies_o>>();
  }
  const res_set_type_periodic_s& periodic() const
  {
    assert_choice_type(types::periodic, type_, "ResourceSetType");
    return c.get<res_set_type_periodic_s>();
  }
  const res_set_type_semi_persistent_s& semi_persistent() const
  {
    assert_choice_type(types::semi_persistent, type_, "ResourceSetType");
    return c.get<res_set_type_semi_persistent_s>();
  }
  const res_set_type_aperiodic_s& aperiodic() const
  {
    assert_choice_type(types::aperiodic, type_, "ResourceSetType");
    return c.get<res_set_type_aperiodic_s>();
  }
  const protocol_ie_single_container_s<res_set_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ResourceSetType");
    return c.get<protocol_ie_single_container_s<res_set_type_ext_ies_o>>();
  }
  res_set_type_periodic_s&                                set_periodic();
  res_set_type_semi_persistent_s&                         set_semi_persistent();
  res_set_type_aperiodic_s&                               set_aperiodic();
  protocol_ie_single_container_s<res_set_type_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<res_set_type_ext_ies_o>,
                  res_set_type_aperiodic_s,
                  res_set_type_periodic_s,
                  res_set_type_semi_persistent_s>
      c;

  void destroy_();
};

// ResourceType ::= CHOICE
struct res_type_c {
  struct types_opts {
    enum options { periodic, semi_persistent, aperiodic, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  res_type_c() = default;
  res_type_c(const res_type_c& other);
  res_type_c& operator=(const res_type_c& other);
  ~res_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  res_type_periodic_s& periodic()
  {
    assert_choice_type(types::periodic, type_, "ResourceType");
    return c.get<res_type_periodic_s>();
  }
  res_type_semi_persistent_s& semi_persistent()
  {
    assert_choice_type(types::semi_persistent, type_, "ResourceType");
    return c.get<res_type_semi_persistent_s>();
  }
  res_type_aperiodic_s& aperiodic()
  {
    assert_choice_type(types::aperiodic, type_, "ResourceType");
    return c.get<res_type_aperiodic_s>();
  }
  protocol_ie_single_container_s<res_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ResourceType");
    return c.get<protocol_ie_single_container_s<res_type_ext_ies_o>>();
  }
  const res_type_periodic_s& periodic() const
  {
    assert_choice_type(types::periodic, type_, "ResourceType");
    return c.get<res_type_periodic_s>();
  }
  const res_type_semi_persistent_s& semi_persistent() const
  {
    assert_choice_type(types::semi_persistent, type_, "ResourceType");
    return c.get<res_type_semi_persistent_s>();
  }
  const res_type_aperiodic_s& aperiodic() const
  {
    assert_choice_type(types::aperiodic, type_, "ResourceType");
    return c.get<res_type_aperiodic_s>();
  }
  const protocol_ie_single_container_s<res_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ResourceType");
    return c.get<protocol_ie_single_container_s<res_type_ext_ies_o>>();
  }
  res_type_periodic_s&                                set_periodic();
  res_type_semi_persistent_s&                         set_semi_persistent();
  res_type_aperiodic_s&                               set_aperiodic();
  protocol_ie_single_container_s<res_type_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<res_type_ext_ies_o>,
                  res_type_aperiodic_s,
                  res_type_periodic_s,
                  res_type_semi_persistent_s>
      c;

  void destroy_();
};

// ResourceTypePos ::= CHOICE
struct res_type_pos_c {
  struct types_opts {
    enum options { periodic, semi_persistent, aperiodic, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  res_type_pos_c() = default;
  res_type_pos_c(const res_type_pos_c& other);
  res_type_pos_c& operator=(const res_type_pos_c& other);
  ~res_type_pos_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  res_type_periodic_pos_s& periodic()
  {
    assert_choice_type(types::periodic, type_, "ResourceTypePos");
    return c.get<res_type_periodic_pos_s>();
  }
  res_type_semi_persistent_pos_s& semi_persistent()
  {
    assert_choice_type(types::semi_persistent, type_, "ResourceTypePos");
    return c.get<res_type_semi_persistent_pos_s>();
  }
  res_type_aperiodic_pos_s& aperiodic()
  {
    assert_choice_type(types::aperiodic, type_, "ResourceTypePos");
    return c.get<res_type_aperiodic_pos_s>();
  }
  protocol_ie_single_container_s<res_type_pos_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ResourceTypePos");
    return c.get<protocol_ie_single_container_s<res_type_pos_ext_ies_o>>();
  }
  const res_type_periodic_pos_s& periodic() const
  {
    assert_choice_type(types::periodic, type_, "ResourceTypePos");
    return c.get<res_type_periodic_pos_s>();
  }
  const res_type_semi_persistent_pos_s& semi_persistent() const
  {
    assert_choice_type(types::semi_persistent, type_, "ResourceTypePos");
    return c.get<res_type_semi_persistent_pos_s>();
  }
  const res_type_aperiodic_pos_s& aperiodic() const
  {
    assert_choice_type(types::aperiodic, type_, "ResourceTypePos");
    return c.get<res_type_aperiodic_pos_s>();
  }
  const protocol_ie_single_container_s<res_type_pos_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ResourceTypePos");
    return c.get<protocol_ie_single_container_s<res_type_pos_ext_ies_o>>();
  }
  res_type_periodic_pos_s&                                set_periodic();
  res_type_semi_persistent_pos_s&                         set_semi_persistent();
  res_type_aperiodic_pos_s&                               set_aperiodic();
  protocol_ie_single_container_s<res_type_pos_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<res_type_pos_ext_ies_o>,
                  res_type_aperiodic_pos_s,
                  res_type_periodic_pos_s,
                  res_type_semi_persistent_pos_s>
      c;

  void destroy_();
};

using result_csi_rsrp_item_ext_ies_container = protocol_ext_container_empty_l;

// ResultCSI-RSRP-PerCSI-RS ::= SEQUENCE (SIZE (1..64)) OF ResultCSI-RSRP-PerCSI-RS-Item
using result_csi_rsrp_per_csi_rs_l = dyn_array<result_csi_rsrp_per_csi_rs_item_s>;

using result_csi_rsrq_item_ext_ies_container = protocol_ext_container_empty_l;

// ResultCSI-RSRQ-PerCSI-RS ::= SEQUENCE (SIZE (1..64)) OF ResultCSI-RSRQ-PerCSI-RS-Item
using result_csi_rsrq_per_csi_rs_l = dyn_array<result_csi_rsrq_per_csi_rs_item_s>;

using result_ss_rsrp_item_ext_ies_container = protocol_ext_container_empty_l;

// ResultSS-RSRP-PerSSB ::= SEQUENCE (SIZE (1..64)) OF ResultSS-RSRP-PerSSB-Item
using result_ss_rsrp_per_ssb_l = dyn_array<result_ss_rsrp_per_ssb_item_s>;

using result_ss_rsrq_item_ext_ies_container = protocol_ext_container_empty_l;

// ResultSS-RSRQ-PerSSB ::= SEQUENCE (SIZE (1..64)) OF ResultSS-RSRQ-PerSSB-Item
using result_ss_rsrq_per_ssb_l = dyn_array<result_ss_rsrq_per_ssb_item_s>;

// SpatialRelationPerSRSResourceItem ::= SEQUENCE
struct spatial_relation_per_srs_res_item_s {
  bool                                                ext             = false;
  bool                                                ie_exts_present = false;
  ref_sig_c                                           ref_sig;
  spatial_relation_per_srs_res_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpatialRelationPos ::= CHOICE
struct spatial_relation_pos_c {
  struct types_opts {
    enum options { ssb_pos, pr_si_nformation_pos, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  spatial_relation_pos_c() = default;
  spatial_relation_pos_c(const spatial_relation_pos_c& other);
  spatial_relation_pos_c& operator=(const spatial_relation_pos_c& other);
  ~spatial_relation_pos_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ssb_s& ssb_pos()
  {
    assert_choice_type(types::ssb_pos, type_, "SpatialRelationPos");
    return c.get<ssb_s>();
  }
  pr_si_nformation_pos_s& pr_si_nformation_pos()
  {
    assert_choice_type(types::pr_si_nformation_pos, type_, "SpatialRelationPos");
    return c.get<pr_si_nformation_pos_s>();
  }
  protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "SpatialRelationPos");
    return c.get<protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>>();
  }
  const ssb_s& ssb_pos() const
  {
    assert_choice_type(types::ssb_pos, type_, "SpatialRelationPos");
    return c.get<ssb_s>();
  }
  const pr_si_nformation_pos_s& pr_si_nformation_pos() const
  {
    assert_choice_type(types::pr_si_nformation_pos, type_, "SpatialRelationPos");
    return c.get<pr_si_nformation_pos_s>();
  }
  const protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "SpatialRelationPos");
    return c.get<protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>>();
  }
  ssb_s&                                                      set_ssb_pos();
  pr_si_nformation_pos_s&                                     set_pr_si_nformation_pos();
  protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                      type_;
  choice_buffer_t<pr_si_nformation_pos_s, protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>, ssb_s> c;

  void destroy_();
};

// SpatialRelationforResourceIDItem ::= SEQUENCE
struct spatial_relationfor_res_id_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  ref_sig_c                                         ref_sig;
  spatial_relationfor_res_id_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srs_res_ext_ies_container = protocol_ext_container_empty_l;

// SRSResourceID-List ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..63)
using srs_res_id_list_l = bounded_array<uint8_t, 16>;

using srs_res_set_ext_ies_container = protocol_ext_container_empty_l;

// SRSResourcetype ::= SEQUENCE
struct srs_restype_s {
  bool                                            ext = false;
  srs_res_type_choice_c                           srs_res_type_choice;
  protocol_ext_container_l<srs_restype_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ssb_info_item_ext_ies_container = protocol_ext_container_empty_l;

using tdd_cfg_eutra_item_item_ext_ies_container = protocol_ext_container_empty_l;

// TF-Configuration ::= SEQUENCE
struct tf_cfg_s {
  struct ssb_subcarrier_spacing_opts {
    enum options { khz15, khz30, khz120, khz240, /*...*/ khz60, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ssb_subcarrier_spacing_e_ = enumerated<ssb_subcarrier_spacing_opts, true, 1>;
  struct ssb_periodicity_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ssb_periodicity_e_ = enumerated<ssb_periodicity_opts, true>;

  // member variables
  bool                             ext                          = false;
  bool                             ssb_burst_position_present   = false;
  bool                             sfn_initisation_time_present = false;
  bool                             ie_exts_present              = false;
  uint32_t                         ssb_freq                     = 0;
  ssb_subcarrier_spacing_e_        ssb_subcarrier_spacing;
  int8_t                           ssb_tx_pwr = -60;
  ssb_periodicity_e_               ssb_periodicity;
  uint8_t                          ssb_half_frame_offset = 0;
  uint8_t                          ssb_sfn_offset        = 0;
  ssb_burst_position_c             ssb_burst_position;
  fixed_bitstring<64, false, true> sfn_initisation_time;
  tf_cfg_ext_ies_container         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeStampSlotIndex-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using time_stamp_slot_idx_ext_ies_o = protocol_ies_empty_o;

// TRPBeamAntennaInformation ::= SEQUENCE
struct trp_beam_ant_info_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  choice_trp_beam_ant_info_item_c     choice_trp_beam_ant_info_item;
  trp_beam_ant_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPPositionDirect ::= SEQUENCE
struct trp_position_direct_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  trp_position_direct_accuracy_c        accuracy;
  trp_position_direct_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPPositionReferenced ::= SEQUENCE
struct trp_position_refd_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  ref_point_c                         ref_point;
  trp_ref_point_type_c                ref_point_type;
  trp_position_refd_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPTxTEGAssociation ::= SEQUENCE (SIZE (1..8)) OF TRPTEGItem
using trp_tx_teg_assoc_l = dyn_array<trpteg_item_s>;

// TRPType ::= ENUMERATED
struct trp_type_opts {
  enum options { prs_only_tp, srs_only_rp, tp, rp, trp, /*...*/ nulltype } value;

  const char* to_string() const;
};
using trp_type_e = enumerated<trp_type_opts, true>;

// TransmissionComb ::= CHOICE
struct tx_comb_c {
  struct n2_s_ {
    uint8_t comb_offset_n2  = 0;
    uint8_t cyclic_shift_n2 = 0;
  };
  struct n4_s_ {
    uint8_t comb_offset_n4  = 0;
    uint8_t cyclic_shift_n4 = 0;
  };
  struct types_opts {
    enum options { n2, n4, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  tx_comb_c() = default;
  tx_comb_c(const tx_comb_c& other);
  tx_comb_c& operator=(const tx_comb_c& other);
  ~tx_comb_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  n2_s_& n2()
  {
    assert_choice_type(types::n2, type_, "TransmissionComb");
    return c.get<n2_s_>();
  }
  n4_s_& n4()
  {
    assert_choice_type(types::n4, type_, "TransmissionComb");
    return c.get<n4_s_>();
  }
  protocol_ie_single_container_s<tx_comb_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TransmissionComb");
    return c.get<protocol_ie_single_container_s<tx_comb_ext_ies_o>>();
  }
  const n2_s_& n2() const
  {
    assert_choice_type(types::n2, type_, "TransmissionComb");
    return c.get<n2_s_>();
  }
  const n4_s_& n4() const
  {
    assert_choice_type(types::n4, type_, "TransmissionComb");
    return c.get<n4_s_>();
  }
  const protocol_ie_single_container_s<tx_comb_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TransmissionComb");
    return c.get<protocol_ie_single_container_s<tx_comb_ext_ies_o>>();
  }
  n2_s_&                                             set_n2();
  n4_s_&                                             set_n4();
  protocol_ie_single_container_s<tx_comb_ext_ies_o>& set_choice_ext();

private:
  types                                                                            type_;
  choice_buffer_t<n2_s_, n4_s_, protocol_ie_single_container_s<tx_comb_ext_ies_o>> c;

  void destroy_();
};

// TransmissionCombPos ::= CHOICE
struct tx_comb_pos_c {
  struct n2_s_ {
    uint8_t comb_offset_n2  = 0;
    uint8_t cyclic_shift_n2 = 0;
  };
  struct n4_s_ {
    uint8_t comb_offset_n4  = 0;
    uint8_t cyclic_shift_n4 = 0;
  };
  struct n8_s_ {
    uint8_t comb_offset_n8  = 0;
    uint8_t cyclic_shift_n8 = 0;
  };
  struct types_opts {
    enum options { n2, n4, n8, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  tx_comb_pos_c() = default;
  tx_comb_pos_c(const tx_comb_pos_c& other);
  tx_comb_pos_c& operator=(const tx_comb_pos_c& other);
  ~tx_comb_pos_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  n2_s_& n2()
  {
    assert_choice_type(types::n2, type_, "TransmissionCombPos");
    return c.get<n2_s_>();
  }
  n4_s_& n4()
  {
    assert_choice_type(types::n4, type_, "TransmissionCombPos");
    return c.get<n4_s_>();
  }
  n8_s_& n8()
  {
    assert_choice_type(types::n8, type_, "TransmissionCombPos");
    return c.get<n8_s_>();
  }
  protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TransmissionCombPos");
    return c.get<protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>>();
  }
  const n2_s_& n2() const
  {
    assert_choice_type(types::n2, type_, "TransmissionCombPos");
    return c.get<n2_s_>();
  }
  const n4_s_& n4() const
  {
    assert_choice_type(types::n4, type_, "TransmissionCombPos");
    return c.get<n4_s_>();
  }
  const n8_s_& n8() const
  {
    assert_choice_type(types::n8, type_, "TransmissionCombPos");
    return c.get<n8_s_>();
  }
  const protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TransmissionCombPos");
    return c.get<protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>>();
  }
  n2_s_&                                                 set_n2();
  n4_s_&                                                 set_n4();
  n8_s_&                                                 set_n8();
  protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>& set_choice_ext();

private:
  types                                                                                       type_;
  choice_buffer_t<n2_s_, n4_s_, n8_s_, protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>> c;

  void destroy_();
};

// AdditionalPathList ::= SEQUENCE (SIZE (1..2)) OF AdditionalPathListItem
using add_path_list_l = dyn_array<add_path_list_item_s>;

// AngleMeasurementType ::= CHOICE
struct angle_meas_type_c {
  struct types_opts {
    enum options { expected_ul_ao_a, expected_zo_a, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  angle_meas_type_c() = default;
  angle_meas_type_c(const angle_meas_type_c& other);
  angle_meas_type_c& operator=(const angle_meas_type_c& other);
  ~angle_meas_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  expected_ul_ao_a_s& expected_ul_ao_a()
  {
    assert_choice_type(types::expected_ul_ao_a, type_, "AngleMeasurementType");
    return c.get<expected_ul_ao_a_s>();
  }
  expected_zo_a_only_s& expected_zo_a()
  {
    assert_choice_type(types::expected_zo_a, type_, "AngleMeasurementType");
    return c.get<expected_zo_a_only_s>();
  }
  protocol_ie_single_container_s<angle_meas_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "AngleMeasurementType");
    return c.get<protocol_ie_single_container_s<angle_meas_type_ext_ies_o>>();
  }
  const expected_ul_ao_a_s& expected_ul_ao_a() const
  {
    assert_choice_type(types::expected_ul_ao_a, type_, "AngleMeasurementType");
    return c.get<expected_ul_ao_a_s>();
  }
  const expected_zo_a_only_s& expected_zo_a() const
  {
    assert_choice_type(types::expected_zo_a, type_, "AngleMeasurementType");
    return c.get<expected_zo_a_only_s>();
  }
  const protocol_ie_single_container_s<angle_meas_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "AngleMeasurementType");
    return c.get<protocol_ie_single_container_s<angle_meas_type_ext_ies_o>>();
  }
  expected_ul_ao_a_s&                                        set_expected_ul_ao_a();
  expected_zo_a_only_s&                                      set_expected_zo_a();
  protocol_ie_single_container_s<angle_meas_type_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<expected_ul_ao_a_s, expected_zo_a_only_s, protocol_ie_single_container_s<angle_meas_type_ext_ies_o>>
      c;

  void destroy_();
};

using ao_a_assist_info_ext_ies_container = protocol_ext_container_empty_l;

using assist_info_meta_data_ext_ies_container = protocol_ext_container_empty_l;

// CGI-EUTRA ::= SEQUENCE
struct cgi_eutra_s {
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<28, false, true> eutr_acell_id;
  cgi_eutra_ext_ies_container      ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLPRSResourceCoordinates ::= SEQUENCE
struct dl_prs_res_coordinates_s {
  using listof_dl_prs_res_set_arp_l_ = dyn_array<dl_prs_res_set_arp_s>;

  // member variables
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  listof_dl_prs_res_set_arp_l_             listof_dl_prs_res_set_arp;
  dl_prs_res_coordinates_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct gnb_rx_tx_time_diff_ext_ies_container {
  bool                     extended_add_path_list_present = false;
  bool                     trpteg_info_present            = false;
  extended_add_path_list_l extended_add_path_list;
  trpteg_info_c            trpteg_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNBRxTxTimeDiffMeas ::= CHOICE
struct gnb_rx_tx_time_diff_meas_c {
  struct types_opts {
    enum options { k0, k1, k2, k3, k4, k5, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  gnb_rx_tx_time_diff_meas_c() = default;
  gnb_rx_tx_time_diff_meas_c(const gnb_rx_tx_time_diff_meas_c& other);
  gnb_rx_tx_time_diff_meas_c& operator=(const gnb_rx_tx_time_diff_meas_c& other);
  ~gnb_rx_tx_time_diff_meas_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint32_t& k0()
  {
    assert_choice_type(types::k0, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  uint32_t& k1()
  {
    assert_choice_type(types::k1, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  uint32_t& k2()
  {
    assert_choice_type(types::k2, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  uint32_t& k3()
  {
    assert_choice_type(types::k3, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  uint32_t& k4()
  {
    assert_choice_type(types::k4, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  uint16_t& k5()
  {
    assert_choice_type(types::k5, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint16_t>();
  }
  protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "GNBRxTxTimeDiffMeas");
    return c.get<protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>>();
  }
  const uint32_t& k0() const
  {
    assert_choice_type(types::k0, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  const uint32_t& k1() const
  {
    assert_choice_type(types::k1, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  const uint32_t& k2() const
  {
    assert_choice_type(types::k2, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  const uint32_t& k3() const
  {
    assert_choice_type(types::k3, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  const uint32_t& k4() const
  {
    assert_choice_type(types::k4, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint32_t>();
  }
  const uint16_t& k5() const
  {
    assert_choice_type(types::k5, type_, "GNBRxTxTimeDiffMeas");
    return c.get<uint16_t>();
  }
  const protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "GNBRxTxTimeDiffMeas");
    return c.get<protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>>();
  }
  uint32_t&                                                           set_k0();
  uint32_t&                                                           set_k1();
  uint32_t&                                                           set_k2();
  uint32_t&                                                           set_k3();
  uint32_t&                                                           set_k4();
  uint16_t&                                                           set_k5();
  protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>& set_choice_ext();

private:
  types                                                                               type_;
  choice_buffer_t<protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>> c;

  void destroy_();
};

// NR-PRS-Beam-Information ::= SEQUENCE
struct nr_prs_beam_info_s {
  using nr_prs_beam_info_list_l_       = dyn_array<nr_prs_beam_info_item_s>;
  using lcs_to_gcs_translation_list_l_ = dyn_array<lcs_to_gcs_translation_item_s>;

  // member variables
  bool                           ext             = false;
  bool                           ie_exts_present = false;
  nr_prs_beam_info_list_l_       nr_prs_beam_info_list;
  lcs_to_gcs_translation_list_l_ lcs_to_gcs_translation_list;
  nr_prs_beam_info_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pos_sib_segments_item_s_ {
  bool                               ext             = false;
  bool                               ie_exts_present = false;
  unbounded_octstring<true>          assist_data_sib_elem;
  pos_sib_segments_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSRSResource-Item ::= SEQUENCE
struct pos_srs_res_item_s {
  struct nrof_symbols_opts {
    enum options { n1, n2, n4, n8, n12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_symbols_e_ = enumerated<nrof_symbols_opts>;
  struct group_or_seq_hop_opts {
    enum options { neither, group_hop, seq_hop, nulltype } value;

    const char* to_string() const;
  };
  using group_or_seq_hop_e_ = enumerated<group_or_seq_hop_opts>;

  // member variables
  bool                               ext                          = false;
  bool                               spatial_relation_pos_present = false;
  bool                               ie_exts_present              = false;
  uint8_t                            srs_pos_res_id               = 0;
  tx_comb_pos_c                      tx_comb_pos;
  uint8_t                            start_position = 0;
  nrof_symbols_e_                    nrof_symbols;
  uint16_t                           freq_domain_shift = 0;
  uint8_t                            c_srs             = 0;
  group_or_seq_hop_e_                group_or_seq_hop;
  res_type_pos_c                     res_type_pos;
  uint32_t                           seq_id = 0;
  spatial_relation_pos_c             spatial_relation_pos;
  pos_srs_res_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSRSResourceSet-Item ::= SEQUENCE
struct pos_srs_res_set_item_s {
  bool                                   ext               = false;
  bool                                   ie_exts_present   = false;
  uint8_t                                possrs_res_set_id = 0;
  pos_srs_res_id_per_set_list_l          poss_rs_res_id_per_set_list;
  pos_res_set_type_c                     posres_set_type;
  pos_srs_res_set_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using prs_cfg_ext_ies_container = protocol_ext_container_empty_l;

// PRSMutingConfiguration-EUTRA-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using prs_muting_cfg_eutra_ext_ie_o = protocol_ies_empty_o;

// PRSResourceSet-List ::= SEQUENCE (SIZE (1..8)) OF PRSResourceSet-Item
using prs_res_set_list_l = dyn_array<prs_res_set_item_s>;

// PRSTransmissionOffIndicationPerResource-Item ::= SEQUENCE
struct prs_tx_off_ind_per_res_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint8_t                                       prs_res_id      = 0;
  prs_tx_off_ind_per_res_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using prs_tx_off_per_res_item_ext_ies_container = protocol_ext_container_empty_l;

using prs_tx_off_per_res_set_item_ext_ies_container = protocol_ext_container_empty_l;

// RequestedDLPRSResource-Item ::= SEQUENCE
struct requested_dl_prs_res_item_s {
  bool                                        ext              = false;
  bool                                        qcl_info_present = false;
  bool                                        ie_exts_present  = false;
  prs_res_qcl_info_c                          qcl_info;
  requested_dl_prs_res_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultCSI-RSRP-Item ::= SEQUENCE
struct result_csi_rsrp_item_s {
  bool                                   ext                         = false;
  bool                                   cgi_nr_present              = false;
  bool                                   value_csi_rsrp_cell_present = false;
  bool                                   ie_exts_present             = false;
  uint16_t                               nr_pci                      = 0;
  uint32_t                               nr_arfcn                    = 0;
  cgi_nr_s                               cgi_nr;
  uint8_t                                value_csi_rsrp_cell = 0;
  result_csi_rsrp_per_csi_rs_l           csi_rsrp_per_csi_rs;
  result_csi_rsrp_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultCSI-RSRQ-Item ::= SEQUENCE
struct result_csi_rsrq_item_s {
  bool                                   ext                         = false;
  bool                                   cgi_nr_present              = false;
  bool                                   value_csi_rsrq_cell_present = false;
  bool                                   ie_exts_present             = false;
  uint16_t                               nr_pci                      = 0;
  uint32_t                               nr_arfcn                    = 0;
  cgi_nr_s                               cgi_nr;
  uint8_t                                value_csi_rsrq_cell = 0;
  result_csi_rsrq_per_csi_rs_l           csi_rsrq_per_csi_rs;
  result_csi_rsrq_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using result_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

using result_nr_item_ext_ies_container = protocol_ext_container_empty_l;

// ResultSS-RSRP-Item ::= SEQUENCE
struct result_ss_rsrp_item_s {
  bool                                  ext                        = false;
  bool                                  cgi_nr_present             = false;
  bool                                  value_ss_rsrp_cell_present = false;
  bool                                  ie_exts_present            = false;
  uint16_t                              nr_pci                     = 0;
  uint32_t                              nr_arfcn                   = 0;
  cgi_nr_s                              cgi_nr;
  uint8_t                               value_ss_rsrp_cell = 0;
  result_ss_rsrp_per_ssb_l              ss_rsrp_per_ssb;
  result_ss_rsrp_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultSS-RSRQ-Item ::= SEQUENCE
struct result_ss_rsrq_item_s {
  bool                                  ext                        = false;
  bool                                  cgi_nr_present             = false;
  bool                                  value_ss_rsrq_cell_present = false;
  bool                                  ie_exts_present            = false;
  uint16_t                              nr_pci                     = 0;
  uint32_t                              nr_arfcn                   = 0;
  cgi_nr_s                              cgi_nr;
  uint8_t                               value_ss_rsrq_cell = 0;
  result_ss_rsrq_per_ssb_l              ss_rsrq_per_ssb;
  result_ss_rsrq_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using spatial_direction_info_ext_ies_container = protocol_ext_container_empty_l;

using spatial_relation_info_ext_ies_container = protocol_ext_container_empty_l;

using spatial_relation_per_srs_res_ext_ies_container = protocol_ext_container_empty_l;

// SpatialRelationPerSRSResource-List ::= SEQUENCE (SIZE (1..16)) OF SpatialRelationPerSRSResourceItem
using spatial_relation_per_srs_res_list_l = dyn_array<spatial_relation_per_srs_res_item_s>;

// SpatialRelationforResourceID ::= SEQUENCE (SIZE (1..64)) OF SpatialRelationforResourceIDItem
using spatial_relationfor_res_id_l = dyn_array<spatial_relationfor_res_id_item_s>;

// SRSResource ::= SEQUENCE
struct srs_res_s {
  struct nrof_srs_ports_opts {
    enum options { port1, ports2, ports4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrof_srs_ports_e_ = enumerated<nrof_srs_ports_opts>;
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
  struct group_or_seq_hop_opts {
    enum options { neither, group_hop, seq_hop, nulltype } value;

    const char* to_string() const;
  };
  using group_or_seq_hop_e_ = enumerated<group_or_seq_hop_opts>;

  // member variables
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  uint8_t                   srs_res_id      = 0;
  nrof_srs_ports_e_         nrof_srs_ports;
  tx_comb_c                 tx_comb;
  uint8_t                   start_position = 0;
  nrof_symbols_e_           nrof_symbols;
  repeat_factor_e_          repeat_factor;
  uint8_t                   freq_domain_position = 0;
  uint16_t                  freq_domain_shift    = 0;
  uint8_t                   c_srs                = 0;
  uint8_t                   b_srs                = 0;
  uint8_t                   b_hop                = 0;
  group_or_seq_hop_e_       group_or_seq_hop;
  res_type_c                res_type;
  uint16_t                  seq_id = 0;
  srs_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRSResourceSet ::= SEQUENCE
struct srs_res_set_s {
  bool                          ext             = false;
  bool                          ie_exts_present = false;
  uint8_t                       srs_res_set_id  = 0;
  srs_res_id_list_l             srs_res_id_list;
  res_set_type_c                res_set_type;
  srs_res_set_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using ssb_info_ext_ies_container = protocol_ext_container_empty_l;

// SSBInfoItem ::= SEQUENCE
struct ssb_info_item_s {
  bool                            ext             = false;
  bool                            ie_exts_present = false;
  tf_cfg_s                        ssb_cfg;
  uint16_t                        pci_nr = 0;
  ssb_info_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using start_time_and_dur_ext_ies_container = protocol_ext_container_empty_l;

// TDD-Config-EUTRA-Item ::= SEQUENCE
struct tdd_cfg_eutra_item_s {
  struct sf_assign_opts {
    enum options { sa0, sa1, sa2, sa3, sa4, sa5, sa6, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sf_assign_e_ = enumerated<sf_assign_opts, true>;

  // member variables
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  sf_assign_e_                              sf_assign;
  tdd_cfg_eutra_item_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPInformationTypeResponseItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct trp_info_type_resp_item_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_type, on_demand_prs, trp_tx_teg_assoc, trp_beam_ant_info, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    trp_type_e&                 trp_type();
    on_demand_prs_info_s&       on_demand_prs();
    trp_tx_teg_assoc_l&         trp_tx_teg_assoc();
    trp_beam_ant_info_s&        trp_beam_ant_info();
    const trp_type_e&           trp_type() const;
    const on_demand_prs_info_s& on_demand_prs() const;
    const trp_tx_teg_assoc_l&   trp_tx_teg_assoc() const;
    const trp_beam_ant_info_s&  trp_beam_ant_info() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TrpMeasurementResultItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct trp_meas_result_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { srs_restype, arp_id, lo_s_n_lo_si_nformation, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srs_restype_s&                   srs_restype();
    uint8_t&                         arp_id();
    lo_s_n_lo_si_nformation_c&       lo_s_n_lo_si_nformation();
    const srs_restype_s&             srs_restype() const;
    const uint8_t&                   arp_id() const;
    const lo_s_n_lo_si_nformation_c& lo_s_n_lo_si_nformation() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TrpMeasuredResultsValue-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct trp_measured_results_value_ext_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { zo_a, multiple_ul_ao_a, ul_srs_rsrp_p, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    zo_a_s&                   zo_a();
    multiple_ul_ao_a_s&       multiple_ul_ao_a();
    ul_srs_rsrp_p_s&          ul_srs_rsrp_p();
    const zo_a_s&             zo_a() const;
    const multiple_ul_ao_a_s& multiple_ul_ao_a() const;
    const ul_srs_rsrp_p_s&    ul_srs_rsrp_p() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRPPositionDefinitionType ::= CHOICE
struct trp_position_definition_type_c {
  struct types_opts {
    enum options { direct, refd, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  trp_position_definition_type_c() = default;
  trp_position_definition_type_c(const trp_position_definition_type_c& other);
  trp_position_definition_type_c& operator=(const trp_position_definition_type_c& other);
  ~trp_position_definition_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  trp_position_direct_s& direct()
  {
    assert_choice_type(types::direct, type_, "TRPPositionDefinitionType");
    return c.get<trp_position_direct_s>();
  }
  trp_position_refd_s& refd()
  {
    assert_choice_type(types::refd, type_, "TRPPositionDefinitionType");
    return c.get<trp_position_refd_s>();
  }
  protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TRPPositionDefinitionType");
    return c.get<protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>>();
  }
  const trp_position_direct_s& direct() const
  {
    assert_choice_type(types::direct, type_, "TRPPositionDefinitionType");
    return c.get<trp_position_direct_s>();
  }
  const trp_position_refd_s& refd() const
  {
    assert_choice_type(types::refd, type_, "TRPPositionDefinitionType");
    return c.get<trp_position_refd_s>();
  }
  const protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TRPPositionDefinitionType");
    return c.get<protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>>();
  }
  trp_position_direct_s&                                                  set_direct();
  trp_position_refd_s&                                                    set_refd();
  protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>,
                  trp_position_direct_s,
                  trp_position_refd_s>
      c;

  void destroy_();
};

// ULRTOAMeas ::= CHOICE
struct ul_rtoa_meas_c {
  struct types_opts {
    enum options { k0, k1, k2, k3, k4, k5, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ul_rtoa_meas_c() = default;
  ul_rtoa_meas_c(const ul_rtoa_meas_c& other);
  ul_rtoa_meas_c& operator=(const ul_rtoa_meas_c& other);
  ~ul_rtoa_meas_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint32_t& k0()
  {
    assert_choice_type(types::k0, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  uint32_t& k1()
  {
    assert_choice_type(types::k1, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  uint32_t& k2()
  {
    assert_choice_type(types::k2, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  uint32_t& k3()
  {
    assert_choice_type(types::k3, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  uint32_t& k4()
  {
    assert_choice_type(types::k4, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  uint16_t& k5()
  {
    assert_choice_type(types::k5, type_, "ULRTOAMeas");
    return c.get<uint16_t>();
  }
  protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "ULRTOAMeas");
    return c.get<protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>>();
  }
  const uint32_t& k0() const
  {
    assert_choice_type(types::k0, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  const uint32_t& k1() const
  {
    assert_choice_type(types::k1, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  const uint32_t& k2() const
  {
    assert_choice_type(types::k2, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  const uint32_t& k3() const
  {
    assert_choice_type(types::k3, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  const uint32_t& k4() const
  {
    assert_choice_type(types::k4, type_, "ULRTOAMeas");
    return c.get<uint32_t>();
  }
  const uint16_t& k5() const
  {
    assert_choice_type(types::k5, type_, "ULRTOAMeas");
    return c.get<uint16_t>();
  }
  const protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "ULRTOAMeas");
    return c.get<protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>>();
  }
  uint32_t&                                               set_k0();
  uint32_t&                                               set_k1();
  uint32_t&                                               set_k2();
  uint32_t&                                               set_k3();
  uint32_t&                                               set_k4();
  uint16_t&                                               set_k5();
  protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>& set_choice_ext();

private:
  types                                                                   type_;
  choice_buffer_t<protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>> c;

  void destroy_();
};

struct ul_rtoameas_ext_ies_container {
  bool                     extended_add_path_list_present = false;
  bool                     trp_rx_teg_info_present        = false;
  extended_add_path_list_l extended_add_path_list;
  trp_rx_teg_info_s        trp_rx_teg_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AoA-AssistanceInfo ::= SEQUENCE
struct ao_a_assist_info_s {
  bool                               ext                            = false;
  bool                               lcs_to_gcs_translation_present = false;
  bool                               ie_exts_present                = false;
  angle_meas_type_c                  angle_meas;
  lcs_to_gcs_translation_s           lcs_to_gcs_translation;
  ao_a_assist_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssistanceInformationMetaData ::= SEQUENCE
struct assist_info_meta_data_s {
  struct encrypted_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using encrypted_e_ = enumerated<encrypted_opts, true>;
  struct gns_si_d_opts {
    enum options { gps, sbas, qzss, galileo, glonass, bds, navic, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using gns_si_d_e_ = enumerated<gns_si_d_opts, true>;
  struct sba_si_d_opts {
    enum options { waas, egnos, msas, gagan, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using sba_si_d_e_ = enumerated<sba_si_d_opts, true>;

  // member variables
  bool                                    ext               = false;
  bool                                    encrypted_present = false;
  bool                                    gns_si_d_present  = false;
  bool                                    sba_si_d_present  = false;
  bool                                    ie_exts_present   = false;
  encrypted_e_                            encrypted;
  gns_si_d_e_                             gns_si_d;
  sba_si_d_e_                             sba_si_d;
  assist_info_meta_data_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GeographicalCoordinates ::= SEQUENCE
struct geographical_coordinates_s {
  bool                                                         ext                            = false;
  bool                                                         dl_prs_res_coordinates_present = false;
  trp_position_definition_type_c                               trp_position_definition_type;
  dl_prs_res_coordinates_s                                     dl_prs_res_coordinates;
  protocol_ext_container_l<geographical_coordinates_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNB-RxTxTimeDiff ::= SEQUENCE
struct gnb_rx_tx_time_diff_s {
  bool                                  ext             = false;
  bool                                  ie_exts_present = false;
  gnb_rx_tx_time_diff_meas_c            rx_tx_time_diff;
  add_path_list_l                       add_path_list;
  gnb_rx_tx_time_diff_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using meas_beam_info_ext_ies_container = protocol_ext_container_empty_l;

// NumberOfFrequencyHoppingBands ::= ENUMERATED
struct nof_freq_hop_bands_opts {
  enum options { twobands, fourbands, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using nof_freq_hop_bands_e = enumerated<nof_freq_hop_bands_opts, true>;

// NumberOfTRPRxTEG ::= ENUMERATED
struct nof_trp_rx_teg_opts {
  enum options { two, three, four, six, eight, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using nof_trp_rx_teg_e = enumerated<nof_trp_rx_teg_opts, true>;

// NumberOfTRPRxTxTEG ::= ENUMERATED
struct nof_trp_rx_tx_teg_opts {
  enum options { two, three, four, six, eight, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using nof_trp_rx_tx_teg_e = enumerated<nof_trp_rx_tx_teg_opts, true>;

// OTDOACell-Information-Item-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct otdoa_cell_info_item_ext_ie_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { tdd_cfg_eutra_item, cgi_nr, sfn_initisation_time_nr, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    tdd_cfg_eutra_item_s&                   tdd_cfg_eutra_item();
    cgi_nr_s&                               cgi_nr();
    fixed_bitstring<64, false, true>&       sfn_initisation_time_nr();
    const tdd_cfg_eutra_item_s&             tdd_cfg_eutra_item() const;
    const cgi_nr_s&                         cgi_nr() const;
    const fixed_bitstring<64, false, true>& sfn_initisation_time_nr() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PathlossReferenceSignal-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using pathloss_ref_sig_ext_ie_o = protocol_ies_empty_o;

// PosSIB-Segments ::= SEQUENCE (SIZE (1..64)) OF PosSIB-Segments-item
using pos_sib_segments_l = dyn_array<pos_sib_segments_item_s_>;

// PosSIB-Type ::= ENUMERATED
struct pos_sib_type_opts {
  enum options {
    pos_sib_type1_neg1,
    pos_sib_type1_neg2,
    pos_sib_type1_neg3,
    pos_sib_type1_neg4,
    pos_sib_type1_neg5,
    pos_sib_type1_neg6,
    pos_sib_type1_neg7,
    pos_sib_type1_neg8,
    pos_sib_type2_neg1,
    pos_sib_type2_neg2,
    pos_sib_type2_neg3,
    pos_sib_type2_neg4,
    pos_sib_type2_neg5,
    pos_sib_type2_neg6,
    pos_sib_type2_neg7,
    pos_sib_type2_neg8,
    pos_sib_type2_neg9,
    pos_sib_type2_neg10,
    pos_sib_type2_neg11,
    pos_sib_type2_neg12,
    pos_sib_type2_neg13,
    pos_sib_type2_neg14,
    pos_sib_type2_neg15,
    pos_sib_type2_neg16,
    pos_sib_type2_neg17,
    pos_sib_type2_neg18,
    pos_sib_type2_neg19,
    pos_sib_type2_neg20,
    pos_sib_type2_neg21,
    pos_sib_type2_neg22,
    pos_sib_type2_neg23,
    pos_sib_type2_neg24,
    pos_sib_type2_neg25,
    pos_sib_type3_neg1,
    pos_sib_type4_neg1,
    pos_sib_type5_neg1,
    pos_sib_type6_neg1,
    pos_sib_type6_neg2,
    pos_sib_type6_neg3,
    // ...
    pos_sib_type1_neg9,
    pos_sib_type1_neg10,
    pos_sib_type6_neg4,
    pos_sib_type6_neg5,
    pos_sib_type6_neg6,
    nulltype
  } value;

  const char* to_string() const;
};
using pos_sib_type_e = enumerated<pos_sib_type_opts, true, 5>;

using pos_sibs_ext_ies_container = protocol_ext_container_empty_l;

// PosSRSResource-List ::= SEQUENCE (SIZE (1..64)) OF PosSRSResource-Item
using pos_srs_res_list_l = dyn_array<pos_srs_res_item_s>;

// PosSRSResourceSet-List ::= SEQUENCE (SIZE (1..16)) OF PosSRSResourceSet-Item
using pos_srs_res_set_list_l = dyn_array<pos_srs_res_set_item_s>;

// PRSConfiguration ::= SEQUENCE
struct prs_cfg_s {
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  prs_res_set_list_l        prs_res_set_list;
  prs_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using prs_freq_hop_cfg_eutra_item_ies_container = protocol_ext_container_empty_l;

// PRSTransmissionOffIndication-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using prs_tx_off_ind_ext_ies_o = protocol_ies_empty_o;

// PRSTransmissionOffPerResource-Item ::= SEQUENCE
struct prs_tx_off_per_res_item_s {
  using prs_tx_off_ind_per_res_list_l_ = dyn_array<prs_tx_off_ind_per_res_item_s>;

  // member variables
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  uint8_t                                   prs_res_set_id  = 0;
  prs_tx_off_ind_per_res_list_l_            prs_tx_off_ind_per_res_list;
  prs_tx_off_per_res_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSTransmissionOffPerResourceSet-Item ::= SEQUENCE
struct prs_tx_off_per_res_set_item_s {
  bool                                          ext             = false;
  bool                                          ie_exts_present = false;
  uint8_t                                       prs_res_set_id  = 0;
  prs_tx_off_per_res_set_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RequestedDLPRSResource-List ::= SEQUENCE (SIZE (1..64)) OF RequestedDLPRSResource-Item
using requested_dl_prs_res_list_l = dyn_array<requested_dl_prs_res_item_s>;

using requested_dl_prs_res_set_item_ext_ies_container = protocol_ext_container_empty_l;

// ResultCSI-RSRP ::= SEQUENCE (SIZE (1..9)) OF ResultCSI-RSRP-Item
using result_csi_rsrp_l = dyn_array<result_csi_rsrp_item_s>;

// ResultCSI-RSRQ ::= SEQUENCE (SIZE (1..9)) OF ResultCSI-RSRQ-Item
using result_csi_rsrq_l = dyn_array<result_csi_rsrq_item_s>;

// ResultEUTRA-Item ::= SEQUENCE
struct result_eutra_item_s {
  bool                                ext                      = false;
  bool                                value_rsrp_eutra_present = false;
  bool                                value_rsrq_eutra_present = false;
  bool                                cgi_eutra_present        = false;
  bool                                ie_exts_present          = false;
  uint16_t                            pci_eutra                = 0;
  uint32_t                            earfcn                   = 0;
  uint8_t                             value_rsrp_eutra         = 0;
  uint8_t                             value_rsrq_eutra         = 0;
  cgi_eutra_s                         cgi_eutra;
  result_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultNR-Item ::= SEQUENCE
struct result_nr_item_s {
  bool                             ext                        = false;
  bool                             value_ss_rsrp_cell_present = false;
  bool                             value_ss_rsrq_cell_present = false;
  bool                             cgi_nr_present             = false;
  bool                             ie_exts_present            = false;
  uint16_t                         nr_pci                     = 0;
  uint32_t                         nr_arfcn                   = 0;
  uint8_t                          value_ss_rsrp_cell         = 0;
  uint8_t                          value_ss_rsrq_cell         = 0;
  result_ss_rsrp_per_ssb_l         ss_rsrp_per_ssb;
  result_ss_rsrq_per_ssb_l         ss_rsrq_per_ssb;
  cgi_nr_s                         cgi_nr;
  result_nr_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using result_rsrp_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

using result_rsrq_eutra_item_ext_ies_container = protocol_ext_container_empty_l;

// ResultSS-RSRP ::= SEQUENCE (SIZE (1..9)) OF ResultSS-RSRP-Item
using result_ss_rsrp_l = dyn_array<result_ss_rsrp_item_s>;

// ResultSS-RSRQ ::= SEQUENCE (SIZE (1..9)) OF ResultSS-RSRQ-Item
using result_ss_rsrq_l = dyn_array<result_ss_rsrq_item_s>;

using scs_specific_carrier_ext_ies_container = protocol_ext_container_empty_l;

// SpatialDirectionInformation ::= SEQUENCE
struct spatial_direction_info_s {
  bool                                     ext             = false;
  bool                                     ie_exts_present = false;
  nr_prs_beam_info_s                       nr_prs_beam_info;
  spatial_direction_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpatialRelationInfo ::= SEQUENCE
struct spatial_relation_info_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  spatial_relationfor_res_id_l            spatial_relationfor_res_id;
  spatial_relation_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpatialRelationPerSRSResource ::= SEQUENCE
struct spatial_relation_per_srs_res_s {
  bool                                           ext             = false;
  bool                                           ie_exts_present = false;
  spatial_relation_per_srs_res_list_l            spatial_relation_per_srs_res_list;
  spatial_relation_per_srs_res_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srs_cfg_ext_ies_container = protocol_ext_container_empty_l;

// SRSResource-List ::= SEQUENCE (SIZE (1..64)) OF SRSResource
using srs_res_list_l = dyn_array<srs_res_s>;

// SRSResourceSet-List ::= SEQUENCE (SIZE (1..16)) OF SRSResourceSet
using srs_res_set_list_l = dyn_array<srs_res_set_s>;

// SSBInfo ::= SEQUENCE
struct ssb_info_s {
  using list_of_ssb_info_l_ = dyn_array<ssb_info_item_s>;

  // member variables
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  list_of_ssb_info_l_        list_of_ssb_info;
  ssb_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// StartTimeAndDuration ::= SEQUENCE
struct start_time_and_dur_s {
  bool                                 ext                = false;
  bool                                 start_time_present = false;
  bool                                 dur_present        = false;
  bool                                 ie_exts_present    = false;
  fixed_bitstring<64, false, true>     start_time;
  uint32_t                             dur = 0;
  start_time_and_dur_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using time_stamp_ext_ies_container = protocol_ext_container_empty_l;

// TimeStampSlotIndex ::= CHOICE
struct time_stamp_slot_idx_c {
  struct types_opts {
    enum options { scs_15, scs_30, scs_60, scs_120, choice_ext, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  time_stamp_slot_idx_c() = default;
  time_stamp_slot_idx_c(const time_stamp_slot_idx_c& other);
  time_stamp_slot_idx_c& operator=(const time_stamp_slot_idx_c& other);
  ~time_stamp_slot_idx_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& scs_15()
  {
    assert_choice_type(types::scs_15, type_, "TimeStampSlotIndex");
    return c.get<uint8_t>();
  }
  uint8_t& scs_30()
  {
    assert_choice_type(types::scs_30, type_, "TimeStampSlotIndex");
    return c.get<uint8_t>();
  }
  uint8_t& scs_60()
  {
    assert_choice_type(types::scs_60, type_, "TimeStampSlotIndex");
    return c.get<uint8_t>();
  }
  uint8_t& scs_120()
  {
    assert_choice_type(types::scs_120, type_, "TimeStampSlotIndex");
    return c.get<uint8_t>();
  }
  protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TimeStampSlotIndex");
    return c.get<protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>>();
  }
  const uint8_t& scs_15() const
  {
    assert_choice_type(types::scs_15, type_, "TimeStampSlotIndex");
    return c.get<uint8_t>();
  }
  const uint8_t& scs_30() const
  {
    assert_choice_type(types::scs_30, type_, "TimeStampSlotIndex");
    return c.get<uint8_t>();
  }
  const uint8_t& scs_60() const
  {
    assert_choice_type(types::scs_60, type_, "TimeStampSlotIndex");
    return c.get<uint8_t>();
  }
  const uint8_t& scs_120() const
  {
    assert_choice_type(types::scs_120, type_, "TimeStampSlotIndex");
    return c.get<uint8_t>();
  }
  const protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TimeStampSlotIndex");
    return c.get<protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>>();
  }
  uint8_t&                                                       set_scs_15();
  uint8_t&                                                       set_scs_30();
  uint8_t&                                                       set_scs_60();
  uint8_t&                                                       set_scs_120();
  protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>& set_choice_ext();

private:
  types                                                                          type_;
  choice_buffer_t<protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>> c;

  void destroy_();
};

// UL-RTOAMeasurement ::= SEQUENCE
struct ul_rtoameas_s {
  bool                          ext             = false;
  bool                          ie_exts_present = false;
  ul_rtoa_meas_c                ul_rto_ameas;
  add_path_list_l               add_path_list;
  ul_rtoameas_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using active_ul_bwp_ext_ies_container = protocol_ext_container_empty_l;

// CPLength-EUTRA ::= ENUMERATED
struct cp_len_eutra_opts {
  enum options { normal, extended, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cp_len_eutra_e = enumerated<cp_len_eutra_opts, true>;

// DL-Bandwidth-EUTRA ::= ENUMERATED
struct dl_bw_eutra_opts {
  enum options { bw6, bw15, bw25, bw50, bw75, bw100, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using dl_bw_eutra_e = enumerated<dl_bw_eutra_opts, true>;

// MeasurementBeamInfo ::= SEQUENCE
struct meas_beam_info_s {
  bool                             ext                    = false;
  bool                             prs_res_id_present     = false;
  bool                             prs_res_set_id_present = false;
  bool                             ssb_idx_present        = false;
  bool                             ie_exts_present        = false;
  uint8_t                          prs_res_id             = 0;
  uint8_t                          prs_res_set_id         = 0;
  uint8_t                          ssb_idx                = 0;
  meas_beam_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementQuantitiesValue ::= ENUMERATED
struct meas_quantities_value_opts {
  enum options {
    cell_id,
    angle_of_arrival,
    timing_advance_type1,
    timing_advance_type2,
    rsrp,
    rsrq,
    // ...
    ss_rsrp,
    ss_rsrq,
    csi_rsrp,
    csi_rsrq,
    angle_of_arrival_nr,
    timing_advance_nr,
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using meas_quantities_value_e = enumerated<meas_quantities_value_opts, true, 6>;

using meas_quantities_value_ext_ies_container = protocol_ext_container_empty_l;

// MeasuredResultsValue-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct measured_results_value_ext_ie_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        result_ss_rsrp,
        result_ss_rsrq,
        result_csi_rsrp,
        result_csi_rsrq,
        angle_of_arrival_nr,
        nr_tadv,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    result_ss_rsrp_l&        result_ss_rsrp();
    result_ss_rsrq_l&        result_ss_rsrq();
    result_csi_rsrp_l&       result_csi_rsrp();
    result_csi_rsrq_l&       result_csi_rsrq();
    ul_ao_a_s&               angle_of_arrival_nr();
    uint16_t&                nr_tadv();
    const result_ss_rsrp_l&  result_ss_rsrp() const;
    const result_ss_rsrq_l&  result_ss_rsrq() const;
    const result_csi_rsrp_l& result_csi_rsrp() const;
    const result_csi_rsrq_l& result_csi_rsrq() const;
    const ul_ao_a_s&         angle_of_arrival_nr() const;
    const uint16_t&          nr_tadv() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NG-RANCell-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using ng_ran_cell_ext_ie_o = protocol_ies_empty_o;

// NumberOfAntennaPorts-EUTRA ::= ENUMERATED
struct nof_ant_ports_eutra_opts {
  enum options { n1_or_n2, n4, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using nof_ant_ports_eutra_e = enumerated<nof_ant_ports_eutra_opts, true>;

// NumberOfDlFrames-EUTRA ::= ENUMERATED
struct nof_dl_frames_eutra_opts {
  enum options { sf1, sf2, sf4, sf6, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using nof_dl_frames_eutra_e = enumerated<nof_dl_frames_eutra_opts, true>;

// OtherRATMeasurementQuantitiesValue ::= ENUMERATED
struct other_rat_meas_quantities_value_opts {
  enum options { geran, utran, /*...*/ nr, eutra, nulltype } value;

  const char* to_string() const;
};
using other_rat_meas_quantities_value_e = enumerated<other_rat_meas_quantities_value_opts, true, 2>;

using other_rat_meas_quantities_value_ext_ies_container = protocol_ext_container_empty_l;

struct pos_sibs_item_s_ {
  bool                       ext                           = false;
  bool                       assist_info_meta_data_present = false;
  bool                       broadcast_prio_present        = false;
  bool                       ie_exts_present               = false;
  pos_sib_type_e             pos_sib_type;
  pos_sib_segments_l         pos_sib_segments;
  assist_info_meta_data_s    assist_info_meta_data;
  uint8_t                    broadcast_prio = 1;
  pos_sibs_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRS-Bandwidth-EUTRA ::= ENUMERATED
struct prs_bw_eutra_opts {
  enum options { bw6, bw15, bw25, bw50, bw75, bw100, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using prs_bw_eutra_e = enumerated<prs_bw_eutra_opts, true>;

// PRSFrequencyHoppingConfiguration-EUTRA ::= SEQUENCE
struct prs_freq_hop_cfg_eutra_s {
  using band_positions_l_ = bounded_array<uint8_t, 7>;

  // member variables
  bool                                      ext             = false;
  bool                                      ie_exts_present = false;
  nof_freq_hop_bands_e                      no_of_freq_hop_bands;
  band_positions_l_                         band_positions;
  prs_freq_hop_cfg_eutra_item_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSMutingConfiguration-EUTRA ::= CHOICE
struct prs_muting_cfg_eutra_c {
  struct types_opts {
    enum options {
      two,
      four,
      eight,
      sixteen,
      thirty_two,
      sixty_four,
      one_hundred_and_twenty_eight,
      two_hundred_and_fifty_six,
      five_hundred_and_twelve,
      one_thousand_and_twenty_four,
      choice_ext,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  prs_muting_cfg_eutra_c() = default;
  prs_muting_cfg_eutra_c(const prs_muting_cfg_eutra_c& other);
  prs_muting_cfg_eutra_c& operator=(const prs_muting_cfg_eutra_c& other);
  ~prs_muting_cfg_eutra_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<2, false, true>& two()
  {
    assert_choice_type(types::two, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<2, false, true>>();
  }
  fixed_bitstring<4, false, true>& four()
  {
    assert_choice_type(types::four, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<4, false, true>>();
  }
  fixed_bitstring<8, false, true>& eight()
  {
    assert_choice_type(types::eight, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<8, false, true>>();
  }
  fixed_bitstring<16, false, true>& sixteen()
  {
    assert_choice_type(types::sixteen, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<16, false, true>>();
  }
  fixed_bitstring<32, false, true>& thirty_two()
  {
    assert_choice_type(types::thirty_two, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<32, false, true>>();
  }
  fixed_bitstring<64, false, true>& sixty_four()
  {
    assert_choice_type(types::sixty_four, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<64, false, true>>();
  }
  fixed_bitstring<128, false, true>& one_hundred_and_twenty_eight()
  {
    assert_choice_type(types::one_hundred_and_twenty_eight, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<128, false, true>>();
  }
  fixed_bitstring<256, false, true>& two_hundred_and_fifty_six()
  {
    assert_choice_type(types::two_hundred_and_fifty_six, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<256, false, true>>();
  }
  fixed_bitstring<512, false, true>& five_hundred_and_twelve()
  {
    assert_choice_type(types::five_hundred_and_twelve, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<512, false, true>>();
  }
  fixed_bitstring<1024, false, true>& one_thousand_and_twenty_four()
  {
    assert_choice_type(types::one_thousand_and_twenty_four, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<1024, false, true>>();
  }
  protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>>();
  }
  const fixed_bitstring<2, false, true>& two() const
  {
    assert_choice_type(types::two, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<2, false, true>>();
  }
  const fixed_bitstring<4, false, true>& four() const
  {
    assert_choice_type(types::four, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<4, false, true>>();
  }
  const fixed_bitstring<8, false, true>& eight() const
  {
    assert_choice_type(types::eight, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<8, false, true>>();
  }
  const fixed_bitstring<16, false, true>& sixteen() const
  {
    assert_choice_type(types::sixteen, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<16, false, true>>();
  }
  const fixed_bitstring<32, false, true>& thirty_two() const
  {
    assert_choice_type(types::thirty_two, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<32, false, true>>();
  }
  const fixed_bitstring<64, false, true>& sixty_four() const
  {
    assert_choice_type(types::sixty_four, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<64, false, true>>();
  }
  const fixed_bitstring<128, false, true>& one_hundred_and_twenty_eight() const
  {
    assert_choice_type(types::one_hundred_and_twenty_eight, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<128, false, true>>();
  }
  const fixed_bitstring<256, false, true>& two_hundred_and_fifty_six() const
  {
    assert_choice_type(types::two_hundred_and_fifty_six, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<256, false, true>>();
  }
  const fixed_bitstring<512, false, true>& five_hundred_and_twelve() const
  {
    assert_choice_type(types::five_hundred_and_twelve, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<512, false, true>>();
  }
  const fixed_bitstring<1024, false, true>& one_thousand_and_twenty_four() const
  {
    assert_choice_type(types::one_thousand_and_twenty_four, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<fixed_bitstring<1024, false, true>>();
  }
  const protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "PRSMutingConfiguration-EUTRA");
    return c.get<protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>>();
  }
  fixed_bitstring<2, false, true>&                               set_two();
  fixed_bitstring<4, false, true>&                               set_four();
  fixed_bitstring<8, false, true>&                               set_eight();
  fixed_bitstring<16, false, true>&                              set_sixteen();
  fixed_bitstring<32, false, true>&                              set_thirty_two();
  fixed_bitstring<64, false, true>&                              set_sixty_four();
  fixed_bitstring<128, false, true>&                             set_one_hundred_and_twenty_eight();
  fixed_bitstring<256, false, true>&                             set_two_hundred_and_fifty_six();
  fixed_bitstring<512, false, true>&                             set_five_hundred_and_twelve();
  fixed_bitstring<1024, false, true>&                            set_one_thousand_and_twenty_four();
  protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<fixed_bitstring<1024, false, true>, protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>> c;

  void destroy_();
};

// PRSOccasionGroup-EUTRA ::= ENUMERATED
struct prs_occasion_group_eutra_opts {
  enum options { og2, og4, og8, og16, og32, og64, og128, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using prs_occasion_group_eutra_e = enumerated<prs_occasion_group_eutra_opts, true>;

// PRSTransmissionOffPerResource ::= SEQUENCE (SIZE (1..8)) OF PRSTransmissionOffPerResource-Item
using prs_tx_off_per_res_l = dyn_array<prs_tx_off_per_res_item_s>;

// PRSTransmissionOffPerResourceSet ::= SEQUENCE (SIZE (1..8)) OF PRSTransmissionOffPerResourceSet-Item
using prs_tx_off_per_res_set_l = dyn_array<prs_tx_off_per_res_set_item_s>;

// RequestedDLPRSResourceSet-Item ::= SEQUENCE
struct requested_dl_prs_res_set_item_s {
  struct comb_size_opts {
    enum options { n2, n4, n6, n12, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using comb_size_e_ = enumerated<comb_size_opts, true>;
  struct res_set_periodicity_opts {
    enum options {
      n4,
      n5,
      n8,
      n10,
      n16,
      n20,
      n32,
      n40,
      n64,
      n80,
      n160,
      n320,
      n640,
      n1280,
      n2560,
      n5120,
      n10240,
      n20480,
      n40960,
      n81920,
      // ...
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  using res_set_periodicity_e_ = enumerated<res_set_periodicity_opts, true>;
  struct res_repeat_factor_opts {
    enum options { rf1, rf2, rf4, rf6, rf8, rf16, rf32, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_repeat_factor_e_ = enumerated<res_repeat_factor_opts, true>;
  struct res_numof_symbols_opts {
    enum options { n2, n4, n6, n12, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using res_numof_symbols_e_ = enumerated<res_numof_symbols_opts, true>;

  // member variables
  bool                                            ext                                = false;
  bool                                            pr_sbw_present                     = false;
  bool                                            comb_size_present                  = false;
  bool                                            res_set_periodicity_present        = false;
  bool                                            res_repeat_factor_present          = false;
  bool                                            res_numof_symbols_present          = false;
  bool                                            res_set_start_time_and_dur_present = false;
  bool                                            ie_exts_present                    = false;
  uint8_t                                         pr_sbw                             = 1;
  comb_size_e_                                    comb_size;
  res_set_periodicity_e_                          res_set_periodicity;
  res_repeat_factor_e_                            res_repeat_factor;
  res_numof_symbols_e_                            res_numof_symbols;
  requested_dl_prs_res_list_l                     requested_dl_prs_res_list;
  start_time_and_dur_s                            res_set_start_time_and_dur;
  requested_dl_prs_res_set_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultEUTRA ::= SEQUENCE (SIZE (1..8)) OF ResultEUTRA-Item
using result_eutra_l = dyn_array<result_eutra_item_s>;

using result_geran_item_ext_ies_container = protocol_ext_container_empty_l;

// ResultNR ::= SEQUENCE (SIZE (1..8)) OF ResultNR-Item
using result_nr_l = dyn_array<result_nr_item_s>;

// ResultRSRP-EUTRA-Item ::= SEQUENCE
struct result_rsrp_eutra_item_s {
  bool                                     ext               = false;
  bool                                     cgi_eutra_present = false;
  bool                                     ie_exts_present   = false;
  uint16_t                                 pci_eutra         = 0;
  uint32_t                                 earfcn            = 0;
  cgi_eutra_s                              cgi_eutra;
  uint8_t                                  value_rsrp_eutra = 0;
  result_rsrp_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultRSRQ-EUTRA-Item ::= SEQUENCE
struct result_rsrq_eutra_item_s {
  bool                                     ext              = false;
  bool                                     cgi_utra_present = false;
  bool                                     ie_exts_present  = false;
  uint16_t                                 pci_eutra        = 0;
  uint32_t                                 earfcn           = 0;
  cgi_eutra_s                              cgi_utra;
  uint8_t                                  value_rsrq_eutra = 0;
  result_rsrq_eutra_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using result_utran_item_ext_ies_container = protocol_ext_container_empty_l;

// SCS-SpecificCarrier ::= SEQUENCE
struct scs_specific_carrier_s {
  struct subcarrier_spacing_opts {
    enum options { khz15, khz30, khz60, khz120, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using subcarrier_spacing_e_ = enumerated<subcarrier_spacing_opts, true>;

  // member variables
  bool                                   ext               = false;
  bool                                   ie_exts_present   = false;
  uint16_t                               offset_to_carrier = 0;
  subcarrier_spacing_e_                  subcarrier_spacing;
  uint16_t                               carrier_bw = 1;
  scs_specific_carrier_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SemipersistentSRS-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct semipersistent_srs_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { srs_spatial_relation, srs_spatial_relation_per_srs_res, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    spatial_relation_info_s&              srs_spatial_relation();
    spatial_relation_per_srs_res_s&       srs_spatial_relation_per_srs_res();
    const spatial_relation_info_s&        srs_spatial_relation() const;
    const spatial_relation_per_srs_res_s& srs_spatial_relation_per_srs_res() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRSConfig ::= SEQUENCE
struct srs_cfg_s {
  bool                      ext             = false;
  bool                      ie_exts_present = false;
  srs_res_list_l            srs_res_list;
  pos_srs_res_list_l        pos_srs_res_list;
  srs_res_set_list_l        srs_res_set_list;
  pos_srs_res_set_list_l    pos_srs_res_set_list;
  srs_cfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeStamp ::= SEQUENCE
struct time_stamp_s {
  bool                             ext               = false;
  bool                             meas_time_present = false;
  bool                             ie_ext_present    = false;
  uint16_t                         sys_frame_num     = 0;
  time_stamp_slot_idx_c            slot_idx;
  fixed_bitstring<64, false, true> meas_time;
  time_stamp_ext_ies_container     ie_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TP-Type-EUTRA ::= ENUMERATED
struct tp_type_eutra_opts {
  enum options { prs_only_tp, /*...*/ nulltype } value;

  const char* to_string() const;
};
using tp_type_eutra_e = enumerated<tp_type_eutra_opts, true>;

// TRPInformationTypeResponseItem ::= CHOICE
struct trp_info_type_resp_item_c {
  struct types_opts {
    enum options {
      pci_nr,
      cgi_nr,
      arfcn,
      prs_cfg,
      ss_binfo,
      sfn_initisation_time,
      spatial_direction_info,
      geographical_coordinates,
      choice_ext,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  trp_info_type_resp_item_c() = default;
  trp_info_type_resp_item_c(const trp_info_type_resp_item_c& other);
  trp_info_type_resp_item_c& operator=(const trp_info_type_resp_item_c& other);
  ~trp_info_type_resp_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& pci_nr()
  {
    assert_choice_type(types::pci_nr, type_, "TRPInformationTypeResponseItem");
    return c.get<uint16_t>();
  }
  cgi_nr_s& cgi_nr()
  {
    assert_choice_type(types::cgi_nr, type_, "TRPInformationTypeResponseItem");
    return c.get<cgi_nr_s>();
  }
  uint32_t& arfcn()
  {
    assert_choice_type(types::arfcn, type_, "TRPInformationTypeResponseItem");
    return c.get<uint32_t>();
  }
  prs_cfg_s& prs_cfg()
  {
    assert_choice_type(types::prs_cfg, type_, "TRPInformationTypeResponseItem");
    return c.get<prs_cfg_s>();
  }
  ssb_info_s& ss_binfo()
  {
    assert_choice_type(types::ss_binfo, type_, "TRPInformationTypeResponseItem");
    return c.get<ssb_info_s>();
  }
  fixed_bitstring<64, false, true>& sfn_initisation_time()
  {
    assert_choice_type(types::sfn_initisation_time, type_, "TRPInformationTypeResponseItem");
    return c.get<fixed_bitstring<64, false, true>>();
  }
  spatial_direction_info_s& spatial_direction_info()
  {
    assert_choice_type(types::spatial_direction_info, type_, "TRPInformationTypeResponseItem");
    return c.get<spatial_direction_info_s>();
  }
  geographical_coordinates_s& geographical_coordinates()
  {
    assert_choice_type(types::geographical_coordinates, type_, "TRPInformationTypeResponseItem");
    return c.get<geographical_coordinates_s>();
  }
  protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TRPInformationTypeResponseItem");
    return c.get<protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>>();
  }
  const uint16_t& pci_nr() const
  {
    assert_choice_type(types::pci_nr, type_, "TRPInformationTypeResponseItem");
    return c.get<uint16_t>();
  }
  const cgi_nr_s& cgi_nr() const
  {
    assert_choice_type(types::cgi_nr, type_, "TRPInformationTypeResponseItem");
    return c.get<cgi_nr_s>();
  }
  const uint32_t& arfcn() const
  {
    assert_choice_type(types::arfcn, type_, "TRPInformationTypeResponseItem");
    return c.get<uint32_t>();
  }
  const prs_cfg_s& prs_cfg() const
  {
    assert_choice_type(types::prs_cfg, type_, "TRPInformationTypeResponseItem");
    return c.get<prs_cfg_s>();
  }
  const ssb_info_s& ss_binfo() const
  {
    assert_choice_type(types::ss_binfo, type_, "TRPInformationTypeResponseItem");
    return c.get<ssb_info_s>();
  }
  const fixed_bitstring<64, false, true>& sfn_initisation_time() const
  {
    assert_choice_type(types::sfn_initisation_time, type_, "TRPInformationTypeResponseItem");
    return c.get<fixed_bitstring<64, false, true>>();
  }
  const spatial_direction_info_s& spatial_direction_info() const
  {
    assert_choice_type(types::spatial_direction_info, type_, "TRPInformationTypeResponseItem");
    return c.get<spatial_direction_info_s>();
  }
  const geographical_coordinates_s& geographical_coordinates() const
  {
    assert_choice_type(types::geographical_coordinates, type_, "TRPInformationTypeResponseItem");
    return c.get<geographical_coordinates_s>();
  }
  const protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TRPInformationTypeResponseItem");
    return c.get<protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>>();
  }
  uint16_t&                                                          set_pci_nr();
  cgi_nr_s&                                                          set_cgi_nr();
  uint32_t&                                                          set_arfcn();
  prs_cfg_s&                                                         set_prs_cfg();
  ssb_info_s&                                                        set_ss_binfo();
  fixed_bitstring<64, false, true>&                                  set_sfn_initisation_time();
  spatial_direction_info_s&                                          set_spatial_direction_info();
  geographical_coordinates_s&                                        set_geographical_coordinates();
  protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<cgi_nr_s,
                  fixed_bitstring<64, false, true>,
                  geographical_coordinates_s,
                  protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>,
                  prs_cfg_s,
                  spatial_direction_info_s,
                  ssb_info_s>
      c;

  void destroy_();
};

// TRP-MeasurementRequestItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct trp_meas_request_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { cell_id, ao_a_search_win, nof_trp_rx_teg, nof_trp_rx_tx_teg, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cgi_nr_s&                  cell_id();
    ao_a_assist_info_s&        ao_a_search_win();
    nof_trp_rx_teg_e&          nof_trp_rx_teg();
    nof_trp_rx_tx_teg_e&       nof_trp_rx_tx_teg();
    const cgi_nr_s&            cell_id() const;
    const ao_a_assist_info_s&  ao_a_search_win() const;
    const nof_trp_rx_teg_e&    nof_trp_rx_teg() const;
    const nof_trp_rx_tx_teg_e& nof_trp_rx_tx_teg() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct trp_meas_result_item_ext_ies_container {
  bool                      srs_restype_present             = false;
  bool                      arp_id_present                  = false;
  bool                      lo_s_n_lo_si_nformation_present = false;
  srs_restype_s             srs_restype;
  uint8_t                   arp_id;
  lo_s_n_lo_si_nformation_c lo_s_n_lo_si_nformation;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRP-MeasurementUpdateItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct trp_meas_upd_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { nof_trp_rx_teg, nof_trp_rx_tx_teg, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ext_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nof_trp_rx_teg_e&          nof_trp_rx_teg();
    nof_trp_rx_tx_teg_e&       nof_trp_rx_tx_teg();
    const nof_trp_rx_teg_e&    nof_trp_rx_teg() const;
    const nof_trp_rx_tx_teg_e& nof_trp_rx_tx_teg() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TrpMeasuredResultsValue ::= CHOICE
struct trp_measured_results_value_c {
  struct types_opts {
    enum options { ul_angle_of_arrival, ul_srs_rsrp, ul_rtoa, gnb_rx_tx_time_diff, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  trp_measured_results_value_c() = default;
  trp_measured_results_value_c(const trp_measured_results_value_c& other);
  trp_measured_results_value_c& operator=(const trp_measured_results_value_c& other);
  ~trp_measured_results_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ul_ao_a_s& ul_angle_of_arrival()
  {
    assert_choice_type(types::ul_angle_of_arrival, type_, "TrpMeasuredResultsValue");
    return c.get<ul_ao_a_s>();
  }
  uint8_t& ul_srs_rsrp()
  {
    assert_choice_type(types::ul_srs_rsrp, type_, "TrpMeasuredResultsValue");
    return c.get<uint8_t>();
  }
  ul_rtoameas_s& ul_rtoa()
  {
    assert_choice_type(types::ul_rtoa, type_, "TrpMeasuredResultsValue");
    return c.get<ul_rtoameas_s>();
  }
  gnb_rx_tx_time_diff_s& gnb_rx_tx_time_diff()
  {
    assert_choice_type(types::gnb_rx_tx_time_diff, type_, "TrpMeasuredResultsValue");
    return c.get<gnb_rx_tx_time_diff_s>();
  }
  protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "TrpMeasuredResultsValue");
    return c.get<protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>>();
  }
  const ul_ao_a_s& ul_angle_of_arrival() const
  {
    assert_choice_type(types::ul_angle_of_arrival, type_, "TrpMeasuredResultsValue");
    return c.get<ul_ao_a_s>();
  }
  const uint8_t& ul_srs_rsrp() const
  {
    assert_choice_type(types::ul_srs_rsrp, type_, "TrpMeasuredResultsValue");
    return c.get<uint8_t>();
  }
  const ul_rtoameas_s& ul_rtoa() const
  {
    assert_choice_type(types::ul_rtoa, type_, "TrpMeasuredResultsValue");
    return c.get<ul_rtoameas_s>();
  }
  const gnb_rx_tx_time_diff_s& gnb_rx_tx_time_diff() const
  {
    assert_choice_type(types::gnb_rx_tx_time_diff, type_, "TrpMeasuredResultsValue");
    return c.get<gnb_rx_tx_time_diff_s>();
  }
  const protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "TrpMeasuredResultsValue");
    return c.get<protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>>();
  }
  ul_ao_a_s&                                                            set_ul_angle_of_arrival();
  uint8_t&                                                              set_ul_srs_rsrp();
  ul_rtoameas_s&                                                        set_ul_rtoa();
  gnb_rx_tx_time_diff_s&                                                set_gnb_rx_tx_time_diff();
  protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<gnb_rx_tx_time_diff_s,
                  protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>,
                  ul_ao_a_s,
                  ul_rtoameas_s>
      c;

  void destroy_();
};

// WLANMeasurementQuantitiesValue ::= ENUMERATED
struct wlan_meas_quantities_value_opts {
  enum options { wlan, /*...*/ nulltype } value;

  const char* to_string() const;
};
using wlan_meas_quantities_value_e = enumerated<wlan_meas_quantities_value_opts, true>;

using wlan_meas_quantities_value_ext_ies_container = protocol_ext_container_empty_l;

// ActiveULBWP ::= SEQUENCE
struct active_ul_bwp_s {
  struct subcarrier_spacing_opts {
    enum options { khz15, khz30, khz60, khz120, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using subcarrier_spacing_e_ = enumerated<subcarrier_spacing_opts, true>;
  struct cp_opts {
    enum options { normal, extended, nulltype } value;

    const char* to_string() const;
  };
  using cp_e_ = enumerated<cp_opts>;
  struct shift7dot5k_hz_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using shift7dot5k_hz_e_ = enumerated<shift7dot5k_hz_opts, true>;

  // member variables
  bool                            ext                    = false;
  bool                            shift7dot5k_hz_present = false;
  bool                            ie_exts_present        = false;
  uint16_t                        location_and_bw        = 0;
  subcarrier_spacing_e_           subcarrier_spacing;
  cp_e_                           cp;
  uint16_t                        tx_direct_current_location = 0;
  shift7dot5k_hz_e_               shift7dot5k_hz;
  srs_cfg_s                       srs_cfg;
  active_ul_bwp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AperiodicSRSResourceTriggerList ::= SEQUENCE (SIZE (1..3)) OF INTEGER (1..3)
using aperiodic_srs_res_trigger_list_l = bounded_array<uint8_t, 3>;

// BroadcastPeriodicity ::= ENUMERATED
struct broadcast_periodicity_opts {
  enum options { ms80, ms160, ms320, ms640, ms1280, ms2560, ms5120, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using broadcast_periodicity_e = enumerated<broadcast_periodicity_opts, true>;

// BandwidthSRS-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using bw_srs_ext_ies_o = protocol_ies_empty_o;

using carrier_freq_ext_ies_container = protocol_ext_container_empty_l;

using crit_diagnostics_ie_list_ext_ies_container = protocol_ext_container_empty_l;

// MeasurementQuantities-Item ::= SEQUENCE
struct meas_quantities_item_s {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  meas_quantities_value_e                 meas_quantities_value;
  meas_quantities_value_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OTDOACell-Information-Item ::= CHOICE
struct otdoa_cell_info_item_c {
  struct types_opts {
    enum options {
      pci_eutra,
      cgi_eutra,
      tac,
      earfcn,
      prs_bw_eutra,
      prs_cfg_idx_eutra,
      cp_len_eutra,
      nof_dl_frames_eutra,
      nof_ant_ports_eutra,
      sfn_initisation_time_eutra,
      ng_ran_access_point_position,
      prs_muting_cfg_eutra,
      prsid_eutra,
      tpid_eutra,
      tp_type_eutra,
      nof_dl_frames_extended_eutra,
      crs_cp_len_eutra,
      dl_bw_eutra,
      prs_occasion_group_eutra,
      prs_freq_hop_cfg_eutra,
      choice_ext,
      nulltype
    } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  otdoa_cell_info_item_c() = default;
  otdoa_cell_info_item_c(const otdoa_cell_info_item_c& other);
  otdoa_cell_info_item_c& operator=(const otdoa_cell_info_item_c& other);
  ~otdoa_cell_info_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& pci_eutra()
  {
    assert_choice_type(types::pci_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint16_t>();
  }
  cgi_eutra_s& cgi_eutra()
  {
    assert_choice_type(types::cgi_eutra, type_, "OTDOACell-Information-Item");
    return c.get<cgi_eutra_s>();
  }
  fixed_octstring<3, true>& tac()
  {
    assert_choice_type(types::tac, type_, "OTDOACell-Information-Item");
    return c.get<fixed_octstring<3, true>>();
  }
  uint32_t& earfcn()
  {
    assert_choice_type(types::earfcn, type_, "OTDOACell-Information-Item");
    return c.get<uint32_t>();
  }
  prs_bw_eutra_e& prs_bw_eutra()
  {
    assert_choice_type(types::prs_bw_eutra, type_, "OTDOACell-Information-Item");
    return c.get<prs_bw_eutra_e>();
  }
  uint16_t& prs_cfg_idx_eutra()
  {
    assert_choice_type(types::prs_cfg_idx_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint16_t>();
  }
  cp_len_eutra_e& cp_len_eutra()
  {
    assert_choice_type(types::cp_len_eutra, type_, "OTDOACell-Information-Item");
    return c.get<cp_len_eutra_e>();
  }
  nof_dl_frames_eutra_e& nof_dl_frames_eutra()
  {
    assert_choice_type(types::nof_dl_frames_eutra, type_, "OTDOACell-Information-Item");
    return c.get<nof_dl_frames_eutra_e>();
  }
  nof_ant_ports_eutra_e& nof_ant_ports_eutra()
  {
    assert_choice_type(types::nof_ant_ports_eutra, type_, "OTDOACell-Information-Item");
    return c.get<nof_ant_ports_eutra_e>();
  }
  fixed_bitstring<64, false, true>& sfn_initisation_time_eutra()
  {
    assert_choice_type(types::sfn_initisation_time_eutra, type_, "OTDOACell-Information-Item");
    return c.get<fixed_bitstring<64, false, true>>();
  }
  ng_ran_access_point_position_s& ng_ran_access_point_position()
  {
    assert_choice_type(types::ng_ran_access_point_position, type_, "OTDOACell-Information-Item");
    return c.get<ng_ran_access_point_position_s>();
  }
  prs_muting_cfg_eutra_c& prs_muting_cfg_eutra()
  {
    assert_choice_type(types::prs_muting_cfg_eutra, type_, "OTDOACell-Information-Item");
    return c.get<prs_muting_cfg_eutra_c>();
  }
  uint16_t& prsid_eutra()
  {
    assert_choice_type(types::prsid_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint16_t>();
  }
  uint16_t& tpid_eutra()
  {
    assert_choice_type(types::tpid_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint16_t>();
  }
  tp_type_eutra_e& tp_type_eutra()
  {
    assert_choice_type(types::tp_type_eutra, type_, "OTDOACell-Information-Item");
    return c.get<tp_type_eutra_e>();
  }
  uint8_t& nof_dl_frames_extended_eutra()
  {
    assert_choice_type(types::nof_dl_frames_extended_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint8_t>();
  }
  cp_len_eutra_e& crs_cp_len_eutra()
  {
    assert_choice_type(types::crs_cp_len_eutra, type_, "OTDOACell-Information-Item");
    return c.get<cp_len_eutra_e>();
  }
  dl_bw_eutra_e& dl_bw_eutra()
  {
    assert_choice_type(types::dl_bw_eutra, type_, "OTDOACell-Information-Item");
    return c.get<dl_bw_eutra_e>();
  }
  prs_occasion_group_eutra_e& prs_occasion_group_eutra()
  {
    assert_choice_type(types::prs_occasion_group_eutra, type_, "OTDOACell-Information-Item");
    return c.get<prs_occasion_group_eutra_e>();
  }
  prs_freq_hop_cfg_eutra_s& prs_freq_hop_cfg_eutra()
  {
    assert_choice_type(types::prs_freq_hop_cfg_eutra, type_, "OTDOACell-Information-Item");
    return c.get<prs_freq_hop_cfg_eutra_s>();
  }
  protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "OTDOACell-Information-Item");
    return c.get<protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>>();
  }
  const uint16_t& pci_eutra() const
  {
    assert_choice_type(types::pci_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint16_t>();
  }
  const cgi_eutra_s& cgi_eutra() const
  {
    assert_choice_type(types::cgi_eutra, type_, "OTDOACell-Information-Item");
    return c.get<cgi_eutra_s>();
  }
  const fixed_octstring<3, true>& tac() const
  {
    assert_choice_type(types::tac, type_, "OTDOACell-Information-Item");
    return c.get<fixed_octstring<3, true>>();
  }
  const uint32_t& earfcn() const
  {
    assert_choice_type(types::earfcn, type_, "OTDOACell-Information-Item");
    return c.get<uint32_t>();
  }
  const prs_bw_eutra_e& prs_bw_eutra() const
  {
    assert_choice_type(types::prs_bw_eutra, type_, "OTDOACell-Information-Item");
    return c.get<prs_bw_eutra_e>();
  }
  const uint16_t& prs_cfg_idx_eutra() const
  {
    assert_choice_type(types::prs_cfg_idx_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint16_t>();
  }
  const cp_len_eutra_e& cp_len_eutra() const
  {
    assert_choice_type(types::cp_len_eutra, type_, "OTDOACell-Information-Item");
    return c.get<cp_len_eutra_e>();
  }
  const nof_dl_frames_eutra_e& nof_dl_frames_eutra() const
  {
    assert_choice_type(types::nof_dl_frames_eutra, type_, "OTDOACell-Information-Item");
    return c.get<nof_dl_frames_eutra_e>();
  }
  const nof_ant_ports_eutra_e& nof_ant_ports_eutra() const
  {
    assert_choice_type(types::nof_ant_ports_eutra, type_, "OTDOACell-Information-Item");
    return c.get<nof_ant_ports_eutra_e>();
  }
  const fixed_bitstring<64, false, true>& sfn_initisation_time_eutra() const
  {
    assert_choice_type(types::sfn_initisation_time_eutra, type_, "OTDOACell-Information-Item");
    return c.get<fixed_bitstring<64, false, true>>();
  }
  const ng_ran_access_point_position_s& ng_ran_access_point_position() const
  {
    assert_choice_type(types::ng_ran_access_point_position, type_, "OTDOACell-Information-Item");
    return c.get<ng_ran_access_point_position_s>();
  }
  const prs_muting_cfg_eutra_c& prs_muting_cfg_eutra() const
  {
    assert_choice_type(types::prs_muting_cfg_eutra, type_, "OTDOACell-Information-Item");
    return c.get<prs_muting_cfg_eutra_c>();
  }
  const uint16_t& prsid_eutra() const
  {
    assert_choice_type(types::prsid_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint16_t>();
  }
  const uint16_t& tpid_eutra() const
  {
    assert_choice_type(types::tpid_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint16_t>();
  }
  const tp_type_eutra_e& tp_type_eutra() const
  {
    assert_choice_type(types::tp_type_eutra, type_, "OTDOACell-Information-Item");
    return c.get<tp_type_eutra_e>();
  }
  const uint8_t& nof_dl_frames_extended_eutra() const
  {
    assert_choice_type(types::nof_dl_frames_extended_eutra, type_, "OTDOACell-Information-Item");
    return c.get<uint8_t>();
  }
  const cp_len_eutra_e& crs_cp_len_eutra() const
  {
    assert_choice_type(types::crs_cp_len_eutra, type_, "OTDOACell-Information-Item");
    return c.get<cp_len_eutra_e>();
  }
  const dl_bw_eutra_e& dl_bw_eutra() const
  {
    assert_choice_type(types::dl_bw_eutra, type_, "OTDOACell-Information-Item");
    return c.get<dl_bw_eutra_e>();
  }
  const prs_occasion_group_eutra_e& prs_occasion_group_eutra() const
  {
    assert_choice_type(types::prs_occasion_group_eutra, type_, "OTDOACell-Information-Item");
    return c.get<prs_occasion_group_eutra_e>();
  }
  const prs_freq_hop_cfg_eutra_s& prs_freq_hop_cfg_eutra() const
  {
    assert_choice_type(types::prs_freq_hop_cfg_eutra, type_, "OTDOACell-Information-Item");
    return c.get<prs_freq_hop_cfg_eutra_s>();
  }
  const protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "OTDOACell-Information-Item");
    return c.get<protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>>();
  }
  uint16_t&                                                      set_pci_eutra();
  cgi_eutra_s&                                                   set_cgi_eutra();
  fixed_octstring<3, true>&                                      set_tac();
  uint32_t&                                                      set_earfcn();
  prs_bw_eutra_e&                                                set_prs_bw_eutra();
  uint16_t&                                                      set_prs_cfg_idx_eutra();
  cp_len_eutra_e&                                                set_cp_len_eutra();
  nof_dl_frames_eutra_e&                                         set_nof_dl_frames_eutra();
  nof_ant_ports_eutra_e&                                         set_nof_ant_ports_eutra();
  fixed_bitstring<64, false, true>&                              set_sfn_initisation_time_eutra();
  ng_ran_access_point_position_s&                                set_ng_ran_access_point_position();
  prs_muting_cfg_eutra_c&                                        set_prs_muting_cfg_eutra();
  uint16_t&                                                      set_prsid_eutra();
  uint16_t&                                                      set_tpid_eutra();
  tp_type_eutra_e&                                               set_tp_type_eutra();
  uint8_t&                                                       set_nof_dl_frames_extended_eutra();
  cp_len_eutra_e&                                                set_crs_cp_len_eutra();
  dl_bw_eutra_e&                                                 set_dl_bw_eutra();
  prs_occasion_group_eutra_e&                                    set_prs_occasion_group_eutra();
  prs_freq_hop_cfg_eutra_s&                                      set_prs_freq_hop_cfg_eutra();
  protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<cgi_eutra_s,
                  fixed_bitstring<64, false, true>,
                  fixed_octstring<3, true>,
                  ng_ran_access_point_position_s,
                  protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>,
                  prs_freq_hop_cfg_eutra_s,
                  prs_muting_cfg_eutra_c>
      c;

  void destroy_();
};

// OTDOA-Information-Item ::= ENUMERATED
struct otdoa_info_item_opts {
  enum options {
    pci,
    cgi,
    tac,
    earfcn,
    prs_bw,
    prs_cfg_idx,
    cp_len,
    no_dl_frames,
    no_ant_ports,
    sfn_init_time,
    ng_ran_access_point_position,
    prsmutingcfg,
    prsid,
    tpid,
    tp_type,
    crs_cp_len,
    dl_bw,
    multipleprs_cfgsper_cell,
    prs_occasion_group,
    prs_freq_hop_cfg,
    // ...
    tdd_cfg,
    nulltype
  } value;

  const char* to_string() const;
};
using otdoa_info_item_e = enumerated<otdoa_info_item_opts, true, 1>;

using otdoa_info_type_item_ext_ies_container = protocol_ext_container_empty_l;

// OtherRATMeasurementQuantities-Item ::= SEQUENCE
struct other_rat_meas_quantities_item_s {
  bool                                              ext             = false;
  bool                                              ie_exts_present = false;
  other_rat_meas_quantities_value_e                 other_rat_meas_quantities_value;
  other_rat_meas_quantities_value_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherRATMeasuredResultsValue-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct other_rat_measured_results_value_ext_ie_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { result_nr, result_eutra, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    result_nr_l&          result_nr();
    result_eutra_l&       result_eutra();
    const result_nr_l&    result_nr() const;
    const result_eutra_l& result_eutra() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using pathloss_ref_info_ext_ies_container = protocol_ext_container_empty_l;

// PathlossReferenceSignal ::= CHOICE
struct pathloss_ref_sig_c {
  struct types_opts {
    enum options { ssb_ref, dl_prs_ref, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  pathloss_ref_sig_c() = default;
  pathloss_ref_sig_c(const pathloss_ref_sig_c& other);
  pathloss_ref_sig_c& operator=(const pathloss_ref_sig_c& other);
  ~pathloss_ref_sig_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ssb_s& ssb_ref()
  {
    assert_choice_type(types::ssb_ref, type_, "PathlossReferenceSignal");
    return c.get<ssb_s>();
  }
  dl_prs_s& dl_prs_ref()
  {
    assert_choice_type(types::dl_prs_ref, type_, "PathlossReferenceSignal");
    return c.get<dl_prs_s>();
  }
  protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "PathlossReferenceSignal");
    return c.get<protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>>();
  }
  const ssb_s& ssb_ref() const
  {
    assert_choice_type(types::ssb_ref, type_, "PathlossReferenceSignal");
    return c.get<ssb_s>();
  }
  const dl_prs_s& dl_prs_ref() const
  {
    assert_choice_type(types::dl_prs_ref, type_, "PathlossReferenceSignal");
    return c.get<dl_prs_s>();
  }
  const protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "PathlossReferenceSignal");
    return c.get<protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>>();
  }
  ssb_s&                                                     set_ssb_ref();
  dl_prs_s&                                                  set_dl_prs_ref();
  protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>& set_choice_ext();

private:
  types                                                                                       type_;
  choice_buffer_t<dl_prs_s, protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>, ssb_s> c;

  void destroy_();
};

// PeriodicityItem ::= ENUMERATED
struct periodicity_item_opts {
  enum options {
    ms0dot125,
    ms0dot25,
    ms0dot5,
    ms0dot625,
    ms1,
    ms1dot25,
    ms2,
    ms2dot5,
    ms4dot,
    ms5,
    ms8,
    ms10,
    ms16,
    ms20,
    ms32,
    ms40,
    ms64,
    ms80m,
    ms160,
    ms320,
    ms640m,
    ms1280,
    ms2560,
    ms5120,
    ms10240,
    // ...
    nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using periodicity_item_e = enumerated<periodicity_item_opts, true>;

// PosSIBs ::= SEQUENCE (SIZE (1..32)) OF PosSIBs-item
using pos_sibs_l = dyn_array<pos_sibs_item_s_>;

// PRSTransmissionOffIndication ::= CHOICE
struct prs_tx_off_ind_c {
  struct types_opts {
    enum options { prs_tx_off_per_trp, prs_tx_off_per_res_set, prs_tx_off_per_res, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  prs_tx_off_ind_c() = default;
  prs_tx_off_ind_c(const prs_tx_off_ind_c& other);
  prs_tx_off_ind_c& operator=(const prs_tx_off_ind_c& other);
  ~prs_tx_off_ind_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  prs_tx_off_per_res_set_l& prs_tx_off_per_res_set()
  {
    assert_choice_type(types::prs_tx_off_per_res_set, type_, "PRSTransmissionOffIndication");
    return c.get<prs_tx_off_per_res_set_l>();
  }
  prs_tx_off_per_res_l& prs_tx_off_per_res()
  {
    assert_choice_type(types::prs_tx_off_per_res, type_, "PRSTransmissionOffIndication");
    return c.get<prs_tx_off_per_res_l>();
  }
  protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "PRSTransmissionOffIndication");
    return c.get<protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>>();
  }
  const prs_tx_off_per_res_set_l& prs_tx_off_per_res_set() const
  {
    assert_choice_type(types::prs_tx_off_per_res_set, type_, "PRSTransmissionOffIndication");
    return c.get<prs_tx_off_per_res_set_l>();
  }
  const prs_tx_off_per_res_l& prs_tx_off_per_res() const
  {
    assert_choice_type(types::prs_tx_off_per_res, type_, "PRSTransmissionOffIndication");
    return c.get<prs_tx_off_per_res_l>();
  }
  const protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "PRSTransmissionOffIndication");
    return c.get<protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>>();
  }
  void                                                      set_prs_tx_off_per_trp();
  prs_tx_off_per_res_set_l&                                 set_prs_tx_off_per_res_set();
  prs_tx_off_per_res_l&                                     set_prs_tx_off_per_res();
  protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>,
                  prs_tx_off_per_res_l,
                  prs_tx_off_per_res_set_l>
      c;

  void destroy_();
};

using prs_tx_off_info_ext_ies_container = protocol_ext_container_empty_l;

// RequestedDLPRSResourceSet-List ::= SEQUENCE (SIZE (1..8)) OF RequestedDLPRSResourceSet-Item
using requested_dl_prs_res_set_list_l = dyn_array<requested_dl_prs_res_set_item_s>;

using requested_dl_prs_tx_characteristics_ext_ies_container = protocol_ext_container_empty_l;

// ResultGERAN-Item ::= SEQUENCE
struct result_geran_item_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  uint16_t                            bcch            = 0;
  uint8_t                             pci_geran       = 0;
  uint8_t                             rssi            = 0;
  result_geran_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultRSRP-EUTRA ::= SEQUENCE (SIZE (1..9)) OF ResultRSRP-EUTRA-Item
using result_rsrp_eutra_l = dyn_array<result_rsrp_eutra_item_s>;

// ResultRSRQ-EUTRA ::= SEQUENCE (SIZE (1..9)) OF ResultRSRQ-EUTRA-Item
using result_rsrq_eutra_l = dyn_array<result_rsrq_eutra_item_s>;

// ResultUTRAN-Item ::= SEQUENCE
struct result_utran_item_s {
  struct pci_utran_c_ {
    struct types_opts {
      enum options { phys_cell_i_du_tra_fdd, phys_cell_i_du_tra_tdd, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    pci_utran_c_() = default;
    pci_utran_c_(const pci_utran_c_& other);
    pci_utran_c_& operator=(const pci_utran_c_& other);
    ~pci_utran_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& phys_cell_i_du_tra_fdd()
    {
      assert_choice_type(types::phys_cell_i_du_tra_fdd, type_, "physCellIDUTRAN");
      return c.get<uint16_t>();
    }
    uint8_t& phys_cell_i_du_tra_tdd()
    {
      assert_choice_type(types::phys_cell_i_du_tra_tdd, type_, "physCellIDUTRAN");
      return c.get<uint8_t>();
    }
    const uint16_t& phys_cell_i_du_tra_fdd() const
    {
      assert_choice_type(types::phys_cell_i_du_tra_fdd, type_, "physCellIDUTRAN");
      return c.get<uint16_t>();
    }
    const uint8_t& phys_cell_i_du_tra_tdd() const
    {
      assert_choice_type(types::phys_cell_i_du_tra_tdd, type_, "physCellIDUTRAN");
      return c.get<uint8_t>();
    }
    uint16_t& set_phys_cell_i_du_tra_fdd();
    uint8_t&  set_phys_cell_i_du_tra_tdd();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                                ext                = false;
  bool                                utra_rs_cp_present = false;
  bool                                utra_ec_n0_present = false;
  bool                                ie_exts_present    = false;
  uint16_t                            uarfcn             = 0;
  pci_utran_c_                        pci_utran;
  int8_t                              utra_rs_cp = -5;
  uint8_t                             utra_ec_n0 = 0;
  result_utran_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using search_win_info_ext_ies_container = protocol_ext_container_empty_l;

using srs_carrier_list_item_ext_ies_container = protocol_ext_container_empty_l;

// SRSResourceSet-Item-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct srs_res_set_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { srs_spatial_relation_per_srs_res, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srs_spatial_relation_per_srs_res; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    spatial_relation_per_srs_res_s&       srs_spatial_relation_per_srs_res() { return c; }
    const spatial_relation_per_srs_res_s& srs_spatial_relation_per_srs_res() const { return c; }

  private:
    spatial_relation_per_srs_res_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using srs_res_trigger_ext_ies_container = protocol_ext_container_empty_l;

using sys_info_ext_ies_container = protocol_ext_container_empty_l;

using trp_info_ext_ies_container = protocol_ext_container_empty_l;

// TRPInformationTypeItem ::= ENUMERATED
struct trp_info_type_item_opts {
  enum options {
    nr_pci,
    ng_ran_cgi,
    arfcn,
    prs_cfg,
    ssb_info,
    sfn_init_time,
    spatial_direct_info,
    geo_coord,
    // ...
    trp_type,
    ondemand_pr_si_nfo,
    trp_tx_teg,
    beam_ant_info,
    nulltype
  } value;

  const char* to_string() const;
};
using trp_info_type_item_e = enumerated<trp_info_type_item_opts, true, 4>;

// TRPInformationTypeResponseList ::= SEQUENCE (SIZE (1..64)) OF TRPInformationTypeResponseItem
using trp_info_type_resp_list_l = dyn_array<trp_info_type_resp_item_c>;

// TRP-MeasurementResponseItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct trp_meas_resp_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { cell_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cell_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cgi_nr_s&       cell_id() { return c; }
    const cgi_nr_s& cell_id() const { return c; }

  private:
    cgi_nr_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TrpMeasurementResultItem ::= SEQUENCE
struct trp_meas_result_item_s {
  bool                                   ext                    = false;
  bool                                   meas_quality_present   = false;
  bool                                   meas_beam_info_present = false;
  bool                                   ie_exts_present        = false;
  trp_measured_results_value_c           measured_results_value;
  time_stamp_s                           time_stamp;
  trp_meas_quality_c                     meas_quality;
  meas_beam_info_s                       meas_beam_info;
  trp_meas_result_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TypeOfError ::= ENUMERATED
struct type_of_error_opts {
  enum options { not_understood, missing, /*...*/ nulltype } value;

  const char* to_string() const;
};
using type_of_error_e = enumerated<type_of_error_opts, true>;

// UETxTEGAssociationItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct ue_tx_teg_assoc_item_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { ue_tx_timing_error_margin, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_tx_timing_error_margin; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    timing_error_margin_e&       ue_tx_timing_error_margin() { return c; }
    const timing_error_margin_e& ue_tx_timing_error_margin() const { return c; }

  private:
    timing_error_margin_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UplinkChannelBW-PerSCS-List ::= SEQUENCE (SIZE (1..5)) OF SCS-SpecificCarrier
using ul_ch_bw_per_scs_list_l = dyn_array<scs_specific_carrier_s>;

// WLANMeasurementQuantities-Item ::= SEQUENCE
struct wlan_meas_quantities_item_s {
  bool                                         ext             = false;
  bool                                         ie_exts_present = false;
  wlan_meas_quantities_value_e                 wlan_meas_quantities_value;
  wlan_meas_quantities_value_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using aperiodic_srs_ext_ies_container = protocol_ext_container_empty_l;

using assist_info_fail_list_ext_ies_container = protocol_ext_container_empty_l;

// CarrierFreq ::= SEQUENCE
struct carrier_freq_s {
  bool                           ext               = false;
  bool                           ie_exts_present   = false;
  uint32_t                       point_a           = 0;
  uint16_t                       offset_to_carrier = 0;
  carrier_freq_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Cause-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using cause_ext_ie_o = protocol_ies_empty_o;

struct crit_diagnostics_ie_list_item_s_ {
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  crit_e                                     ie_crit;
  uint32_t                                   ie_id = 0;
  type_of_error_e                            type_of_error;
  crit_diagnostics_ie_list_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-CID-MeasurementResult-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct e_c_id_meas_result_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { geographical_coordinates, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::geographical_coordinates; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    geographical_coordinates_s&       geographical_coordinates() { return c; }
    const geographical_coordinates_s& geographical_coordinates() const { return c; }

  private:
    geographical_coordinates_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementQuantities-ItemIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_quantities_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { meas_quantities_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::meas_quantities_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_quantities_item_s&       meas_quantities_item() { return c; }
    const meas_quantities_item_s& meas_quantities_item() const { return c; }

  private:
    meas_quantities_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasuredResultsValue ::= CHOICE
struct measured_results_value_c {
  struct types_opts {
    enum options {
      value_angle_of_arrival_eutra,
      value_timing_advance_type1_eutra,
      value_timing_advance_type2_eutra,
      result_rsrp_eutra,
      result_rsrq_eutra,
      choice_ext,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  measured_results_value_c() = default;
  measured_results_value_c(const measured_results_value_c& other);
  measured_results_value_c& operator=(const measured_results_value_c& other);
  ~measured_results_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& value_angle_of_arrival_eutra()
  {
    assert_choice_type(types::value_angle_of_arrival_eutra, type_, "MeasuredResultsValue");
    return c.get<uint16_t>();
  }
  uint16_t& value_timing_advance_type1_eutra()
  {
    assert_choice_type(types::value_timing_advance_type1_eutra, type_, "MeasuredResultsValue");
    return c.get<uint16_t>();
  }
  uint16_t& value_timing_advance_type2_eutra()
  {
    assert_choice_type(types::value_timing_advance_type2_eutra, type_, "MeasuredResultsValue");
    return c.get<uint16_t>();
  }
  result_rsrp_eutra_l& result_rsrp_eutra()
  {
    assert_choice_type(types::result_rsrp_eutra, type_, "MeasuredResultsValue");
    return c.get<result_rsrp_eutra_l>();
  }
  result_rsrq_eutra_l& result_rsrq_eutra()
  {
    assert_choice_type(types::result_rsrq_eutra, type_, "MeasuredResultsValue");
    return c.get<result_rsrq_eutra_l>();
  }
  protocol_ie_single_container_s<measured_results_value_ext_ie_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "MeasuredResultsValue");
    return c.get<protocol_ie_single_container_s<measured_results_value_ext_ie_o>>();
  }
  const uint16_t& value_angle_of_arrival_eutra() const
  {
    assert_choice_type(types::value_angle_of_arrival_eutra, type_, "MeasuredResultsValue");
    return c.get<uint16_t>();
  }
  const uint16_t& value_timing_advance_type1_eutra() const
  {
    assert_choice_type(types::value_timing_advance_type1_eutra, type_, "MeasuredResultsValue");
    return c.get<uint16_t>();
  }
  const uint16_t& value_timing_advance_type2_eutra() const
  {
    assert_choice_type(types::value_timing_advance_type2_eutra, type_, "MeasuredResultsValue");
    return c.get<uint16_t>();
  }
  const result_rsrp_eutra_l& result_rsrp_eutra() const
  {
    assert_choice_type(types::result_rsrp_eutra, type_, "MeasuredResultsValue");
    return c.get<result_rsrp_eutra_l>();
  }
  const result_rsrq_eutra_l& result_rsrq_eutra() const
  {
    assert_choice_type(types::result_rsrq_eutra, type_, "MeasuredResultsValue");
    return c.get<result_rsrq_eutra_l>();
  }
  const protocol_ie_single_container_s<measured_results_value_ext_ie_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "MeasuredResultsValue");
    return c.get<protocol_ie_single_container_s<measured_results_value_ext_ie_o>>();
  }
  uint16_t&                                                        set_value_angle_of_arrival_eutra();
  uint16_t&                                                        set_value_timing_advance_type1_eutra();
  uint16_t&                                                        set_value_timing_advance_type2_eutra();
  result_rsrp_eutra_l&                                             set_result_rsrp_eutra();
  result_rsrq_eutra_l&                                             set_result_rsrq_eutra();
  protocol_ie_single_container_s<measured_results_value_ext_ie_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<measured_results_value_ext_ie_o>,
                  result_rsrp_eutra_l,
                  result_rsrq_eutra_l>
      c;

  void destroy_();
};

// NG-RANCell ::= CHOICE
struct ng_ran_cell_c {
  struct types_opts {
    enum options { eutra_cell_id, nr_cell_id, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  ng_ran_cell_c() = default;
  ng_ran_cell_c(const ng_ran_cell_c& other);
  ng_ran_cell_c& operator=(const ng_ran_cell_c& other);
  ~ng_ran_cell_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<28, false, true>& eutra_cell_id()
  {
    assert_choice_type(types::eutra_cell_id, type_, "NG-RANCell");
    return c.get<fixed_bitstring<28, false, true>>();
  }
  fixed_bitstring<36, false, true>& nr_cell_id()
  {
    assert_choice_type(types::nr_cell_id, type_, "NG-RANCell");
    return c.get<fixed_bitstring<36, false, true>>();
  }
  protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "NG-RANCell");
    return c.get<protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>>();
  }
  const fixed_bitstring<28, false, true>& eutra_cell_id() const
  {
    assert_choice_type(types::eutra_cell_id, type_, "NG-RANCell");
    return c.get<fixed_bitstring<28, false, true>>();
  }
  const fixed_bitstring<36, false, true>& nr_cell_id() const
  {
    assert_choice_type(types::nr_cell_id, type_, "NG-RANCell");
    return c.get<fixed_bitstring<36, false, true>>();
  }
  const protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "NG-RANCell");
    return c.get<protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>>();
  }
  fixed_bitstring<28, false, true>&                     set_eutra_cell_id();
  fixed_bitstring<36, false, true>&                     set_nr_cell_id();
  protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>& set_choice_ext();

private:
  types                                                                                                   type_;
  choice_buffer_t<fixed_bitstring<36, false, true>, protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>> c;

  void destroy_();
};

using ng_ran_cgi_ext_ies_container = protocol_ext_container_empty_l;

// OTDOACell-Information ::= SEQUENCE (SIZE (1..63)) OF OTDOACell-Information-Item
using otdoa_cell_info_l = dyn_array<otdoa_cell_info_item_c>;

using otdoa_cells_ext_ies_container = protocol_ext_container_empty_l;

// OTDOA-Information-Type-Item ::= SEQUENCE
struct otdoa_info_type_item_s {
  bool                                   ext             = false;
  bool                                   ie_exts_present = false;
  otdoa_info_item_e                      otdoa_info_item;
  otdoa_info_type_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherRATMeasurementQuantities-ItemIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct other_rat_meas_quantities_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { other_rat_meas_quantities_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::other_rat_meas_quantities_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    other_rat_meas_quantities_item_s&       other_rat_meas_quantities_item() { return c; }
    const other_rat_meas_quantities_item_s& other_rat_meas_quantities_item() const { return c; }

  private:
    other_rat_meas_quantities_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Outcome ::= ENUMERATED
struct outcome_opts {
  enum options { failed, /*...*/ nulltype } value;

  const char* to_string() const;
};
using outcome_e = enumerated<outcome_opts, true>;

// PathlossReferenceInformation ::= SEQUENCE
struct pathloss_ref_info_s {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  pathloss_ref_sig_c                  pathloss_ref_sig;
  pathloss_ref_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PeriodicityList ::= SEQUENCE (SIZE (1..16)) OF PeriodicityItem
using periodicity_list_l = bounded_array<periodicity_item_e, 16>;

// PosSRSResourceID-List ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..63)
using pos_srs_res_id_list_l = dyn_array<uint8_t>;

using prs_meass_info_list_item_ext_ies_container = protocol_ext_container_empty_l;

// PRSTransmissionOffInformation ::= SEQUENCE
struct prs_tx_off_info_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  prs_tx_off_ind_c                  prs_tx_off_ind;
  prs_tx_off_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using prs_tx_trp_item_ext_ies_container = protocol_ext_container_empty_l;

using prstrp_item_ext_ies_container = protocol_ext_container_empty_l;

// RequestedDLPRSTransmissionCharacteristics ::= SEQUENCE
struct requested_dl_prs_tx_characteristics_s {
  bool                                                  ext                        = false;
  bool                                                  numof_freq_layers_present  = false;
  bool                                                  start_time_and_dur_present = false;
  bool                                                  ie_exts_present            = false;
  requested_dl_prs_res_set_list_l                       requested_dl_prs_res_set_list;
  uint8_t                                               numof_freq_layers = 1;
  start_time_and_dur_s                                  start_time_and_dur;
  requested_dl_prs_tx_characteristics_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RequestedSRSTransmissionCharacteristics-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
struct requested_srs_tx_characteristics_ext_ies_o {
  // Extension ::= OPEN TYPE
  struct ext_c {
    struct types_opts {
      enum options { srs_freq, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srs_freq; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&       srs_freq() { return c; }
    const uint32_t& srs_freq() const { return c; }

  private:
    uint32_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static ext_c      get_ext(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResultGERAN ::= SEQUENCE (SIZE (1..8)) OF ResultGERAN-Item
using result_geran_l = dyn_array<result_geran_item_s>;

// ResultUTRAN ::= SEQUENCE (SIZE (1..8)) OF ResultUTRAN-Item
using result_utran_l = dyn_array<result_utran_item_s>;

// Search-window-information ::= SEQUENCE
struct search_win_info_s {
  bool                              ext                        = false;
  bool                              ie_exts_present            = false;
  int16_t                           expected_propagation_delay = -3841;
  uint8_t                           delay_uncertainty          = 1;
  search_win_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct semipersistent_srs_ext_ies_container {
  bool                           srs_spatial_relation_present             = false;
  bool                           srs_spatial_relation_per_srs_res_present = false;
  spatial_relation_info_s        srs_spatial_relation;
  spatial_relation_per_srs_res_s srs_spatial_relation_per_srs_res;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRSCarrier-List-Item ::= SEQUENCE
struct srs_carrier_list_item_s {
  bool                                    ext             = false;
  bool                                    pci_nr_present  = false;
  bool                                    ie_exts_present = false;
  uint32_t                                point_a         = 0;
  ul_ch_bw_per_scs_list_l                 ul_ch_bw_per_scs_list;
  active_ul_bwp_s                         active_ul_bwp;
  uint16_t                                pci_nr = 0;
  srs_carrier_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRSResourceTrigger ::= SEQUENCE
struct srs_res_trigger_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  aperiodic_srs_res_trigger_list_l  aperiodic_srs_res_trigger_list;
  srs_res_trigger_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRSType-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using srs_type_ext_ies_o = protocol_ies_empty_o;

struct sys_info_item_s_ {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  broadcast_periodicity_e    broadcast_periodicity;
  pos_sibs_l                 pos_sibs;
  sys_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPInformation ::= SEQUENCE
struct trp_info_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  uint32_t                   trp_id          = 1;
  trp_info_type_resp_list_l  trp_info_type_resp_list;
  trp_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using trp_info_trp_resp_ext_ies_container = protocol_ext_container_empty_l;

// TRPInformationTypeItemTRPReq ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct trp_info_type_item_trp_req_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_info_type_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::trp_info_type_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    trp_info_type_item_e&       trp_info_type_item() { return c; }
    const trp_info_type_item_e& trp_info_type_item() const { return c; }

  private:
    trp_info_type_item_e c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using trp_item_ext_ies_container = protocol_ext_container_empty_l;

// TRPMeasurementQuantities-Item ::= ENUMERATED
struct trp_meas_quantities_item_opts {
  enum options {
    gnb_rx_tx_time_diff,
    ul_srs_rsrp,
    ul_ao_a,
    ul_rtoa,
    /*...*/ multiple_ul_ao_a,
    ul_srs_rsrp_p,
    nulltype
  } value;

  const char* to_string() const;
};
using trp_meas_quantities_item_e = enumerated<trp_meas_quantities_item_opts, true, 2>;

using trp_meas_quantities_list_item_ext_ies_container = protocol_ext_container_empty_l;

struct trp_meas_request_item_ext_ies_container {
  bool                cell_id_present           = false;
  bool                ao_a_search_win_present   = false;
  bool                nof_trp_rx_teg_present    = false;
  bool                nof_trp_rx_tx_teg_present = false;
  cgi_nr_s            cell_id;
  ao_a_assist_info_s  ao_a_search_win;
  nof_trp_rx_teg_e    nof_trp_rx_teg;
  nof_trp_rx_tx_teg_e nof_trp_rx_tx_teg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrpMeasurementResult ::= SEQUENCE (SIZE (1..16384)) OF TrpMeasurementResultItem
using trp_meas_result_l = dyn_array<trp_meas_result_item_s>;

struct trp_meas_upd_item_ext_ies_container {
  bool                nof_trp_rx_teg_present    = false;
  bool                nof_trp_rx_tx_teg_present = false;
  nof_trp_rx_teg_e    nof_trp_rx_teg;
  nof_trp_rx_tx_teg_e nof_trp_rx_tx_teg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLANBand ::= ENUMERATED
struct wlan_band_opts {
  enum options { band2dot4, band5, /*...*/ nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using wlan_band_e = enumerated<wlan_band_opts, true>;

// WLANChannelList ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..255)
using wlan_ch_list_l = bounded_array<uint16_t, 16>;

// WLANCountryCode ::= ENUMERATED
struct wlan_country_code_opts {
  enum options { united_states, europe, japan, global, /*...*/ nulltype } value;

  const char* to_string() const;
};
using wlan_country_code_e = enumerated<wlan_country_code_opts, true>;

// WLANMeasurementQuantities-ItemIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct wlan_meas_quantities_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { wlan_meas_quantities_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::wlan_meas_quantities_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    wlan_meas_quantities_item_s&       wlan_meas_quantities_item() { return c; }
    const wlan_meas_quantities_item_s& wlan_meas_quantities_item() const { return c; }

  private:
    wlan_meas_quantities_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

using wlan_meas_result_item_ext_ies_container = protocol_ext_container_empty_l;

// AbortTransmission-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
using abort_tx_ext_ies_o = protocol_ies_empty_o;

// AperiodicSRS ::= SEQUENCE
struct aperiodic_srs_s {
  struct aperiodic_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using aperiodic_e_ = enumerated<aperiodic_opts, true>;

  // member variables
  bool                            ext                     = false;
  bool                            srs_res_trigger_present = false;
  bool                            ie_exts_present         = false;
  aperiodic_e_                    aperiodic;
  srs_res_trigger_s               srs_res_trigger;
  aperiodic_srs_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using assist_info_ext_ies_container = protocol_ext_container_empty_l;

struct assist_info_fail_list_item_s_ {
  bool                                    ext             = false;
  bool                                    ie_exts_present = false;
  pos_sib_type_e                          pos_sib_type;
  outcome_e                               outcome;
  assist_info_fail_list_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandwidthSRS ::= CHOICE
struct bw_srs_c {
  struct fr1_opts {
    enum options { mhz5, mhz10, mhz20, mhz40, mhz50, mhz80, mhz100, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using fr1_e_ = enumerated<fr1_opts, true>;
  struct fr2_opts {
    enum options { mhz50, mhz100, mhz200, mhz400, /*...*/ nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using fr2_e_ = enumerated<fr2_opts, true>;
  struct types_opts {
    enum options { fr1, fr2, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bw_srs_c() = default;
  bw_srs_c(const bw_srs_c& other);
  bw_srs_c& operator=(const bw_srs_c& other);
  ~bw_srs_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fr1_e_& fr1()
  {
    assert_choice_type(types::fr1, type_, "BandwidthSRS");
    return c.get<fr1_e_>();
  }
  fr2_e_& fr2()
  {
    assert_choice_type(types::fr2, type_, "BandwidthSRS");
    return c.get<fr2_e_>();
  }
  protocol_ie_single_container_s<bw_srs_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "BandwidthSRS");
    return c.get<protocol_ie_single_container_s<bw_srs_ext_ies_o>>();
  }
  const fr1_e_& fr1() const
  {
    assert_choice_type(types::fr1, type_, "BandwidthSRS");
    return c.get<fr1_e_>();
  }
  const fr2_e_& fr2() const
  {
    assert_choice_type(types::fr2, type_, "BandwidthSRS");
    return c.get<fr2_e_>();
  }
  const protocol_ie_single_container_s<bw_srs_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "BandwidthSRS");
    return c.get<protocol_ie_single_container_s<bw_srs_ext_ies_o>>();
  }
  fr1_e_&                                           set_fr1();
  fr2_e_&                                           set_fr2();
  protocol_ie_single_container_s<bw_srs_ext_ies_o>& set_choice_ext();

private:
  types                                                             type_;
  choice_buffer_t<protocol_ie_single_container_s<bw_srs_ext_ies_o>> c;

  void destroy_();
};

// CauseMisc ::= ENUMERATED
struct cause_misc_opts {
  enum options { unspecified, /*...*/ nulltype } value;

  const char* to_string() const;
};
using cause_misc_e = enumerated<cause_misc_opts, true>;

// CauseProtocol ::= ENUMERATED
struct cause_protocol_opts {
  enum options {
    transfer_syntax_error,
    abstract_syntax_error_reject,
    abstract_syntax_error_ignore_and_notify,
    msg_not_compatible_with_receiver_state,
    semantic_error,
    unspecified,
    abstract_syntax_error_falsely_constructed_msg,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
using cause_protocol_e = enumerated<cause_protocol_opts, true>;

// CauseRadioNetwork ::= ENUMERATED
struct cause_radio_network_opts {
  enum options {
    unspecified,
    requested_item_not_supported,
    requested_item_temporarily_not_available,
    // ...
    serving_ng_ran_node_changed,
    requested_item_not_supported_on_time,
    nulltype
  } value;

  const char* to_string() const;
};
using cause_radio_network_e = enumerated<cause_radio_network_opts, true, 2>;

using crit_diagnostics_ext_ies_container = protocol_ext_container_empty_l;

// CriticalityDiagnostics-IE-List ::= SEQUENCE (SIZE (1..256)) OF CriticalityDiagnostics-IE-List-item
using crit_diagnostics_ie_list_l = dyn_array<crit_diagnostics_ie_list_item_s_>;

// MeasuredResults ::= SEQUENCE (SIZE (1..64)) OF MeasuredResultsValue
using measured_results_l = dyn_array<measured_results_value_c>;

// NG-RAN-CGI ::= SEQUENCE
struct ng_ran_cgi_s {
  bool                         ext             = false;
  bool                         ie_exts_present = false;
  fixed_octstring<3, true>     plmn_id;
  ng_ran_cell_c                ng_ra_ncell;
  ng_ran_cgi_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct nr_ppa_private_ies_empty_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
};

struct otdoa_cells_item_s_ {
  bool                          ext             = false;
  bool                          ie_exts_present = false;
  otdoa_cell_info_l             otdoa_cell_info;
  otdoa_cells_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OTDOA-Information-Type-ItemIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct otdoa_info_type_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { otdoa_info_type_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::otdoa_info_type_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    otdoa_info_type_item_s&       otdoa_info_type_item() { return c; }
    const otdoa_info_type_item_s& otdoa_info_type_item() const { return c; }

  private:
    otdoa_info_type_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OtherRATMeasuredResultsValue ::= CHOICE
struct other_rat_measured_results_value_c {
  struct types_opts {
    enum options { result_geran, result_utran, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  other_rat_measured_results_value_c() = default;
  other_rat_measured_results_value_c(const other_rat_measured_results_value_c& other);
  other_rat_measured_results_value_c& operator=(const other_rat_measured_results_value_c& other);
  ~other_rat_measured_results_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  result_geran_l& result_geran()
  {
    assert_choice_type(types::result_geran, type_, "OtherRATMeasuredResultsValue");
    return c.get<result_geran_l>();
  }
  result_utran_l& result_utran()
  {
    assert_choice_type(types::result_utran, type_, "OtherRATMeasuredResultsValue");
    return c.get<result_utran_l>();
  }
  protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "OtherRATMeasuredResultsValue");
    return c.get<protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>>();
  }
  const result_geran_l& result_geran() const
  {
    assert_choice_type(types::result_geran, type_, "OtherRATMeasuredResultsValue");
    return c.get<result_geran_l>();
  }
  const result_utran_l& result_utran() const
  {
    assert_choice_type(types::result_utran, type_, "OtherRATMeasuredResultsValue");
    return c.get<result_utran_l>();
  }
  const protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "OtherRATMeasuredResultsValue");
    return c.get<protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>>();
  }
  result_geran_l&                                                            set_result_geran();
  result_utran_l&                                                            set_result_utran();
  protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>& set_choice_ext();

private:
  types type_;
  choice_buffer_t<protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>,
                  result_geran_l,
                  result_utran_l>
      c;

  void destroy_();
};

// PrivateIE-ID ::= CHOICE
struct private_ie_id_c {
  struct types_opts {
    enum options { local, global, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  private_ie_id_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint32_t& local()
  {
    assert_choice_type(types::local, type_, "PrivateIE-ID");
    return c;
  }
  const uint32_t& local() const
  {
    assert_choice_type(types::local, type_, "PrivateIE-ID");
    return c;
  }
  uint32_t& set_local();
  void      set_global();

private:
  types    type_;
  uint32_t c;
};

// PRS-Measurements-Info-List-Item ::= SEQUENCE
struct prs_meass_info_list_item_s {
  struct meas_prs_periodicity_opts {
    enum options { ms20, ms40, ms80, ms160, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using meas_prs_periodicity_e_ = enumerated<meas_prs_periodicity_opts, true>;
  struct meas_prs_len_opts {
    enum options { ms1dot5, ms3, ms3dot5, ms4, ms5dot5, ms6, ms10, ms20, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using meas_prs_len_e_ = enumerated<meas_prs_len_opts>;

  // member variables
  bool                                       ext             = false;
  bool                                       ie_exts_present = false;
  uint32_t                                   point_a         = 0;
  meas_prs_periodicity_e_                    meas_prs_periodicity;
  uint8_t                                    meas_prs_offset = 0;
  meas_prs_len_e_                            meas_prs_len;
  prs_meass_info_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSTransmissionTRPItem ::= SEQUENCE
struct prs_tx_trp_item_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  uint32_t                          trp_id          = 1;
  prs_cfg_s                         prs_cfg;
  prs_tx_trp_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSTRPItem ::= SEQUENCE
struct prstrp_item_s {
  bool                                  ext                                         = false;
  bool                                  requested_dl_prs_tx_characteristics_present = false;
  bool                                  prs_tx_off_info_present                     = false;
  bool                                  ie_exts_present                             = false;
  uint32_t                              trp_id                                      = 1;
  requested_dl_prs_tx_characteristics_s requested_dl_prs_tx_characteristics;
  prs_tx_off_info_s                     prs_tx_off_info;
  prstrp_item_ext_ies_container         ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using resp_time_ext_ies_container = protocol_ext_container_empty_l;

// SemipersistentSRS ::= SEQUENCE
struct semipersistent_srs_s {
  bool                                 ext             = false;
  bool                                 ie_exts_present = false;
  uint8_t                              srs_res_set_id  = 0;
  semipersistent_srs_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRSCarrier-List ::= SEQUENCE (SIZE (1..32)) OF SRSCarrier-List-Item
using srs_carrier_list_l = dyn_array<srs_carrier_list_item_s>;

// SRSResourceSet-Item ::= SEQUENCE
struct srs_res_set_item_s {
  bool                                                 ext                           = false;
  bool                                                 nof_srs_res_per_set_present   = false;
  bool                                                 spatial_relation_info_present = false;
  bool                                                 pathloss_ref_info_present     = false;
  uint8_t                                              nof_srs_res_per_set           = 1;
  periodicity_list_l                                   periodicity_list;
  spatial_relation_info_s                              spatial_relation_info;
  pathloss_ref_info_s                                  pathloss_ref_info;
  protocol_ext_container_l<srs_res_set_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using srscfg_ext_ies_container = protocol_ext_container_empty_l;

// SystemInformation ::= SEQUENCE (SIZE (1..32)) OF SystemInformation-item
using sys_info_l = dyn_array<sys_info_item_s_>;

// TriggeringMessage ::= ENUMERATED
struct trigger_msg_opts {
  enum options { init_msg, successful_outcome, unsuccessful_outcome, nulltype } value;

  const char* to_string() const;
};
using trigger_msg_e = enumerated<trigger_msg_opts>;

struct trp_info_list_trp_resp_item_s_ {
  bool                                ext             = false;
  bool                                ie_exts_present = false;
  trp_info_s                          trp_info;
  trp_info_trp_resp_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPItem ::= SEQUENCE
struct trp_item_s {
  bool                       ext             = false;
  bool                       ie_exts_present = false;
  uint32_t                   trp_id          = 1;
  trp_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPMeasurementQuantitiesList-Item ::= SEQUENCE
struct trp_meas_quantities_list_item_s {
  bool                                            ext                                      = false;
  bool                                            timing_report_granularity_factor_present = false;
  bool                                            ie_exts_present                          = false;
  trp_meas_quantities_item_e                      trp_meas_quantities_item;
  uint8_t                                         timing_report_granularity_factor = 0;
  trp_meas_quantities_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRP-MeasurementRequestItem ::= SEQUENCE
struct trp_meas_request_item_s {
  bool                                    ext                     = false;
  bool                                    search_win_info_present = false;
  bool                                    ie_exts_present         = false;
  uint32_t                                trp_id                  = 1;
  search_win_info_s                       search_win_info;
  trp_meas_request_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRP-MeasurementResponseItem ::= SEQUENCE
struct trp_meas_resp_item_s {
  bool                                                   ext    = false;
  uint32_t                                               trp_id = 1;
  trp_meas_result_l                                      meas_result;
  protocol_ext_container_l<trp_meas_resp_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRP-MeasurementUpdateItem ::= SEQUENCE
struct trp_meas_upd_item_s {
  bool                                ext                   = false;
  bool                                ao_a_win_info_present = false;
  bool                                ie_exts_present       = false;
  uint32_t                            trp_id                = 1;
  ao_a_assist_info_s                  ao_a_win_info;
  trp_meas_upd_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using trp_prs_info_list_item_ext_ies_container = protocol_ext_container_empty_l;

using ue_report_info_ext_ies_container = protocol_ext_container_empty_l;

// UETxTEGAssociationItem ::= SEQUENCE
struct ue_tx_teg_assoc_item_s {
  bool                                                     ext                  = false;
  bool                                                     carrier_freq_present = false;
  uint8_t                                                  ue_tx_teg_id         = 0;
  pos_srs_res_id_list_l                                    pos_srs_res_id_list;
  time_stamp_s                                             time_stamp;
  carrier_freq_s                                           carrier_freq;
  protocol_ext_container_l<ue_tx_teg_assoc_item_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLANMeasurementResult-Item ::= SEQUENCE
struct wlan_meas_result_item_s {
  bool                                    ext                     = false;
  bool                                    bs_si_d_present         = false;
  bool                                    hes_si_d_present        = false;
  bool                                    operating_class_present = false;
  bool                                    country_code_present    = false;
  bool                                    wlan_band_present       = false;
  bool                                    ie_exts_present         = false;
  uint8_t                                 wlan_rssi               = 0;
  bounded_octstring<1, 32, true>          ssi_d;
  fixed_octstring<6, true>                bs_si_d;
  fixed_octstring<6, true>                hes_si_d;
  uint16_t                                operating_class = 0;
  wlan_country_code_e                     country_code;
  wlan_ch_list_l                          wlan_ch_list;
  wlan_band_e                             wlan_band;
  wlan_meas_result_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssistanceInformationFailureList ::= SEQUENCE (SIZE (1..32)) OF AssistanceInformationFailureList-item
using assist_info_fail_list_l = dyn_array<assist_info_fail_list_item_s_>;

// Assistance-Information ::= SEQUENCE
struct assist_info_s {
  bool                          ext             = false;
  bool                          ie_exts_present = false;
  sys_info_l                    sys_info;
  assist_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Broadcast ::= ENUMERATED
struct broadcast_opts {
  enum options { start, stop, /*...*/ nulltype } value;

  const char* to_string() const;
};
using broadcast_e = enumerated<broadcast_opts, true>;

// Cause ::= CHOICE
struct cause_c {
  struct types_opts {
    enum options { radio_network, protocol, misc, choice_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  cause_c() = default;
  cause_c(const cause_c& other);
  cause_c& operator=(const cause_c& other);
  ~cause_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cause_radio_network_e& radio_network()
  {
    assert_choice_type(types::radio_network, type_, "Cause");
    return c.get<cause_radio_network_e>();
  }
  cause_protocol_e& protocol()
  {
    assert_choice_type(types::protocol, type_, "Cause");
    return c.get<cause_protocol_e>();
  }
  cause_misc_e& misc()
  {
    assert_choice_type(types::misc, type_, "Cause");
    return c.get<cause_misc_e>();
  }
  protocol_ie_single_container_s<cause_ext_ie_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "Cause");
    return c.get<protocol_ie_single_container_s<cause_ext_ie_o>>();
  }
  const cause_radio_network_e& radio_network() const
  {
    assert_choice_type(types::radio_network, type_, "Cause");
    return c.get<cause_radio_network_e>();
  }
  const cause_protocol_e& protocol() const
  {
    assert_choice_type(types::protocol, type_, "Cause");
    return c.get<cause_protocol_e>();
  }
  const cause_misc_e& misc() const
  {
    assert_choice_type(types::misc, type_, "Cause");
    return c.get<cause_misc_e>();
  }
  const protocol_ie_single_container_s<cause_ext_ie_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "Cause");
    return c.get<protocol_ie_single_container_s<cause_ext_ie_o>>();
  }
  cause_radio_network_e&                          set_radio_network();
  cause_protocol_e&                               set_protocol();
  cause_misc_e&                                   set_misc();
  protocol_ie_single_container_s<cause_ext_ie_o>& set_choice_ext();

private:
  types                                                           type_;
  choice_buffer_t<protocol_ie_single_container_s<cause_ext_ie_o>> c;

  void destroy_();
};

// CriticalityDiagnostics ::= SEQUENCE
struct crit_diagnostics_s {
  bool                               ext                         = false;
  bool                               proc_code_present           = false;
  bool                               trigger_msg_present         = false;
  bool                               proc_crit_present           = false;
  bool                               nrppatransaction_id_present = false;
  bool                               ie_exts_present             = false;
  uint16_t                           proc_code                   = 0;
  trigger_msg_e                      trigger_msg;
  crit_e                             proc_crit;
  uint16_t                           nrppatransaction_id = 0;
  crit_diagnostics_ie_list_l         ies_crit_diagnostics;
  crit_diagnostics_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E-CID-MeasurementResult ::= SEQUENCE
struct e_c_id_meas_result_s {
  bool                                                   ext                                  = false;
  bool                                                   ng_ran_access_point_position_present = false;
  ng_ran_cgi_s                                           serving_cell_id;
  fixed_octstring<3, true>                               serving_cell_tac;
  ng_ran_access_point_position_s                         ng_ran_access_point_position;
  measured_results_l                                     measured_results;
  protocol_ext_container_l<e_c_id_meas_result_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementAmount ::= ENUMERATED
struct meas_amount_opts {
  enum options { ma0, ma1, ma2, ma4, ma8, ma16, ma32, ma64, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using meas_amount_e = enumerated<meas_amount_opts>;

// MeasurementBeamInfoRequest ::= ENUMERATED
struct meas_beam_info_request_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
using meas_beam_info_request_e = enumerated<meas_beam_info_request_opts, true>;

// MeasurementPeriodicity ::= ENUMERATED
struct meas_periodicity_opts {
  enum options {
    ms120,
    ms240,
    ms480,
    ms640,
    ms1024,
    ms2048,
    ms5120,
    ms10240,
    min1,
    min6,
    min12,
    min30,
    min60,
    // ...
    ms20480,
    ms40960,
    extended,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using meas_periodicity_e = enumerated<meas_periodicity_opts, true, 3>;

// MeasurementPeriodicityExtended ::= ENUMERATED
struct meas_periodicity_extended_opts {
  enum options {
    ms160,
    ms320,
    ms1280,
    ms2560,
    ms61440,
    ms81920,
    ms368640,
    ms737280,
    ms1843200,
    /*...*/ nulltype
  } value;
  typedef uint32_t number_type;

  const char* to_string() const;
  uint32_t    to_number() const;
};
using meas_periodicity_extended_e = enumerated<meas_periodicity_extended_opts, true>;

// MeasurementPeriodicityNR-AoA ::= ENUMERATED
struct meas_periodicity_nr_ao_a_opts {
  enum options {
    ms160,
    ms320,
    ms640,
    ms1280,
    ms2560,
    ms5120,
    ms10240,
    ms20480,
    ms40960,
    ms61440,
    ms81920,
    ms368640,
    ms737280,
    ms1843200,
    // ...
    nulltype
  } value;
  typedef uint32_t number_type;

  const char* to_string() const;
  uint32_t    to_number() const;
};
using meas_periodicity_nr_ao_a_e = enumerated<meas_periodicity_nr_ao_a_opts, true>;

// MeasurementQuantities ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-Single-Container{NRPPA-PROTOCOL-IES : IEsSetParam}
using meas_quantities_l = dyn_array<protocol_ie_single_container_s<meas_quantities_item_ies_o>>;

// MeasurementTimeOccasion ::= ENUMERATED
struct meas_time_occasion_opts {
  enum options { o1, o4, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using meas_time_occasion_e = enumerated<meas_time_occasion_opts, true>;

// OTDOACells ::= SEQUENCE (SIZE (1..3840)) OF OTDOACells-item
using otdoa_cells_l = dyn_array<otdoa_cells_item_s_>;

// OtherRATMeasurementQuantities ::= SEQUENCE (SIZE (0..64)) OF ProtocolIE-Single-Container{NRPPA-PROTOCOL-IES :
// IEsSetParam}
using other_rat_meas_quantities_l = dyn_array<protocol_ie_single_container_s<other_rat_meas_quantities_item_ies_o>>;

// OtherRATMeasurementResult ::= SEQUENCE (SIZE (1..64)) OF OtherRATMeasuredResultsValue
using other_rat_meas_result_l = dyn_array<other_rat_measured_results_value_c>;

// PositioningBroadcastCells ::= SEQUENCE (SIZE (1..16384)) OF NG-RAN-CGI
using positioning_broadcast_cells_l = dyn_array<ng_ran_cgi_s>;

// PrivateIE-Field{NRPPA-PRIVATE-IES : IEsSetParam} ::= SEQUENCE{{NRPPA-PRIVATE-IES}}
template <class ies_set_paramT_>
struct private_ie_field_s {
  private_ie_id_c                   id;
  crit_e                            crit;
  typename ies_set_paramT_::value_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PrivateMessage-IEs ::= OBJECT SET OF NRPPA-PRIVATE-IES
using private_msg_ies_o = nr_ppa_private_ies_empty_o;

// PRSConfigRequestType ::= ENUMERATED
struct prs_cfg_request_type_opts {
  enum options { cfgure, off, /*...*/ nulltype } value;

  const char* to_string() const;
};
using prs_cfg_request_type_e = enumerated<prs_cfg_request_type_opts, true>;

// PRS-Measurements-Info-List ::= SEQUENCE (SIZE (1..4)) OF PRS-Measurements-Info-List-Item
using prs_meass_info_list_l = dyn_array<prs_meass_info_list_item_s>;

// PRSTransmissionTRPList ::= SEQUENCE (SIZE (1..65535)) OF PRSTransmissionTRPItem
using prs_tx_trp_list_l = dyn_array<prs_tx_trp_item_s>;

// PRSTRPList ::= SEQUENCE (SIZE (1..65535)) OF PRSTRPItem
using prstrp_list_l = dyn_array<prstrp_item_s>;

// ReportCharacteristics ::= ENUMERATED
struct report_characteristics_opts {
  enum options { on_demand, periodic, /*...*/ nulltype } value;

  const char* to_string() const;
};
using report_characteristics_e = enumerated<report_characteristics_opts, true>;

// RequestType ::= ENUMERATED
struct request_type_opts {
  enum options { activ, deactiv, /*...*/ nulltype } value;

  const char* to_string() const;
};
using request_type_e = enumerated<request_type_opts, true>;

// RequestedSRSTransmissionCharacteristics ::= SEQUENCE
struct requested_srs_tx_characteristics_s {
  struct res_type_opts {
    enum options { periodic, semi_persistent, aperiodic, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using res_type_e_            = enumerated<res_type_opts, true>;
  using list_of_srs_res_set_l_ = dyn_array<srs_res_set_item_s>;

  // member variables
  bool                                                                 ext              = false;
  bool                                                                 nof_txs_present  = false;
  bool                                                                 ssb_info_present = false;
  uint16_t                                                             nof_txs          = 0;
  res_type_e_                                                          res_type;
  bw_srs_c                                                             bw;
  list_of_srs_res_set_l_                                               list_of_srs_res_set;
  ssb_info_s                                                           ssb_info;
  protocol_ext_container_l<requested_srs_tx_characteristics_ext_ies_o> ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResponseTime ::= SEQUENCE
struct resp_time_s {
  struct time_unit_opts {
    enum options { second, ten_seconds, ten_milliseconds, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using time_unit_e_ = enumerated<time_unit_opts, true>;

  // member variables
  bool                        ext             = false;
  bool                        ie_exts_present = false;
  uint8_t                     time            = 1;
  time_unit_e_                time_unit;
  resp_time_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRSTransmissionStatus ::= ENUMERATED
struct srs_tx_status_opts {
  enum options { stopped, /*...*/ nulltype } value;

  const char* to_string() const;
};
using srs_tx_status_e = enumerated<srs_tx_status_opts, true>;

// SRSType ::= CHOICE
struct srs_type_c {
  struct types_opts {
    enum options { semipersistent_srs, aperiodic_srs, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  srs_type_c() = default;
  srs_type_c(const srs_type_c& other);
  srs_type_c& operator=(const srs_type_c& other);
  ~srs_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  semipersistent_srs_s& semipersistent_srs()
  {
    assert_choice_type(types::semipersistent_srs, type_, "SRSType");
    return c.get<semipersistent_srs_s>();
  }
  aperiodic_srs_s& aperiodic_srs()
  {
    assert_choice_type(types::aperiodic_srs, type_, "SRSType");
    return c.get<aperiodic_srs_s>();
  }
  protocol_ie_single_container_s<srs_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "SRSType");
    return c.get<protocol_ie_single_container_s<srs_type_ext_ies_o>>();
  }
  const semipersistent_srs_s& semipersistent_srs() const
  {
    assert_choice_type(types::semipersistent_srs, type_, "SRSType");
    return c.get<semipersistent_srs_s>();
  }
  const aperiodic_srs_s& aperiodic_srs() const
  {
    assert_choice_type(types::aperiodic_srs, type_, "SRSType");
    return c.get<aperiodic_srs_s>();
  }
  const protocol_ie_single_container_s<srs_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "SRSType");
    return c.get<protocol_ie_single_container_s<srs_type_ext_ies_o>>();
  }
  semipersistent_srs_s&                               set_semipersistent_srs();
  aperiodic_srs_s&                                    set_aperiodic_srs();
  protocol_ie_single_container_s<srs_type_ext_ies_o>& set_choice_ext();

private:
  types                                                                                                      type_;
  choice_buffer_t<aperiodic_srs_s, protocol_ie_single_container_s<srs_type_ext_ies_o>, semipersistent_srs_s> c;

  void destroy_();
};

// SRSConfiguration ::= SEQUENCE
struct srscfg_s {
  bool                     ext             = false;
  bool                     ie_exts_present = false;
  srs_carrier_list_l       srs_carrier_list;
  srscfg_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TRPInformationListTRPResp ::= SEQUENCE (SIZE (1..65535)) OF TRPInformationListTRPResp-item
using trp_info_list_trp_resp_l = dyn_array<trp_info_list_trp_resp_item_s_>;

// TRPInformationTypeListTRPReq ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-Single-Container{NRPPA-PROTOCOL-IES :
// IEsSetParam}
using trp_info_type_list_trp_req_l = dyn_array<protocol_ie_single_container_s<trp_info_type_item_trp_req_o>>;

// TRPList ::= SEQUENCE (SIZE (1..65535)) OF TRPItem
using trp_list_l = dyn_array<trp_item_s>;

// TRPMeasurementQuantities ::= SEQUENCE (SIZE (1..16384)) OF TRPMeasurementQuantitiesList-Item
using trp_meas_quantities_l = dyn_array<trp_meas_quantities_list_item_s>;

// TRP-MeasurementRequestList ::= SEQUENCE (SIZE (1..64)) OF TRP-MeasurementRequestItem
using trp_meas_request_list_l = dyn_array<trp_meas_request_item_s>;

// TRP-MeasurementResponseList ::= SEQUENCE (SIZE (1..64)) OF TRP-MeasurementResponseItem
using trp_meas_resp_list_l = dyn_array<trp_meas_resp_item_s>;

// TRP-MeasurementUpdateList ::= SEQUENCE (SIZE (1..64)) OF TRP-MeasurementUpdateItem
using trp_meas_upd_list_l = dyn_array<trp_meas_upd_item_s>;

// TRP-PRS-Information-List-Item ::= SEQUENCE
struct trp_prs_info_list_item_s {
  bool                                     ext             = false;
  bool                                     cgi_nr_present  = false;
  bool                                     ie_exts_present = false;
  uint32_t                                 trp_id          = 1;
  uint16_t                                 nr_pci          = 0;
  cgi_nr_s                                 cgi_nr;
  prs_cfg_s                                prs_cfg;
  trp_prs_info_list_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEReportingInformation ::= SEQUENCE
struct ue_report_info_s {
  struct report_amount_opts {
    enum options { ma0, ma1, ma2, ma4, ma8, ma16, ma32, ma64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using report_amount_e_ = enumerated<report_amount_opts>;
  struct report_interv_opts {
    enum options { none, one, two, four, eight, ten, sixteen, twenty, thirty_two, sixty_four, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using report_interv_e_ = enumerated<report_interv_opts, true>;

  // member variables
  bool                             ext             = false;
  bool                             ie_exts_present = false;
  report_amount_e_                 report_amount;
  report_interv_e_                 report_interv;
  ue_report_info_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-TEG-Info-Request ::= ENUMERATED
struct ue_teg_info_request_opts {
  enum options { on_demand, periodic, stop, /*...*/ nulltype } value;

  const char* to_string() const;
};
using ue_teg_info_request_e = enumerated<ue_teg_info_request_opts, true>;

// UE-TEG-ReportingPeriodicity ::= ENUMERATED
struct ue_teg_report_periodicity_opts {
  enum options { ms160, ms320, ms1280, ms2560, ms61440, ms81920, ms368640, ms737280, /*...*/ nulltype } value;
  typedef uint32_t number_type;

  const char* to_string() const;
  uint32_t    to_number() const;
};
using ue_teg_report_periodicity_e = enumerated<ue_teg_report_periodicity_opts, true>;

// UETxTEGAssociationList ::= SEQUENCE (SIZE (1..256)) OF UETxTEGAssociationItem
using ue_tx_teg_assoc_list_l = dyn_array<ue_tx_teg_assoc_item_s>;

// WLANMeasurementQuantities ::= SEQUENCE (SIZE (0..64)) OF ProtocolIE-Single-Container{NRPPA-PROTOCOL-IES :
// IEsSetParam}
using wlan_meas_quantities_l = dyn_array<protocol_ie_single_container_s<wlan_meas_quantities_item_ies_o>>;

// WLANMeasurementResult ::= SEQUENCE (SIZE (1..64)) OF WLANMeasurementResult-Item
using wlan_meas_result_l = dyn_array<wlan_meas_result_item_s>;

// AbortTransmission ::= CHOICE
struct abort_tx_c {
  struct types_opts {
    enum options { deactiv_srs_res_set_id, release_all, choice_ext, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  abort_tx_c() = default;
  abort_tx_c(const abort_tx_c& other);
  abort_tx_c& operator=(const abort_tx_c& other);
  ~abort_tx_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& deactiv_srs_res_set_id()
  {
    assert_choice_type(types::deactiv_srs_res_set_id, type_, "AbortTransmission");
    return c.get<uint8_t>();
  }
  protocol_ie_single_container_s<abort_tx_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, type_, "AbortTransmission");
    return c.get<protocol_ie_single_container_s<abort_tx_ext_ies_o>>();
  }
  const uint8_t& deactiv_srs_res_set_id() const
  {
    assert_choice_type(types::deactiv_srs_res_set_id, type_, "AbortTransmission");
    return c.get<uint8_t>();
  }
  const protocol_ie_single_container_s<abort_tx_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, type_, "AbortTransmission");
    return c.get<protocol_ie_single_container_s<abort_tx_ext_ies_o>>();
  }
  uint8_t&                                            set_deactiv_srs_res_set_id();
  void                                                set_release_all();
  protocol_ie_single_container_s<abort_tx_ext_ies_o>& set_choice_ext();

private:
  types                                                               type_;
  choice_buffer_t<protocol_ie_single_container_s<abort_tx_ext_ies_o>> c;

  void destroy_();
};

// AssistanceInformationControl-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct assist_info_ctrl_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { assist_info, broadcast, positioning_broadcast_cells, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    assist_info_s&                       assist_info();
    broadcast_e&                         broadcast();
    positioning_broadcast_cells_l&       positioning_broadcast_cells();
    const assist_info_s&                 assist_info() const;
    const broadcast_e&                   broadcast() const;
    const positioning_broadcast_cells_l& positioning_broadcast_cells() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// AssistanceInformationFeedback-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct assist_info_feedback_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { assist_info_fail_list, positioning_broadcast_cells, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    assist_info_fail_list_l&             assist_info_fail_list();
    positioning_broadcast_cells_l&       positioning_broadcast_cells();
    crit_diagnostics_s&                  crit_diagnostics();
    const assist_info_fail_list_l&       assist_info_fail_list() const;
    const positioning_broadcast_cells_l& positioning_broadcast_cells() const;
    const crit_diagnostics_s&            crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-CIDMeasurementFailureIndication-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct e_c_id_meas_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { lmf_ue_meas_id, ran_ue_meas_id, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       lmf_ue_meas_id();
    uint8_t&       ran_ue_meas_id();
    cause_c&       cause();
    const uint8_t& lmf_ue_meas_id() const;
    const uint8_t& ran_ue_meas_id() const;
    const cause_c& cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-CIDMeasurementInitiationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct e_c_id_meas_initiation_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { lmf_ue_meas_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&                  lmf_ue_meas_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint8_t&            lmf_ue_meas_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-CIDMeasurementInitiationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct e_c_id_meas_initiation_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        lmf_ue_meas_id,
        report_characteristics,
        meas_periodicity,
        meas_quantities,
        other_rat_meas_quantities,
        wlan_meas_quantities,
        meas_periodicity_nr_ao_a,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&                           lmf_ue_meas_id();
    report_characteristics_e&          report_characteristics();
    meas_periodicity_e&                meas_periodicity();
    meas_quantities_l&                 meas_quantities();
    other_rat_meas_quantities_l&       other_rat_meas_quantities();
    wlan_meas_quantities_l&            wlan_meas_quantities();
    meas_periodicity_nr_ao_a_e&        meas_periodicity_nr_ao_a();
    const uint8_t&                     lmf_ue_meas_id() const;
    const report_characteristics_e&    report_characteristics() const;
    const meas_periodicity_e&          meas_periodicity() const;
    const meas_quantities_l&           meas_quantities() const;
    const other_rat_meas_quantities_l& other_rat_meas_quantities() const;
    const wlan_meas_quantities_l&      wlan_meas_quantities() const;
    const meas_periodicity_nr_ao_a_e&  meas_periodicity_nr_ao_a() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-CIDMeasurementInitiationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct e_c_id_meas_initiation_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        lmf_ue_meas_id,
        ran_ue_meas_id,
        e_c_id_meas_result,
        crit_diagnostics,
        cell_portion_id,
        other_rat_meas_result,
        wlan_meas_result,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&                       lmf_ue_meas_id();
    uint8_t&                       ran_ue_meas_id();
    e_c_id_meas_result_s&          e_c_id_meas_result();
    crit_diagnostics_s&            crit_diagnostics();
    uint16_t&                      cell_portion_id();
    other_rat_meas_result_l&       other_rat_meas_result();
    wlan_meas_result_l&            wlan_meas_result();
    const uint8_t&                 lmf_ue_meas_id() const;
    const uint8_t&                 ran_ue_meas_id() const;
    const e_c_id_meas_result_s&    e_c_id_meas_result() const;
    const crit_diagnostics_s&      crit_diagnostics() const;
    const uint16_t&                cell_portion_id() const;
    const other_rat_meas_result_l& other_rat_meas_result() const;
    const wlan_meas_result_l&      wlan_meas_result() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-CIDMeasurementReport-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct e_c_id_meas_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { lmf_ue_meas_id, ran_ue_meas_id, e_c_id_meas_result, cell_portion_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&                    lmf_ue_meas_id();
    uint8_t&                    ran_ue_meas_id();
    e_c_id_meas_result_s&       e_c_id_meas_result();
    uint16_t&                   cell_portion_id();
    const uint8_t&              lmf_ue_meas_id() const;
    const uint8_t&              ran_ue_meas_id() const;
    const e_c_id_meas_result_s& e_c_id_meas_result() const;
    const uint16_t&             cell_portion_id() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E-CIDMeasurementTerminationCommand-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct e_c_id_meas_termination_cmd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { lmf_ue_meas_id, ran_ue_meas_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t&       lmf_ue_meas_id();
    uint8_t&       ran_ue_meas_id();
    const uint8_t& lmf_ue_meas_id() const;
    const uint8_t& ran_ue_meas_id() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ErrorIndication-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct error_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementAbort-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_abort_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { lmf_meas_id, ran_meas_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&       lmf_meas_id();
    uint32_t&       ran_meas_id();
    const uint32_t& lmf_meas_id() const;
    const uint32_t& ran_meas_id() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementActivation-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_activation_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { request_type, prs_meass_info_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    request_type_e&              request_type();
    prs_meass_info_list_l&       prs_meass_info_list();
    const request_type_e&        request_type() const;
    const prs_meass_info_list_l& prs_meass_info_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { lmf_meas_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                 lmf_meas_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint32_t&           lmf_meas_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementFailureIndication-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_fail_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { lmf_meas_id, ran_meas_id, cause, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&       lmf_meas_id();
    uint32_t&       ran_meas_id();
    cause_c&        cause();
    const uint32_t& lmf_meas_id() const;
    const uint32_t& ran_meas_id() const;
    const cause_c&  cause() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementPreconfigurationConfirm-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_precfg_confirm_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { precfg_result, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<8, false, true>&       precfg_result();
    crit_diagnostics_s&                    crit_diagnostics();
    const fixed_bitstring<8, false, true>& precfg_result() const;
    const crit_diagnostics_s&              crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementPreconfigurationRefuse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_precfg_refuse_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementReport-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_report_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { lmf_meas_id, ran_meas_id, trp_meas_report_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                   lmf_meas_id();
    uint32_t&                   ran_meas_id();
    trp_meas_resp_list_l&       trp_meas_report_list();
    const uint32_t&             lmf_meas_id() const;
    const uint32_t&             ran_meas_id() const;
    const trp_meas_resp_list_l& trp_meas_report_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        lmf_meas_id,
        trp_meas_request_list,
        report_characteristics,
        meas_periodicity,
        trp_meas_quantities,
        sfn_initisation_time,
        srscfg,
        meas_beam_info_request,
        sys_frame_num,
        slot_num,
        meas_periodicity_extended,
        resp_time,
        meas_characteristics_request_ind,
        meas_time_occasion,
        meas_amount,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                               lmf_meas_id();
    trp_meas_request_list_l&                trp_meas_request_list();
    report_characteristics_e&               report_characteristics();
    meas_periodicity_e&                     meas_periodicity();
    trp_meas_quantities_l&                  trp_meas_quantities();
    fixed_bitstring<64, false, true>&       sfn_initisation_time();
    srscfg_s&                               srscfg();
    meas_beam_info_request_e&               meas_beam_info_request();
    uint16_t&                               sys_frame_num();
    uint8_t&                                slot_num();
    meas_periodicity_extended_e&            meas_periodicity_extended();
    resp_time_s&                            resp_time();
    fixed_bitstring<16, false, true>&       meas_characteristics_request_ind();
    meas_time_occasion_e&                   meas_time_occasion();
    meas_amount_e&                          meas_amount();
    const uint32_t&                         lmf_meas_id() const;
    const trp_meas_request_list_l&          trp_meas_request_list() const;
    const report_characteristics_e&         report_characteristics() const;
    const meas_periodicity_e&               meas_periodicity() const;
    const trp_meas_quantities_l&            trp_meas_quantities() const;
    const fixed_bitstring<64, false, true>& sfn_initisation_time() const;
    const srscfg_s&                         srscfg() const;
    const meas_beam_info_request_e&         meas_beam_info_request() const;
    const uint16_t&                         sys_frame_num() const;
    const uint8_t&                          slot_num() const;
    const meas_periodicity_extended_e&      meas_periodicity_extended() const;
    const resp_time_s&                      resp_time() const;
    const fixed_bitstring<16, false, true>& meas_characteristics_request_ind() const;
    const meas_time_occasion_e&             meas_time_occasion() const;
    const meas_amount_e&                    meas_amount() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { lmf_meas_id, ran_meas_id, trp_meas_resp_list, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                   lmf_meas_id();
    uint32_t&                   ran_meas_id();
    trp_meas_resp_list_l&       trp_meas_resp_list();
    crit_diagnostics_s&         crit_diagnostics();
    const uint32_t&             lmf_meas_id() const;
    const uint32_t&             ran_meas_id() const;
    const trp_meas_resp_list_l& trp_meas_resp_list() const;
    const crit_diagnostics_s&   crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MeasurementUpdate-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        lmf_meas_id,
        ran_meas_id,
        srscfg,
        trp_meas_upd_list,
        meas_characteristics_request_ind,
        meas_time_occasion,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t&                               lmf_meas_id();
    uint32_t&                               ran_meas_id();
    srscfg_s&                               srscfg();
    trp_meas_upd_list_l&                    trp_meas_upd_list();
    fixed_bitstring<16, false, true>&       meas_characteristics_request_ind();
    meas_time_occasion_e&                   meas_time_occasion();
    const uint32_t&                         lmf_meas_id() const;
    const uint32_t&                         ran_meas_id() const;
    const srscfg_s&                         srscfg() const;
    const trp_meas_upd_list_l&              trp_meas_upd_list() const;
    const fixed_bitstring<16, false, true>& meas_characteristics_request_ind() const;
    const meas_time_occasion_e&             meas_time_occasion() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OTDOAInformationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct otdoa_info_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OTDOAInformationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct otdoa_info_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { otdoa_cells, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    otdoa_cells_l&            otdoa_cells();
    crit_diagnostics_s&       crit_diagnostics();
    const otdoa_cells_l&      otdoa_cells() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OTDOA-Information-Type ::= SEQUENCE (SIZE (1..63)) OF ProtocolIE-Single-Container{NRPPA-PROTOCOL-IES : IEsSetParam}
using otdoa_info_type_l = dyn_array<protocol_ie_single_container_s<otdoa_info_type_item_ies_o>>;

// PositioningActivationFailureIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct positioning_activation_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningActivationRequestIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct positioning_activation_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srs_type, activation_time, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srs_type_c&                             srs_type();
    fixed_bitstring<64, false, true>&       activation_time();
    const srs_type_c&                       srs_type() const;
    const fixed_bitstring<64, false, true>& activation_time() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningActivationResponseIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct positioning_activation_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { crit_diagnostics, sys_frame_num, slot_num, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    crit_diagnostics_s&       crit_diagnostics();
    uint16_t&                 sys_frame_num();
    uint8_t&                  slot_num();
    const crit_diagnostics_s& crit_diagnostics() const;
    const uint16_t&           sys_frame_num() const;
    const uint8_t&            slot_num() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningInformationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct positioning_info_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningInformationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct positioning_info_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        requested_srs_tx_characteristics,
        ue_report_info,
        ue_teg_info_request,
        ue_teg_report_periodicity,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    requested_srs_tx_characteristics_s&       requested_srs_tx_characteristics();
    ue_report_info_s&                         ue_report_info();
    ue_teg_info_request_e&                    ue_teg_info_request();
    ue_teg_report_periodicity_e&              ue_teg_report_periodicity();
    const requested_srs_tx_characteristics_s& requested_srs_tx_characteristics() const;
    const ue_report_info_s&                   ue_report_info() const;
    const ue_teg_info_request_e&              ue_teg_info_request() const;
    const ue_teg_report_periodicity_e&        ue_teg_report_periodicity() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningInformationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct positioning_info_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srscfg, sfn_initisation_time, crit_diagnostics, ue_tx_teg_assoc_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srscfg_s&                               srscfg();
    fixed_bitstring<64, false, true>&       sfn_initisation_time();
    crit_diagnostics_s&                     crit_diagnostics();
    ue_tx_teg_assoc_list_l&                 ue_tx_teg_assoc_list();
    const srscfg_s&                         srscfg() const;
    const fixed_bitstring<64, false, true>& sfn_initisation_time() const;
    const crit_diagnostics_s&               crit_diagnostics() const;
    const ue_tx_teg_assoc_list_l&           ue_tx_teg_assoc_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningInformationUpdate-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct positioning_info_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srscfg, sfn_initisation_time, ue_tx_teg_assoc_list, srs_tx_status, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srscfg_s&                               srscfg();
    fixed_bitstring<64, false, true>&       sfn_initisation_time();
    ue_tx_teg_assoc_list_l&                 ue_tx_teg_assoc_list();
    srs_tx_status_e&                        srs_tx_status();
    const srscfg_s&                         srscfg() const;
    const fixed_bitstring<64, false, true>& sfn_initisation_time() const;
    const ue_tx_teg_assoc_list_l&           ue_tx_teg_assoc_list() const;
    const srs_tx_status_e&                  srs_tx_status() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PRSConfigurationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct prs_cfg_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PRSConfigurationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct prs_cfg_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { prs_cfg_request_type, prstrp_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    prs_cfg_request_type_e&       prs_cfg_request_type();
    prstrp_list_l&                prstrp_list();
    const prs_cfg_request_type_e& prs_cfg_request_type() const;
    const prstrp_list_l&          prstrp_list() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PRSConfigurationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct prs_cfg_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { prs_tx_trp_list, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    prs_tx_trp_list_l&        prs_tx_trp_list();
    crit_diagnostics_s&       crit_diagnostics();
    const prs_tx_trp_list_l&  prs_tx_trp_list() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRPInformationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct trp_info_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRPInformationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct trp_info_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_list, trp_info_type_list_trp_req, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    trp_list_l&                         trp_list();
    trp_info_type_list_trp_req_l&       trp_info_type_list_trp_req();
    const trp_list_l&                   trp_list() const;
    const trp_info_type_list_trp_req_l& trp_info_type_list_trp_req() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRPInformationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct trp_info_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_info_list_trp_resp, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    value_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    trp_info_list_trp_resp_l&       trp_info_list_trp_resp();
    crit_diagnostics_s&             crit_diagnostics();
    const trp_info_list_trp_resp_l& trp_info_list_trp_resp() const;
    const crit_diagnostics_s&       crit_diagnostics() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// TRP-PRS-Information-List ::= SEQUENCE (SIZE (1..256)) OF TRP-PRS-Information-List-Item
using trp_prs_info_list_l = dyn_array<trp_prs_info_list_item_s>;

// MeasurementPreconfigurationRequired-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct meas_precfg_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_prs_info_list, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::trp_prs_info_list; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    trp_prs_info_list_l&       trp_prs_info_list() { return c; }
    const trp_prs_info_list_l& trp_prs_info_list() const { return c; }

  private:
    trp_prs_info_list_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// OTDOAInformationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct otdoa_info_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { otdoa_info_type_group, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::otdoa_info_type_group; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    otdoa_info_type_l&       otdoa_info_type_group() { return c; }
    const otdoa_info_type_l& otdoa_info_type_group() const { return c; }

  private:
    otdoa_info_type_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PositioningDeactivationIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
struct positioning_deactivation_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { abort_tx, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::abort_tx; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    abort_tx_c&       abort_tx() { return c; }
    const abort_tx_c& abort_tx() const { return c; }

  private:
    abort_tx_c c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct private_ie_container_empty_l {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct assist_info_ctrl_ies_container {
  bool                          assist_info_present                 = false;
  bool                          broadcast_present                   = false;
  bool                          positioning_broadcast_cells_present = false;
  assist_info_s                 assist_info;
  broadcast_e                   broadcast;
  positioning_broadcast_cells_l positioning_broadcast_cells;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct assist_info_feedback_ies_container {
  bool                          assist_info_fail_list_present       = false;
  bool                          positioning_broadcast_cells_present = false;
  bool                          crit_diagnostics_present            = false;
  assist_info_fail_list_l       assist_info_fail_list;
  positioning_broadcast_cells_l positioning_broadcast_cells;
  crit_diagnostics_s            crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e_c_id_meas_fail_ind_ies_container {
  uint8_t lmf_ue_meas_id;
  uint8_t ran_ue_meas_id;
  cause_c cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e_c_id_meas_initiation_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint8_t            lmf_ue_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e_c_id_meas_initiation_request_ies_container {
  bool                        meas_periodicity_present          = false;
  bool                        other_rat_meas_quantities_present = false;
  bool                        wlan_meas_quantities_present      = false;
  bool                        meas_periodicity_nr_ao_a_present  = false;
  uint8_t                     lmf_ue_meas_id;
  report_characteristics_e    report_characteristics;
  meas_periodicity_e          meas_periodicity;
  meas_quantities_l           meas_quantities;
  other_rat_meas_quantities_l other_rat_meas_quantities;
  wlan_meas_quantities_l      wlan_meas_quantities;
  meas_periodicity_nr_ao_a_e  meas_periodicity_nr_ao_a;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e_c_id_meas_initiation_resp_ies_container {
  bool                    e_c_id_meas_result_present    = false;
  bool                    crit_diagnostics_present      = false;
  bool                    cell_portion_id_present       = false;
  bool                    other_rat_meas_result_present = false;
  bool                    wlan_meas_result_present      = false;
  uint8_t                 lmf_ue_meas_id;
  uint8_t                 ran_ue_meas_id;
  e_c_id_meas_result_s    e_c_id_meas_result;
  crit_diagnostics_s      crit_diagnostics;
  uint16_t                cell_portion_id;
  other_rat_meas_result_l other_rat_meas_result;
  wlan_meas_result_l      wlan_meas_result;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e_c_id_meas_report_ies_container {
  bool                 cell_portion_id_present = false;
  uint8_t              lmf_ue_meas_id;
  uint8_t              ran_ue_meas_id;
  e_c_id_meas_result_s e_c_id_meas_result;
  uint16_t             cell_portion_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct e_c_id_meas_termination_cmd_ies_container {
  uint8_t lmf_ue_meas_id;
  uint8_t ran_ue_meas_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct error_ind_ies_container {
  bool               cause_present            = false;
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_abort_ies_container {
  uint32_t lmf_meas_id;
  uint32_t ran_meas_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_activation_ies_container {
  bool                  prs_meass_info_list_present = false;
  request_type_e        request_type;
  prs_meass_info_list_l prs_meass_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint32_t           lmf_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_fail_ind_ies_container {
  uint32_t lmf_meas_id;
  uint32_t ran_meas_id;
  cause_c  cause;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_precfg_confirm_ies_container {
  bool                            crit_diagnostics_present = false;
  fixed_bitstring<8, false, true> precfg_result;
  crit_diagnostics_s              crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_precfg_refuse_ies_container {
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_report_ies_container {
  uint32_t             lmf_meas_id;
  uint32_t             ran_meas_id;
  trp_meas_resp_list_l trp_meas_report_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_request_ies_container {
  bool                             meas_periodicity_present                 = false;
  bool                             sfn_initisation_time_present             = false;
  bool                             srscfg_present                           = false;
  bool                             meas_beam_info_request_present           = false;
  bool                             sys_frame_num_present                    = false;
  bool                             slot_num_present                         = false;
  bool                             meas_periodicity_extended_present        = false;
  bool                             resp_time_present                        = false;
  bool                             meas_characteristics_request_ind_present = false;
  bool                             meas_time_occasion_present               = false;
  bool                             meas_amount_present                      = false;
  uint32_t                         lmf_meas_id;
  trp_meas_request_list_l          trp_meas_request_list;
  report_characteristics_e         report_characteristics;
  meas_periodicity_e               meas_periodicity;
  trp_meas_quantities_l            trp_meas_quantities;
  fixed_bitstring<64, false, true> sfn_initisation_time;
  srscfg_s                         srscfg;
  meas_beam_info_request_e         meas_beam_info_request;
  uint16_t                         sys_frame_num;
  uint8_t                          slot_num;
  meas_periodicity_extended_e      meas_periodicity_extended;
  resp_time_s                      resp_time;
  fixed_bitstring<16, false, true> meas_characteristics_request_ind;
  meas_time_occasion_e             meas_time_occasion;
  meas_amount_e                    meas_amount;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_resp_ies_container {
  bool                 trp_meas_resp_list_present = false;
  bool                 crit_diagnostics_present   = false;
  uint32_t             lmf_meas_id;
  uint32_t             ran_meas_id;
  trp_meas_resp_list_l trp_meas_resp_list;
  crit_diagnostics_s   crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct meas_upd_ies_container {
  bool                             srscfg_present                           = false;
  bool                             trp_meas_upd_list_present                = false;
  bool                             meas_characteristics_request_ind_present = false;
  bool                             meas_time_occasion_present               = false;
  uint32_t                         lmf_meas_id;
  uint32_t                         ran_meas_id;
  srscfg_s                         srscfg;
  trp_meas_upd_list_l              trp_meas_upd_list;
  fixed_bitstring<16, false, true> meas_characteristics_request_ind;
  meas_time_occasion_e             meas_time_occasion;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct otdoa_info_fail_ies_container {
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct otdoa_info_resp_ies_container {
  bool               crit_diagnostics_present = false;
  otdoa_cells_l      otdoa_cells;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct positioning_activation_fail_ies_container {
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct positioning_activation_request_ies_container {
  bool                             activation_time_present = false;
  srs_type_c                       srs_type;
  fixed_bitstring<64, false, true> activation_time;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct positioning_activation_resp_ies_container {
  bool               crit_diagnostics_present = false;
  bool               sys_frame_num_present    = false;
  bool               slot_num_present         = false;
  crit_diagnostics_s crit_diagnostics;
  uint16_t           sys_frame_num;
  uint8_t            slot_num;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct positioning_info_fail_ies_container {
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct positioning_info_request_ies_container {
  bool                               requested_srs_tx_characteristics_present = false;
  bool                               ue_report_info_present                   = false;
  bool                               ue_teg_info_request_present              = false;
  bool                               ue_teg_report_periodicity_present        = false;
  requested_srs_tx_characteristics_s requested_srs_tx_characteristics;
  ue_report_info_s                   ue_report_info;
  ue_teg_info_request_e              ue_teg_info_request;
  ue_teg_report_periodicity_e        ue_teg_report_periodicity;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct positioning_info_resp_ies_container {
  bool                             srscfg_present               = false;
  bool                             sfn_initisation_time_present = false;
  bool                             crit_diagnostics_present     = false;
  bool                             ue_tx_teg_assoc_list_present = false;
  srscfg_s                         srscfg;
  fixed_bitstring<64, false, true> sfn_initisation_time;
  crit_diagnostics_s               crit_diagnostics;
  ue_tx_teg_assoc_list_l           ue_tx_teg_assoc_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct positioning_info_upd_ies_container {
  bool                             srscfg_present               = false;
  bool                             sfn_initisation_time_present = false;
  bool                             ue_tx_teg_assoc_list_present = false;
  bool                             srs_tx_status_present        = false;
  srscfg_s                         srscfg;
  fixed_bitstring<64, false, true> sfn_initisation_time;
  ue_tx_teg_assoc_list_l           ue_tx_teg_assoc_list;
  srs_tx_status_e                  srs_tx_status;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

using private_msg_ies_container = private_ie_container_empty_l;

struct prs_cfg_fail_ies_container {
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct prs_cfg_request_ies_container {
  prs_cfg_request_type_e prs_cfg_request_type;
  prstrp_list_l          prstrp_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct prs_cfg_resp_ies_container {
  bool               prs_tx_trp_list_present  = false;
  bool               crit_diagnostics_present = false;
  prs_tx_trp_list_l  prs_tx_trp_list;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct trp_info_fail_ies_container {
  bool               crit_diagnostics_present = false;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct trp_info_request_ies_container {
  bool                         trp_list_present = false;
  trp_list_l                   trp_list;
  trp_info_type_list_trp_req_l trp_info_type_list_trp_req;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct trp_info_resp_ies_container {
  bool                     crit_diagnostics_present = false;
  trp_info_list_trp_resp_l trp_info_list_trp_resp;
  crit_diagnostics_s       crit_diagnostics;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AssistanceInformationControl ::= SEQUENCE
using assist_info_ctrl_s = elementary_procedure_option<assist_info_ctrl_ies_container>;

// AssistanceInformationFeedback ::= SEQUENCE
using assist_info_feedback_s = elementary_procedure_option<assist_info_feedback_ies_container>;

// E-CIDMeasurementFailureIndication ::= SEQUENCE
using e_c_id_meas_fail_ind_s = elementary_procedure_option<e_c_id_meas_fail_ind_ies_container>;

// E-CIDMeasurementInitiationFailure ::= SEQUENCE
using e_c_id_meas_initiation_fail_s = elementary_procedure_option<e_c_id_meas_initiation_fail_ies_container>;

// E-CIDMeasurementInitiationRequest ::= SEQUENCE
using e_c_id_meas_initiation_request_s = elementary_procedure_option<e_c_id_meas_initiation_request_ies_container>;

// E-CIDMeasurementInitiationResponse ::= SEQUENCE
using e_c_id_meas_initiation_resp_s = elementary_procedure_option<e_c_id_meas_initiation_resp_ies_container>;

// E-CIDMeasurementReport ::= SEQUENCE
using e_c_id_meas_report_s = elementary_procedure_option<e_c_id_meas_report_ies_container>;

// E-CIDMeasurementTerminationCommand ::= SEQUENCE
using e_c_id_meas_termination_cmd_s = elementary_procedure_option<e_c_id_meas_termination_cmd_ies_container>;

// ErrorIndication ::= SEQUENCE
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;

// MeasurementAbort ::= SEQUENCE
using meas_abort_s = elementary_procedure_option<meas_abort_ies_container>;

// MeasurementActivation ::= SEQUENCE
using meas_activation_s = elementary_procedure_option<meas_activation_ies_container>;

// MeasurementFailureIndication ::= SEQUENCE
using meas_fail_ind_s = elementary_procedure_option<meas_fail_ind_ies_container>;

// MeasurementFailure ::= SEQUENCE
using meas_fail_s = elementary_procedure_option<meas_fail_ies_container>;

// MeasurementPreconfigurationConfirm ::= SEQUENCE
using meas_precfg_confirm_s = elementary_procedure_option<meas_precfg_confirm_ies_container>;

// MeasurementPreconfigurationRefuse ::= SEQUENCE
using meas_precfg_refuse_s = elementary_procedure_option<meas_precfg_refuse_ies_container>;

// MeasurementPreconfigurationRequired ::= SEQUENCE
using meas_precfg_required_s = elementary_procedure_option<protocol_ie_container_l<meas_precfg_required_ies_o>>;

// MeasurementReport ::= SEQUENCE
using meas_report_s = elementary_procedure_option<meas_report_ies_container>;

// MeasurementRequest ::= SEQUENCE
using meas_request_s = elementary_procedure_option<meas_request_ies_container>;

// MeasurementResponse ::= SEQUENCE
using meas_resp_s = elementary_procedure_option<meas_resp_ies_container>;

// MeasurementUpdate ::= SEQUENCE
using meas_upd_s = elementary_procedure_option<meas_upd_ies_container>;

// OTDOAInformationFailure ::= SEQUENCE
using otdoa_info_fail_s = elementary_procedure_option<otdoa_info_fail_ies_container>;

// OTDOAInformationRequest ::= SEQUENCE
using otdoa_info_request_s = elementary_procedure_option<protocol_ie_container_l<otdoa_info_request_ies_o>>;

// OTDOAInformationResponse ::= SEQUENCE
using otdoa_info_resp_s = elementary_procedure_option<otdoa_info_resp_ies_container>;

// PositioningActivationFailure ::= SEQUENCE
using positioning_activation_fail_s = elementary_procedure_option<positioning_activation_fail_ies_container>;

// PositioningActivationRequest ::= SEQUENCE
using positioning_activation_request_s = elementary_procedure_option<positioning_activation_request_ies_container>;

// PositioningActivationResponse ::= SEQUENCE
using positioning_activation_resp_s = elementary_procedure_option<positioning_activation_resp_ies_container>;

// PositioningDeactivation ::= SEQUENCE
using positioning_deactivation_s = elementary_procedure_option<protocol_ie_container_l<positioning_deactivation_ies_o>>;

// PositioningInformationFailure ::= SEQUENCE
using positioning_info_fail_s = elementary_procedure_option<positioning_info_fail_ies_container>;

// PositioningInformationRequest ::= SEQUENCE
using positioning_info_request_s = elementary_procedure_option<positioning_info_request_ies_container>;

// PositioningInformationResponse ::= SEQUENCE
using positioning_info_resp_s = elementary_procedure_option<positioning_info_resp_ies_container>;

// PositioningInformationUpdate ::= SEQUENCE
using positioning_info_upd_s = elementary_procedure_option<positioning_info_upd_ies_container>;

// PrivateMessage ::= SEQUENCE
struct private_msg_s {
  bool                      ext = false;
  private_msg_ies_container private_ies;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRSConfigurationFailure ::= SEQUENCE
using prs_cfg_fail_s = elementary_procedure_option<prs_cfg_fail_ies_container>;

// PRSConfigurationRequest ::= SEQUENCE
using prs_cfg_request_s = elementary_procedure_option<prs_cfg_request_ies_container>;

// PRSConfigurationResponse ::= SEQUENCE
using prs_cfg_resp_s = elementary_procedure_option<prs_cfg_resp_ies_container>;

// TRPInformationFailure ::= SEQUENCE
using trp_info_fail_s = elementary_procedure_option<trp_info_fail_ies_container>;

// TRPInformationRequest ::= SEQUENCE
using trp_info_request_s = elementary_procedure_option<trp_info_request_ies_container>;

// TRPInformationResponse ::= SEQUENCE
using trp_info_resp_s = elementary_procedure_option<trp_info_resp_ies_container>;

// NRPPA-ELEMENTARY-PROCEDURES ::= OBJECT SET OF NRPPA-ELEMENTARY-PROCEDURE
struct nr_ppa_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        e_c_id_meas_initiation_request,
        otdoa_info_request,
        positioning_info_request,
        meas_request,
        trp_info_request,
        positioning_activation_request,
        prs_cfg_request,
        meas_precfg_required,
        e_c_id_meas_fail_ind,
        e_c_id_meas_report,
        e_c_id_meas_termination_cmd,
        error_ind,
        private_msg,
        assist_info_ctrl,
        assist_info_feedback,
        positioning_info_upd,
        meas_report,
        meas_upd,
        meas_abort,
        meas_fail_ind,
        positioning_deactivation,
        meas_activation,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e_c_id_meas_initiation_request_s&       e_c_id_meas_initiation_request();
    otdoa_info_request_s&                   otdoa_info_request();
    positioning_info_request_s&             positioning_info_request();
    meas_request_s&                         meas_request();
    trp_info_request_s&                     trp_info_request();
    positioning_activation_request_s&       positioning_activation_request();
    prs_cfg_request_s&                      prs_cfg_request();
    meas_precfg_required_s&                 meas_precfg_required();
    e_c_id_meas_fail_ind_s&                 e_c_id_meas_fail_ind();
    e_c_id_meas_report_s&                   e_c_id_meas_report();
    e_c_id_meas_termination_cmd_s&          e_c_id_meas_termination_cmd();
    error_ind_s&                            error_ind();
    private_msg_s&                          private_msg();
    assist_info_ctrl_s&                     assist_info_ctrl();
    assist_info_feedback_s&                 assist_info_feedback();
    positioning_info_upd_s&                 positioning_info_upd();
    meas_report_s&                          meas_report();
    meas_upd_s&                             meas_upd();
    meas_abort_s&                           meas_abort();
    meas_fail_ind_s&                        meas_fail_ind();
    positioning_deactivation_s&             positioning_deactivation();
    meas_activation_s&                      meas_activation();
    const e_c_id_meas_initiation_request_s& e_c_id_meas_initiation_request() const;
    const otdoa_info_request_s&             otdoa_info_request() const;
    const positioning_info_request_s&       positioning_info_request() const;
    const meas_request_s&                   meas_request() const;
    const trp_info_request_s&               trp_info_request() const;
    const positioning_activation_request_s& positioning_activation_request() const;
    const prs_cfg_request_s&                prs_cfg_request() const;
    const meas_precfg_required_s&           meas_precfg_required() const;
    const e_c_id_meas_fail_ind_s&           e_c_id_meas_fail_ind() const;
    const e_c_id_meas_report_s&             e_c_id_meas_report() const;
    const e_c_id_meas_termination_cmd_s&    e_c_id_meas_termination_cmd() const;
    const error_ind_s&                      error_ind() const;
    const private_msg_s&                    private_msg() const;
    const assist_info_ctrl_s&               assist_info_ctrl() const;
    const assist_info_feedback_s&           assist_info_feedback() const;
    const positioning_info_upd_s&           positioning_info_upd() const;
    const meas_report_s&                    meas_report() const;
    const meas_upd_s&                       meas_upd() const;
    const meas_abort_s&                     meas_abort() const;
    const meas_fail_ind_s&                  meas_fail_ind() const;
    const positioning_deactivation_s&       positioning_deactivation() const;
    const meas_activation_s&                meas_activation() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        e_c_id_meas_initiation_resp,
        otdoa_info_resp,
        positioning_info_resp,
        meas_resp,
        trp_info_resp,
        positioning_activation_resp,
        prs_cfg_resp,
        meas_precfg_confirm,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    successful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e_c_id_meas_initiation_resp_s&       e_c_id_meas_initiation_resp();
    otdoa_info_resp_s&                   otdoa_info_resp();
    positioning_info_resp_s&             positioning_info_resp();
    meas_resp_s&                         meas_resp();
    trp_info_resp_s&                     trp_info_resp();
    positioning_activation_resp_s&       positioning_activation_resp();
    prs_cfg_resp_s&                      prs_cfg_resp();
    meas_precfg_confirm_s&               meas_precfg_confirm();
    const e_c_id_meas_initiation_resp_s& e_c_id_meas_initiation_resp() const;
    const otdoa_info_resp_s&             otdoa_info_resp() const;
    const positioning_info_resp_s&       positioning_info_resp() const;
    const meas_resp_s&                   meas_resp() const;
    const trp_info_resp_s&               trp_info_resp() const;
    const positioning_activation_resp_s& positioning_activation_resp() const;
    const prs_cfg_resp_s&                prs_cfg_resp() const;
    const meas_precfg_confirm_s&         meas_precfg_confirm() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        e_c_id_meas_initiation_fail,
        otdoa_info_fail,
        positioning_info_fail,
        meas_fail,
        trp_info_fail,
        positioning_activation_fail,
        prs_cfg_fail,
        meas_precfg_refuse,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    unsuccessful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e_c_id_meas_initiation_fail_s&       e_c_id_meas_initiation_fail();
    otdoa_info_fail_s&                   otdoa_info_fail();
    positioning_info_fail_s&             positioning_info_fail();
    meas_fail_s&                         meas_fail();
    trp_info_fail_s&                     trp_info_fail();
    positioning_activation_fail_s&       positioning_activation_fail();
    prs_cfg_fail_s&                      prs_cfg_fail();
    meas_precfg_refuse_s&                meas_precfg_refuse();
    const e_c_id_meas_initiation_fail_s& e_c_id_meas_initiation_fail() const;
    const otdoa_info_fail_s&             otdoa_info_fail() const;
    const positioning_info_fail_s&       positioning_info_fail() const;
    const meas_fail_s&                   meas_fail() const;
    const trp_info_fail_s&               trp_info_fail() const;
    const positioning_activation_fail_s& positioning_activation_fail() const;
    const prs_cfg_fail_s&                prs_cfg_fail() const;
    const meas_precfg_refuse_s&          meas_precfg_refuse() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// InitiatingMessage ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                        proc_code = 0;
  crit_e                          crit;
  uint16_t                        nrppatransaction_id = 0;
  nr_ppa_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

using srs_res_id_item_ext_ies_container = protocol_ext_container_empty_l;

// SuccessfulOutcome ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  uint16_t                                  nrppatransaction_id = 0;
  nr_ppa_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                    proc_code = 0;
  crit_e                                      crit;
  uint16_t                                    nrppatransaction_id = 0;
  nr_ppa_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// NRPPA-ELEMENTARY-PROCEDURES-CLASS-1 ::= OBJECT SET OF NRPPA-ELEMENTARY-PROCEDURE
struct nr_ppa_elem_procs_class_1_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        e_c_id_meas_initiation_request,
        otdoa_info_request,
        positioning_info_request,
        meas_request,
        trp_info_request,
        positioning_activation_request,
        prs_cfg_request,
        meas_precfg_required,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e_c_id_meas_initiation_request_s&       e_c_id_meas_initiation_request();
    otdoa_info_request_s&                   otdoa_info_request();
    positioning_info_request_s&             positioning_info_request();
    meas_request_s&                         meas_request();
    trp_info_request_s&                     trp_info_request();
    positioning_activation_request_s&       positioning_activation_request();
    prs_cfg_request_s&                      prs_cfg_request();
    meas_precfg_required_s&                 meas_precfg_required();
    const e_c_id_meas_initiation_request_s& e_c_id_meas_initiation_request() const;
    const otdoa_info_request_s&             otdoa_info_request() const;
    const positioning_info_request_s&       positioning_info_request() const;
    const meas_request_s&                   meas_request() const;
    const trp_info_request_s&               trp_info_request() const;
    const positioning_activation_request_s& positioning_activation_request() const;
    const prs_cfg_request_s&                prs_cfg_request() const;
    const meas_precfg_required_s&           meas_precfg_required() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        e_c_id_meas_initiation_resp,
        otdoa_info_resp,
        positioning_info_resp,
        meas_resp,
        trp_info_resp,
        positioning_activation_resp,
        prs_cfg_resp,
        meas_precfg_confirm,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    successful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e_c_id_meas_initiation_resp_s&       e_c_id_meas_initiation_resp();
    otdoa_info_resp_s&                   otdoa_info_resp();
    positioning_info_resp_s&             positioning_info_resp();
    meas_resp_s&                         meas_resp();
    trp_info_resp_s&                     trp_info_resp();
    positioning_activation_resp_s&       positioning_activation_resp();
    prs_cfg_resp_s&                      prs_cfg_resp();
    meas_precfg_confirm_s&               meas_precfg_confirm();
    const e_c_id_meas_initiation_resp_s& e_c_id_meas_initiation_resp() const;
    const otdoa_info_resp_s&             otdoa_info_resp() const;
    const positioning_info_resp_s&       positioning_info_resp() const;
    const meas_resp_s&                   meas_resp() const;
    const trp_info_resp_s&               trp_info_resp() const;
    const positioning_activation_resp_s& positioning_activation_resp() const;
    const prs_cfg_resp_s&                prs_cfg_resp() const;
    const meas_precfg_confirm_s&         meas_precfg_confirm() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        e_c_id_meas_initiation_fail,
        otdoa_info_fail,
        positioning_info_fail,
        meas_fail,
        trp_info_fail,
        positioning_activation_fail,
        prs_cfg_fail,
        meas_precfg_refuse,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    unsuccessful_outcome_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e_c_id_meas_initiation_fail_s&       e_c_id_meas_initiation_fail();
    otdoa_info_fail_s&                   otdoa_info_fail();
    positioning_info_fail_s&             positioning_info_fail();
    meas_fail_s&                         meas_fail();
    trp_info_fail_s&                     trp_info_fail();
    positioning_activation_fail_s&       positioning_activation_fail();
    prs_cfg_fail_s&                      prs_cfg_fail();
    meas_precfg_refuse_s&                meas_precfg_refuse();
    const e_c_id_meas_initiation_fail_s& e_c_id_meas_initiation_fail() const;
    const otdoa_info_fail_s&             otdoa_info_fail() const;
    const positioning_info_fail_s&       positioning_info_fail() const;
    const meas_fail_s&                   meas_fail() const;
    const trp_info_fail_s&               trp_info_fail() const;
    const positioning_activation_fail_s& positioning_activation_fail() const;
    const prs_cfg_fail_s&                prs_cfg_fail() const;
    const meas_precfg_refuse_s&          meas_precfg_refuse() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// NRPPA-ELEMENTARY-PROCEDURES-CLASS-2 ::= OBJECT SET OF NRPPA-ELEMENTARY-PROCEDURE
struct nr_ppa_elem_procs_class_2_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        e_c_id_meas_fail_ind,
        e_c_id_meas_report,
        e_c_id_meas_termination_cmd,
        error_ind,
        private_msg,
        assist_info_ctrl,
        assist_info_feedback,
        positioning_info_upd,
        meas_report,
        meas_upd,
        meas_abort,
        meas_fail_ind,
        positioning_deactivation,
        meas_activation,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e_c_id_meas_fail_ind_s&              e_c_id_meas_fail_ind();
    e_c_id_meas_report_s&                e_c_id_meas_report();
    e_c_id_meas_termination_cmd_s&       e_c_id_meas_termination_cmd();
    error_ind_s&                         error_ind();
    private_msg_s&                       private_msg();
    assist_info_ctrl_s&                  assist_info_ctrl();
    assist_info_feedback_s&              assist_info_feedback();
    positioning_info_upd_s&              positioning_info_upd();
    meas_report_s&                       meas_report();
    meas_upd_s&                          meas_upd();
    meas_abort_s&                        meas_abort();
    meas_fail_ind_s&                     meas_fail_ind();
    positioning_deactivation_s&          positioning_deactivation();
    meas_activation_s&                   meas_activation();
    const e_c_id_meas_fail_ind_s&        e_c_id_meas_fail_ind() const;
    const e_c_id_meas_report_s&          e_c_id_meas_report() const;
    const e_c_id_meas_termination_cmd_s& e_c_id_meas_termination_cmd() const;
    const error_ind_s&                   error_ind() const;
    const private_msg_s&                 private_msg() const;
    const assist_info_ctrl_s&            assist_info_ctrl() const;
    const assist_info_feedback_s&        assist_info_feedback() const;
    const positioning_info_upd_s&        positioning_info_upd() const;
    const meas_report_s&                 meas_report() const;
    const meas_upd_s&                    meas_upd() const;
    const meas_abort_s&                  meas_abort() const;
    const meas_fail_ind_s&               meas_fail_ind() const;
    const positioning_deactivation_s&    positioning_deactivation() const;
    const meas_activation_s&             meas_activation() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options { nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options { nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nulltype; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// NRPPA-PDU ::= CHOICE
struct nr_ppa_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  nr_ppa_pdu_c() = default;
  nr_ppa_pdu_c(const nr_ppa_pdu_c& other);
  nr_ppa_pdu_c& operator=(const nr_ppa_pdu_c& other);
  ~nr_ppa_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type(types::init_msg, type_, "NRPPA-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, type_, "NRPPA-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "NRPPA-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, type_, "NRPPA-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, type_, "NRPPA-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "NRPPA-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  init_msg_s&             set_init_msg();
  successful_outcome_s&   set_successful_outcome();
  unsuccessful_outcome_s& set_unsuccessful_outcome();

private:
  types                                                                     type_;
  choice_buffer_t<init_msg_s, successful_outcome_s, unsuccessful_outcome_s> c;

  void destroy_();
};

// PrivateIE-Container{NRPPA-PRIVATE-IES : IEsSetParam} ::= SEQUENCE (SIZE (1..65535)) OF PrivateIE-Field
template <class ies_set_paramT_>
using private_ie_container_l = dyn_seq_of<private_ie_field_s<ies_set_paramT_>, 1, 65535, true>;

// SRSResourceID-Item ::= SEQUENCE
struct srs_res_id_item_s {
  bool                              ext             = false;
  bool                              ie_exts_present = false;
  uint8_t                           srs_res_id      = 0;
  srs_res_id_item_ext_ies_container ie_exts;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace nrppa
} // namespace asn1

extern template struct asn1::protocol_ext_field_s<asn1::nrppa::add_path_list_item_ext_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::assist_info_ctrl_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::assist_info_feedback_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::measured_results_value_ext_ie_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::e_c_id_meas_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::e_c_id_meas_initiation_fail_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::meas_quantities_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::other_rat_meas_quantities_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::wlan_meas_quantities_item_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::e_c_id_meas_initiation_request_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::other_rat_measured_results_value_ext_ie_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::e_c_id_meas_initiation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::e_c_id_meas_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::e_c_id_meas_termination_cmd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::error_ind_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::nrppa::gnb_rx_tx_time_diff_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::nrppa::ul_rtoameas_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::trp_measured_results_value_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::trp_info_type_resp_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::nrppa::trp_meas_result_item_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::otdoa_cell_info_item_ext_ie_o>;
extern template struct asn1::protocol_ext_field_s<asn1::nrppa::semipersistent_srs_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::nrppa::trp_meas_request_item_ext_ies_o>;
extern template struct asn1::protocol_ext_field_s<asn1::nrppa::trp_meas_upd_item_ext_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::otdoa_info_type_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::nrppa::trp_info_type_item_trp_req_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_abort_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_activation_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_fail_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_precfg_confirm_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_precfg_refuse_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_report_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::meas_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::otdoa_info_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::otdoa_info_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::prs_cfg_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::prs_cfg_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::prs_cfg_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::positioning_activation_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::positioning_activation_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::positioning_activation_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::positioning_info_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::positioning_info_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::positioning_info_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::positioning_info_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::trp_info_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::trp_info_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::nrppa::trp_info_resp_ies_o>;
