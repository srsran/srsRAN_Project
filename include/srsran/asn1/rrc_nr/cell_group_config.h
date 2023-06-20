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

#include "dl_dcch_msg.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// TA-Info-r17 ::= SEQUENCE
struct ta_info_r17_s {
  bool     ta_common_drift_r17_present         = false;
  bool     ta_common_drift_variant_r17_present = false;
  uint32_t ta_common_r17                       = 0;
  int32_t  ta_common_drift_r17                 = -257303;
  uint16_t ta_common_drift_variant_r17         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NTN-Config-r17 ::= SEQUENCE
struct ntn_cfg_r17_s {
  struct ntn_ul_sync_validity_dur_r17_opts {
    enum options { s5, s10, s15, s20, s25, s30, s35, s40, s45, s50, s55, s60, s120, s180, s240, s900, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using ntn_ul_sync_validity_dur_r17_e_ = enumerated<ntn_ul_sync_validity_dur_r17_opts>;
  struct ntn_polarization_dl_r17_opts {
    enum options { rhcp, lhcp, linear, nulltype } value;

    const char* to_string() const;
  };
  using ntn_polarization_dl_r17_e_ = enumerated<ntn_polarization_dl_r17_opts>;
  struct ntn_polarization_ul_r17_opts {
    enum options { rhcp, lhcp, linear, nulltype } value;

    const char* to_string() const;
  };
  using ntn_polarization_ul_r17_e_ = enumerated<ntn_polarization_ul_r17_opts>;

