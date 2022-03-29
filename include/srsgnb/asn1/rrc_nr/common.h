/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v15.11.0 (2020-10)
 *
 ******************************************************************************/

#ifndef SRSASN1_RRC_NR_COMMON_H
#define SRSASN1_RRC_NR_COMMON_H

#include "srsgnb/asn1/asn1_utils.h"
#include <cstdio>
#include <stdarg.h>

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_RRC_NR_MAX_BAND_COMB 65536
#define ASN1_RRC_NR_MAX_CELL_BLACK 16
#define ASN1_RRC_NR_MAX_CELL_INTER 16
#define ASN1_RRC_NR_MAX_CELL_INTRA 16
#define ASN1_RRC_NR_MAX_CELL_MEAS_EUTRA 32
#define ASN1_RRC_NR_MAX_EARFCN 262143
#define ASN1_RRC_NR_MAX_EUTRA_CELL_BLACK 16
#define ASN1_RRC_NR_MAX_EUTRA_NS_PMAX 8
#define ASN1_RRC_NR_MAX_MULTI_BANDS 8
#define ASN1_RRC_NR_MAX_NARFCN 3279165
#define ASN1_RRC_NR_MAX_NR_NS_PMAX 8
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS 32
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS_MINUS1 31
#define ASN1_RRC_NR_MAX_NROF_AGGREGATED_CELLS_PER_CELL_GROUP 16
#define ASN1_RRC_NR_MAX_NROF_SCELLS 31
#define ASN1_RRC_NR_MAX_NROF_CELL_MEAS 32
#define ASN1_RRC_NR_MAX_NROF_SS_BLOCKS_TO_AVERAGE 16
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_TO_AVERAGE 16
#define ASN1_RRC_NR_MAX_NROF_DL_ALLOCS 16
#define ASN1_RRC_NR_MAX_NROF_SR_CFG_PER_CELL_GROUP 8
#define ASN1_RRC_NR_MAX_LCG_ID 7
#define ASN1_RRC_NR_MAX_LC_ID 32
#define ASN1_RRC_NR_MAX_NROF_TAGS 4
#define ASN1_RRC_NR_MAX_NROF_TAGS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_BWPS 4
#define ASN1_RRC_NR_MAX_NROF_COMB_IDC 128
#define ASN1_RRC_NR_MAX_NROF_SYMBOLS_MINUS1 13
#define ASN1_RRC_NR_MAX_NROF_SLOTS 320
#define ASN1_RRC_NR_MAX_NROF_SLOTS_MINUS1 319
#define ASN1_RRC_NR_MAX_NROF_PHYS_RES_BLOCKS 275
#define ASN1_RRC_NR_MAX_NROF_PHYS_RES_BLOCKS_MINUS1 274
#define ASN1_RRC_NR_MAX_NROF_PHYS_RES_BLOCKS_PLUS1 276
#define ASN1_RRC_NR_MAX_NROF_CORESETS_MINUS1 11
#define ASN1_RRC_NR_MAX_CO_RE_SET_DUR 3
#define ASN1_RRC_NR_MAX_NROF_SEARCH_SPACES_MINUS1 39
#define ASN1_RRC_NR_MAX_SFI_DCI_PAYLOAD_SIZE 128
#define ASN1_RRC_NR_MAX_SFI_DCI_PAYLOAD_SIZE_MINUS1 127
#define ASN1_RRC_NR_MAX_INT_DCI_PAYLOAD_SIZE 126
#define ASN1_RRC_NR_MAX_INT_DCI_PAYLOAD_SIZE_MINUS1 125
#define ASN1_RRC_NR_MAX_NROF_RATE_MATCH_PATTERNS 4
#define ASN1_RRC_NR_MAX_NROF_RATE_MATCH_PATTERNS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_RATE_MATCH_PATTERNS_PER_GROUP 8
#define ASN1_RRC_NR_MAX_NROF_CSI_REPORT_CFGS 48
#define ASN1_RRC_NR_MAX_NROF_CSI_REPORT_CFGS_MINUS1 47
#define ASN1_RRC_NR_MAX_NROF_CSI_RES_CFGS 112
#define ASN1_RRC_NR_MAX_NROF_CSI_RES_CFGS_MINUS1 111
#define ASN1_RRC_NR_MAX_NROF_AP_CSI_RS_RES_PER_SET 16
#define ASN1_RRC_NR_MAX_NR_OF_CSI_APERIODIC_TRIGGERS 128
#define ASN1_RRC_NR_MAX_NROF_REPORT_CFG_PER_APERIODIC_TRIGGER 16
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES 192
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_MINUS1 191
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_PER_SET 64
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_SETS 64
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_SETS_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_SETS_PER_CFG 16
#define ASN1_RRC_NR_MAX_NROF_NZP_CSI_RS_RES_PER_CFG 128
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES 32
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_MINUS1 31
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_SETS_MINUS1 15
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_PER_SET 16
#define ASN1_RRC_NR_MAX_NROF_ZP_CSI_RS_RES_SETS 16
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES 32
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_MINUS1 31
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_PER_SET 8
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_SETS 64
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_SETS_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_CSI_IM_RES_SETS_PER_CFG 16
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_PER_SET 64
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS 64
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_CSI_SSB_RES_SETS_PER_CFG 1
#define ASN1_RRC_NR_MAX_NROF_FAIL_DETECTION_RES 10
#define ASN1_RRC_NR_MAX_NROF_FAIL_DETECTION_RES_MINUS1 9
#define ASN1_RRC_NR_MAX_NROF_OBJ_ID 64
#define ASN1_RRC_NR_MAX_NROF_PAGE_REC 32
#define ASN1_RRC_NR_MAX_NROF_PCI_RANGES 8
#define ASN1_RRC_NR_MAX_PLMN 12
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_RRM 96
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES_RRM_MINUS1 95
#define ASN1_RRC_NR_MAX_NROF_MEAS_ID 64
#define ASN1_RRC_NR_MAX_NROF_QUANT_CFG 2
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_CELLS_RRM 96
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_SETS 16
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_SETS_MINUS1 15
#define ASN1_RRC_NR_MAX_NROF_SRS_RES 64
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_SRS_RES_PER_SET 16
#define ASN1_RRC_NR_MAX_NROF_SRS_TRIGGER_STATES_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_SRS_TRIGGER_STATES_MINUS2 2
#define ASN1_RRC_NR_MAX_RAT_CAP_CONTAINERS 8
#define ASN1_RRC_NR_MAX_SIMUL_BANDS 32
#define ASN1_RRC_NR_MAX_NROF_SLOT_FORMAT_COMBINATIONS_PER_SET 512
#define ASN1_RRC_NR_MAX_NROF_SLOT_FORMAT_COMBINATIONS_PER_SET_MINUS1 511
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES 128
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_MINUS1 127
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_SETS 4
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_SETS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_PUCCH_RES_PER_SET 32
#define ASN1_RRC_NR_MAX_NROF_PUCCH_P0_PER_SET 8
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS 4
#define ASN1_RRC_NR_MAX_NROF_PUCCH_PATHLOSS_REF_RSS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_P0_PUSCH_ALPHA_SETS 30
#define ASN1_RRC_NR_MAX_NROF_P0_PUSCH_ALPHA_SETS_MINUS1 29
#define ASN1_RRC_NR_MAX_NROF_PUSCH_PATHLOSS_REF_RSS 4
#define ASN1_RRC_NR_MAX_NROF_PUSCH_PATHLOSS_REF_RSS_MINUS1 3
#define ASN1_RRC_NR_MAX_NROF_NAICS_ENTRIES 8
#define ASN1_RRC_NR_MAX_BANDS 1024
#define ASN1_RRC_NR_MAX_BANDS_MRDC 1280
#define ASN1_RRC_NR_MAX_BANDS_EUTRA 256
#define ASN1_RRC_NR_MAX_CELL_REPORT 8
#define ASN1_RRC_NR_MAX_DRB 29
#define ASN1_RRC_NR_MAX_FREQ 8
#define ASN1_RRC_NR_MAX_FREQ_IDC_MRDC 32
#define ASN1_RRC_NR_MAX_NROF_CANDIDATE_BEAMS 16
#define ASN1_RRC_NR_MAX_NROF_PCIS_PER_SMTC 64
#define ASN1_RRC_NR_MAX_NROF_QFIS 64
#define ASN1_RRC_NR_MAX_NR_OF_SEMI_PERSISTENT_PUSCH_TRIGGERS 64
#define ASN1_RRC_NR_MAX_NROF_SR_RES 8
#define ASN1_RRC_NR_MAX_NROF_SLOT_FORMATS_PER_COMBINATION 256
#define ASN1_RRC_NR_MAX_NROF_SPATIAL_RELATION_INFOS 8
#define ASN1_RRC_NR_MAX_NROF_IDXES_TO_REPORT 32
#define ASN1_RRC_NR_MAX_NROF_IDXES_TO_REPORT2 64
#define ASN1_RRC_NR_MAX_NROF_SSBS_MINUS1 63
#define ASN1_RRC_NR_MAX_NROF_S_NSSAI 8
#define ASN1_RRC_NR_MAX_NROF_TCI_STATES_PDCCH 64
#define ASN1_RRC_NR_MAX_NROF_TCI_STATES 128
#define ASN1_RRC_NR_MAX_NROF_TCI_STATES_MINUS1 127
#define ASN1_RRC_NR_MAX_NROF_UL_ALLOCS 16
#define ASN1_RRC_NR_MAX_QFI 63
#define ASN1_RRC_NR_MAX_RA_CSIRS_RES 96
#define ASN1_RRC_NR_MAX_RA_OCCASIONS_PER_CSIRS 64
#define ASN1_RRC_NR_MAX_RA_OCCASIONS_MINUS1 511
#define ASN1_RRC_NR_MAX_RA_SSB_RES 64
#define ASN1_RRC_NR_MAX_SCSS 5
#define ASN1_RRC_NR_MAX_SECONDARY_CELL_GROUPS 3
#define ASN1_RRC_NR_MAX_NROF_SERVING_CELLS_EUTRA 32
#define ASN1_RRC_NR_MAX_MBSFN_ALLOCS 8
#define ASN1_RRC_NR_MAX_NROF_MULTI_BANDS 8
#define ASN1_RRC_NR_MAX_CELL_SFTD 3
#define ASN1_RRC_NR_MAX_REPORT_CFG_ID 64
#define ASN1_RRC_NR_MAX_NROF_CODEBOOKS 16
#define ASN1_RRC_NR_MAX_NROF_CSI_RS_RES 7
#define ASN1_RRC_NR_MAX_NROF_SRI_PUSCH_MAPS 16
#define ASN1_RRC_NR_MAX_NROF_SRI_PUSCH_MAPS_MINUS1 15
#define ASN1_RRC_NR_MAX_SIB 32
#define ASN1_RRC_NR_MAX_SI_MSG 32
#define ASN1_RRC_NR_MAX_PO_PER_PF 4
#define ASN1_RRC_NR_MAX_ACCESS_CAT_MINUS1 63
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
#define ASN1_RRC_NR_MAX_MEAS_FREQS_MN 32
#define ASN1_RRC_NR_MAX_MEAS_FREQS_SN 32
#define ASN1_RRC_NR_MAX_MEAS_IDENTITIES_MN 62
#define ASN1_RRC_NR_MAX_CELL_PREP 32

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// SubcarrierSpacing ::= ENUMERATED
struct subcarrier_spacing_opts {
  enum options { khz15, khz30, khz60, khz120, khz240, spare3, spare2, spare1, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<subcarrier_spacing_opts> subcarrier_spacing_e;

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
  typedef enumerated<types_opts> types;

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
    typedef enumerated<types_opts> types;

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

// SRS-Resource ::= SEQUENCE
struct srs_res_s {
  struct nrof_srs_ports_opts {
    enum options { port1, ports2, ports4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nrof_srs_ports_opts> nrof_srs_ports_e_;
  struct ptrs_port_idx_opts {
    enum options { n0, n1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ptrs_port_idx_opts> ptrs_port_idx_e_;
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
    typedef enumerated<types_opts> types;

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
    typedef enumerated<nrof_symbols_opts> nrof_symbols_e_;
    struct repeat_factor_opts {
      enum options { n1, n2, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<repeat_factor_opts> repeat_factor_e_;

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
  typedef enumerated<group_or_seq_hop_opts> group_or_seq_hop_e_;
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
    typedef enumerated<types_opts> types;

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

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1

#endif // SRSASN1_RRC_NR_COMMON_H