  // member variables
  bool                            ext                                  = false;
  bool                            epoch_time_r17_present               = false;
  bool                            ntn_ul_sync_validity_dur_r17_present = false;
  bool                            cell_specific_koffset_r17_present    = false;
  bool                            kmac_r17_present                     = false;
  bool                            ta_info_r17_present                  = false;
  bool                            ntn_polarization_dl_r17_present      = false;
  bool                            ntn_polarization_ul_r17_present      = false;
  bool                            ephemeris_info_r17_present           = false;
  bool                            ta_report_r17_present                = false;
  epoch_time_r17_s                epoch_time_r17;
  ntn_ul_sync_validity_dur_r17_e_ ntn_ul_sync_validity_dur_r17;
  uint16_t                        cell_specific_koffset_r17 = 1;
  uint16_t                        kmac_r17                  = 1;
  ta_info_r17_s                   ta_info_r17;
  ntn_polarization_dl_r17_e_      ntn_polarization_dl_r17;
  ntn_polarization_ul_r17_e_      ntn_polarization_ul_r17;
  ephemeris_info_r17_c            ephemeris_info_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// T-Reassembly ::= ENUMERATED
struct t_reassembly_opts {
  enum options {
    ms0,
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms110,
    ms120,
    ms130,
    ms140,
    ms150,
    ms160,
    ms170,
    ms180,
    ms190,
    ms200,
    spare1,
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using t_reassembly_e = enumerated<t_reassembly_opts>;

// DRX-ConfigPTM-r17 ::= SEQUENCE
struct drx_cfg_ptm_r17_s {
  struct drx_on_dur_timer_ptm_r17_c_ {
    struct milli_seconds_opts {
      enum options {
        ms1,
        ms2,
        ms3,
        ms4,
        ms5,
        ms6,
        ms8,
        ms10,
        ms20,
        ms30,
        ms40,
        ms50,
        ms60,
        ms80,
        ms100,
        ms200,
        ms300,
        ms400,
        ms500,
        ms600,
        ms800,
        ms1000,
        ms1200,
        ms1600,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using milli_seconds_e_ = enumerated<milli_seconds_opts>;
    struct types_opts {
      enum options { sub_milli_seconds, milli_seconds, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    drx_on_dur_timer_ptm_r17_c_() = default;
    drx_on_dur_timer_ptm_r17_c_(const drx_on_dur_timer_ptm_r17_c_& other);
    drx_on_dur_timer_ptm_r17_c_& operator=(const drx_on_dur_timer_ptm_r17_c_& other);
    ~drx_on_dur_timer_ptm_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sub_milli_seconds()
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimerPTM-r17");
      return c.get<uint8_t>();
    }
    milli_seconds_e_& milli_seconds()
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimerPTM-r17");
      return c.get<milli_seconds_e_>();
    }
    const uint8_t& sub_milli_seconds() const
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimerPTM-r17");
      return c.get<uint8_t>();
    }
    const milli_seconds_e_& milli_seconds() const
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimerPTM-r17");
      return c.get<milli_seconds_e_>();
    }
    uint8_t&          set_sub_milli_seconds();
    milli_seconds_e_& set_milli_seconds();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct drx_inactivity_timer_ptm_r17_opts {
    enum options {
      ms0,
      ms1,
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms8,
      ms10,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms200,
      ms300,
      ms500,
      ms750,
      ms1280,
      ms1920,
      ms2560,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using drx_inactivity_timer_ptm_r17_e_ = enumerated<drx_inactivity_timer_ptm_r17_opts>;
  struct drx_retx_timer_dl_ptm_r17_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using drx_retx_timer_dl_ptm_r17_e_ = enumerated<drx_retx_timer_dl_ptm_r17_opts>;
  struct drx_long_cycle_start_offset_ptm_r17_c_ {
    struct types_opts {
      enum options {
        ms10,
        ms20,
        ms32,
        ms40,
        ms60,
        ms64,
        ms70,
        ms80,
        ms128,
        ms160,
        ms256,
        ms320,
        ms512,
        ms640,
        ms1024,
        ms1280,
        ms2048,
        ms2560,
        ms5120,
        ms10240,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    drx_long_cycle_start_offset_ptm_r17_c_() = default;
    drx_long_cycle_start_offset_ptm_r17_c_(const drx_long_cycle_start_offset_ptm_r17_c_& other);
    drx_long_cycle_start_offset_ptm_r17_c_& operator=(const drx_long_cycle_start_offset_ptm_r17_c_& other);
    ~drx_long_cycle_start_offset_ptm_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms10()
    {
      assert_choice_type(types::ms10, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type(types::ms20, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms32()
    {
      assert_choice_type(types::ms32, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms40()
    {
      assert_choice_type(types::ms40, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms60()
    {
      assert_choice_type(types::ms60, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms64()
    {
      assert_choice_type(types::ms64, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms70()
    {
      assert_choice_type(types::ms70, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms80()
    {
      assert_choice_type(types::ms80, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms128()
    {
      assert_choice_type(types::ms128, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint8_t& ms160()
    {
      assert_choice_type(types::ms160, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    uint16_t& ms256()
    {
      assert_choice_type(types::ms256, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms320()
    {
      assert_choice_type(types::ms320, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms512()
    {
      assert_choice_type(types::ms512, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms640()
    {
      assert_choice_type(types::ms640, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms1024()
    {
      assert_choice_type(types::ms1024, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms1280()
    {
      assert_choice_type(types::ms1280, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms2048()
    {
      assert_choice_type(types::ms2048, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms2560()
    {
      assert_choice_type(types::ms2560, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms5120()
    {
      assert_choice_type(types::ms5120, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms10240()
    {
      assert_choice_type(types::ms10240, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type(types::ms10, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type(types::ms20, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms32() const
    {
      assert_choice_type(types::ms32, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms40() const
    {
      assert_choice_type(types::ms40, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms60() const
    {
      assert_choice_type(types::ms60, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms64() const
    {
      assert_choice_type(types::ms64, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms70() const
    {
      assert_choice_type(types::ms70, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms80() const
    {
      assert_choice_type(types::ms80, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms128() const
    {
      assert_choice_type(types::ms128, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint8_t& ms160() const
    {
      assert_choice_type(types::ms160, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint8_t>();
    }
    const uint16_t& ms256() const
    {
      assert_choice_type(types::ms256, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms320() const
    {
      assert_choice_type(types::ms320, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms512() const
    {
      assert_choice_type(types::ms512, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms640() const
    {
      assert_choice_type(types::ms640, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1024() const
    {
      assert_choice_type(types::ms1024, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1280() const
    {
      assert_choice_type(types::ms1280, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2048() const
    {
      assert_choice_type(types::ms2048, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2560() const
    {
      assert_choice_type(types::ms2560, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5120() const
    {
      assert_choice_type(types::ms5120, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms10240() const
    {
      assert_choice_type(types::ms10240, type_, "drx-LongCycleStartOffsetPTM-r17");
      return c.get<uint16_t>();
    }
    uint8_t&  set_ms10();
    uint8_t&  set_ms20();
    uint8_t&  set_ms32();
    uint8_t&  set_ms40();
    uint8_t&  set_ms60();
    uint8_t&  set_ms64();
    uint8_t&  set_ms70();
    uint8_t&  set_ms80();
    uint8_t&  set_ms128();
    uint8_t&  set_ms160();
    uint16_t& set_ms256();
    uint16_t& set_ms320();
    uint16_t& set_ms512();
    uint16_t& set_ms640();
    uint16_t& set_ms1024();
    uint16_t& set_ms1280();
    uint16_t& set_ms2048();
    uint16_t& set_ms2560();
    uint16_t& set_ms5120();
    uint16_t& set_ms10240();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                                   drx_harq_rtt_timer_dl_ptm_r17_present = false;
  bool                                   drx_retx_timer_dl_ptm_r17_present     = false;
  drx_on_dur_timer_ptm_r17_c_            drx_on_dur_timer_ptm_r17;
  drx_inactivity_timer_ptm_r17_e_        drx_inactivity_timer_ptm_r17;
  uint8_t                                drx_harq_rtt_timer_dl_ptm_r17 = 0;
  drx_retx_timer_dl_ptm_r17_e_           drx_retx_timer_dl_ptm_r17;
  drx_long_cycle_start_offset_ptm_r17_c_ drx_long_cycle_start_offset_ptm_r17;
  uint8_t                                drx_slot_offset_ptm_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BH-LogicalChannelIdentity-r16 ::= CHOICE
struct bh_lc_ch_id_r16_c {
  struct types_opts {
    enum options { bh_lc_ch_id_r16, bh_lc_ch_id_ext_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bh_lc_ch_id_r16_c() = default;
  bh_lc_ch_id_r16_c(const bh_lc_ch_id_r16_c& other);
  bh_lc_ch_id_r16_c& operator=(const bh_lc_ch_id_r16_c& other);
  ~bh_lc_ch_id_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& bh_lc_ch_id_r16()
  {
    assert_choice_type(types::bh_lc_ch_id_r16, type_, "BH-LogicalChannelIdentity-r16");
    return c.get<uint8_t>();
  }
  uint32_t& bh_lc_ch_id_ext_r16()
  {
    assert_choice_type(types::bh_lc_ch_id_ext_r16, type_, "BH-LogicalChannelIdentity-r16");
    return c.get<uint32_t>();
  }
  const uint8_t& bh_lc_ch_id_r16() const
  {
    assert_choice_type(types::bh_lc_ch_id_r16, type_, "BH-LogicalChannelIdentity-r16");
    return c.get<uint8_t>();
  }
  const uint32_t& bh_lc_ch_id_ext_r16() const
  {
    assert_choice_type(types::bh_lc_ch_id_ext_r16, type_, "BH-LogicalChannelIdentity-r16");
    return c.get<uint32_t>();
  }
  uint8_t&  set_bh_lc_ch_id_r16();
  uint32_t& set_bh_lc_ch_id_ext_r16();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// T-StatusProhibit ::= ENUMERATED
struct t_status_prohibit_opts {
  enum options {
    ms0,
    ms5,
    ms10,
    ms15,
    ms20,
    ms25,
    ms30,
    ms35,
    ms40,
    ms45,
    ms50,
    ms55,
    ms60,
    ms65,
    ms70,
    ms75,
    ms80,
    ms85,
    ms90,
    ms95,
    ms100,
    ms105,
    ms110,
    ms115,
    ms120,
    ms125,
    ms130,
    ms135,
    ms140,
    ms145,
    ms150,
    ms155,
    ms160,
    ms165,
    ms170,
    ms175,
    ms180,
    ms185,
    ms190,
    ms195,
    ms200,
    ms205,
    ms210,
    ms215,
    ms220,
    ms225,
    ms230,
    ms235,
    ms240,
    ms245,
    ms250,
    ms300,
    ms350,
    ms400,
    ms450,
    ms500,
    ms800,
    ms1000,
    ms1200,
    ms1600,
    ms2000,
    ms2400,
    spare2,
    spare1,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using t_status_prohibit_e = enumerated<t_status_prohibit_opts>;

// DL-AM-RLC ::= SEQUENCE
struct dl_am_rlc_s {
  bool                sn_field_len_present = false;
  sn_field_len_am_e   sn_field_len;
  t_reassembly_e      t_reassembly;
  t_status_prohibit_e t_status_prohibit;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-UM-RLC ::= SEQUENCE
struct dl_um_rlc_s {
  bool              sn_field_len_present = false;
  sn_field_len_um_e sn_field_len;
  t_reassembly_e    t_reassembly;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-AM-RLC ::= SEQUENCE
struct ul_am_rlc_s {
  struct max_retx_thres_opts {
    enum options { t1, t2, t3, t4, t6, t8, t16, t32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using max_retx_thres_e_ = enumerated<max_retx_thres_opts>;

  // member variables
  bool              sn_field_len_present = false;
  sn_field_len_am_e sn_field_len;
  t_poll_retx_e     t_poll_retx;
  poll_pdu_e        poll_pdu;
  poll_byte_e       poll_byte;
  max_retx_thres_e_ max_retx_thres;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-UM-RLC ::= SEQUENCE
struct ul_um_rlc_s {
  bool              sn_field_len_present = false;
  sn_field_len_um_e sn_field_len;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogicalChannelConfig ::= SEQUENCE
struct lc_ch_cfg_s {
  struct ul_specific_params_s_ {
    struct prioritised_bit_rate_opts {
      enum options {
        kbps0,
        kbps8,
        kbps16,
        kbps32,
        kbps64,
        kbps128,
        kbps256,
        kbps512,
        kbps1024,
        kbps2048,
        kbps4096,
        kbps8192,
        kbps16384,
        kbps32768,
        kbps65536,
        infinity,
        nulltype
      } value;
      typedef int32_t number_type;

      const char* to_string() const;
      int32_t     to_number() const;
    };
    using prioritised_bit_rate_e_ = enumerated<prioritised_bit_rate_opts>;
    struct bucket_size_dur_opts {
      enum options {
        ms5,
        ms10,
        ms20,
        ms50,
        ms100,
        ms150,
        ms300,
        ms500,
        ms1000,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using bucket_size_dur_e_       = enumerated<bucket_size_dur_opts>;
    using allowed_serving_cells_l_ = bounded_array<uint8_t, 31>;
    using allowed_scs_list_l_      = bounded_array<subcarrier_spacing_e, 5>;
    struct max_pusch_dur_opts {
      enum options { ms0p02, ms0p04, ms0p0625, ms0p125, ms0p25, ms0p5, ms0p01_v1700, spare1, nulltype } value;

      const char* to_string() const;
    };
    using max_pusch_dur_e_ = enumerated<max_pusch_dur_opts>;
    struct bit_rate_query_prohibit_timer_opts {
      enum options { s0, s0dot4, s0dot8, s1dot6, s3, s6, s12, s30, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using bit_rate_query_prohibit_timer_e_ = enumerated<bit_rate_query_prohibit_timer_opts>;
    using allowed_cg_list_r16_l_           = bounded_array<uint8_t, 31>;
    struct allowed_phy_prio_idx_r16_opts {
      enum options { p0, p1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using allowed_phy_prio_idx_r16_e_ = enumerated<allowed_phy_prio_idx_r16_opts>;
    struct allowed_harq_mode_r17_opts {
      enum options { harq_mode_a, harq_mode_b, nulltype } value;

      const char* to_string() const;
    };
    using allowed_harq_mode_r17_e_ = enumerated<allowed_harq_mode_r17_opts>;

    // member variables
    bool                     ext                             = false;
    bool                     max_pusch_dur_present           = false;
    bool                     cfg_grant_type1_allowed_present = false;
    bool                     lc_ch_group_present             = false;
    bool                     sched_request_id_present        = false;
    uint8_t                  prio                            = 1;
    prioritised_bit_rate_e_  prioritised_bit_rate;
    bucket_size_dur_e_       bucket_size_dur;
    allowed_serving_cells_l_ allowed_serving_cells;
    allowed_scs_list_l_      allowed_scs_list;
    max_pusch_dur_e_         max_pusch_dur;
    uint8_t                  lc_ch_group                  = 0;
    uint8_t                  sched_request_id             = 0;
    bool                     lc_ch_sr_mask                = false;
    bool                     lc_ch_sr_delay_timer_applied = false;
    // ...
    bool                             bit_rate_query_prohibit_timer_present = false;
    bit_rate_query_prohibit_timer_e_ bit_rate_query_prohibit_timer;
    // group 0
    bool                             allowed_phy_prio_idx_r16_present = false;
    copy_ptr<allowed_cg_list_r16_l_> allowed_cg_list_r16;
    allowed_phy_prio_idx_r16_e_      allowed_phy_prio_idx_r16;
    // group 1
    bool                     lc_ch_group_iab_ext_r17_present = false;
    bool                     allowed_harq_mode_r17_present   = false;
    uint16_t                 lc_ch_group_iab_ext_r17         = 0;
    allowed_harq_mode_r17_e_ allowed_harq_mode_r17;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  struct bit_rate_multiplier_r16_opts {
    enum options { x40, x70, x100, x200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using bit_rate_multiplier_r16_e_ = enumerated<bit_rate_multiplier_r16_opts>;

  // member variables
  bool                  ext                        = false;
  bool                  ul_specific_params_present = false;
  ul_specific_params_s_ ul_specific_params;
  // ...
  // group 0
  bool                       ch_access_prio_r16_present      = false;
  bool                       bit_rate_multiplier_r16_present = false;
  uint8_t                    ch_access_prio_r16              = 1;
  bit_rate_multiplier_r16_e_ bit_rate_multiplier_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Config ::= CHOICE
struct rlc_cfg_c {
  struct am_s_ {
    ul_am_rlc_s ul_am_rlc;
    dl_am_rlc_s dl_am_rlc;
  };
  struct um_bi_dir_s_ {
    ul_um_rlc_s ul_um_rlc;
    dl_um_rlc_s dl_um_rlc;
  };
  struct um_uni_dir_ul_s_ {
    ul_um_rlc_s ul_um_rlc;
  };
  struct um_uni_dir_dl_s_ {
    dl_um_rlc_s dl_um_rlc;
  };
  struct types_opts {
    enum options { am, um_bi_dir, um_uni_dir_ul, um_uni_dir_dl, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  rlc_cfg_c() = default;
  rlc_cfg_c(const rlc_cfg_c& other);
  rlc_cfg_c& operator=(const rlc_cfg_c& other);
  ~rlc_cfg_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  am_s_& am()
  {
    assert_choice_type(types::am, type_, "RLC-Config");
    return c.get<am_s_>();
  }
  um_bi_dir_s_& um_bi_dir()
  {
    assert_choice_type(types::um_bi_dir, type_, "RLC-Config");
    return c.get<um_bi_dir_s_>();
  }
  um_uni_dir_ul_s_& um_uni_dir_ul()
  {
    assert_choice_type(types::um_uni_dir_ul, type_, "RLC-Config");
    return c.get<um_uni_dir_ul_s_>();
  }
  um_uni_dir_dl_s_& um_uni_dir_dl()
  {
    assert_choice_type(types::um_uni_dir_dl, type_, "RLC-Config");
    return c.get<um_uni_dir_dl_s_>();
  }
  const am_s_& am() const
  {
    assert_choice_type(types::am, type_, "RLC-Config");
    return c.get<am_s_>();
  }
  const um_bi_dir_s_& um_bi_dir() const
  {
    assert_choice_type(types::um_bi_dir, type_, "RLC-Config");
    return c.get<um_bi_dir_s_>();
  }
  const um_uni_dir_ul_s_& um_uni_dir_ul() const
  {
    assert_choice_type(types::um_uni_dir_ul, type_, "RLC-Config");
    return c.get<um_uni_dir_ul_s_>();
  }
  const um_uni_dir_dl_s_& um_uni_dir_dl() const
  {
    assert_choice_type(types::um_uni_dir_dl, type_, "RLC-Config");
    return c.get<um_uni_dir_dl_s_>();
  }
  am_s_&            set_am();
  um_bi_dir_s_&     set_um_bi_dir();
  um_uni_dir_ul_s_& set_um_uni_dir_ul();
  um_uni_dir_dl_s_& set_um_uni_dir_dl();

private:
  types                                                                    type_;
  choice_buffer_t<am_s_, um_bi_dir_s_, um_uni_dir_dl_s_, um_uni_dir_ul_s_> c;

  void destroy_();
};

// BH-RLC-ChannelConfig-r16 ::= SEQUENCE
struct bh_rlc_ch_cfg_r16_s {
  bool                ext                         = false;
  bool                bh_lc_ch_id_r16_present     = false;
  bool                reestablish_rlc_r16_present = false;
  bool                rlc_cfg_r16_present         = false;
  bool                mac_lc_ch_cfg_r16_present   = false;
  bh_lc_ch_id_r16_c   bh_lc_ch_id_r16;
  fixed_bitstring<16> bh_rlc_ch_id_r16;
  rlc_cfg_c           rlc_cfg_r16;
  lc_ch_cfg_s         mac_lc_ch_cfg_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierState-r17 ::= CHOICE
struct carrier_state_r17_c {
  struct types_opts {
    enum options { de_activ_r17, active_bwp_r17, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  carrier_state_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& active_bwp_r17()
  {
    assert_choice_type(types::active_bwp_r17, type_, "CarrierState-r17");
    return c;
  }
  const uint8_t& active_bwp_r17() const
  {
    assert_choice_type(types::active_bwp_r17, type_, "CarrierState-r17");
    return c;
  }
  void     set_de_activ_r17();
  uint8_t& set_active_bwp_r17();

private:
  types   type_;
  uint8_t c;
};

// CC-State-r17 ::= SEQUENCE
struct cc_state_r17_s {
  bool                dl_carrier_r17_present = false;
  bool                ul_carrier_r17_present = false;
  carrier_state_r17_c dl_carrier_r17;
  carrier_state_r17_c ul_carrier_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA-CSIRS-Resource ::= SEQUENCE
struct cfra_csi_rs_res_s {
  using ra_occasion_list_l_ = dyn_array<uint16_t>;

  // member variables
  bool                ext    = false;
  uint8_t             csi_rs = 0;
  ra_occasion_list_l_ ra_occasion_list;
  uint8_t             ra_preamb_idx = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA-SSB-Resource ::= SEQUENCE
struct cfra_ssb_res_s {
  bool    ext           = false;
  uint8_t ssb           = 0;
  uint8_t ra_preamb_idx = 0;
  // ...
  // group 0
  bool     msg_a_pusch_res_idx_r16_present = false;
  uint16_t msg_a_pusch_res_idx_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA ::= SEQUENCE
struct cfra_s {
  struct occasions_s_ {
    struct ssb_per_rach_occasion_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using ssb_per_rach_occasion_e_ = enumerated<ssb_per_rach_occasion_opts>;

    // member variables
    bool                     ssb_per_rach_occasion_present = false;
    rach_cfg_generic_s       rach_cfg_generic;
    ssb_per_rach_occasion_e_ ssb_per_rach_occasion;
  };
  struct res_c_ {
    struct ssb_s_ {
      using ssb_res_list_l_ = dyn_array<cfra_ssb_res_s>;

      // member variables
      ssb_res_list_l_ ssb_res_list;
      uint8_t         ra_ssb_occasion_mask_idx = 0;
    };
    struct csirs_s_ {
      using csirs_res_list_l_ = dyn_array<cfra_csi_rs_res_s>;

      // member variables
      csirs_res_list_l_ csirs_res_list;
      uint8_t           rsrp_thres_csi_rs = 0;
    };
    struct types_opts {
      enum options { ssb, csirs, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    res_c_() = default;
    res_c_(const res_c_& other);
    res_c_& operator=(const res_c_& other);
    ~res_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ssb_s_& ssb()
    {
      assert_choice_type(types::ssb, type_, "resources");
      return c.get<ssb_s_>();
    }
    csirs_s_& csirs()
    {
      assert_choice_type(types::csirs, type_, "resources");
      return c.get<csirs_s_>();
    }
    const ssb_s_& ssb() const
    {
      assert_choice_type(types::ssb, type_, "resources");
      return c.get<ssb_s_>();
    }
    const csirs_s_& csirs() const
    {
      assert_choice_type(types::csirs, type_, "resources");
      return c.get<csirs_s_>();
    }
    ssb_s_&   set_ssb();
    csirs_s_& set_csirs();

  private:
    types                             type_;
    choice_buffer_t<csirs_s_, ssb_s_> c;

    void destroy_();
  };

  // member variables
  bool         ext               = false;
  bool         occasions_present = false;
  occasions_s_ occasions;
  res_c_       res;
  // ...
  // group 0
  bool    total_nof_ra_preambs_present = false;
  uint8_t total_nof_ra_preambs         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CFRA-TwoStep-r16 ::= SEQUENCE
struct cfra_two_step_r16_s {
  struct occasions_two_step_ra_r16_s_ {
    struct ssb_per_rach_occasion_two_step_ra_r16_opts {
      enum options { one_eighth, one_fourth, one_half, one, two, four, eight, sixteen, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    using ssb_per_rach_occasion_two_step_ra_r16_e_ = enumerated<ssb_per_rach_occasion_two_step_ra_r16_opts>;

    // member variables
    rach_cfg_generic_two_step_ra_r16_s       rach_cfg_generic_two_step_ra_r16;
    ssb_per_rach_occasion_two_step_ra_r16_e_ ssb_per_rach_occasion_two_step_ra_r16;
  };
  struct msg_a_trans_max_r16_opts {
    enum options { n1, n2, n4, n6, n8, n10, n20, n50, n100, n200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using msg_a_trans_max_r16_e_ = enumerated<msg_a_trans_max_r16_opts>;
  struct res_two_step_r16_s_ {
    using ssb_res_list_l_ = dyn_array<cfra_ssb_res_s>;

    // member variables
    ssb_res_list_l_ ssb_res_list;
    uint8_t         ra_ssb_occasion_mask_idx = 0;
  };

  // member variables
  bool                         ext                               = false;
  bool                         occasions_two_step_ra_r16_present = false;
  bool                         msg_a_trans_max_r16_present       = false;
  occasions_two_step_ra_r16_s_ occasions_two_step_ra_r16;
  msg_a_pusch_res_r16_s        msg_a_cfra_pusch_r16;
  msg_a_trans_max_r16_e_       msg_a_trans_max_r16;
  res_two_step_r16_s_          res_two_step_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyInfoDL ::= SEQUENCE
struct freq_info_dl_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                         ext                       = false;
  bool                         absolute_freq_ssb_present = false;
  uint32_t                     absolute_freq_ssb         = 0;
  multi_freq_band_list_nr_l    freq_band_list;
  uint32_t                     absolute_freq_point_a = 0;
  scs_specific_carrier_list_l_ scs_specific_carrier_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FrequencyInfoUL ::= SEQUENCE
struct freq_info_ul_s {
  using scs_specific_carrier_list_l_ = dyn_array<scs_specific_carrier_s>;

  // member variables
  bool                         ext                           = false;
  bool                         absolute_freq_point_a_present = false;
  bool                         add_spec_emission_present     = false;
  bool                         p_max_present                 = false;
  bool                         freq_shift7p5khz_present      = false;
  multi_freq_band_list_nr_l    freq_band_list;
  uint32_t                     absolute_freq_point_a = 0;
  scs_specific_carrier_list_l_ scs_specific_carrier_list;
  uint8_t                      add_spec_emission = 0;
  int8_t                       p_max             = -30;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DownlinkConfigCommon ::= SEQUENCE
struct dl_cfg_common_s {
  bool            ext                  = false;
  bool            freq_info_dl_present = false;
  bool            init_dl_bwp_present  = false;
  freq_info_dl_s  freq_info_dl;
  bwp_dl_common_s init_dl_bwp;
  // ...
  // group 0
  copy_ptr<bwp_dl_common_s> init_dl_bwp_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedConfig-v1700 ::= SEQUENCE
struct high_speed_cfg_v1700_s {
  bool ext                                    = false;
  bool high_speed_meas_ca_scell_r17_present   = false;
  bool high_speed_meas_inter_freq_r17_present = false;
  bool high_speed_demod_ca_scell_r17_present  = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// T-ReassemblyExt-r17 ::= ENUMERATED
struct t_reassembly_ext_r17_opts {
  enum options { ms210, ms220, ms340, ms350, ms550, ms1100, ms1650, ms2200, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using t_reassembly_ext_r17_e = enumerated<t_reassembly_ext_r17_opts>;

// T-StatusProhibit-v1610 ::= ENUMERATED
struct t_status_prohibit_v1610_opts {
  enum options { ms1, ms2, ms3, ms4, spare4, spare3, spare2, spare1, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using t_status_prohibit_v1610_e = enumerated<t_status_prohibit_v1610_opts>;

// UplinkConfigCommon ::= SEQUENCE
struct ul_cfg_common_s {
  bool               freq_info_ul_present = false;
  bool               init_ul_bwp_present  = false;
  freq_info_ul_s     freq_info_ul;
  bwp_ul_common_s    init_ul_bwp;
  time_align_timer_e dummy;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkConfigCommon-v1700 ::= SEQUENCE
struct ul_cfg_common_v1700_s {
  bool            init_ul_bwp_red_cap_r17_present = false;
  bwp_ul_common_s init_ul_bwp_red_cap_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DAPS-UplinkPowerConfig-r16 ::= SEQUENCE
struct daps_ul_pwr_cfg_r16_s {
  struct ul_pwr_sharing_daps_mode_r16_opts {
    enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ul_pwr_sharing_daps_mode_r16_e_ = enumerated<ul_pwr_sharing_daps_mode_r16_opts>;

  // member variables
  int8_t                          p_daps_source_r16 = -30;
  int8_t                          p_daps_target_r16 = -30;
  ul_pwr_sharing_daps_mode_r16_e_ ul_pwr_sharing_daps_mode_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-AM-RLC-v1610 ::= SEQUENCE
struct dl_am_rlc_v1610_s {
  bool                      ext                             = false;
  bool                      t_status_prohibit_v1610_present = false;
  t_status_prohibit_v1610_e t_status_prohibit_v1610;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-AM-RLC-v1700 ::= SEQUENCE
struct dl_am_rlc_v1700_s {
  bool                   t_reassembly_ext_r17_present = false;
  t_reassembly_ext_r17_e t_reassembly_ext_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-UM-RLC-v1700 ::= SEQUENCE
struct dl_um_rlc_v1700_s {
  bool                   t_reassembly_ext_r17_present = false;
  t_reassembly_ext_r17_e t_reassembly_ext_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraBandCC-Combination-r17 ::= SEQUENCE (SIZE (1..32)) OF CC-State-r17
using intra_band_cc_combination_r17_l = dyn_array<cc_state_r17_s>;

// MPE-Config-FR2-r16 ::= SEQUENCE
struct mpe_cfg_fr2_r16_s {
  struct mpe_prohibit_timer_r16_opts {
    enum options { sf0, sf10, sf20, sf50, sf100, sf200, sf500, sf1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mpe_prohibit_timer_r16_e_ = enumerated<mpe_prohibit_timer_r16_opts>;
  struct mpe_thres_r16_opts {
    enum options { db3, db6, db9, db12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mpe_thres_r16_e_ = enumerated<mpe_thres_r16_opts>;

  // member variables
  mpe_prohibit_timer_r16_e_ mpe_prohibit_timer_r16;
  mpe_thres_r16_e_          mpe_thres_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MPE-Config-FR2-r17 ::= SEQUENCE
struct mpe_cfg_fr2_r17_s {
  struct mpe_prohibit_timer_r17_opts {
    enum options { sf0, sf10, sf20, sf50, sf100, sf200, sf500, sf1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using mpe_prohibit_timer_r17_e_ = enumerated<mpe_prohibit_timer_r17_opts>;
  struct mpe_thres_r17_opts {
    enum options { db3, db6, db9, db12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mpe_thres_r17_e_ = enumerated<mpe_thres_r17_opts>;

  // member variables
  bool                      ext = false;
  mpe_prohibit_timer_r17_e_ mpe_prohibit_timer_r17;
  mpe_thres_r17_e_          mpe_thres_r17;
  uint8_t                   nof_n_r17 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct pdsch_harq_ack_codebook_list_r16_item_opts {
  enum options { semi_static, dyn, nulltype } value;

  const char* to_string() const;
};
using pdsch_harq_ack_codebook_list_r16_item_e_ = enumerated<pdsch_harq_ack_codebook_list_r16_item_opts>;

// PDSCH-HARQ-ACK-CodebookList-r16 ::= SEQUENCE (SIZE (1..2)) OF PDSCH-HARQ-ACK-CodebookList-r16-item
using pdsch_harq_ack_codebook_list_r16_l = bounded_array<pdsch_harq_ack_codebook_list_r16_item_e_, 2>;

// RACH-ConfigDedicated ::= SEQUENCE
struct rach_cfg_ded_s {
  bool                ext                       = false;
  bool                cfra_present              = false;
  bool                ra_prioritization_present = false;
  cfra_s              cfra;
  ra_prioritization_s ra_prioritization;
  // ...
  // group 0
  copy_ptr<ra_prioritization_s> ra_prioritization_two_step_r16;
  copy_ptr<cfra_two_step_r16_s> cfra_two_step_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PathSwitchConfig-r17 ::= SEQUENCE
struct sl_path_switch_cfg_r17_s {
  struct t420_r17_opts {
    enum options { ms50, ms100, ms150, ms200, ms500, ms1000, ms2000, ms10000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t420_r17_e_ = enumerated<t420_r17_opts>;

  // member variables
  bool                ext = false;
  fixed_bitstring<24> target_relay_ue_id_r17;
  t420_r17_e_         t420_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestToAddMod ::= SEQUENCE
struct sched_request_to_add_mod_s {
  struct sr_prohibit_timer_opts {
    enum options { ms1, ms2, ms4, ms8, ms16, ms32, ms64, ms128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sr_prohibit_timer_e_ = enumerated<sr_prohibit_timer_opts>;
  struct sr_trans_max_opts {
    enum options { n4, n8, n16, n32, n64, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using sr_trans_max_e_ = enumerated<sr_trans_max_opts>;

  // member variables
  bool                 sr_prohibit_timer_present = false;
  uint8_t              sched_request_id          = 0;
  sr_prohibit_timer_e_ sr_prohibit_timer;
  sr_trans_max_e_      sr_trans_max;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestToAddModExt-v1700 ::= SEQUENCE
struct sched_request_to_add_mod_ext_v1700_s {
  struct sr_prohibit_timer_v1700_opts {
    enum options {
      ms192,
      ms256,
      ms320,
      ms384,
      ms448,
      ms512,
      ms576,
      ms640,
      ms1082,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using sr_prohibit_timer_v1700_e_ = enumerated<sr_prohibit_timer_v1700_opts>;

  // member variables
  bool                       sr_prohibit_timer_v1700_present = false;
  sr_prohibit_timer_v1700_e_ sr_prohibit_timer_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCellConfigCommon ::= SEQUENCE
struct serving_cell_cfg_common_s {
  struct n_timing_advance_offset_opts {
    enum options { n0, n25600, n39936, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using n_timing_advance_offset_e_ = enumerated<n_timing_advance_offset_opts>;
  struct ssb_positions_in_burst_c_ {
    struct types_opts {
      enum options { short_bitmap, medium_bitmap, long_bitmap, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ssb_positions_in_burst_c_() = default;
    ssb_positions_in_burst_c_(const ssb_positions_in_burst_c_& other);
    ssb_positions_in_burst_c_& operator=(const ssb_positions_in_burst_c_& other);
    ~ssb_positions_in_burst_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& short_bitmap()
    {
      assert_choice_type(types::short_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<8>& medium_bitmap()
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<8>>();
    }
    fixed_bitstring<64>& long_bitmap()
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<64>>();
    }
    const fixed_bitstring<4>& short_bitmap() const
    {
      assert_choice_type(types::short_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<8>& medium_bitmap() const
    {
      assert_choice_type(types::medium_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<8>>();
    }
    const fixed_bitstring<64>& long_bitmap() const
    {
      assert_choice_type(types::long_bitmap, type_, "ssb-PositionsInBurst");
      return c.get<fixed_bitstring<64>>();
    }
    fixed_bitstring<4>&  set_short_bitmap();
    fixed_bitstring<8>&  set_medium_bitmap();
    fixed_bitstring<64>& set_long_bitmap();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<64>> c;

    void destroy_();
  };
  struct ssb_periodicity_serving_cell_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ssb_periodicity_serving_cell_e_ = enumerated<ssb_periodicity_serving_cell_opts>;
  struct dmrs_type_a_position_opts {
    enum options { pos2, pos3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dmrs_type_a_position_e_               = enumerated<dmrs_type_a_position_opts>;
  using rate_match_pattern_to_add_mod_list_l_ = dyn_array<rate_match_pattern_s>;
  using rate_match_pattern_to_release_list_l_ = bounded_array<uint8_t, 4>;
  struct ch_access_mode_r16_c_ {
    struct types_opts {
      enum options { dyn, semi_static, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    ch_access_mode_r16_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    semi_static_ch_access_cfg_r16_s& semi_static()
    {
      assert_choice_type(types::semi_static, type_, "channelAccessMode-r16");
      return c;
    }
    const semi_static_ch_access_cfg_r16_s& semi_static() const
    {
      assert_choice_type(types::semi_static, type_, "channelAccessMode-r16");
      return c;
    }
    void                             set_dyn();
    semi_static_ch_access_cfg_r16_s& set_semi_static();

  private:
    types                           type_;
    semi_static_ch_access_cfg_r16_s c;
  };
  struct discovery_burst_win_len_r16_opts {
    enum options { ms0dot5, ms1, ms2, ms3, ms4, ms5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using discovery_burst_win_len_r16_e_ = enumerated<discovery_burst_win_len_r16_opts>;
  struct discovery_burst_win_len_r17_opts {
    enum options { ms0dot125, ms0dot25, ms0dot5, ms0dot75, ms1, ms1dot25, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using discovery_burst_win_len_r17_e_ = enumerated<discovery_burst_win_len_r17_opts>;
  struct feature_priorities_r17_s_ {
    bool    red_cap_prio_r17_present      = false;
    bool    slicing_prio_r17_present      = false;
    bool    msg3_repeats_prio_r17_present = false;
    bool    sdt_prio_r17_present          = false;
    uint8_t red_cap_prio_r17              = 0;
    uint8_t slicing_prio_r17              = 0;
    uint8_t msg3_repeats_prio_r17         = 0;
    uint8_t sdt_prio_r17                  = 0;
  };

  // member variables
  bool                                          ext                                  = false;
  bool                                          pci_present                          = false;
  bool                                          dl_cfg_common_present                = false;
  bool                                          ul_cfg_common_present                = false;
  bool                                          supplementary_ul_cfg_present         = false;
  bool                                          n_timing_advance_offset_present      = false;
  bool                                          ssb_positions_in_burst_present       = false;
  bool                                          ssb_periodicity_serving_cell_present = false;
  bool                                          lte_crs_to_match_around_present      = false;
  bool                                          ssb_subcarrier_spacing_present       = false;
  bool                                          tdd_ul_dl_cfg_common_present         = false;
  uint16_t                                      pci                                  = 0;
  dl_cfg_common_s                               dl_cfg_common;
  ul_cfg_common_s                               ul_cfg_common;
  ul_cfg_common_s                               supplementary_ul_cfg;
  n_timing_advance_offset_e_                    n_timing_advance_offset;
  ssb_positions_in_burst_c_                     ssb_positions_in_burst;
  ssb_periodicity_serving_cell_e_               ssb_periodicity_serving_cell;
  dmrs_type_a_position_e_                       dmrs_type_a_position;
  setup_release_c<rate_match_pattern_lte_crs_s> lte_crs_to_match_around;
  rate_match_pattern_to_add_mod_list_l_         rate_match_pattern_to_add_mod_list;
  rate_match_pattern_to_release_list_l_         rate_match_pattern_to_release_list;
  subcarrier_spacing_e                          ssb_subcarrier_spacing;
  tdd_ul_dl_cfg_common_s                        tdd_ul_dl_cfg_common;
  int8_t                                        ss_pbch_block_pwr = -60;
  // ...
  // group 0
  bool                            discovery_burst_win_len_r16_present = false;
  bool                            ssb_position_qcl_r16_present        = false;
  copy_ptr<ch_access_mode_r16_c_> ch_access_mode_r16;
  discovery_burst_win_len_r16_e_  discovery_burst_win_len_r16;
  ssb_position_qcl_relation_r16_e ssb_position_qcl_r16;
  copy_ptr<high_speed_cfg_r16_s>  high_speed_cfg_r16;
  // group 1
  bool                               ch_access_mode2_r17_present         = false;
  bool                               discovery_burst_win_len_r17_present = false;
  bool                               ssb_position_qcl_r17_present        = false;
  copy_ptr<high_speed_cfg_v1700_s>   high_speed_cfg_v1700;
  discovery_burst_win_len_r17_e_     discovery_burst_win_len_r17;
  ssb_position_qcl_relation_r17_e    ssb_position_qcl_r17;
  copy_ptr<high_speed_cfg_fr2_r17_s> high_speed_cfg_fr2_r17;
  copy_ptr<ul_cfg_common_v1700_s>    ul_cfg_common_v1700;
  copy_ptr<ntn_cfg_r17_s>            ntn_cfg_r17;
  // group 2
  copy_ptr<feature_priorities_r17_s_> feature_priorities_r17;
  // group 3
  bool ra_ch_access_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAG ::= SEQUENCE
struct tag_s {
  bool               ext    = false;
  uint8_t            tag_id = 0;
  time_align_timer_e time_align_timer;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DCP-Config-r16 ::= SEQUENCE
struct d_cp_cfg_r16_s {
  bool     ps_wake_up_r16_present               = false;
  bool     ps_tx_periodic_l1_rsrp_r16_present   = false;
  bool     ps_tx_other_periodic_csi_r16_present = false;
  uint32_t ps_rnti_r16                          = 0;
  uint8_t  ps_offset_r16                        = 1;
  uint8_t  size_dci_2_6_r16                     = 1;
  uint8_t  ps_position_dci_2_6_r16              = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Config ::= SEQUENCE
struct drx_cfg_s {
  struct drx_on_dur_timer_c_ {
    struct milli_seconds_opts {
      enum options {
        ms1,
        ms2,
        ms3,
        ms4,
        ms5,
        ms6,
        ms8,
        ms10,
        ms20,
        ms30,
        ms40,
        ms50,
        ms60,
        ms80,
        ms100,
        ms200,
        ms300,
        ms400,
        ms500,
        ms600,
        ms800,
        ms1000,
        ms1200,
        ms1600,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using milli_seconds_e_ = enumerated<milli_seconds_opts>;
    struct types_opts {
      enum options { sub_milli_seconds, milli_seconds, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    drx_on_dur_timer_c_() = default;
    drx_on_dur_timer_c_(const drx_on_dur_timer_c_& other);
    drx_on_dur_timer_c_& operator=(const drx_on_dur_timer_c_& other);
    ~drx_on_dur_timer_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sub_milli_seconds()
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimer");
      return c.get<uint8_t>();
    }
    milli_seconds_e_& milli_seconds()
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimer");
      return c.get<milli_seconds_e_>();
    }
    const uint8_t& sub_milli_seconds() const
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimer");
      return c.get<uint8_t>();
    }
    const milli_seconds_e_& milli_seconds() const
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimer");
      return c.get<milli_seconds_e_>();
    }
    uint8_t&          set_sub_milli_seconds();
    milli_seconds_e_& set_milli_seconds();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct drx_inactivity_timer_opts {
    enum options {
      ms0,
      ms1,
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms8,
      ms10,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms200,
      ms300,
      ms500,
      ms750,
      ms1280,
      ms1920,
      ms2560,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using drx_inactivity_timer_e_ = enumerated<drx_inactivity_timer_opts>;
  struct drx_retx_timer_dl_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using drx_retx_timer_dl_e_ = enumerated<drx_retx_timer_dl_opts>;
  struct drx_retx_timer_ul_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using drx_retx_timer_ul_e_ = enumerated<drx_retx_timer_ul_opts>;
  struct drx_long_cycle_start_offset_c_ {
    struct types_opts {
      enum options {
        ms10,
        ms20,
        ms32,
        ms40,
        ms60,
        ms64,
        ms70,
        ms80,
        ms128,
        ms160,
        ms256,
        ms320,
        ms512,
        ms640,
        ms1024,
        ms1280,
        ms2048,
        ms2560,
        ms5120,
        ms10240,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    drx_long_cycle_start_offset_c_() = default;
    drx_long_cycle_start_offset_c_(const drx_long_cycle_start_offset_c_& other);
    drx_long_cycle_start_offset_c_& operator=(const drx_long_cycle_start_offset_c_& other);
    ~drx_long_cycle_start_offset_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms10()
    {
      assert_choice_type(types::ms10, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type(types::ms20, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms32()
    {
      assert_choice_type(types::ms32, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms40()
    {
      assert_choice_type(types::ms40, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms60()
    {
      assert_choice_type(types::ms60, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms64()
    {
      assert_choice_type(types::ms64, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms70()
    {
      assert_choice_type(types::ms70, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms80()
    {
      assert_choice_type(types::ms80, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms128()
    {
      assert_choice_type(types::ms128, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint8_t& ms160()
    {
      assert_choice_type(types::ms160, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    uint16_t& ms256()
    {
      assert_choice_type(types::ms256, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms320()
    {
      assert_choice_type(types::ms320, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms512()
    {
      assert_choice_type(types::ms512, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms640()
    {
      assert_choice_type(types::ms640, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms1024()
    {
      assert_choice_type(types::ms1024, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms1280()
    {
      assert_choice_type(types::ms1280, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms2048()
    {
      assert_choice_type(types::ms2048, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms2560()
    {
      assert_choice_type(types::ms2560, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms5120()
    {
      assert_choice_type(types::ms5120, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint16_t& ms10240()
    {
      assert_choice_type(types::ms10240, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type(types::ms10, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type(types::ms20, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms32() const
    {
      assert_choice_type(types::ms32, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms40() const
    {
      assert_choice_type(types::ms40, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms60() const
    {
      assert_choice_type(types::ms60, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms64() const
    {
      assert_choice_type(types::ms64, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms70() const
    {
      assert_choice_type(types::ms70, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms80() const
    {
      assert_choice_type(types::ms80, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms128() const
    {
      assert_choice_type(types::ms128, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint8_t& ms160() const
    {
      assert_choice_type(types::ms160, type_, "drx-LongCycleStartOffset");
      return c.get<uint8_t>();
    }
    const uint16_t& ms256() const
    {
      assert_choice_type(types::ms256, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms320() const
    {
      assert_choice_type(types::ms320, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms512() const
    {
      assert_choice_type(types::ms512, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms640() const
    {
      assert_choice_type(types::ms640, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1024() const
    {
      assert_choice_type(types::ms1024, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms1280() const
    {
      assert_choice_type(types::ms1280, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2048() const
    {
      assert_choice_type(types::ms2048, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms2560() const
    {
      assert_choice_type(types::ms2560, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5120() const
    {
      assert_choice_type(types::ms5120, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    const uint16_t& ms10240() const
    {
      assert_choice_type(types::ms10240, type_, "drx-LongCycleStartOffset");
      return c.get<uint16_t>();
    }
    uint8_t&  set_ms10();
    uint8_t&  set_ms20();
    uint8_t&  set_ms32();
    uint8_t&  set_ms40();
    uint8_t&  set_ms60();
    uint8_t&  set_ms64();
    uint8_t&  set_ms70();
    uint8_t&  set_ms80();
    uint8_t&  set_ms128();
    uint8_t&  set_ms160();
    uint16_t& set_ms256();
    uint16_t& set_ms320();
    uint16_t& set_ms512();
    uint16_t& set_ms640();
    uint16_t& set_ms1024();
    uint16_t& set_ms1280();
    uint16_t& set_ms2048();
    uint16_t& set_ms2560();
    uint16_t& set_ms5120();
    uint16_t& set_ms10240();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct short_drx_s_ {
    struct drx_short_cycle_opts {
      enum options {
        ms2,
        ms3,
        ms4,
        ms5,
        ms6,
        ms7,
        ms8,
        ms10,
        ms14,
        ms16,
        ms20,
        ms30,
        ms32,
        ms35,
        ms40,
        ms64,
        ms80,
        ms128,
        ms160,
        ms256,
        ms320,
        ms512,
        ms640,
        spare9,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using drx_short_cycle_e_ = enumerated<drx_short_cycle_opts>;

    // member variables
    drx_short_cycle_e_ drx_short_cycle;
    uint8_t            drx_short_cycle_timer = 1;
  };

  // member variables
  bool                           short_drx_present = false;
  drx_on_dur_timer_c_            drx_on_dur_timer;
  drx_inactivity_timer_e_        drx_inactivity_timer;
  uint8_t                        drx_harq_rtt_timer_dl = 0;
  uint8_t                        drx_harq_rtt_timer_ul = 0;
  drx_retx_timer_dl_e_           drx_retx_timer_dl;
  drx_retx_timer_ul_e_           drx_retx_timer_ul;
  drx_long_cycle_start_offset_c_ drx_long_cycle_start_offset;
  short_drx_s_                   short_drx;
  uint8_t                        drx_slot_offset = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-ConfigExt-v1700 ::= SEQUENCE
struct drx_cfg_ext_v1700_s {
  uint16_t drx_harq_rtt_timer_dl_r17 = 0;
  uint16_t drx_harq_rtt_timer_ul_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-ConfigSL-r17 ::= SEQUENCE
struct drx_cfg_sl_r17_s {
  struct drx_retx_timer_sl_r17_opts {
    enum options {
      sl0,
      sl1,
      sl2,
      sl4,
      sl6,
      sl8,
      sl16,
      sl24,
      sl33,
      sl40,
      sl64,
      sl80,
      sl96,
      sl112,
      sl128,
      sl160,
      sl320,
      spare15,
      spare14,
      spare13,
      spare12,
      spare11,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using drx_retx_timer_sl_r17_e_ = enumerated<drx_retx_timer_sl_r17_opts>;

  // member variables
  uint8_t                  drx_harq_rtt_timer_sl_r17 = 0;
  drx_retx_timer_sl_r17_e_ drx_retx_timer_sl_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-ConfigSecondaryGroup-r16 ::= SEQUENCE
struct drx_cfg_secondary_group_r16_s {
  struct drx_on_dur_timer_r16_c_ {
    struct milli_seconds_opts {
      enum options {
        ms1,
        ms2,
        ms3,
        ms4,
        ms5,
        ms6,
        ms8,
        ms10,
        ms20,
        ms30,
        ms40,
        ms50,
        ms60,
        ms80,
        ms100,
        ms200,
        ms300,
        ms400,
        ms500,
        ms600,
        ms800,
        ms1000,
        ms1200,
        ms1600,
        spare8,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using milli_seconds_e_ = enumerated<milli_seconds_opts>;
    struct types_opts {
      enum options { sub_milli_seconds, milli_seconds, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    drx_on_dur_timer_r16_c_() = default;
    drx_on_dur_timer_r16_c_(const drx_on_dur_timer_r16_c_& other);
    drx_on_dur_timer_r16_c_& operator=(const drx_on_dur_timer_r16_c_& other);
    ~drx_on_dur_timer_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sub_milli_seconds()
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimer-r16");
      return c.get<uint8_t>();
    }
    milli_seconds_e_& milli_seconds()
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimer-r16");
      return c.get<milli_seconds_e_>();
    }
    const uint8_t& sub_milli_seconds() const
    {
      assert_choice_type(types::sub_milli_seconds, type_, "drx-onDurationTimer-r16");
      return c.get<uint8_t>();
    }
    const milli_seconds_e_& milli_seconds() const
    {
      assert_choice_type(types::milli_seconds, type_, "drx-onDurationTimer-r16");
      return c.get<milli_seconds_e_>();
    }
    uint8_t&          set_sub_milli_seconds();
    milli_seconds_e_& set_milli_seconds();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct drx_inactivity_timer_r16_opts {
    enum options {
      ms0,
      ms1,
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms8,
      ms10,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms200,
      ms300,
      ms500,
      ms750,
      ms1280,
      ms1920,
      ms2560,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using drx_inactivity_timer_r16_e_ = enumerated<drx_inactivity_timer_r16_opts>;

  // member variables
  drx_on_dur_timer_r16_c_     drx_on_dur_timer_r16;
  drx_inactivity_timer_r16_e_ drx_inactivity_timer_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataInactivityTimer ::= ENUMERATED
struct data_inactivity_timer_opts {
  enum options { s1, s2, s3, s5, s7, s10, s15, s20, s40, s50, s60, s80, s100, s120, s150, s180, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
using data_inactivity_timer_e = enumerated<data_inactivity_timer_opts>;

// DeactivatedSCG-Config-r17 ::= SEQUENCE
struct deactiv_scg_cfg_r17_s {
  bool ext             = false;
  bool bfd_and_rlm_r17 = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GoodServingCellEvaluation-r17 ::= SEQUENCE
struct good_serving_cell_eval_r17_s {
  struct offset_r17_opts {
    enum options { db2, db4, db6, db8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using offset_r17_e_ = enumerated<offset_r17_opts>;

  // member variables
  bool          offset_r17_present = false;
  offset_r17_e_ offset_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraBandCC-CombinationReqList-r17 ::= SEQUENCE
struct intra_band_cc_combination_req_list_r17_s {
  using serv_cell_idx_list_r17_l_  = bounded_array<uint8_t, 32>;
  using cc_combination_list_r17_l_ = dyn_array<intra_band_cc_combination_r17_l>;

  // member variables
  serv_cell_idx_list_r17_l_  serv_cell_idx_list_r17;
  cc_combination_list_r17_l_ cc_combination_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBS-RNTI-SpecificConfig-r17 ::= SEQUENCE
struct mbs_rnti_specific_cfg_r17_s {
  struct group_common_rnti_r17_c_ {
    struct types_opts {
      enum options { g_rnti, g_cs_rnti, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    group_common_rnti_r17_c_() = default;
    group_common_rnti_r17_c_(const group_common_rnti_r17_c_& other);
    group_common_rnti_r17_c_& operator=(const group_common_rnti_r17_c_& other);
    ~group_common_rnti_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t& g_rnti()
    {
      assert_choice_type(types::g_rnti, type_, "groupCommon-RNTI-r17");
      return c.get<uint32_t>();
    }
    uint32_t& g_cs_rnti()
    {
      assert_choice_type(types::g_cs_rnti, type_, "groupCommon-RNTI-r17");
      return c.get<uint32_t>();
    }
    const uint32_t& g_rnti() const
    {
      assert_choice_type(types::g_rnti, type_, "groupCommon-RNTI-r17");
      return c.get<uint32_t>();
    }
    const uint32_t& g_cs_rnti() const
    {
      assert_choice_type(types::g_cs_rnti, type_, "groupCommon-RNTI-r17");
      return c.get<uint32_t>();
    }
    uint32_t& set_g_rnti();
    uint32_t& set_g_cs_rnti();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct harq_feedback_enabler_multicast_r17_opts {
    enum options { dci_enabler, enabled, nulltype } value;

    const char* to_string() const;
  };
  using harq_feedback_enabler_multicast_r17_e_ = enumerated<harq_feedback_enabler_multicast_r17_opts>;
  struct harq_feedback_option_multicast_r17_opts {
    enum options { ack_nack, nack_only, nulltype } value;

    const char* to_string() const;
  };
  using harq_feedback_option_multicast_r17_e_ = enumerated<harq_feedback_option_multicast_r17_opts>;
  struct pdsch_aggregation_factor_r17_opts {
    enum options { n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using pdsch_aggregation_factor_r17_e_ = enumerated<pdsch_aggregation_factor_r17_opts>;

  // member variables
  bool                                   drx_cfg_ptm_r17_present                     = false;
  bool                                   harq_feedback_enabler_multicast_r17_present = false;
  bool                                   harq_feedback_option_multicast_r17_present  = false;
  bool                                   pdsch_aggregation_factor_r17_present        = false;
  uint8_t                                mbs_rnti_specific_cfg_id_r17                = 0;
  group_common_rnti_r17_c_               group_common_rnti_r17;
  setup_release_c<drx_cfg_ptm_r17_s>     drx_cfg_ptm_r17;
  harq_feedback_enabler_multicast_r17_e_ harq_feedback_enabler_multicast_r17;
  harq_feedback_option_multicast_r17_e_  harq_feedback_option_multicast_r17;
  pdsch_aggregation_factor_r17_e_        pdsch_aggregation_factor_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastConfig-r17 ::= SEQUENCE
struct multicast_cfg_r17_s {
  struct type1_codebook_generation_mode_r17_opts {
    enum options { mode1, mode2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using type1_codebook_generation_mode_r17_e_ = enumerated<type1_codebook_generation_mode_r17_opts>;

  // member variables
  bool pdsch_harq_ack_codebook_list_multicast_r17_present = false;
  bool type1_codebook_generation_mode_r17_present         = false;
  setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>
                                        pdsch_harq_ack_codebook_list_multicast_r17;
  type1_codebook_generation_mode_r17_e_ type1_codebook_generation_mode_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MulticastRLC-BearerConfig-r17 ::= SEQUENCE
struct multicast_rlc_bearer_cfg_r17_s {
  bool     is_ptm_entity_r17_present   = false;
  uint16_t served_mbs_radio_bearer_r17 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCA-CombIndicator-r16 ::= SEQUENCE
struct pdcch_blind_detection_ca_comb_ind_r16_s {
  uint8_t pdcch_blind_detection_ca1_r16 = 1;
  uint8_t pdcch_blind_detection_ca2_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCCH-BlindDetectionCA-CombIndicator-r17 ::= SEQUENCE
struct pdcch_blind_detection_ca_comb_ind_r17_s {
  bool    pdcch_blind_detection_ca1_r17_present = false;
  bool    pdcch_blind_detection_ca2_r17_present = false;
  uint8_t pdcch_blind_detection_ca1_r17         = 1;
  uint8_t pdcch_blind_detection_ca2_r17         = 1;
  uint8_t pdcch_blind_detection_ca3_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-HARQ-ACK-EnhType3-r17 ::= SEQUENCE
struct pdsch_harq_ack_enh_type3_r17_s {
  struct applicable_r17_c_ {
    using per_cc_l_   = bounded_array<uint8_t, 32>;
    using per_harq_l_ = bounded_array<fixed_bitstring<16>, 32>;
    struct types_opts {
      enum options { per_cc, per_harq, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    applicable_r17_c_() = default;
    applicable_r17_c_(const applicable_r17_c_& other);
    applicable_r17_c_& operator=(const applicable_r17_c_& other);
    ~applicable_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    per_cc_l_& per_cc()
    {
      assert_choice_type(types::per_cc, type_, "applicable-r17");
      return c.get<per_cc_l_>();
    }
    per_harq_l_& per_harq()
    {
      assert_choice_type(types::per_harq, type_, "applicable-r17");
      return c.get<per_harq_l_>();
    }
    const per_cc_l_& per_cc() const
    {
      assert_choice_type(types::per_cc, type_, "applicable-r17");
      return c.get<per_cc_l_>();
    }
    const per_harq_l_& per_harq() const
    {
      assert_choice_type(types::per_harq, type_, "applicable-r17");
      return c.get<per_harq_l_>();
    }
    per_cc_l_&   set_per_cc();
    per_harq_l_& set_per_harq();

  private:
    types                                   type_;
    choice_buffer_t<per_cc_l_, per_harq_l_> c;

    void destroy_();
  };

  // member variables
  bool              ext                                      = false;
  bool              pdsch_harq_ack_enh_type3_ndi_r17_present = false;
  bool              pdsch_harq_ack_enh_type3_cbg_r17_present = false;
  uint8_t           pdsch_harq_ack_enh_type3_idx_r17         = 0;
  applicable_r17_c_ applicable_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PHR-Config ::= SEQUENCE
struct phr_cfg_s {
  struct phr_periodic_timer_opts {
    enum options { sf10, sf20, sf50, sf100, sf200, sf500, sf1000, infinity, nulltype } value;
    typedef int16_t number_type;

    const char* to_string() const;
    int16_t     to_number() const;
  };
  using phr_periodic_timer_e_ = enumerated<phr_periodic_timer_opts>;
  struct phr_prohibit_timer_opts {
    enum options { sf0, sf10, sf20, sf50, sf100, sf200, sf500, sf1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using phr_prohibit_timer_e_ = enumerated<phr_prohibit_timer_opts>;
  struct phr_tx_pwr_factor_change_opts {
    enum options { db1, db3, db6, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using phr_tx_pwr_factor_change_e_ = enumerated<phr_tx_pwr_factor_change_opts>;
  struct phr_mode_other_cg_opts {
    enum options { real, virtual_value, nulltype } value;

    const char* to_string() const;
  };
  using phr_mode_other_cg_e_ = enumerated<phr_mode_other_cg_opts>;

  // member variables
  bool                        ext = false;
  phr_periodic_timer_e_       phr_periodic_timer;
  phr_prohibit_timer_e_       phr_prohibit_timer;
  phr_tx_pwr_factor_change_e_ phr_tx_pwr_factor_change;
  bool                        multiple_phr         = false;
  bool                        dummy                = false;
  bool                        phr_type2_other_cell = false;
  phr_mode_other_cg_e_        phr_mode_other_cg;
  // ...
  // group 0
  copy_ptr<setup_release_c<mpe_cfg_fr2_r16_s>> mpe_report_fr2_r16;
  // group 1
  bool                                         two_phr_mode_r17_present = false;
  copy_ptr<setup_release_c<mpe_cfg_fr2_r17_s>> mpe_report_fr2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Config-v1610 ::= SEQUENCE
struct rlc_cfg_v1610_s {
  dl_am_rlc_v1610_s dl_am_rlc_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Config-v1700 ::= SEQUENCE
struct rlc_cfg_v1700_s {
  dl_am_rlc_v1700_s dl_am_rlc_v1700;
  dl_um_rlc_v1700_s dl_um_rlc_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLF-TimersAndConstants ::= SEQUENCE
struct rlf_timers_and_consts_s {
  struct t310_opts {
    enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, ms4000, ms6000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t310_e_ = enumerated<t310_opts>;
  struct n310_opts {
    enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using n310_e_ = enumerated<n310_opts>;
  struct n311_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using n311_e_ = enumerated<n311_opts>;
  struct t311_opts {
    enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t311_e_ = enumerated<t311_opts>;

  // member variables
  bool    ext = false;
  t310_e_ t310;
  n310_e_ n310;
  n311_e_ n311;
  // ...
  // group 0
  t311_e_ t311;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReconfigurationWithSync ::= SEQUENCE
struct recfg_with_sync_s {
  struct t304_opts {
    enum options { ms50, ms100, ms150, ms200, ms500, ms1000, ms2000, ms10000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using t304_e_ = enumerated<t304_opts>;
  struct rach_cfg_ded_c_ {
    struct types_opts {
      enum options { ul, supplementary_ul, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    rach_cfg_ded_c_() = default;
    rach_cfg_ded_c_(const rach_cfg_ded_c_& other);
    rach_cfg_ded_c_& operator=(const rach_cfg_ded_c_& other);
    ~rach_cfg_ded_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rach_cfg_ded_s& ul()
    {
      assert_choice_type(types::ul, type_, "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    rach_cfg_ded_s& supplementary_ul()
    {
      assert_choice_type(types::supplementary_ul, type_, "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    const rach_cfg_ded_s& ul() const
    {
      assert_choice_type(types::ul, type_, "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    const rach_cfg_ded_s& supplementary_ul() const
    {
      assert_choice_type(types::supplementary_ul, type_, "rach-ConfigDedicated");
      return c.get<rach_cfg_ded_s>();
    }
    rach_cfg_ded_s& set_ul();
    rach_cfg_ded_s& set_supplementary_ul();

  private:
    types                           type_;
    choice_buffer_t<rach_cfg_ded_s> c;

    void destroy_();
  };

  // member variables
  bool                      ext                        = false;
  bool                      sp_cell_cfg_common_present = false;
  bool                      rach_cfg_ded_present       = false;
  serving_cell_cfg_common_s sp_cell_cfg_common;
  uint32_t                  new_ue_id = 0;
  t304_e_                   t304;
  rach_cfg_ded_c_           rach_cfg_ded;
  // ...
  // group 0
  copy_ptr<ssb_mtc_s> smtc;
  // group 1
  copy_ptr<daps_ul_pwr_cfg_r16_s> daps_ul_pwr_cfg_r16;
  // group 2
  copy_ptr<sl_path_switch_cfg_r17_s> sl_path_switch_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestConfig ::= SEQUENCE
struct sched_request_cfg_s {
  using sched_request_to_add_mod_list_l_ = dyn_array<sched_request_to_add_mod_s>;
  using sched_request_to_release_list_l_ = bounded_array<uint8_t, 8>;

  // member variables
  sched_request_to_add_mod_list_l_ sched_request_to_add_mod_list;
  sched_request_to_release_list_l_ sched_request_to_release_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingRequestConfig-v1700 ::= SEQUENCE
struct sched_request_cfg_v1700_s {
  using sched_request_to_add_mod_list_ext_v1700_l_ = dyn_array<sched_request_to_add_mod_ext_v1700_s>;

  // member variables
  sched_request_to_add_mod_list_ext_v1700_l_ sched_request_to_add_mod_list_ext_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAG-Config ::= SEQUENCE
struct tag_cfg_s {
  using tag_to_release_list_l_ = bounded_array<uint8_t, 4>;
  using tag_to_add_mod_list_l_ = dyn_array<tag_s>;

  // member variables
  tag_to_release_list_l_ tag_to_release_list;
  tag_to_add_mod_list_l_ tag_to_add_mod_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TAR-Config-r17 ::= SEQUENCE
struct tar_cfg_r17_s {
  struct offset_thres_ta_r17_opts {
    enum options {
      ms0dot5,
      ms1,
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms7,
      ms8,
      ms9,
      ms10,
      ms11,
      ms12,
      ms13,
      ms14,
      ms15,
      spare13,
      spare12,
      spare11,
      spare10,
      spare9,
      spare8,
      spare7,
      spare6,
      spare5,
      spare4,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using offset_thres_ta_r17_e_ = enumerated<offset_thres_ta_r17_opts>;

  // member variables
  bool                   ext                           = false;
  bool                   offset_thres_ta_r17_present   = false;
  bool                   timing_advance_sr_r17_present = false;
  offset_thres_ta_r17_e_ offset_thres_ta_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IAB-ResourceConfig-r17 ::= SEQUENCE
struct iab_res_cfg_r17_s {
  using slot_list_r17_l_ = dyn_array<uint16_t>;
  struct periodicity_slot_list_r17_opts {
    enum options { ms0p5, ms0p625, ms1, ms1p25, ms2, ms2p5, ms5, ms10, ms20, ms40, ms80, ms160, nulltype } value;

    const char* to_string() const;
  };
  using periodicity_slot_list_r17_e_ = enumerated<periodicity_slot_list_r17_opts>;

  // member variables
  bool                         ext                                      = false;
  bool                         periodicity_slot_list_r17_present        = false;
  bool                         slot_list_subcarrier_spacing_r17_present = false;
  uint32_t                     iab_res_cfg_id_r17                       = 0;
  slot_list_r17_l_             slot_list_r17;
  periodicity_slot_list_r17_e_ periodicity_slot_list_r17;
  subcarrier_spacing_e         slot_list_subcarrier_spacing_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-CellGroupConfig ::= SEQUENCE
struct mac_cell_group_cfg_s {
  using g_rnti_cfg_to_add_mod_list_r17_l_    = dyn_array<mbs_rnti_specific_cfg_r17_s>;
  using g_rnti_cfg_to_release_list_r17_l_    = bounded_array<uint8_t, 16>;
  using g_cs_rnti_cfg_to_add_mod_list_r17_l_ = dyn_array<mbs_rnti_specific_cfg_r17_s>;
  using g_cs_rnti_cfg_to_release_list_r17_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                       ext                       = false;
  bool                       drx_cfg_present           = false;
  bool                       sched_request_cfg_present = false;
  bool                       bsr_cfg_present           = false;
  bool                       tag_cfg_present           = false;
  bool                       phr_cfg_present           = false;
  setup_release_c<drx_cfg_s> drx_cfg;
  sched_request_cfg_s        sched_request_cfg;
  bsr_cfg_s                  bsr_cfg;
  tag_cfg_s                  tag_cfg;
  setup_release_c<phr_cfg_s> phr_cfg;
  bool                       skip_ul_tx_dyn = false;
  // ...
  // group 0
  bool                                               csi_mask_present = false;
  bool                                               csi_mask         = false;
  copy_ptr<setup_release_c<data_inactivity_timer_e>> data_inactivity_timer;
  // group 1
  bool                                                     use_pre_bsr_r16_present                = false;
  bool                                                     sched_request_id_lbt_scell_r16_present = false;
  bool                                                     lch_based_prioritization_r16_present   = false;
  bool                                                     sched_request_id_bfr_scell_r16_present = false;
  uint8_t                                                  sched_request_id_lbt_scell_r16         = 0;
  uint8_t                                                  sched_request_id_bfr_scell_r16         = 0;
  copy_ptr<setup_release_c<drx_cfg_secondary_group_r16_s>> drx_cfg_secondary_group_r16;
  // group 2
  bool enhanced_skip_ul_tx_dyn_r16_present = false;
  bool enhanced_skip_ul_tx_cfg_r16_present = false;
  // group 3
  bool                                           intra_cg_prioritization_r17_present               = false;
  bool                                           sched_request_id_bfr_r17_present                  = false;
  bool                                           sched_request_id_bfr2_r17_present                 = false;
  bool                                           allow_csi_srs_tx_multicast_drx_active_r17_present = false;
  copy_ptr<setup_release_c<drx_cfg_sl_r17_s>>    drx_cfg_sl_r17;
  copy_ptr<setup_release_c<drx_cfg_ext_v1700_s>> drx_cfg_ext_v1700;
  uint8_t                                        sched_request_id_bfr_r17  = 0;
  uint8_t                                        sched_request_id_bfr2_r17 = 0;
  copy_ptr<sched_request_cfg_v1700_s>            sched_request_cfg_v1700;
  copy_ptr<setup_release_c<tar_cfg_r17_s>>       tar_cfg_r17;
  copy_ptr<g_rnti_cfg_to_add_mod_list_r17_l_>    g_rnti_cfg_to_add_mod_list_r17;
  copy_ptr<g_rnti_cfg_to_release_list_r17_l_>    g_rnti_cfg_to_release_list_r17;
  copy_ptr<g_cs_rnti_cfg_to_add_mod_list_r17_l_> g_cs_rnti_cfg_to_add_mod_list_r17;
  copy_ptr<g_cs_rnti_cfg_to_release_list_r17_l_> g_cs_rnti_cfg_to_release_list_r17;
  bool                                           allow_csi_srs_tx_multicast_drx_active_r17 = false;
  // group 4
  bool    sched_request_id_pos_mg_request_r17_present = false;
  bool    drx_last_tx_ul_r17_present                  = false;
  uint8_t sched_request_id_pos_mg_request_r17         = 0;
  // group 5
  bool pos_mg_request_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhysicalCellGroupConfig ::= SEQUENCE
struct phys_cell_group_cfg_s {
  struct pdsch_harq_ack_codebook_opts {
    enum options { semi_static, dyn, nulltype } value;

    const char* to_string() const;
  };
  using pdsch_harq_ack_codebook_e_ = enumerated<pdsch_harq_ack_codebook_opts>;
  struct xscale_opts {
    enum options { db0, db6, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using xscale_e_ = enumerated<xscale_opts>;
  struct harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
  };
  using harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_e_ =
      enumerated<harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_opts>;
  struct harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_opts {
    enum options { enabled, disabled, nulltype } value;

    const char* to_string() const;
  };
  using harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_e_ =
      enumerated<harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_opts>;
  struct pdsch_harq_ack_codebook_secondary_pucch_group_r16_opts {
    enum options { semi_static, dyn, nulltype } value;

    const char* to_string() const;
  };
  using pdsch_harq_ack_codebook_secondary_pucch_group_r16_e_ =
      enumerated<pdsch_harq_ack_codebook_secondary_pucch_group_r16_opts>;
  struct nrdc_p_cmode_fr1_r16_opts {
    enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrdc_p_cmode_fr1_r16_e_ = enumerated<nrdc_p_cmode_fr1_r16_opts>;
  struct nrdc_p_cmode_fr2_r16_opts {
    enum options { semi_static_mode1, semi_static_mode2, dyn, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using nrdc_p_cmode_fr2_r16_e_ = enumerated<nrdc_p_cmode_fr2_r16_opts>;
  struct dl_assign_idx_dci_1_2_r16_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dl_assign_idx_dci_1_2_r16_e_ = enumerated<dl_assign_idx_dci_1_2_r16_opts>;
  struct ack_nack_feedback_mode_r16_opts {
    enum options { joint, separate, nulltype } value;

    const char* to_string() const;
  };
  using ack_nack_feedback_mode_r16_e_                             = enumerated<ack_nack_feedback_mode_r16_opts>;
  using pdsch_harq_ack_enh_type3_to_add_mod_list_r17_l_           = dyn_array<pdsch_harq_ack_enh_type3_r17_s>;
  using pdsch_harq_ack_enh_type3_to_release_list_r17_l_           = bounded_array<uint8_t, 8>;
  using pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17_l_ = dyn_array<pdsch_harq_ack_enh_type3_r17_s>;
  using pdsch_harq_ack_enh_type3_secondary_to_release_list_r17_l_ = bounded_array<uint8_t, 8>;
  using pucch_s_scell_pattern_r17_l_                              = dyn_array<uint8_t>;
  using pucch_s_scell_pattern_secondary_pucch_group_r17_l_        = dyn_array<uint8_t>;

  // member variables
  bool                                         ext                                     = false;
  bool                                         harq_ack_spatial_bundling_pucch_present = false;
  bool                                         harq_ack_spatial_bundling_pusch_present = false;
  bool                                         p_nr_fr1_present                        = false;
  bool                                         tpc_srs_rnti_present                    = false;
  bool                                         tpc_pucch_rnti_present                  = false;
  bool                                         tpc_pusch_rnti_present                  = false;
  bool                                         sp_csi_rnti_present                     = false;
  bool                                         cs_rnti_present                         = false;
  int8_t                                       p_nr_fr1                                = -30;
  pdsch_harq_ack_codebook_e_                   pdsch_harq_ack_codebook;
  uint32_t                                     tpc_srs_rnti   = 0;
  uint32_t                                     tpc_pucch_rnti = 0;
  uint32_t                                     tpc_pusch_rnti = 0;
  uint32_t                                     sp_csi_rnti    = 0;
  setup_release_c<integer<uint32_t, 0, 65535>> cs_rnti;
  // ...
  // group 0
  bool     mcs_c_rnti_present = false;
  bool     p_ue_fr1_present   = false;
  uint32_t mcs_c_rnti         = 0;
  int8_t   p_ue_fr1           = -30;
  // group 1
  bool      xscale_present = false;
  xscale_e_ xscale;
  // group 2
  copy_ptr<setup_release_c<integer<uint8_t, 1, 15>>> pdcch_blind_detection;
  // group 3
  bool                                      harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present = false;
  bool                                      harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present = false;
  bool                                      pdsch_harq_ack_codebook_secondary_pucch_group_r16_present         = false;
  bool                                      p_nr_fr2_r16_present                                              = false;
  bool                                      p_ue_fr2_r16_present                                              = false;
  bool                                      nrdc_p_cmode_fr1_r16_present                                      = false;
  bool                                      nrdc_p_cmode_fr2_r16_present                                      = false;
  bool                                      pdsch_harq_ack_codebook_r16_present                               = false;
  bool                                      nfi_total_dai_included_r16_present                                = false;
  bool                                      ul_total_dai_included_r16_present                                 = false;
  bool                                      pdsch_harq_ack_one_shot_feedback_r16_present                      = false;
  bool                                      pdsch_harq_ack_one_shot_feedback_ndi_r16_present                  = false;
  bool                                      pdsch_harq_ack_one_shot_feedback_cbg_r16_present                  = false;
  bool                                      dl_assign_idx_dci_0_2_r16_present                                 = false;
  bool                                      dl_assign_idx_dci_1_2_r16_present                                 = false;
  bool                                      ack_nack_feedback_mode_r16_present                                = false;
  bool                                      bd_factor_r_r16_present                                           = false;
  copy_ptr<setup_release_c<d_cp_cfg_r16_s>> dcp_cfg_r16;
  harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_e_
      harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16;
  harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_e_
                                                       harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16;
  pdsch_harq_ack_codebook_secondary_pucch_group_r16_e_ pdsch_harq_ack_codebook_secondary_pucch_group_r16;
  int8_t                                               p_nr_fr2_r16 = -30;
  int8_t                                               p_ue_fr2_r16 = -30;
  nrdc_p_cmode_fr1_r16_e_                              nrdc_p_cmode_fr1_r16;
  nrdc_p_cmode_fr2_r16_e_                              nrdc_p_cmode_fr2_r16;
  dl_assign_idx_dci_1_2_r16_e_                         dl_assign_idx_dci_1_2_r16;
  copy_ptr<setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>>
                                                                     pdsch_harq_ack_codebook_list_r16;
  ack_nack_feedback_mode_r16_e_                                      ack_nack_feedback_mode_r16;
  copy_ptr<setup_release_c<pdcch_blind_detection_ca_comb_ind_r16_s>> pdcch_blind_detection_ca_comb_ind_r16;
  copy_ptr<setup_release_c<integer<uint8_t, 1, 15>>>                 pdcch_blind_detection2_r16;
  copy_ptr<setup_release_c<integer<uint8_t, 1, 15>>>                 pdcch_blind_detection3_r16;
  // group 4
  bool pdsch_harq_ack_enh_type3_dci_field_secondary_pucch_group_r17_present = false;
  bool pdsch_harq_ack_enh_type3_dci_field_r17_present                       = false;
  bool pdsch_harq_ack_retx_r17_present                                      = false;
  bool pdsch_harq_ack_retx_secondary_pucch_group_r17_present                = false;
  bool pucch_s_scell_r17_present                                            = false;
  bool pucch_s_scell_secondary_pucch_group_r17_present                      = false;
  bool pucch_s_scell_dyn_r17_present                                        = false;
  bool pucch_s_scell_dyn_secondary_pucch_group_r17_present                  = false;
  bool uci_mux_with_diff_prio_r17_present                                   = false;
  bool uci_mux_with_diff_prio_secondary_pucch_group_r17_present             = false;
  bool simul_pucch_pusch_r17_present                                        = false;
  bool simul_pucch_pusch_secondary_pucch_group_r17_present                  = false;
  bool prio_low_dg_high_cg_r17_present                                      = false;
  bool prio_high_dg_low_cg_r17_present                                      = false;
  bool two_qcl_type_d_for_pdcch_repeat_r17_present                          = false;
  copy_ptr<pdsch_harq_ack_enh_type3_to_add_mod_list_r17_l_> pdsch_harq_ack_enh_type3_to_add_mod_list_r17;
  copy_ptr<pdsch_harq_ack_enh_type3_to_release_list_r17_l_> pdsch_harq_ack_enh_type3_to_release_list_r17;
  copy_ptr<pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17_l_>
      pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17;
  copy_ptr<pdsch_harq_ack_enh_type3_secondary_to_release_list_r17_l_>
                                                               pdsch_harq_ack_enh_type3_secondary_to_release_list_r17;
  uint8_t                                                      pucch_s_scell_r17                       = 1;
  uint8_t                                                      pucch_s_scell_secondary_pucch_group_r17 = 1;
  copy_ptr<pucch_s_scell_pattern_r17_l_>                       pucch_s_scell_pattern_r17;
  copy_ptr<pucch_s_scell_pattern_secondary_pucch_group_r17_l_> pucch_s_scell_pattern_secondary_pucch_group_r17;
  copy_ptr<setup_release_c<multicast_cfg_r17_s>>               multicast_cfg_r17;
  copy_ptr<setup_release_c<pdcch_blind_detection_ca_comb_ind_r17_s>> pdcch_blind_detection_ca_comb_ind_r17;
  // group 5
  bool simul_sr_pusch_diff_pucch_groups_r17_present = false;
  // group 6
  bool intra_band_nc_prach_simul_tx_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-BearerConfig ::= SEQUENCE
struct rlc_bearer_cfg_s {
  struct served_radio_bearer_c_ {
    struct types_opts {
      enum options { srb_id, drb_id, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    served_radio_bearer_c_() = default;
    served_radio_bearer_c_(const served_radio_bearer_c_& other);
    served_radio_bearer_c_& operator=(const served_radio_bearer_c_& other);
    ~served_radio_bearer_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& srb_id()
    {
      assert_choice_type(types::srb_id, type_, "servedRadioBearer");
      return c.get<uint8_t>();
    }
    uint8_t& drb_id()
    {
      assert_choice_type(types::drb_id, type_, "servedRadioBearer");
      return c.get<uint8_t>();
    }
    const uint8_t& srb_id() const
    {
      assert_choice_type(types::srb_id, type_, "servedRadioBearer");
      return c.get<uint8_t>();
    }
    const uint8_t& drb_id() const
    {
      assert_choice_type(types::drb_id, type_, "servedRadioBearer");
      return c.get<uint8_t>();
    }
    uint8_t& set_srb_id();
    uint8_t& set_drb_id();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                   ext                         = false;
  bool                   served_radio_bearer_present = false;
  bool                   reestablish_rlc_present     = false;
  bool                   rlc_cfg_present             = false;
  bool                   mac_lc_ch_cfg_present       = false;
  uint8_t                lc_ch_id                    = 1;
  served_radio_bearer_c_ served_radio_bearer;
  rlc_cfg_c              rlc_cfg;
  lc_ch_cfg_s            mac_lc_ch_cfg;
  // ...
  // group 0
  copy_ptr<rlc_cfg_v1610_s> rlc_cfg_v1610;
  // group 1
  bool                                     lc_ch_id_ext_r17_present             = false;
  bool                                     served_radio_bearer_srb4_r17_present = false;
  copy_ptr<rlc_cfg_v1700_s>                rlc_cfg_v1700;
  uint32_t                                 lc_ch_id_ext_r17 = 320;
  copy_ptr<multicast_rlc_bearer_cfg_r17_s> multicast_rlc_bearer_cfg_r17;
  uint8_t                                  served_radio_bearer_srb4_r17 = 4;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportUplinkTxDirectCurrentMoreCarrier-r17 ::= SEQUENCE (SIZE (1..32)) OF IntraBandCC-CombinationReqList-r17
using report_ul_tx_direct_current_more_carrier_r17_l = dyn_array<intra_band_cc_combination_req_list_r17_s>;

// SCellConfig ::= SEQUENCE
struct scell_cfg_s {
  bool                      ext                      = false;
  bool                      scell_cfg_common_present = false;
  bool                      scell_cfg_ded_present    = false;
  uint8_t                   scell_idx                = 1;
  serving_cell_cfg_common_s scell_cfg_common;
  serving_cell_cfg_s        scell_cfg_ded;
  // ...
  // group 0
  copy_ptr<ssb_mtc_s> smtc;
  // group 1
  bool scell_state_r16_present             = false;
  bool secondary_drx_group_cfg_r16_present = false;
  // group 2
  bool                                     pre_conf_gap_status_r17_present = false;
  fixed_bitstring<8>                       pre_conf_gap_status_r17;
  copy_ptr<good_serving_cell_eval_r17_s>   good_serving_cell_eval_bfd_r17;
  copy_ptr<setup_release_c<dyn_octstring>> scell_sib20_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SpCellConfig ::= SEQUENCE
struct sp_cell_cfg_s {
  struct low_mob_eval_connected_r17_s_ {
    struct s_search_delta_p_connected_r17_opts {
      enum options { db3, db6, db9, db12, db15, spare3, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using s_search_delta_p_connected_r17_e_ = enumerated<s_search_delta_p_connected_r17_opts>;
    struct t_search_delta_p_connected_r17_opts {
      enum options {
        s5,
        s10,
        s20,
        s30,
        s60,
        s120,
        s180,
        s240,
        s300,
        spare7,
        spare6,
        spare5,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using t_search_delta_p_connected_r17_e_ = enumerated<t_search_delta_p_connected_r17_opts>;

    // member variables
    s_search_delta_p_connected_r17_e_ s_search_delta_p_connected_r17;
    t_search_delta_p_connected_r17_e_ t_search_delta_p_connected_r17;
  };

  // member variables
  bool                                     ext                                   = false;
  bool                                     serv_cell_idx_present                 = false;
  bool                                     recfg_with_sync_present               = false;
  bool                                     rlf_timers_and_consts_present         = false;
  bool                                     rlm_in_sync_out_of_sync_thres_present = false;
  bool                                     sp_cell_cfg_ded_present               = false;
  uint8_t                                  serv_cell_idx                         = 0;
  recfg_with_sync_s                        recfg_with_sync;
  setup_release_c<rlf_timers_and_consts_s> rlf_timers_and_consts;
  serving_cell_cfg_s                       sp_cell_cfg_ded;
  // ...
  // group 0
  copy_ptr<low_mob_eval_connected_r17_s_>          low_mob_eval_connected_r17;
  copy_ptr<good_serving_cell_eval_r17_s>           good_serving_cell_eval_rlm_r17;
  copy_ptr<good_serving_cell_eval_r17_s>           good_serving_cell_eval_bfd_r17;
  copy_ptr<setup_release_c<deactiv_scg_cfg_r17_s>> deactiv_scg_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Uu-RelayRLC-ChannelConfig-r17 ::= SEQUENCE
struct uu_relay_rlc_ch_cfg_r17_s {
  bool        ext                         = false;
  bool        uu_lc_ch_id_r17_present     = false;
  bool        reestablish_rlc_r17_present = false;
  bool        rlc_cfg_r17_present         = false;
  bool        mac_lc_ch_cfg_r17_present   = false;
  uint8_t     uu_lc_ch_id_r17             = 1;
  uint8_t     uu_relay_rlc_ch_id_r17      = 1;
  rlc_cfg_c   rlc_cfg_r17;
  lc_ch_cfg_s mac_lc_ch_cfg_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGroupConfig ::= SEQUENCE
struct cell_group_cfg_s {
  using rlc_bearer_to_add_mod_list_l_    = dyn_array<rlc_bearer_cfg_s>;
  using rlc_bearer_to_release_list_l_    = bounded_array<uint8_t, 32>;
  using scell_to_add_mod_list_l_         = dyn_array<scell_cfg_s>;
  using scell_to_release_list_l_         = bounded_array<uint8_t, 31>;
  using bh_rlc_ch_to_add_mod_list_r16_l_ = dyn_array<bh_rlc_ch_cfg_r16_s>;
  using bh_rlc_ch_to_release_list_r16_l_ = dyn_array<fixed_bitstring<16>>;
  struct f1c_transfer_path_r16_opts {
    enum options { lte, nr, both, nulltype } value;

    const char* to_string() const;
  };
  using f1c_transfer_path_r16_e_       = enumerated<f1c_transfer_path_r16_opts>;
  using simul_tci_upd_list1_r16_l_     = bounded_array<uint8_t, 32>;
  using simul_tci_upd_list2_r16_l_     = bounded_array<uint8_t, 32>;
  using simul_spatial_upd_list1_r16_l_ = bounded_array<uint8_t, 32>;
  using simul_spatial_upd_list2_r16_l_ = bounded_array<uint8_t, 32>;
  struct ul_tx_switching_option_r16_opts {
    enum options { switched_ul, dual_ul, nulltype } value;

    const char* to_string() const;
  };
  using ul_tx_switching_option_r16_e_ = enumerated<ul_tx_switching_option_r16_opts>;
  struct f1c_transfer_path_nrdc_r17_opts {
    enum options { mcg, scg, both, nulltype } value;

    const char* to_string() const;
  };
  using f1c_transfer_path_nrdc_r17_e_ = enumerated<f1c_transfer_path_nrdc_r17_opts>;
  struct ul_tx_switching_dual_ul_tx_state_r17_opts {
    enum options { one_t, two_t, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ul_tx_switching_dual_ul_tx_state_r17_e_ = enumerated<ul_tx_switching_dual_ul_tx_state_r17_opts>;
  using uu_relay_rlc_ch_to_add_mod_list_r17_l_  = dyn_array<uu_relay_rlc_ch_cfg_r17_s>;
  using uu_relay_rlc_ch_to_release_list_r17_l_  = bounded_array<uint8_t, 32>;
  using simul_u_tci_upd_list1_r17_l_            = bounded_array<uint8_t, 32>;
  using simul_u_tci_upd_list2_r17_l_            = bounded_array<uint8_t, 32>;
  using simul_u_tci_upd_list3_r17_l_            = bounded_array<uint8_t, 32>;
  using simul_u_tci_upd_list4_r17_l_            = bounded_array<uint8_t, 32>;
  using rlc_bearer_to_release_list_ext_r17_l_   = bounded_array<uint32_t, 32>;
  using iab_res_cfg_to_add_mod_list_r17_l_      = dyn_array<iab_res_cfg_r17_s>;
  using iab_res_cfg_to_release_list_r17_l_      = dyn_array<uint32_t>;

  // member variables
  bool                          ext                         = false;
  bool                          mac_cell_group_cfg_present  = false;
  bool                          phys_cell_group_cfg_present = false;
  bool                          sp_cell_cfg_present         = false;
  uint8_t                       cell_group_id               = 0;
  rlc_bearer_to_add_mod_list_l_ rlc_bearer_to_add_mod_list;
  rlc_bearer_to_release_list_l_ rlc_bearer_to_release_list;
  mac_cell_group_cfg_s          mac_cell_group_cfg;
  phys_cell_group_cfg_s         phys_cell_group_cfg;
  sp_cell_cfg_s                 sp_cell_cfg;
  scell_to_add_mod_list_l_      scell_to_add_mod_list;
  scell_to_release_list_l_      scell_to_release_list;
  // ...
  // group 0
  bool report_ul_tx_direct_current_present = false;
  // group 1
  bool                                       bap_address_r16_present                  = false;
  bool                                       f1c_transfer_path_r16_present            = false;
  bool                                       ul_tx_switching_option_r16_present       = false;
  bool                                       ul_tx_switching_pwr_boosting_r16_present = false;
  fixed_bitstring<10>                        bap_address_r16;
  copy_ptr<bh_rlc_ch_to_add_mod_list_r16_l_> bh_rlc_ch_to_add_mod_list_r16;
  copy_ptr<bh_rlc_ch_to_release_list_r16_l_> bh_rlc_ch_to_release_list_r16;
  f1c_transfer_path_r16_e_                   f1c_transfer_path_r16;
  copy_ptr<simul_tci_upd_list1_r16_l_>       simul_tci_upd_list1_r16;
  copy_ptr<simul_tci_upd_list2_r16_l_>       simul_tci_upd_list2_r16;
  copy_ptr<simul_spatial_upd_list1_r16_l_>   simul_spatial_upd_list1_r16;
  copy_ptr<simul_spatial_upd_list2_r16_l_>   simul_spatial_upd_list2_r16;
  ul_tx_switching_option_r16_e_              ul_tx_switching_option_r16;
  // group 2
  bool report_ul_tx_direct_current_two_carrier_r16_present = false;
  // group 3
  bool                                             f1c_transfer_path_nrdc_r17_present           = false;
  bool                                             ul_tx_switching_2_t_mode_r17_present         = false;
  bool                                             ul_tx_switching_dual_ul_tx_state_r17_present = false;
  f1c_transfer_path_nrdc_r17_e_                    f1c_transfer_path_nrdc_r17;
  ul_tx_switching_dual_ul_tx_state_r17_e_          ul_tx_switching_dual_ul_tx_state_r17;
  copy_ptr<uu_relay_rlc_ch_to_add_mod_list_r17_l_> uu_relay_rlc_ch_to_add_mod_list_r17;
  copy_ptr<uu_relay_rlc_ch_to_release_list_r17_l_> uu_relay_rlc_ch_to_release_list_r17;
  copy_ptr<simul_u_tci_upd_list1_r17_l_>           simul_u_tci_upd_list1_r17;
  copy_ptr<simul_u_tci_upd_list2_r17_l_>           simul_u_tci_upd_list2_r17;
  copy_ptr<simul_u_tci_upd_list3_r17_l_>           simul_u_tci_upd_list3_r17;
  copy_ptr<simul_u_tci_upd_list4_r17_l_>           simul_u_tci_upd_list4_r17;
  copy_ptr<rlc_bearer_to_release_list_ext_r17_l_>  rlc_bearer_to_release_list_ext_r17;
  copy_ptr<iab_res_cfg_to_add_mod_list_r17_l_>     iab_res_cfg_to_add_mod_list_r17;
  copy_ptr<iab_res_cfg_to_release_list_r17_l_>     iab_res_cfg_to_release_list_r17;
  // group 4
  copy_ptr<report_ul_tx_direct_current_more_carrier_r17_l> report_ul_tx_direct_current_more_carrier_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
