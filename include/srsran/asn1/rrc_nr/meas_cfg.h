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
 *                    3GPP TS ASN1 RRC NR v17.4.0 (2023-03)
 *
 ******************************************************************************/

#pragma once

#include "dl_ccch_msg_ies.h"
#include "sys_info.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// CSI-RS-Resource-Mobility ::= SEQUENCE
struct csi_rs_res_mob_s {
  struct slot_cfg_c_ {
    struct types_opts {
      enum options { ms4, ms5, ms10, ms20, ms40, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    slot_cfg_c_() = default;
    slot_cfg_c_(const slot_cfg_c_& other);
    slot_cfg_c_& operator=(const slot_cfg_c_& other);
    ~slot_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ms4()
    {
      assert_choice_type(types::ms4, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    uint8_t& ms5()
    {
      assert_choice_type(types::ms5, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    uint8_t& ms10()
    {
      assert_choice_type(types::ms10, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    uint8_t& ms20()
    {
      assert_choice_type(types::ms20, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    uint16_t& ms40()
    {
      assert_choice_type(types::ms40, type_, "slotConfig");
      return c.get<uint16_t>();
    }
    const uint8_t& ms4() const
    {
      assert_choice_type(types::ms4, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ms5() const
    {
      assert_choice_type(types::ms5, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ms10() const
    {
      assert_choice_type(types::ms10, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ms20() const
    {
      assert_choice_type(types::ms20, type_, "slotConfig");
      return c.get<uint8_t>();
    }
    const uint16_t& ms40() const
    {
      assert_choice_type(types::ms40, type_, "slotConfig");
      return c.get<uint16_t>();
    }
    uint8_t&  set_ms4();
    uint8_t&  set_ms5();
    uint8_t&  set_ms10();
    uint8_t&  set_ms20();
    uint16_t& set_ms40();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct associated_ssb_s_ {
    uint8_t ssb_idx            = 0;
    bool    is_quasi_colocated = false;
  };
  struct freq_domain_alloc_c_ {
    struct types_opts {
      enum options { row1, row2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    freq_domain_alloc_c_() = default;
    freq_domain_alloc_c_(const freq_domain_alloc_c_& other);
    freq_domain_alloc_c_& operator=(const freq_domain_alloc_c_& other);
    ~freq_domain_alloc_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<4>& row1()
    {
      assert_choice_type(types::row1, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4>>();
    }
    fixed_bitstring<12>& row2()
    {
      assert_choice_type(types::row2, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12>>();
    }
    const fixed_bitstring<4>& row1() const
    {
      assert_choice_type(types::row1, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<4>>();
    }
    const fixed_bitstring<12>& row2() const
    {
      assert_choice_type(types::row2, type_, "frequencyDomainAllocation");
      return c.get<fixed_bitstring<12>>();
    }
    fixed_bitstring<4>&  set_row1();
    fixed_bitstring<12>& set_row2();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<12>> c;

    void destroy_();
  };
  struct slot_cfg_r17_c_ {
    struct types_opts {
      enum options { ms4, ms5, ms10, ms20, ms40, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    slot_cfg_r17_c_() = default;
    slot_cfg_r17_c_(const slot_cfg_r17_c_& other);
    slot_cfg_r17_c_& operator=(const slot_cfg_r17_c_& other);
    ~slot_cfg_r17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& ms4()
    {
      assert_choice_type(types::ms4, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms5()
    {
      assert_choice_type(types::ms5, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms10()
    {
      assert_choice_type(types::ms10, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms20()
    {
      assert_choice_type(types::ms20, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& ms40()
    {
      assert_choice_type(types::ms40, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms4() const
    {
      assert_choice_type(types::ms4, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms5() const
    {
      assert_choice_type(types::ms5, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms10() const
    {
      assert_choice_type(types::ms10, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms20() const
    {
      assert_choice_type(types::ms20, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    const uint16_t& ms40() const
    {
      assert_choice_type(types::ms40, type_, "slotConfig-r17");
      return c.get<uint16_t>();
    }
    uint16_t& set_ms4();
    uint16_t& set_ms5();
    uint16_t& set_ms10();
    uint16_t& set_ms20();
    uint16_t& set_ms40();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                 ext                    = false;
  bool                 associated_ssb_present = false;
  uint8_t              csi_rs_idx             = 0;
  slot_cfg_c_          slot_cfg;
  associated_ssb_s_    associated_ssb;
  freq_domain_alloc_c_ freq_domain_alloc;
  uint8_t              first_ofdm_symbol_in_time_domain = 0;
  uint16_t             seq_generation_cfg               = 0;
  // ...
  // group 0
  copy_ptr<slot_cfg_r17_c_> slot_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasTriggerQuantity ::= CHOICE
struct meas_trigger_quant_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  meas_trigger_quant_c() = default;
  meas_trigger_quant_c(const meas_trigger_quant_c& other);
  meas_trigger_quant_c& operator=(const meas_trigger_quant_c& other);
  ~meas_trigger_quant_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& rsrp()
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  uint8_t& rsrq()
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  uint8_t& sinr()
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrp() const
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrq() const
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  const uint8_t& sinr() const
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantity");
    return c.get<uint8_t>();
  }
  uint8_t& set_rsrp();
  uint8_t& set_rsrq();
  uint8_t& set_sinr();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MeasTriggerQuantityOffset ::= CHOICE
struct meas_trigger_quant_offset_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  meas_trigger_quant_offset_c() = default;
  meas_trigger_quant_offset_c(const meas_trigger_quant_offset_c& other);
  meas_trigger_quant_offset_c& operator=(const meas_trigger_quant_offset_c& other);
  ~meas_trigger_quant_offset_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int8_t& rsrp()
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  int8_t& rsrq()
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  int8_t& sinr()
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  const int8_t& rsrp() const
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  const int8_t& rsrq() const
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  const int8_t& sinr() const
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantityOffset");
    return c.get<int8_t>();
  }
  int8_t& set_rsrp();
  int8_t& set_rsrq();
  int8_t& set_sinr();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// NR-RS-Type ::= ENUMERATED
struct nr_rs_type_opts {
  enum options { ssb, csi_rs, nulltype } value;

  const char* to_string() const;
};
using nr_rs_type_e = enumerated<nr_rs_type_opts>;

// RSSI-PeriodicityAndOffset-r16 ::= CHOICE
struct rssi_periodicity_and_offset_r16_c {
  struct types_opts {
    enum options { sl10, sl20, sl40, sl80, sl160, sl320, s1640, /*...*/ nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  rssi_periodicity_and_offset_r16_c() = default;
  rssi_periodicity_and_offset_r16_c(const rssi_periodicity_and_offset_r16_c& other);
  rssi_periodicity_and_offset_r16_c& operator=(const rssi_periodicity_and_offset_r16_c& other);
  ~rssi_periodicity_and_offset_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& sl10()
  {
    assert_choice_type(types::sl10, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl20()
  {
    assert_choice_type(types::sl20, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl40()
  {
    assert_choice_type(types::sl40, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl80()
  {
    assert_choice_type(types::sl80, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& sl160()
  {
    assert_choice_type(types::sl160, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint16_t& sl320()
  {
    assert_choice_type(types::sl320, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& s1640()
  {
    assert_choice_type(types::s1640, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint8_t& sl10() const
  {
    assert_choice_type(types::sl10, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl20() const
  {
    assert_choice_type(types::sl20, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl40() const
  {
    assert_choice_type(types::sl40, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl80() const
  {
    assert_choice_type(types::sl80, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& sl160() const
  {
    assert_choice_type(types::sl160, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint16_t& sl320() const
  {
    assert_choice_type(types::sl320, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& s1640() const
  {
    assert_choice_type(types::s1640, type_, "RSSI-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint8_t&  set_sl10();
  uint8_t&  set_sl20();
  uint8_t&  set_sl40();
  uint8_t&  set_sl80();
  uint8_t&  set_sl160();
  uint16_t& set_sl320();
  uint16_t& set_s1640();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// SSB-PositionQCL-Cell-r17 ::= SEQUENCE
struct ssb_position_qcl_cell_r17_s {
  uint16_t                        pci_r17 = 0;
  ssb_position_qcl_relation_r17_e ssb_position_qcl_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CondTriggerConfig-r16 ::= SEQUENCE
struct cond_trigger_cfg_r16_s {
  struct cond_event_id_c_ {
    struct cond_event_a3_s_ {
      meas_trigger_quant_offset_c a3_offset;
      uint8_t                     hysteresis = 0;
      time_to_trigger_e           time_to_trigger;
    };
    struct cond_event_a5_s_ {
      meas_trigger_quant_c a5_thres1;
      meas_trigger_quant_c a5_thres2;
      uint8_t              hysteresis = 0;
      time_to_trigger_e    time_to_trigger;
    };
    struct cond_event_a4_r17_s_ {
      meas_trigger_quant_c a4_thres_r17;
      uint8_t              hysteresis_r17 = 0;
      time_to_trigger_e    time_to_trigger_r17;
    };
    struct cond_event_d1_r17_s_ {
      uint16_t          distance_thresh_from_ref1_r17 = 0;
      uint16_t          distance_thresh_from_ref2_r17 = 0;
      dyn_octstring     ref_location1_r17;
      dyn_octstring     ref_location2_r17;
      uint16_t          hysteresis_location_r17 = 0;
      time_to_trigger_e time_to_trigger_r17;
    };
    struct cond_event_t1_r17_s_ {
      uint64_t t1_thres_r17 = 0;
      uint16_t dur_r17      = 1;
    };
    struct types_opts {
      enum options {
        cond_event_a3,
        cond_event_a5,
        /*...*/ cond_event_a4_r17,
        cond_event_d1_r17,
        cond_event_t1_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 3>;

    // choice methods
    cond_event_id_c_() = default;
    cond_event_id_c_(const cond_event_id_c_& other);
    cond_event_id_c_& operator=(const cond_event_id_c_& other);
    ~cond_event_id_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cond_event_a3_s_& cond_event_a3()
    {
      assert_choice_type(types::cond_event_a3, type_, "condEventId");
      return c.get<cond_event_a3_s_>();
    }
    cond_event_a5_s_& cond_event_a5()
    {
      assert_choice_type(types::cond_event_a5, type_, "condEventId");
      return c.get<cond_event_a5_s_>();
    }
    cond_event_a4_r17_s_& cond_event_a4_r17()
    {
      assert_choice_type(types::cond_event_a4_r17, type_, "condEventId");
      return c.get<cond_event_a4_r17_s_>();
    }
    cond_event_d1_r17_s_& cond_event_d1_r17()
    {
      assert_choice_type(types::cond_event_d1_r17, type_, "condEventId");
      return c.get<cond_event_d1_r17_s_>();
    }
    cond_event_t1_r17_s_& cond_event_t1_r17()
    {
      assert_choice_type(types::cond_event_t1_r17, type_, "condEventId");
      return c.get<cond_event_t1_r17_s_>();
    }
    const cond_event_a3_s_& cond_event_a3() const
    {
      assert_choice_type(types::cond_event_a3, type_, "condEventId");
      return c.get<cond_event_a3_s_>();
    }
    const cond_event_a5_s_& cond_event_a5() const
    {
      assert_choice_type(types::cond_event_a5, type_, "condEventId");
      return c.get<cond_event_a5_s_>();
    }
    const cond_event_a4_r17_s_& cond_event_a4_r17() const
    {
      assert_choice_type(types::cond_event_a4_r17, type_, "condEventId");
      return c.get<cond_event_a4_r17_s_>();
    }
    const cond_event_d1_r17_s_& cond_event_d1_r17() const
    {
      assert_choice_type(types::cond_event_d1_r17, type_, "condEventId");
      return c.get<cond_event_d1_r17_s_>();
    }
    const cond_event_t1_r17_s_& cond_event_t1_r17() const
    {
      assert_choice_type(types::cond_event_t1_r17, type_, "condEventId");
      return c.get<cond_event_t1_r17_s_>();
    }
    cond_event_a3_s_&     set_cond_event_a3();
    cond_event_a5_s_&     set_cond_event_a5();
    cond_event_a4_r17_s_& set_cond_event_a4_r17();
    cond_event_d1_r17_s_& set_cond_event_d1_r17();
    cond_event_t1_r17_s_& set_cond_event_t1_r17();

  private:
    types type_;
    choice_buffer_t<cond_event_a3_s_,
                    cond_event_a4_r17_s_,
                    cond_event_a5_s_,
                    cond_event_d1_r17_s_,
                    cond_event_t1_r17_s_>
        c;

    void destroy_();
  };

  // member variables
  bool             ext = false;
  cond_event_id_c_ cond_event_id;
  nr_rs_type_e     rs_type_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-CellMobility ::= SEQUENCE
struct csi_rs_cell_mob_s {
  struct csi_rs_meas_bw_s_ {
    struct nrof_prbs_opts {
      enum options { size24, size48, size96, size192, size264, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using nrof_prbs_e_ = enumerated<nrof_prbs_opts>;

    // member variables
    nrof_prbs_e_ nrof_prbs;
    uint16_t     start_prb = 0;
  };
  struct density_opts {
    enum options { d1, d3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using density_e_             = enumerated<density_opts>;
  using csi_rs_res_list_mob_l_ = dyn_array<csi_rs_res_mob_s>;

  // member variables
  bool                   density_present = false;
  uint16_t               cell_id         = 0;
  csi_rs_meas_bw_s_      csi_rs_meas_bw;
  density_e_             density;
  csi_rs_res_list_mob_l_ csi_rs_res_list_mob;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ExcessDelay-DRB-IdentityInfo-r17 ::= SEQUENCE
struct excess_delay_drb_id_info_r17_s {
  using drb_id_list_l_ = bounded_array<uint8_t, 29>;
  struct delay_thres_opts {
    enum options {
      ms0dot25,
      ms0dot5,
      ms1,
      ms2,
      ms4,
      ms5,
      ms10,
      ms20,
      ms30,
      ms40,
      ms50,
      ms60,
      ms70,
      ms80,
      ms90,
      ms100,
      ms150,
      ms300,
      ms500,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using delay_thres_e_ = enumerated<delay_thres_opts>;

  // member variables
  drb_id_list_l_ drb_id_list;
  delay_thres_e_ delay_thres;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSSI-ResourceConfigCLI-r16 ::= SEQUENCE
struct rssi_res_cfg_cli_r16_s {
  bool                              ext                           = false;
  bool                              ref_serv_cell_idx_r16_present = false;
  uint8_t                           rssi_res_id_r16               = 0;
  subcarrier_spacing_e              rssi_scs_r16;
  uint16_t                          start_prb_r16      = 0;
  uint16_t                          nrof_prbs_r16      = 4;
  uint8_t                           start_position_r16 = 0;
  uint8_t                           nrof_symbols_r16   = 1;
  rssi_periodicity_and_offset_r16_c rssi_periodicity_and_offset_r16;
  uint8_t                           ref_serv_cell_idx_r16 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-ResourceConfigCLI-r16 ::= SEQUENCE
struct srs_res_cfg_cli_r16_s {
  bool                 ext                           = false;
  bool                 ref_serv_cell_idx_r16_present = false;
  srs_res_s            srs_res_r16;
  subcarrier_spacing_e srs_scs_r16;
  uint8_t              ref_serv_cell_idx_r16 = 0;
  uint8_t              ref_bwp_r16           = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-PositionQCL-CellList-r17 ::= SEQUENCE (SIZE (1..32)) OF SSB-PositionQCL-Cell-r17
using ssb_position_qcl_cell_list_r17_l = dyn_array<ssb_position_qcl_cell_r17_s>;

// SSB-PositionQCL-CellsToAddMod-r16 ::= SEQUENCE
struct ssb_position_qcl_cells_to_add_mod_r16_s {
  uint16_t                        pci_r16 = 0;
  ssb_position_qcl_relation_r16_e ssb_position_qcl_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BT-NameList-r16 ::= SEQUENCE (SIZE (1..4)) OF OCTET STRING (SIZE (1..248))
using bt_name_list_r16_l = bounded_array<bounded_octstring<1, 248>, 4>;

// CSI-RS-ResourceConfigMobility ::= SEQUENCE
struct csi_rs_res_cfg_mob_s {
  using csi_rs_cell_list_mob_l_ = dyn_array<csi_rs_cell_mob_s>;

  // member variables
  bool                    ext = false;
  subcarrier_spacing_e    subcarrier_spacing;
  csi_rs_cell_list_mob_l_ csi_rs_cell_list_mob;
  // ...
  // group 0
  bool    ref_serv_cell_idx_present = false;
  uint8_t ref_serv_cell_idx         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-List ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..1007)
using pci_list_l = bounded_array<uint16_t, 32>;

// Q-OffsetRangeList ::= SEQUENCE
struct q_offset_range_list_s {
  bool             rsrp_offset_ssb_present    = false;
  bool             rsrq_offset_ssb_present    = false;
  bool             sinr_offset_ssb_present    = false;
  bool             rsrp_offset_csi_rs_present = false;
  bool             rsrq_offset_csi_rs_present = false;
  bool             sinr_offset_csi_rs_present = false;
  q_offset_range_e rsrp_offset_ssb;
  q_offset_range_e rsrq_offset_ssb;
  q_offset_range_e sinr_offset_ssb;
  q_offset_range_e rsrp_offset_csi_rs;
  q_offset_range_e rsrq_offset_csi_rs;
  q_offset_range_e sinr_offset_csi_rs;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSSI-ResourceListConfigCLI-r16 ::= SEQUENCE (SIZE (1..64)) OF RSSI-ResourceConfigCLI-r16
using rssi_res_list_cfg_cli_r16_l = dyn_array<rssi_res_cfg_cli_r16_s>;

// Sensor-NameList-r16 ::= SEQUENCE
struct sensor_name_list_r16_s {
  bool meas_uncom_bar_pre_r16_present = false;
  bool meas_ue_speed_present          = false;
  bool meas_ue_orientation_present    = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-ResourceListConfigCLI-r16 ::= SEQUENCE (SIZE (1..32)) OF SRS-ResourceConfigCLI-r16
using srs_res_list_cfg_cli_r16_l = dyn_array<srs_res_cfg_cli_r16_s>;

// SSB-PositionQCL-CellsToAddModList-r16 ::= SEQUENCE (SIZE (1..32)) OF SSB-PositionQCL-CellsToAddMod-r16
using ssb_position_qcl_cells_to_add_mod_list_r16_l = dyn_array<ssb_position_qcl_cells_to_add_mod_r16_s>;

// UL-DelayValueConfig-r16 ::= SEQUENCE
struct ul_delay_value_cfg_r16_s {
  using delay_drb_list_r16_l_ = bounded_array<uint8_t, 29>;

  // member variables
  delay_drb_list_r16_l_ delay_drb_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-ExcessDelayConfig-r17 ::= SEQUENCE
struct ul_excess_delay_cfg_r17_s {
  using excess_delay_drb_list_r17_l_ = dyn_array<excess_delay_drb_id_info_r17_s>;

  // member variables
  excess_delay_drb_list_r17_l_ excess_delay_drb_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-NameList-r16 ::= SEQUENCE (SIZE (1..4)) OF OCTET STRING (SIZE (1..32))
using wlan_name_list_r16_l = bounded_array<bounded_octstring<1, 32>, 4>;

// CellsToAddModExt-v1710 ::= SEQUENCE
struct cells_to_add_mod_ext_v1710_s {
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
  bool                       ntn_polarization_dl_r17_present = false;
  bool                       ntn_polarization_ul_r17_present = false;
  ntn_polarization_dl_r17_e_ ntn_polarization_dl_r17;
  ntn_polarization_ul_r17_e_ ntn_polarization_ul_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToAddMod ::= SEQUENCE
struct cells_to_add_mod_s {
  uint16_t              pci = 0;
  q_offset_range_list_s cell_individual_offset;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellsToAddModUTRA-FDD-r16 ::= SEQUENCE
struct cells_to_add_mod_utra_fdd_r16_s {
  uint8_t  cell_idx_utra_fdd_r16 = 1;
  uint16_t pci_r16               = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportQuantityCLI-r16 ::= ENUMERATED
struct meas_report_quant_cli_r16_opts {
  enum options { srs_rsrp, cli_rssi, nulltype } value;

  const char* to_string() const;
};
using meas_report_quant_cli_r16_e = enumerated<meas_report_quant_cli_r16_opts>;

// MeasReportQuantity-r16 ::= SEQUENCE
struct meas_report_quant_r16_s {
  bool ext     = false;
  bool cbr_r16 = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportQuantity ::= SEQUENCE
struct meas_report_quant_s {
  bool rsrp = false;
  bool rsrq = false;
  bool sinr = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportQuantityUTRA-FDD-r16 ::= SEQUENCE
struct meas_report_quant_utra_fdd_r16_s {
  bool cpich_rs_cp = false;
  bool cpich_ec_n0 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasRSSI-ReportConfig-r16 ::= SEQUENCE
struct meas_rssi_report_cfg_r16_s {
  bool    ch_occupancy_thres_r16_present = false;
  uint8_t ch_occupancy_thres_r16         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasTriggerQuantityCLI-r16 ::= CHOICE
struct meas_trigger_quant_cli_r16_c {
  struct types_opts {
    enum options { srs_rsrp_r16, cli_rssi_r16, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  meas_trigger_quant_cli_r16_c() = default;
  meas_trigger_quant_cli_r16_c(const meas_trigger_quant_cli_r16_c& other);
  meas_trigger_quant_cli_r16_c& operator=(const meas_trigger_quant_cli_r16_c& other);
  ~meas_trigger_quant_cli_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& srs_rsrp_r16()
  {
    assert_choice_type(types::srs_rsrp_r16, type_, "MeasTriggerQuantityCLI-r16");
    return c.get<uint8_t>();
  }
  uint8_t& cli_rssi_r16()
  {
    assert_choice_type(types::cli_rssi_r16, type_, "MeasTriggerQuantityCLI-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& srs_rsrp_r16() const
  {
    assert_choice_type(types::srs_rsrp_r16, type_, "MeasTriggerQuantityCLI-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& cli_rssi_r16() const
  {
    assert_choice_type(types::cli_rssi_r16, type_, "MeasTriggerQuantityCLI-r16");
    return c.get<uint8_t>();
  }
  uint8_t& set_srs_rsrp_r16();
  uint8_t& set_cli_rssi_r16();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MeasTriggerQuantityEUTRA ::= CHOICE
struct meas_trigger_quant_eutra_c {
  struct types_opts {
    enum options { rsrp, rsrq, sinr, nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  meas_trigger_quant_eutra_c() = default;
  meas_trigger_quant_eutra_c(const meas_trigger_quant_eutra_c& other);
  meas_trigger_quant_eutra_c& operator=(const meas_trigger_quant_eutra_c& other);
  ~meas_trigger_quant_eutra_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& rsrp()
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& rsrq()
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& sinr()
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrp() const
  {
    assert_choice_type(types::rsrp, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& rsrq() const
  {
    assert_choice_type(types::rsrq, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& sinr() const
  {
    assert_choice_type(types::sinr, type_, "MeasTriggerQuantityEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& set_rsrp();
  uint8_t& set_rsrq();
  uint8_t& set_sinr();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// MeasTriggerQuantityUTRA-FDD-r16 ::= CHOICE
struct meas_trigger_quant_utra_fdd_r16_c {
  struct types_opts {
    enum options { utra_fdd_rs_cp_r16, utra_fdd_ec_n0_r16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  meas_trigger_quant_utra_fdd_r16_c() = default;
  meas_trigger_quant_utra_fdd_r16_c(const meas_trigger_quant_utra_fdd_r16_c& other);
  meas_trigger_quant_utra_fdd_r16_c& operator=(const meas_trigger_quant_utra_fdd_r16_c& other);
  ~meas_trigger_quant_utra_fdd_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int8_t& utra_fdd_rs_cp_r16()
  {
    assert_choice_type(types::utra_fdd_rs_cp_r16, type_, "MeasTriggerQuantityUTRA-FDD-r16");
    return c.get<int8_t>();
  }
  uint8_t& utra_fdd_ec_n0_r16()
  {
    assert_choice_type(types::utra_fdd_ec_n0_r16, type_, "MeasTriggerQuantityUTRA-FDD-r16");
    return c.get<uint8_t>();
  }
  const int8_t& utra_fdd_rs_cp_r16() const
  {
    assert_choice_type(types::utra_fdd_rs_cp_r16, type_, "MeasTriggerQuantityUTRA-FDD-r16");
    return c.get<int8_t>();
  }
  const uint8_t& utra_fdd_ec_n0_r16() const
  {
    assert_choice_type(types::utra_fdd_ec_n0_r16, type_, "MeasTriggerQuantityUTRA-FDD-r16");
    return c.get<uint8_t>();
  }
  int8_t&  set_utra_fdd_rs_cp_r16();
  uint8_t& set_utra_fdd_ec_n0_r16();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// RMTC-Config-r16 ::= SEQUENCE
struct rmtc_cfg_r16_s {
  struct rmtc_periodicity_r16_opts {
    enum options { ms40, ms80, ms160, ms320, ms640, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using rmtc_periodicity_r16_e_ = enumerated<rmtc_periodicity_r16_opts>;
  struct meas_dur_symbols_r16_opts {
    enum options { sym1, sym14or12, sym28or24, sym42or36, sym70or60, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using meas_dur_symbols_r16_e_ = enumerated<meas_dur_symbols_r16_opts>;
  struct ref_scs_cp_r16_opts {
    enum options { khz15, khz30, khz60_n_cp, khz60_e_cp, nulltype } value;

    const char* to_string() const;
  };
  using ref_scs_cp_r16_e_ = enumerated<ref_scs_cp_r16_opts>;
  struct rmtc_bw_r17_opts {
    enum options { mhz100, mhz400, mhz800, mhz1600, mhz2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using rmtc_bw_r17_e_ = enumerated<rmtc_bw_r17_opts>;
  struct meas_dur_symbols_v1700_opts {
    enum options { sym140, sym560, sym1120, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using meas_dur_symbols_v1700_e_ = enumerated<meas_dur_symbols_v1700_opts>;
  struct ref_scs_cp_v1700_opts {
    enum options { khz120, khz480, khz960, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using ref_scs_cp_v1700_e_ = enumerated<ref_scs_cp_v1700_opts>;
  struct tci_state_info_r17_s_ {
    bool    ref_serv_cell_id_r17_present = false;
    uint8_t tci_state_id_r17             = 0;
    uint8_t ref_serv_cell_id_r17         = 0;
  };

  // member variables
  bool                    ext                        = false;
  bool                    rmtc_sf_offset_r16_present = false;
  rmtc_periodicity_r16_e_ rmtc_periodicity_r16;
  uint16_t                rmtc_sf_offset_r16 = 0;
  meas_dur_symbols_r16_e_ meas_dur_symbols_r16;
  uint32_t                rmtc_freq_r16 = 0;
  ref_scs_cp_r16_e_       ref_scs_cp_r16;
  // ...
  // group 0
  bool                            rmtc_bw_r17_present            = false;
  bool                            meas_dur_symbols_v1700_present = false;
  bool                            ref_scs_cp_v1700_present       = false;
  rmtc_bw_r17_e_                  rmtc_bw_r17;
  meas_dur_symbols_v1700_e_       meas_dur_symbols_v1700;
  ref_scs_cp_v1700_e_             ref_scs_cp_v1700;
  copy_ptr<tci_state_info_r17_s_> tci_state_info_r17;
  // group 1
  bool    ref_bwp_id_r17_present = false;
  uint8_t ref_bwp_id_r17         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RxTxReportInterval-r17 ::= ENUMERATED
struct rx_tx_report_interv_r17_opts {
  enum options {
    ms80,
    ms120,
    ms160,
    ms240,
    ms320,
    ms480,
    ms640,
    ms1024,
    ms1280,
    ms2048,
    ms2560,
    ms5120,
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
using rx_tx_report_interv_r17_e = enumerated<rx_tx_report_interv_r17_opts>;

// SSB-ConfigMobility ::= SEQUENCE
struct ssb_cfg_mob_s {
  bool                              ext                    = false;
  bool                              ssb_to_measure_present = false;
  bool                              ss_rssi_meas_present   = false;
  setup_release_c<ssb_to_measure_c> ssb_to_measure;
  bool                              derive_ssb_idx_from_cell = false;
  ss_rssi_meas_s                    ss_rssi_meas;
  // ...
  // group 0
  bool                                                   ssb_position_qcl_common_r16_present = false;
  ssb_position_qcl_relation_r16_e                        ssb_position_qcl_common_r16;
  copy_ptr<ssb_position_qcl_cells_to_add_mod_list_r16_l> ssb_position_qcl_cells_to_add_mod_list_r16;
  copy_ptr<pci_list_l>                                   ssb_position_qcl_cells_to_rem_list_r16;
  // group 1
  bool                            derive_ssb_idx_from_cell_inter_r17_present = false;
  bool                            ssb_position_qcl_common_r17_present        = false;
  uint8_t                         derive_ssb_idx_from_cell_inter_r17         = 0;
  ssb_position_qcl_relation_r17_e ssb_position_qcl_common_r17;
  copy_ptr<setup_release_c<dyn_seq_of<ssb_position_qcl_cell_r17_s, 1, 32>>> ssb_position_qcl_cells_r17;
  // group 2
  copy_ptr<pci_list_l> cca_cells_to_add_mod_list_r17;
  copy_ptr<pci_list_l> cca_cells_to_rem_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC3-r16 ::= SEQUENCE
struct ssb_mtc3_r16_s {
  struct periodicity_and_offset_r16_c_ {
    struct types_opts {
      enum options {
        sf5_r16,
        sf10_r16,
        sf20_r16,
        sf40_r16,
        sf80_r16,
        sf160_r16,
        sf320_r16,
        sf640_r16,
        sf1280_r16,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    periodicity_and_offset_r16_c_() = default;
    periodicity_and_offset_r16_c_(const periodicity_and_offset_r16_c_& other);
    periodicity_and_offset_r16_c_& operator=(const periodicity_and_offset_r16_c_& other);
    ~periodicity_and_offset_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sf5_r16()
    {
      assert_choice_type(types::sf5_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf10_r16()
    {
      assert_choice_type(types::sf10_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf20_r16()
    {
      assert_choice_type(types::sf20_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf40_r16()
    {
      assert_choice_type(types::sf40_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf80_r16()
    {
      assert_choice_type(types::sf80_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint8_t& sf160_r16()
    {
      assert_choice_type(types::sf160_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    uint16_t& sf320_r16()
    {
      assert_choice_type(types::sf320_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    uint16_t& sf640_r16()
    {
      assert_choice_type(types::sf640_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    uint16_t& sf1280_r16()
    {
      assert_choice_type(types::sf1280_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    const uint8_t& sf5_r16() const
    {
      assert_choice_type(types::sf5_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf10_r16() const
    {
      assert_choice_type(types::sf10_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf20_r16() const
    {
      assert_choice_type(types::sf20_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf40_r16() const
    {
      assert_choice_type(types::sf40_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf80_r16() const
    {
      assert_choice_type(types::sf80_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint8_t& sf160_r16() const
    {
      assert_choice_type(types::sf160_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint8_t>();
    }
    const uint16_t& sf320_r16() const
    {
      assert_choice_type(types::sf320_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    const uint16_t& sf640_r16() const
    {
      assert_choice_type(types::sf640_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    const uint16_t& sf1280_r16() const
    {
      assert_choice_type(types::sf1280_r16, type_, "periodicityAndOffset-r16");
      return c.get<uint16_t>();
    }
    uint8_t&  set_sf5_r16();
    uint8_t&  set_sf10_r16();
    uint8_t&  set_sf20_r16();
    uint8_t&  set_sf40_r16();
    uint8_t&  set_sf80_r16();
    uint8_t&  set_sf160_r16();
    uint16_t& set_sf320_r16();
    uint16_t& set_sf640_r16();
    uint16_t& set_sf1280_r16();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct dur_r16_opts {
    enum options { sf1, sf2, sf3, sf4, sf5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using dur_r16_e_      = enumerated<dur_r16_opts>;
  using pci_list_r16_l_ = dyn_array<uint16_t>;

  // member variables
  bool                              ssb_to_measure_r16_present = false;
  periodicity_and_offset_r16_c_     periodicity_and_offset_r16;
  dur_r16_e_                        dur_r16;
  pci_list_r16_l_                   pci_list_r16;
  setup_release_c<ssb_to_measure_c> ssb_to_measure_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// T312-r16 ::= ENUMERATED
struct t312_r16_opts {
  enum options { ms0, ms50, ms100, ms200, ms300, ms400, ms500, ms1000, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
using t312_r16_e = enumerated<t312_r16_opts>;

// CellsToAddModListExt-v1710 ::= SEQUENCE (SIZE (1..32)) OF CellsToAddModExt-v1710
using cells_to_add_mod_list_ext_v1710_l = dyn_array<cells_to_add_mod_ext_v1710_s>;

// CellsToAddModList ::= SEQUENCE (SIZE (1..32)) OF CellsToAddMod
using cells_to_add_mod_list_l = dyn_array<cells_to_add_mod_s>;

// CellsToAddModListUTRA-FDD-r16 ::= SEQUENCE (SIZE (1..32)) OF CellsToAddModUTRA-FDD-r16
using cells_to_add_mod_list_utra_fdd_r16_l = dyn_array<cells_to_add_mod_utra_fdd_r16_s>;

// CLI-EventTriggerConfig-r16 ::= SEQUENCE
struct cli_event_trigger_cfg_r16_s {
  struct event_id_r16_c_ {
    struct event_i1_r16_s_ {
      meas_trigger_quant_cli_r16_c i1_thres_r16;
      bool                         report_on_leave_r16 = false;
      uint8_t                      hysteresis_r16      = 0;
      time_to_trigger_e            time_to_trigger_r16;
    };
    struct types_opts {
      enum options { event_i1_r16, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    types       type() const { return types::event_i1_r16; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_i1_r16_s_&       event_i1_r16() { return c; }
    const event_i1_r16_s_& event_i1_r16() const { return c; }

  private:
    event_i1_r16_s_ c;
  };
  struct report_amount_r16_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r16_e_ = enumerated<report_amount_r16_opts>;

  // member variables
  bool                 ext = false;
  event_id_r16_c_      event_id_r16;
  report_interv_e      report_interv_r16;
  report_amount_r16_e_ report_amount_r16;
  uint8_t              max_report_cli_r16 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CLI-PeriodicalReportConfig-r16 ::= SEQUENCE
struct cli_periodical_report_cfg_r16_s {
  struct report_amount_r16_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r16_e_ = enumerated<report_amount_r16_opts>;

  // member variables
  bool                        ext = false;
  report_interv_e             report_interv_r16;
  report_amount_r16_e_        report_amount_r16;
  meas_report_quant_cli_r16_e report_quant_cli_r16;
  uint8_t                     max_report_cli_r16 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CLI-ResourceConfig-r16 ::= SEQUENCE
struct cli_res_cfg_r16_s {
  bool                                                       srs_res_cfg_r16_present  = false;
  bool                                                       rssi_res_cfg_r16_present = false;
  setup_release_c<dyn_seq_of<srs_res_cfg_cli_r16_s, 1, 32>>  srs_res_cfg_r16;
  setup_release_c<dyn_seq_of<rssi_res_cfg_cli_r16_s, 1, 64>> rssi_res_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-CellIndexList ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..32)
using eutra_cell_idx_list_l = bounded_array<uint8_t, 32>;

// EUTRA-Cell ::= SEQUENCE
struct eutra_cell_s {
  uint8_t                cell_idx_eutra = 1;
  uint16_t               pci            = 0;
  eutra_q_offset_range_e cell_individual_offset;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-ExcludedCell ::= SEQUENCE
struct eutra_excluded_cell_s {
  uint8_t           cell_idx_eutra = 1;
  eutra_pci_range_s pci_range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTriggerConfigInterRAT ::= SEQUENCE
struct event_trigger_cfg_inter_rat_s {
  struct event_id_c_ {
    struct event_b1_s_ {
      bool                       ext = false;
      meas_trigger_quant_eutra_c b1_thres_eutra;
      bool                       report_on_leave = false;
      uint8_t                    hysteresis      = 0;
      time_to_trigger_e          time_to_trigger;
      // ...
    };
    struct event_b2_s_ {
      bool                       ext = false;
      meas_trigger_quant_c       b2_thres1;
      meas_trigger_quant_eutra_c b2_thres2_eutra;
      bool                       report_on_leave = false;
      uint8_t                    hysteresis      = 0;
      time_to_trigger_e          time_to_trigger;
      // ...
    };
    struct event_b1_utra_fdd_r16_s_ {
      bool                              ext = false;
      meas_trigger_quant_utra_fdd_r16_c b1_thres_utra_fdd_r16;
      bool                              report_on_leave_r16 = false;
      uint8_t                           hysteresis_r16      = 0;
      time_to_trigger_e                 time_to_trigger_r16;
      // ...
    };
    struct event_b2_utra_fdd_r16_s_ {
      bool                              ext = false;
      meas_trigger_quant_c              b2_thres1_r16;
      meas_trigger_quant_utra_fdd_r16_c b2_thres2_utra_fdd_r16;
      bool                              report_on_leave_r16 = false;
      uint8_t                           hysteresis_r16      = 0;
      time_to_trigger_e                 time_to_trigger_r16;
      // ...
    };
    struct event_y1_relay_r17_s_ {
      bool                        ext = false;
      meas_trigger_quant_c        y1_thres1_r17;
      sl_meas_trigger_quant_r16_c y1_thres2_relay_r17;
      bool                        report_on_leave_r17 = false;
      uint8_t                     hysteresis_r17      = 0;
      time_to_trigger_e           time_to_trigger_r17;
      // ...
    };
    struct event_y2_relay_r17_s_ {
      bool                        ext = false;
      sl_meas_trigger_quant_r16_c y2_thres_relay_r17;
      bool                        report_on_leave_r17 = false;
      uint8_t                     hysteresis_r17      = 0;
      time_to_trigger_e           time_to_trigger_r17;
      // ...
    };
    struct types_opts {
      enum options {
        event_b1,
        event_b2,
        // ...
        event_b1_utra_fdd_r16,
        event_b2_utra_fdd_r16,
        event_y1_relay_r17,
        event_y2_relay_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 4>;

    // choice methods
    event_id_c_() = default;
    event_id_c_(const event_id_c_& other);
    event_id_c_& operator=(const event_id_c_& other);
    ~event_id_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_b1_s_& event_b1()
    {
      assert_choice_type(types::event_b1, type_, "eventId");
      return c.get<event_b1_s_>();
    }
    event_b2_s_& event_b2()
    {
      assert_choice_type(types::event_b2, type_, "eventId");
      return c.get<event_b2_s_>();
    }
    event_b1_utra_fdd_r16_s_& event_b1_utra_fdd_r16()
    {
      assert_choice_type(types::event_b1_utra_fdd_r16, type_, "eventId");
      return c.get<event_b1_utra_fdd_r16_s_>();
    }
    event_b2_utra_fdd_r16_s_& event_b2_utra_fdd_r16()
    {
      assert_choice_type(types::event_b2_utra_fdd_r16, type_, "eventId");
      return c.get<event_b2_utra_fdd_r16_s_>();
    }
    event_y1_relay_r17_s_& event_y1_relay_r17()
    {
      assert_choice_type(types::event_y1_relay_r17, type_, "eventId");
      return c.get<event_y1_relay_r17_s_>();
    }
    event_y2_relay_r17_s_& event_y2_relay_r17()
    {
      assert_choice_type(types::event_y2_relay_r17, type_, "eventId");
      return c.get<event_y2_relay_r17_s_>();
    }
    const event_b1_s_& event_b1() const
    {
      assert_choice_type(types::event_b1, type_, "eventId");
      return c.get<event_b1_s_>();
    }
    const event_b2_s_& event_b2() const
    {
      assert_choice_type(types::event_b2, type_, "eventId");
      return c.get<event_b2_s_>();
    }
    const event_b1_utra_fdd_r16_s_& event_b1_utra_fdd_r16() const
    {
      assert_choice_type(types::event_b1_utra_fdd_r16, type_, "eventId");
      return c.get<event_b1_utra_fdd_r16_s_>();
    }
    const event_b2_utra_fdd_r16_s_& event_b2_utra_fdd_r16() const
    {
      assert_choice_type(types::event_b2_utra_fdd_r16, type_, "eventId");
      return c.get<event_b2_utra_fdd_r16_s_>();
    }
    const event_y1_relay_r17_s_& event_y1_relay_r17() const
    {
      assert_choice_type(types::event_y1_relay_r17, type_, "eventId");
      return c.get<event_y1_relay_r17_s_>();
    }
    const event_y2_relay_r17_s_& event_y2_relay_r17() const
    {
      assert_choice_type(types::event_y2_relay_r17, type_, "eventId");
      return c.get<event_y2_relay_r17_s_>();
    }
    event_b1_s_&              set_event_b1();
    event_b2_s_&              set_event_b2();
    event_b1_utra_fdd_r16_s_& set_event_b1_utra_fdd_r16();
    event_b2_utra_fdd_r16_s_& set_event_b2_utra_fdd_r16();
    event_y1_relay_r17_s_&    set_event_y1_relay_r17();
    event_y2_relay_r17_s_&    set_event_y2_relay_r17();

  private:
    types type_;
    choice_buffer_t<event_b1_s_,
                    event_b1_utra_fdd_r16_s_,
                    event_b2_s_,
                    event_b2_utra_fdd_r16_s_,
                    event_y1_relay_r17_s_,
                    event_y2_relay_r17_s_>
        c;

    void destroy_();
  };
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_e_ = enumerated<report_amount_opts>;

  // member variables
  bool                ext = false;
  event_id_c_         event_id;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant;
  uint8_t             max_report_cells = 1;
  // ...
  // group 0
  copy_ptr<meas_report_quant_utra_fdd_r16_s> report_quant_utra_fdd_r16;
  // group 1
  bool include_common_location_info_r16_present = false;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>>> include_bt_meas_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>>  include_wlan_meas_r16;
  copy_ptr<setup_release_c<sensor_name_list_r16_s>>                      include_sensor_meas_r16;
  // group 2
  copy_ptr<sl_meas_report_quant_r16_c> report_quant_relay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTriggerConfigNR-SL-r16 ::= SEQUENCE
struct event_trigger_cfg_nr_sl_r16_s {
  struct event_id_r16_c_ {
    struct event_c1_s_ {
      uint8_t           c1_thres_r16   = 0;
      uint8_t           hysteresis_r16 = 0;
      time_to_trigger_e time_to_trigger_r16;
    };
    struct event_c2_r16_s_ {
      uint8_t           c2_thres_r16   = 0;
      uint8_t           hysteresis_r16 = 0;
      time_to_trigger_e time_to_trigger_r16;
    };
    struct types_opts {
      enum options { event_c1, event_c2_r16, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    event_id_r16_c_() = default;
    event_id_r16_c_(const event_id_r16_c_& other);
    event_id_r16_c_& operator=(const event_id_r16_c_& other);
    ~event_id_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_c1_s_& event_c1()
    {
      assert_choice_type(types::event_c1, type_, "eventId-r16");
      return c.get<event_c1_s_>();
    }
    event_c2_r16_s_& event_c2_r16()
    {
      assert_choice_type(types::event_c2_r16, type_, "eventId-r16");
      return c.get<event_c2_r16_s_>();
    }
    const event_c1_s_& event_c1() const
    {
      assert_choice_type(types::event_c1, type_, "eventId-r16");
      return c.get<event_c1_s_>();
    }
    const event_c2_r16_s_& event_c2_r16() const
    {
      assert_choice_type(types::event_c2_r16, type_, "eventId-r16");
      return c.get<event_c2_r16_s_>();
    }
    event_c1_s_&     set_event_c1();
    event_c2_r16_s_& set_event_c2_r16();

  private:
    types                                         type_;
    choice_buffer_t<event_c1_s_, event_c2_r16_s_> c;

    void destroy_();
  };
  struct report_amount_r16_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r16_e_ = enumerated<report_amount_r16_opts>;

  // member variables
  bool                    ext = false;
  event_id_r16_c_         event_id_r16;
  report_interv_e         report_interv_r16;
  report_amount_r16_e_    report_amount_r16;
  meas_report_quant_r16_s report_quant_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventTriggerConfig ::= SEQUENCE
struct event_trigger_cfg_s {
  struct event_id_c_ {
    struct event_a1_s_ {
      meas_trigger_quant_c a1_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
    };
    struct event_a2_s_ {
      meas_trigger_quant_c a2_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
    };
    struct event_a3_s_ {
      meas_trigger_quant_offset_c a3_offset;
      bool                        report_on_leave = false;
      uint8_t                     hysteresis      = 0;
      time_to_trigger_e           time_to_trigger;
      bool                        use_allowed_cell_list = false;
    };
    struct event_a4_s_ {
      meas_trigger_quant_c a4_thres;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
      bool                 use_allowed_cell_list = false;
    };
    struct event_a5_s_ {
      meas_trigger_quant_c a5_thres1;
      meas_trigger_quant_c a5_thres2;
      bool                 report_on_leave = false;
      uint8_t              hysteresis      = 0;
      time_to_trigger_e    time_to_trigger;
      bool                 use_allowed_cell_list = false;
    };
    struct event_a6_s_ {
      meas_trigger_quant_offset_c a6_offset;
      bool                        report_on_leave = false;
      uint8_t                     hysteresis      = 0;
      time_to_trigger_e           time_to_trigger;
      bool                        use_allowed_cell_list = false;
    };
    struct event_x1_r17_s_ {
      sl_meas_trigger_quant_r16_c x1_thres1_relay_r17;
      meas_trigger_quant_c        x1_thres2_r17;
      bool                        report_on_leave_r17 = false;
      uint8_t                     hysteresis_r17      = 0;
      time_to_trigger_e           time_to_trigger_r17;
      bool                        use_allowed_cell_list_r17 = false;
    };
    struct event_x2_r17_s_ {
      sl_meas_trigger_quant_r16_c x2_thres_relay_r17;
      bool                        report_on_leave_r17 = false;
      uint8_t                     hysteresis_r17      = 0;
      time_to_trigger_e           time_to_trigger_r17;
    };
    struct event_d1_r17_s_ {
      uint16_t          distance_thresh_from_ref1_r17 = 1;
      uint16_t          distance_thresh_from_ref2_r17 = 1;
      dyn_octstring     ref_location1_r17;
      dyn_octstring     ref_location2_r17;
      bool              report_on_leave_r17     = false;
      uint16_t          hysteresis_location_r17 = 0;
      time_to_trigger_e time_to_trigger_r17;
    };
    struct types_opts {
      enum options {
        event_a1,
        event_a2,
        event_a3,
        event_a4,
        event_a5,
        event_a6,
        // ...
        event_x1_r17,
        event_x2_r17,
        event_d1_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 3>;

    // choice methods
    event_id_c_() = default;
    event_id_c_(const event_id_c_& other);
    event_id_c_& operator=(const event_id_c_& other);
    ~event_id_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    event_a1_s_& event_a1()
    {
      assert_choice_type(types::event_a1, type_, "eventId");
      return c.get<event_a1_s_>();
    }
    event_a2_s_& event_a2()
    {
      assert_choice_type(types::event_a2, type_, "eventId");
      return c.get<event_a2_s_>();
    }
    event_a3_s_& event_a3()
    {
      assert_choice_type(types::event_a3, type_, "eventId");
      return c.get<event_a3_s_>();
    }
    event_a4_s_& event_a4()
    {
      assert_choice_type(types::event_a4, type_, "eventId");
      return c.get<event_a4_s_>();
    }
    event_a5_s_& event_a5()
    {
      assert_choice_type(types::event_a5, type_, "eventId");
      return c.get<event_a5_s_>();
    }
    event_a6_s_& event_a6()
    {
      assert_choice_type(types::event_a6, type_, "eventId");
      return c.get<event_a6_s_>();
    }
    event_x1_r17_s_& event_x1_r17()
    {
      assert_choice_type(types::event_x1_r17, type_, "eventId");
      return c.get<event_x1_r17_s_>();
    }
    event_x2_r17_s_& event_x2_r17()
    {
      assert_choice_type(types::event_x2_r17, type_, "eventId");
      return c.get<event_x2_r17_s_>();
    }
    event_d1_r17_s_& event_d1_r17()
    {
      assert_choice_type(types::event_d1_r17, type_, "eventId");
      return c.get<event_d1_r17_s_>();
    }
    const event_a1_s_& event_a1() const
    {
      assert_choice_type(types::event_a1, type_, "eventId");
      return c.get<event_a1_s_>();
    }
    const event_a2_s_& event_a2() const
    {
      assert_choice_type(types::event_a2, type_, "eventId");
      return c.get<event_a2_s_>();
    }
    const event_a3_s_& event_a3() const
    {
      assert_choice_type(types::event_a3, type_, "eventId");
      return c.get<event_a3_s_>();
    }
    const event_a4_s_& event_a4() const
    {
      assert_choice_type(types::event_a4, type_, "eventId");
      return c.get<event_a4_s_>();
    }
    const event_a5_s_& event_a5() const
    {
      assert_choice_type(types::event_a5, type_, "eventId");
      return c.get<event_a5_s_>();
    }
    const event_a6_s_& event_a6() const
    {
      assert_choice_type(types::event_a6, type_, "eventId");
      return c.get<event_a6_s_>();
    }
    const event_x1_r17_s_& event_x1_r17() const
    {
      assert_choice_type(types::event_x1_r17, type_, "eventId");
      return c.get<event_x1_r17_s_>();
    }
    const event_x2_r17_s_& event_x2_r17() const
    {
      assert_choice_type(types::event_x2_r17, type_, "eventId");
      return c.get<event_x2_r17_s_>();
    }
    const event_d1_r17_s_& event_d1_r17() const
    {
      assert_choice_type(types::event_d1_r17, type_, "eventId");
      return c.get<event_d1_r17_s_>();
    }
    event_a1_s_&     set_event_a1();
    event_a2_s_&     set_event_a2();
    event_a3_s_&     set_event_a3();
    event_a4_s_&     set_event_a4();
    event_a5_s_&     set_event_a5();
    event_a6_s_&     set_event_a6();
    event_x1_r17_s_& set_event_x1_r17();
    event_x2_r17_s_& set_event_x2_r17();
    event_d1_r17_s_& set_event_d1_r17();

  private:
    types type_;
    choice_buffer_t<event_a1_s_,
                    event_a2_s_,
                    event_a3_s_,
                    event_a4_s_,
                    event_a5_s_,
                    event_a6_s_,
                    event_d1_r17_s_,
                    event_x1_r17_s_,
                    event_x2_r17_s_>
        c;

    void destroy_();
  };
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_e_ = enumerated<report_amount_opts>;

  // member variables
  bool                ext                                 = false;
  bool                report_quant_rs_idxes_present       = false;
  bool                max_nrof_rs_idxes_to_report_present = false;
  bool                report_add_neigh_meas_present       = false;
  event_id_c_         event_id;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant_cell;
  uint8_t             max_report_cells = 1;
  meas_report_quant_s report_quant_rs_idxes;
  uint8_t             max_nrof_rs_idxes_to_report = 1;
  bool                include_beam_meass          = false;
  // ...
  // group 0
  bool                                 use_t312_r16_present                     = false;
  bool                                 include_common_location_info_r16_present = false;
  copy_ptr<meas_rssi_report_cfg_r16_s> meas_rssi_report_cfg_r16;
  bool                                 use_t312_r16 = false;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>>> include_bt_meas_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>>  include_wlan_meas_r16;
  copy_ptr<setup_release_c<sensor_name_list_r16_s>>                      include_sensor_meas_r16;
  // group 1
  bool                                 coarse_location_request_r17_present = false;
  copy_ptr<sl_meas_report_quant_r16_c> report_quant_relay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FilterConfig ::= SEQUENCE
struct filt_cfg_s {
  bool        filt_coef_rsrp_present    = false;
  bool        filt_coef_rsrq_present    = false;
  bool        filt_coef_rs_sinr_present = false;
  filt_coef_e filt_coef_rsrp;
  filt_coef_e filt_coef_rsrq;
  filt_coef_e filt_coef_rs_sinr;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-RangeElement ::= SEQUENCE
struct pci_range_elem_s {
  uint8_t     pci_range_idx = 1;
  pci_range_s pci_range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-RangeIndexList ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using pci_range_idx_list_l = bounded_array<uint8_t, 8>;

// PeriodicalReportConfigInterRAT ::= SEQUENCE
struct periodical_report_cfg_inter_rat_s {
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_e_ = enumerated<report_amount_opts>;

  // member variables
  bool                ext = false;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant;
  uint8_t             max_report_cells = 1;
  // ...
  // group 0
  copy_ptr<meas_report_quant_utra_fdd_r16_s> report_quant_utra_fdd_r16;
  // group 1
  bool include_common_location_info_r16_present = false;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>>> include_bt_meas_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>>  include_wlan_meas_r16;
  copy_ptr<setup_release_c<sensor_name_list_r16_s>>                      include_sensor_meas_r16;
  // group 2
  copy_ptr<sl_meas_report_quant_r16_c> report_quant_relay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PeriodicalReportConfigNR-SL-r16 ::= SEQUENCE
struct periodical_report_cfg_nr_sl_r16_s {
  struct report_amount_r16_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r16_e_ = enumerated<report_amount_r16_opts>;

  // member variables
  bool                    ext = false;
  report_interv_e         report_interv_r16;
  report_amount_r16_e_    report_amount_r16;
  meas_report_quant_r16_s report_quant_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PeriodicalReportConfig ::= SEQUENCE
struct periodical_report_cfg_s {
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_e_ = enumerated<report_amount_opts>;

  // member variables
  bool                ext                                 = false;
  bool                report_quant_rs_idxes_present       = false;
  bool                max_nrof_rs_idxes_to_report_present = false;
  nr_rs_type_e        rs_type;
  report_interv_e     report_interv;
  report_amount_e_    report_amount;
  meas_report_quant_s report_quant_cell;
  uint8_t             max_report_cells = 1;
  meas_report_quant_s report_quant_rs_idxes;
  uint8_t             max_nrof_rs_idxes_to_report = 1;
  bool                include_beam_meass          = false;
  bool                use_allowed_cell_list       = false;
  // ...
  // group 0
  bool                                 include_common_location_info_r16_present = false;
  bool                                 report_add_neigh_meas_r16_present        = false;
  copy_ptr<meas_rssi_report_cfg_r16_s> meas_rssi_report_cfg_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 248>, 1, 4>>> include_bt_meas_r16;
  copy_ptr<setup_release_c<dyn_seq_of<bounded_octstring<1, 32>, 1, 4>>>  include_wlan_meas_r16;
  copy_ptr<setup_release_c<sensor_name_list_r16_s>>                      include_sensor_meas_r16;
  copy_ptr<setup_release_c<ul_delay_value_cfg_r16_s>>                    ul_delay_value_cfg_r16;
  // group 1
  bool                                                 coarse_location_request_r17_present = false;
  copy_ptr<setup_release_c<ul_excess_delay_cfg_r17_s>> ul_excess_delay_cfg_r17;
  copy_ptr<sl_meas_report_quant_r16_c>                 report_quant_relay_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReferenceSignalConfig ::= SEQUENCE
struct ref_sig_cfg_s {
  bool                                  ssb_cfg_mob_present        = false;
  bool                                  csi_rs_res_cfg_mob_present = false;
  ssb_cfg_mob_s                         ssb_cfg_mob;
  setup_release_c<csi_rs_res_cfg_mob_s> csi_rs_res_cfg_mob;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportCGI-EUTRA ::= SEQUENCE
struct report_cgi_eutra_s {
  bool     ext                          = false;
  uint16_t cell_for_which_to_report_cgi = 0;
  // ...
  // group 0
  bool use_autonomous_gaps_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportCGI ::= SEQUENCE
struct report_cgi_s {
  bool     ext                          = false;
  uint16_t cell_for_which_to_report_cgi = 0;
  // ...
  // group 0
  bool use_autonomous_gaps_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportSFTD-EUTRA ::= SEQUENCE
struct report_sftd_eutra_s {
  bool ext              = false;
  bool report_sftd_meas = false;
  bool report_rsrp      = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportSFTD-NR ::= SEQUENCE
struct report_sftd_nr_s {
  using cells_for_which_to_report_sftd_l_ = bounded_array<uint16_t, 3>;

  // member variables
  bool ext              = false;
  bool report_sftd_meas = false;
  bool report_rsrp      = false;
  // ...
  // group 0
  bool                                        report_sftd_neigh_meas_present = false;
  bool                                        drx_sftd_neigh_meas_present    = false;
  copy_ptr<cells_for_which_to_report_sftd_l_> cells_for_which_to_report_sftd;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RxTxPeriodical-r17 ::= SEQUENCE
struct rx_tx_periodical_r17_s {
  struct report_amount_r17_opts {
    enum options { r1, infinity, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  using report_amount_r17_e_ = enumerated<report_amount_r17_opts>;

  // member variables
  bool                      ext                             = false;
  bool                      rx_tx_report_interv_r17_present = false;
  rx_tx_report_interv_r17_e rx_tx_report_interv_r17;
  report_amount_r17_e_      report_amount_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC2 ::= SEQUENCE
struct ssb_mtc2_s {
  using pci_list_l_ = dyn_array<uint16_t>;
  struct periodicity_opts {
    enum options { sf5, sf10, sf20, sf40, sf80, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts>;

  // member variables
  pci_list_l_    pci_list;
  periodicity_e_ periodicity;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SSB-MTC3List-r16 ::= SEQUENCE (SIZE (1..4)) OF SSB-MTC3-r16
using ssb_mtc3_list_r16_l = dyn_array<ssb_mtc3_r16_s>;

// Tx-PoolMeasList-r16 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..16)
using tx_pool_meas_list_r16_l = bounded_array<uint8_t, 8>;

// UTRA-FDD-CellIndexList-r16 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..32)
using utra_fdd_cell_idx_list_r16_l = bounded_array<uint8_t, 32>;

// UTRA-FDD-Q-OffsetRange-r16 ::= ENUMERATED
struct utra_fdd_q_offset_range_r16_opts {
  enum options {
    db_neg24,
    db_neg22,
    db_neg20,
    db_neg18,
    db_neg16,
    db_neg14,
    db_neg12,
    db_neg10,
    db_neg8,
    db_neg6,
    db_neg5,
    db_neg4,
    db_neg3,
    db_neg2,
    db_neg1,
    db0,
    db1,
    db2,
    db3,
    db4,
    db5,
    db6,
    db8,
    db10,
    db12,
    db14,
    db16,
    db18,
    db20,
    db22,
    db24,
    nulltype
  } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
using utra_fdd_q_offset_range_r16_e = enumerated<utra_fdd_q_offset_range_r16_opts>;

// GapConfig ::= SEQUENCE
struct gap_cfg_s {
  struct mgl_opts {
    enum options { ms1dot5, ms3, ms3dot5, ms4, ms5dot5, ms6, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgl_e_ = enumerated<mgl_opts>;
  struct mgrp_opts {
    enum options { ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mgrp_e_ = enumerated<mgrp_opts>;
  struct mgta_opts {
    enum options { ms0, ms0dot25, ms0dot5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgta_e_ = enumerated<mgta_opts>;
  struct ref_serv_cell_ind_opts {
    enum options { pcell, pscell, mcg_fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ref_serv_cell_ind_e_ = enumerated<ref_serv_cell_ind_opts>;
  struct mgl_r16_opts {
    enum options { ms10, ms20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mgl_r16_e_ = enumerated<mgl_r16_opts>;

  // member variables
  bool    ext        = false;
  uint8_t gap_offset = 0;
  mgl_e_  mgl;
  mgrp_e_ mgrp;
  mgta_e_ mgta;
  // ...
  // group 0
  bool                 ref_serv_cell_ind_present = false;
  ref_serv_cell_ind_e_ ref_serv_cell_ind;
  // group 1
  bool       ref_fr2_serv_cell_async_ca_r16_present = false;
  bool       mgl_r16_present                        = false;
  uint8_t    ref_fr2_serv_cell_async_ca_r16         = 0;
  mgl_r16_e_ mgl_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasGapSharingScheme ::= ENUMERATED
struct meas_gap_sharing_scheme_opts {
  enum options { scheme00, scheme01, scheme10, scheme11, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
using meas_gap_sharing_scheme_e = enumerated<meas_gap_sharing_scheme_opts>;

// MeasObjectCLI-r16 ::= SEQUENCE
struct meas_obj_cli_r16_s {
  bool              ext = false;
  cli_res_cfg_r16_s cli_res_cfg_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectEUTRA ::= SEQUENCE
struct meas_obj_eutra_s {
  using cells_to_add_mod_list_eutran_l_          = dyn_array<eutra_cell_s>;
  using excluded_cells_to_add_mod_list_eutran_l_ = dyn_array<eutra_excluded_cell_s>;

  // member variables
  bool                                     ext                          = false;
  bool                                     eutra_q_offset_range_present = false;
  uint32_t                                 carrier_freq                 = 0;
  eutra_allowed_meas_bw_e                  allowed_meas_bw;
  eutra_cell_idx_list_l                    cells_to_rem_list_eutran;
  cells_to_add_mod_list_eutran_l_          cells_to_add_mod_list_eutran;
  eutra_cell_idx_list_l                    excluded_cells_to_rem_list_eutran;
  excluded_cells_to_add_mod_list_eutran_l_ excluded_cells_to_add_mod_list_eutran;
  bool                                     eutra_presence_ant_port1 = false;
  eutra_q_offset_range_e                   eutra_q_offset_range;
  bool                                     wideband_rsrq_meas = false;
  // ...
  // group 0
  bool    associated_meas_gap_r17_present = false;
  uint8_t associated_meas_gap_r17         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectNR ::= SEQUENCE
struct meas_obj_nr_s {
  using excluded_cells_to_add_mod_list_l_ = dyn_array<pci_range_elem_s>;
  using allowed_cells_to_add_mod_list_l_  = dyn_array<pci_range_elem_s>;
  struct meas_cycle_scell_opts {
    enum options { sf160, sf256, sf320, sf512, sf640, sf1024, sf1280, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using meas_cycle_scell_e_ = enumerated<meas_cycle_scell_opts>;
  struct meas_cycle_pscell_r17_opts {
    enum options { ms160, ms256, ms320, ms512, ms640, ms1024, ms1280, spare1, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  using meas_cycle_pscell_r17_e_ = enumerated<meas_cycle_pscell_r17_opts>;

  // member variables
  bool                              ext                                        = false;
  bool                              ssb_freq_present                           = false;
  bool                              ssb_subcarrier_spacing_present             = false;
  bool                              smtc1_present                              = false;
  bool                              smtc2_present                              = false;
  bool                              ref_freq_csi_rs_present                    = false;
  bool                              abs_thresh_ss_blocks_consolidation_present = false;
  bool                              abs_thresh_csi_rs_consolidation_present    = false;
  bool                              nrof_ss_blocks_to_average_present          = false;
  bool                              nrof_csi_rs_res_to_average_present         = false;
  uint32_t                          ssb_freq                                   = 0;
  subcarrier_spacing_e              ssb_subcarrier_spacing;
  ssb_mtc_s                         smtc1;
  ssb_mtc2_s                        smtc2;
  uint32_t                          ref_freq_csi_rs = 0;
  ref_sig_cfg_s                     ref_sig_cfg;
  thres_nr_s                        abs_thresh_ss_blocks_consolidation;
  thres_nr_s                        abs_thresh_csi_rs_consolidation;
  uint8_t                           nrof_ss_blocks_to_average  = 2;
  uint8_t                           nrof_csi_rs_res_to_average = 2;
  uint8_t                           quant_cfg_idx              = 1;
  q_offset_range_list_s             offset_mo;
  pci_list_l                        cells_to_rem_list;
  cells_to_add_mod_list_l           cells_to_add_mod_list;
  pci_range_idx_list_l              excluded_cells_to_rem_list;
  excluded_cells_to_add_mod_list_l_ excluded_cells_to_add_mod_list;
  pci_range_idx_list_l              allowed_cells_to_rem_list;
  allowed_cells_to_add_mod_list_l_  allowed_cells_to_add_mod_list;
  // ...
  // group 0
  bool                freq_band_ind_nr_present = false;
  bool                meas_cycle_scell_present = false;
  uint16_t            freq_band_ind_nr         = 1;
  meas_cycle_scell_e_ meas_cycle_scell;
  // group 1
  copy_ptr<ssb_mtc3_list_r16_l>             smtc3list_r16;
  copy_ptr<setup_release_c<rmtc_cfg_r16_s>> rmtc_cfg_r16;
  copy_ptr<setup_release_c<t312_r16_e>>     t312_r16;
  // group 2
  bool                                        associated_meas_gap_ssb_r17_present    = false;
  bool                                        associated_meas_gap_csi_rs_r17_present = false;
  bool                                        meas_cycle_pscell_r17_present          = false;
  uint8_t                                     associated_meas_gap_ssb_r17            = 1;
  uint8_t                                     associated_meas_gap_csi_rs_r17         = 1;
  copy_ptr<ssb_mtc4_list_r17_l>               smtc4list_r17;
  meas_cycle_pscell_r17_e_                    meas_cycle_pscell_r17;
  copy_ptr<cells_to_add_mod_list_ext_v1710_l> cells_to_add_mod_list_ext_v1710;
  // group 3
  bool    associated_meas_gap_ssb2_v1720_present    = false;
  bool    associated_meas_gap_csi_rs2_v1720_present = false;
  uint8_t associated_meas_gap_ssb2_v1720            = 1;
  uint8_t associated_meas_gap_csi_rs2_v1720         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectNR-SL-r16 ::= SEQUENCE
struct meas_obj_nr_sl_r16_s {
  tx_pool_meas_list_r16_l tx_pool_meas_to_rem_list_r16;
  tx_pool_meas_list_r16_l tx_pool_meas_to_add_mod_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectRxTxDiff-r17 ::= SEQUENCE
struct meas_obj_rx_tx_diff_r17_s {
  struct dl_ref_r17_c_ {
    struct types_opts {
      enum options { prs_ref_r17, csi_rs_ref_r17, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true>;

    // choice methods
    dl_ref_r17_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    void set_prs_ref_r17();
    void set_csi_rs_ref_r17();

  private:
    types type_;
  };

  // member variables
  bool          ext                = false;
  bool          dl_ref_r17_present = false;
  dl_ref_r17_c_ dl_ref_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectUTRA-FDD-r16 ::= SEQUENCE
struct meas_obj_utra_fdd_r16_s {
  bool                                 ext                                 = false;
  bool                                 utra_fdd_q_offset_range_r16_present = false;
  uint16_t                             carrier_freq_r16                    = 0;
  utra_fdd_q_offset_range_r16_e        utra_fdd_q_offset_range_r16;
  utra_fdd_cell_idx_list_r16_l         cells_to_rem_list_r16;
  cells_to_add_mod_list_utra_fdd_r16_l cells_to_add_mod_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosGapConfig-r17 ::= SEQUENCE
struct pos_gap_cfg_r17_s {
  struct mgl_r17_opts {
    enum options { ms1dot5, ms3, ms3dot5, ms4, ms5dot5, ms6, ms10, ms20, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgl_r17_e_ = enumerated<mgl_r17_opts>;
  struct mgrp_r17_opts {
    enum options { ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mgrp_r17_e_ = enumerated<mgrp_r17_opts>;
  struct mgta_r17_opts {
    enum options { ms0, ms0dot25, ms0dot5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgta_r17_e_ = enumerated<mgta_r17_opts>;
  struct gap_type_r17_opts {
    enum options { per_ue, per_fr1, per_fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using gap_type_r17_e_ = enumerated<gap_type_r17_opts>;

  // member variables
  bool            ext                         = false;
  uint8_t         meas_pos_pre_cfg_gap_id_r17 = 1;
  uint8_t         gap_offset_r17              = 0;
  mgl_r17_e_      mgl_r17;
  mgrp_r17_e_     mgrp_r17;
  mgta_r17_e_     mgta_r17;
  gap_type_r17_e_ gap_type_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QuantityConfigRS ::= SEQUENCE
struct quant_cfg_rs_s {
  filt_cfg_s ssb_filt_cfg;
  filt_cfg_s csi_rs_filt_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigInterRAT ::= SEQUENCE
struct report_cfg_inter_rat_s {
  struct report_type_c_ {
    struct types_opts {
      enum options { periodical, event_triggered, report_cgi, /*...*/ report_sftd, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 1>;

    // choice methods
    report_type_c_() = default;
    report_type_c_(const report_type_c_& other);
    report_type_c_& operator=(const report_type_c_& other);
    ~report_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodical_report_cfg_inter_rat_s& periodical()
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<periodical_report_cfg_inter_rat_s>();
    }
    event_trigger_cfg_inter_rat_s& event_triggered()
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<event_trigger_cfg_inter_rat_s>();
    }
    report_cgi_eutra_s& report_cgi()
    {
      assert_choice_type(types::report_cgi, type_, "reportType");
      return c.get<report_cgi_eutra_s>();
    }
    report_sftd_eutra_s& report_sftd()
    {
      assert_choice_type(types::report_sftd, type_, "reportType");
      return c.get<report_sftd_eutra_s>();
    }
    const periodical_report_cfg_inter_rat_s& periodical() const
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<periodical_report_cfg_inter_rat_s>();
    }
    const event_trigger_cfg_inter_rat_s& event_triggered() const
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<event_trigger_cfg_inter_rat_s>();
    }
    const report_cgi_eutra_s& report_cgi() const
    {
      assert_choice_type(types::report_cgi, type_, "reportType");
      return c.get<report_cgi_eutra_s>();
    }
    const report_sftd_eutra_s& report_sftd() const
    {
      assert_choice_type(types::report_sftd, type_, "reportType");
      return c.get<report_sftd_eutra_s>();
    }
    periodical_report_cfg_inter_rat_s& set_periodical();
    event_trigger_cfg_inter_rat_s&     set_event_triggered();
    report_cgi_eutra_s&                set_report_cgi();
    report_sftd_eutra_s&               set_report_sftd();

  private:
    types type_;
    choice_buffer_t<event_trigger_cfg_inter_rat_s,
                    periodical_report_cfg_inter_rat_s,
                    report_cgi_eutra_s,
                    report_sftd_eutra_s>
        c;

    void destroy_();
  };

  // member variables
  report_type_c_ report_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigNR ::= SEQUENCE
struct report_cfg_nr_s {
  struct report_type_c_ {
    struct types_opts {
      enum options {
        periodical,
        event_triggered,
        // ...
        report_cgi,
        report_sftd,
        cond_trigger_cfg_r16,
        cli_periodical_r16,
        cli_event_triggered_r16,
        rx_tx_periodical_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 6>;

    // choice methods
    report_type_c_() = default;
    report_type_c_(const report_type_c_& other);
    report_type_c_& operator=(const report_type_c_& other);
    ~report_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodical_report_cfg_s& periodical()
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<periodical_report_cfg_s>();
    }
    event_trigger_cfg_s& event_triggered()
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<event_trigger_cfg_s>();
    }
    report_cgi_s& report_cgi()
    {
      assert_choice_type(types::report_cgi, type_, "reportType");
      return c.get<report_cgi_s>();
    }
    report_sftd_nr_s& report_sftd()
    {
      assert_choice_type(types::report_sftd, type_, "reportType");
      return c.get<report_sftd_nr_s>();
    }
    cond_trigger_cfg_r16_s& cond_trigger_cfg_r16()
    {
      assert_choice_type(types::cond_trigger_cfg_r16, type_, "reportType");
      return c.get<cond_trigger_cfg_r16_s>();
    }
    cli_periodical_report_cfg_r16_s& cli_periodical_r16()
    {
      assert_choice_type(types::cli_periodical_r16, type_, "reportType");
      return c.get<cli_periodical_report_cfg_r16_s>();
    }
    cli_event_trigger_cfg_r16_s& cli_event_triggered_r16()
    {
      assert_choice_type(types::cli_event_triggered_r16, type_, "reportType");
      return c.get<cli_event_trigger_cfg_r16_s>();
    }
    rx_tx_periodical_r17_s& rx_tx_periodical_r17()
    {
      assert_choice_type(types::rx_tx_periodical_r17, type_, "reportType");
      return c.get<rx_tx_periodical_r17_s>();
    }
    const periodical_report_cfg_s& periodical() const
    {
      assert_choice_type(types::periodical, type_, "reportType");
      return c.get<periodical_report_cfg_s>();
    }
    const event_trigger_cfg_s& event_triggered() const
    {
      assert_choice_type(types::event_triggered, type_, "reportType");
      return c.get<event_trigger_cfg_s>();
    }
    const report_cgi_s& report_cgi() const
    {
      assert_choice_type(types::report_cgi, type_, "reportType");
      return c.get<report_cgi_s>();
    }
    const report_sftd_nr_s& report_sftd() const
    {
      assert_choice_type(types::report_sftd, type_, "reportType");
      return c.get<report_sftd_nr_s>();
    }
    const cond_trigger_cfg_r16_s& cond_trigger_cfg_r16() const
    {
      assert_choice_type(types::cond_trigger_cfg_r16, type_, "reportType");
      return c.get<cond_trigger_cfg_r16_s>();
    }
    const cli_periodical_report_cfg_r16_s& cli_periodical_r16() const
    {
      assert_choice_type(types::cli_periodical_r16, type_, "reportType");
      return c.get<cli_periodical_report_cfg_r16_s>();
    }
    const cli_event_trigger_cfg_r16_s& cli_event_triggered_r16() const
    {
      assert_choice_type(types::cli_event_triggered_r16, type_, "reportType");
      return c.get<cli_event_trigger_cfg_r16_s>();
    }
    const rx_tx_periodical_r17_s& rx_tx_periodical_r17() const
    {
      assert_choice_type(types::rx_tx_periodical_r17, type_, "reportType");
      return c.get<rx_tx_periodical_r17_s>();
    }
    periodical_report_cfg_s&         set_periodical();
    event_trigger_cfg_s&             set_event_triggered();
    report_cgi_s&                    set_report_cgi();
    report_sftd_nr_s&                set_report_sftd();
    cond_trigger_cfg_r16_s&          set_cond_trigger_cfg_r16();
    cli_periodical_report_cfg_r16_s& set_cli_periodical_r16();
    cli_event_trigger_cfg_r16_s&     set_cli_event_triggered_r16();
    rx_tx_periodical_r17_s&          set_rx_tx_periodical_r17();

  private:
    types type_;
    choice_buffer_t<cli_event_trigger_cfg_r16_s,
                    cli_periodical_report_cfg_r16_s,
                    cond_trigger_cfg_r16_s,
                    event_trigger_cfg_s,
                    periodical_report_cfg_s,
                    report_cgi_s,
                    report_sftd_nr_s,
                    rx_tx_periodical_r17_s>
        c;

    void destroy_();
  };

  // member variables
  report_type_c_ report_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigNR-SL-r16 ::= SEQUENCE
struct report_cfg_nr_sl_r16_s {
  struct report_type_r16_c_ {
    struct types_opts {
      enum options { periodical_r16, event_triggered_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    report_type_r16_c_() = default;
    report_type_r16_c_(const report_type_r16_c_& other);
    report_type_r16_c_& operator=(const report_type_r16_c_& other);
    ~report_type_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    periodical_report_cfg_nr_sl_r16_s& periodical_r16()
    {
      assert_choice_type(types::periodical_r16, type_, "reportType-r16");
      return c.get<periodical_report_cfg_nr_sl_r16_s>();
    }
    event_trigger_cfg_nr_sl_r16_s& event_triggered_r16()
    {
      assert_choice_type(types::event_triggered_r16, type_, "reportType-r16");
      return c.get<event_trigger_cfg_nr_sl_r16_s>();
    }
    const periodical_report_cfg_nr_sl_r16_s& periodical_r16() const
    {
      assert_choice_type(types::periodical_r16, type_, "reportType-r16");
      return c.get<periodical_report_cfg_nr_sl_r16_s>();
    }
    const event_trigger_cfg_nr_sl_r16_s& event_triggered_r16() const
    {
      assert_choice_type(types::event_triggered_r16, type_, "reportType-r16");
      return c.get<event_trigger_cfg_nr_sl_r16_s>();
    }
    periodical_report_cfg_nr_sl_r16_s& set_periodical_r16();
    event_trigger_cfg_nr_sl_r16_s&     set_event_triggered_r16();

  private:
    types                                                                             type_;
    choice_buffer_t<event_trigger_cfg_nr_sl_r16_s, periodical_report_cfg_nr_sl_r16_s> c;

    void destroy_();
  };

  // member variables
  report_type_r16_c_ report_type_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FilterConfigCLI-r16 ::= SEQUENCE
struct filt_cfg_cli_r16_s {
  bool        filt_coef_srs_rsrp_r16_present = false;
  bool        filt_coef_cli_rssi_r16_present = false;
  filt_coef_e filt_coef_srs_rsrp_r16;
  filt_coef_e filt_coef_cli_rssi_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GapConfig-r17 ::= SEQUENCE
struct gap_cfg_r17_s {
  struct gap_type_r17_opts {
    enum options { per_ue, per_fr1, per_fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using gap_type_r17_e_ = enumerated<gap_type_r17_opts>;
  struct mgl_r17_opts {
    enum options { ms1, ms1dot5, ms2, ms3, ms3dot5, ms4, ms5, ms5dot5, ms6, ms10, ms20, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgl_r17_e_ = enumerated<mgl_r17_opts>;
  struct mgrp_r17_opts {
    enum options { ms20, ms40, ms80, ms160, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using mgrp_r17_e_ = enumerated<mgrp_r17_opts>;
  struct mgta_r17_opts {
    enum options { ms0, ms0dot25, ms0dot5, ms0dot75, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  using mgta_r17_e_ = enumerated<mgta_r17_opts>;
  struct ref_serv_cell_ind_r17_opts {
    enum options { pcell, pscell, mcg_fr2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using ref_serv_cell_ind_r17_e_ = enumerated<ref_serv_cell_ind_r17_opts>;

  // member variables
  bool                      ext                                    = false;
  bool                      ref_serv_cell_ind_r17_present          = false;
  bool                      ref_fr2_serv_cell_async_ca_r17_present = false;
  bool                      pre_cfg_ind_r17_present                = false;
  bool                      ncsg_ind_r17_present                   = false;
  bool                      gap_assoc_prs_r17_present              = false;
  bool                      gap_sharing_r17_present                = false;
  bool                      gap_prio_r17_present                   = false;
  uint8_t                   meas_gap_id_r17                        = 1;
  gap_type_r17_e_           gap_type_r17;
  uint8_t                   gap_offset_r17 = 0;
  mgl_r17_e_                mgl_r17;
  mgrp_r17_e_               mgrp_r17;
  mgta_r17_e_               mgta_r17;
  ref_serv_cell_ind_r17_e_  ref_serv_cell_ind_r17;
  uint8_t                   ref_fr2_serv_cell_async_ca_r17 = 0;
  meas_gap_sharing_scheme_e gap_sharing_r17;
  uint8_t                   gap_prio_r17 = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdToAddMod ::= SEQUENCE
struct meas_id_to_add_mod_s {
  uint8_t meas_id       = 1;
  uint8_t meas_obj_id   = 1;
  uint8_t report_cfg_id = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectToAddMod ::= SEQUENCE
struct meas_obj_to_add_mod_s {
  struct meas_obj_c_ {
    struct types_opts {
      enum options {
        meas_obj_nr,
        // ...
        meas_obj_eutra,
        meas_obj_utra_fdd_r16,
        meas_obj_nr_sl_r16,
        meas_obj_cli_r16,
        meas_obj_rx_tx_diff_r17,
        meas_obj_relay_r17,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 6>;

    // choice methods
    meas_obj_c_() = default;
    meas_obj_c_(const meas_obj_c_& other);
    meas_obj_c_& operator=(const meas_obj_c_& other);
    ~meas_obj_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_obj_nr_s& meas_obj_nr()
    {
      assert_choice_type(types::meas_obj_nr, type_, "measObject");
      return c.get<meas_obj_nr_s>();
    }
    meas_obj_eutra_s& meas_obj_eutra()
    {
      assert_choice_type(types::meas_obj_eutra, type_, "measObject");
      return c.get<meas_obj_eutra_s>();
    }
    meas_obj_utra_fdd_r16_s& meas_obj_utra_fdd_r16()
    {
      assert_choice_type(types::meas_obj_utra_fdd_r16, type_, "measObject");
      return c.get<meas_obj_utra_fdd_r16_s>();
    }
    meas_obj_nr_sl_r16_s& meas_obj_nr_sl_r16()
    {
      assert_choice_type(types::meas_obj_nr_sl_r16, type_, "measObject");
      return c.get<meas_obj_nr_sl_r16_s>();
    }
    meas_obj_cli_r16_s& meas_obj_cli_r16()
    {
      assert_choice_type(types::meas_obj_cli_r16, type_, "measObject");
      return c.get<meas_obj_cli_r16_s>();
    }
    meas_obj_rx_tx_diff_r17_s& meas_obj_rx_tx_diff_r17()
    {
      assert_choice_type(types::meas_obj_rx_tx_diff_r17, type_, "measObject");
      return c.get<meas_obj_rx_tx_diff_r17_s>();
    }
    sl_meas_obj_r16_s& meas_obj_relay_r17()
    {
      assert_choice_type(types::meas_obj_relay_r17, type_, "measObject");
      return c.get<sl_meas_obj_r16_s>();
    }
    const meas_obj_nr_s& meas_obj_nr() const
    {
      assert_choice_type(types::meas_obj_nr, type_, "measObject");
      return c.get<meas_obj_nr_s>();
    }
    const meas_obj_eutra_s& meas_obj_eutra() const
    {
      assert_choice_type(types::meas_obj_eutra, type_, "measObject");
      return c.get<meas_obj_eutra_s>();
    }
    const meas_obj_utra_fdd_r16_s& meas_obj_utra_fdd_r16() const
    {
      assert_choice_type(types::meas_obj_utra_fdd_r16, type_, "measObject");
      return c.get<meas_obj_utra_fdd_r16_s>();
    }
    const meas_obj_nr_sl_r16_s& meas_obj_nr_sl_r16() const
    {
      assert_choice_type(types::meas_obj_nr_sl_r16, type_, "measObject");
      return c.get<meas_obj_nr_sl_r16_s>();
    }
    const meas_obj_cli_r16_s& meas_obj_cli_r16() const
    {
      assert_choice_type(types::meas_obj_cli_r16, type_, "measObject");
      return c.get<meas_obj_cli_r16_s>();
    }
    const meas_obj_rx_tx_diff_r17_s& meas_obj_rx_tx_diff_r17() const
    {
      assert_choice_type(types::meas_obj_rx_tx_diff_r17, type_, "measObject");
      return c.get<meas_obj_rx_tx_diff_r17_s>();
    }
    const sl_meas_obj_r16_s& meas_obj_relay_r17() const
    {
      assert_choice_type(types::meas_obj_relay_r17, type_, "measObject");
      return c.get<sl_meas_obj_r16_s>();
    }
    meas_obj_nr_s&             set_meas_obj_nr();
    meas_obj_eutra_s&          set_meas_obj_eutra();
    meas_obj_utra_fdd_r16_s&   set_meas_obj_utra_fdd_r16();
    meas_obj_nr_sl_r16_s&      set_meas_obj_nr_sl_r16();
    meas_obj_cli_r16_s&        set_meas_obj_cli_r16();
    meas_obj_rx_tx_diff_r17_s& set_meas_obj_rx_tx_diff_r17();
    sl_meas_obj_r16_s&         set_meas_obj_relay_r17();

  private:
    types type_;
    choice_buffer_t<meas_obj_cli_r16_s,
                    meas_obj_eutra_s,
                    meas_obj_nr_s,
                    meas_obj_nr_sl_r16_s,
                    meas_obj_rx_tx_diff_r17_s,
                    meas_obj_utra_fdd_r16_s,
                    sl_meas_obj_r16_s>
        c;

    void destroy_();
  };

  // member variables
  uint8_t     meas_obj_id = 1;
  meas_obj_c_ meas_obj;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosMeasGapPreConfigToAddModList-r17 ::= SEQUENCE (SIZE (1..16)) OF PosGapConfig-r17
using pos_meas_gap_pre_cfg_to_add_mod_list_r17_l = dyn_array<pos_gap_cfg_r17_s>;

// PosMeasGapPreConfigToReleaseList-r17 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (1..16)
using pos_meas_gap_pre_cfg_to_release_list_r17_l = bounded_array<uint8_t, 16>;

// QuantityConfigNR ::= SEQUENCE
struct quant_cfg_nr_s {
  bool           quant_cfg_rs_idx_present = false;
  quant_cfg_rs_s quant_cfg_cell;
  quant_cfg_rs_s quant_cfg_rs_idx;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QuantityConfigUTRA-FDD-r16 ::= SEQUENCE
struct quant_cfg_utra_fdd_r16_s {
  bool        filt_coef_rs_cp_r16_present = false;
  bool        filt_coef_ec_no_r16_present = false;
  filt_coef_e filt_coef_rs_cp_r16;
  filt_coef_e filt_coef_ec_no_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigToAddMod ::= SEQUENCE
struct report_cfg_to_add_mod_s {
  struct report_cfg_c_ {
    struct types_opts {
      enum options { report_cfg_nr, /*...*/ report_cfg_inter_rat, report_cfg_nr_sl_r16, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts, true, 2>;

    // choice methods
    report_cfg_c_() = default;
    report_cfg_c_(const report_cfg_c_& other);
    report_cfg_c_& operator=(const report_cfg_c_& other);
    ~report_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    report_cfg_nr_s& report_cfg_nr()
    {
      assert_choice_type(types::report_cfg_nr, type_, "reportConfig");
      return c.get<report_cfg_nr_s>();
    }
    report_cfg_inter_rat_s& report_cfg_inter_rat()
    {
      assert_choice_type(types::report_cfg_inter_rat, type_, "reportConfig");
      return c.get<report_cfg_inter_rat_s>();
    }
    report_cfg_nr_sl_r16_s& report_cfg_nr_sl_r16()
    {
      assert_choice_type(types::report_cfg_nr_sl_r16, type_, "reportConfig");
      return c.get<report_cfg_nr_sl_r16_s>();
    }
    const report_cfg_nr_s& report_cfg_nr() const
    {
      assert_choice_type(types::report_cfg_nr, type_, "reportConfig");
      return c.get<report_cfg_nr_s>();
    }
    const report_cfg_inter_rat_s& report_cfg_inter_rat() const
    {
      assert_choice_type(types::report_cfg_inter_rat, type_, "reportConfig");
      return c.get<report_cfg_inter_rat_s>();
    }
    const report_cfg_nr_sl_r16_s& report_cfg_nr_sl_r16() const
    {
      assert_choice_type(types::report_cfg_nr_sl_r16, type_, "reportConfig");
      return c.get<report_cfg_nr_sl_r16_s>();
    }
    report_cfg_nr_s&        set_report_cfg_nr();
    report_cfg_inter_rat_s& set_report_cfg_inter_rat();
    report_cfg_nr_sl_r16_s& set_report_cfg_nr_sl_r16();

  private:
    types                                                                            type_;
    choice_buffer_t<report_cfg_inter_rat_s, report_cfg_nr_s, report_cfg_nr_sl_r16_s> c;

    void destroy_();
  };

  // member variables
  uint8_t       report_cfg_id = 1;
  report_cfg_c_ report_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasGapConfig ::= SEQUENCE
struct meas_gap_cfg_s {
  using gap_to_add_mod_list_r17_l_ = dyn_array<gap_cfg_r17_s>;
  using gap_to_release_list_r17_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                       ext             = false;
  bool                       gap_fr2_present = false;
  setup_release_c<gap_cfg_s> gap_fr2;
  // ...
  // group 0
  copy_ptr<setup_release_c<gap_cfg_s>> gap_fr1;
  copy_ptr<setup_release_c<gap_cfg_s>> gap_ue;
  // group 1
  copy_ptr<gap_to_add_mod_list_r17_l_>                 gap_to_add_mod_list_r17;
  copy_ptr<gap_to_release_list_r17_l_>                 gap_to_release_list_r17;
  copy_ptr<pos_meas_gap_pre_cfg_to_add_mod_list_r17_l> pos_meas_gap_pre_cfg_to_add_mod_list_r17;
  copy_ptr<pos_meas_gap_pre_cfg_to_release_list_r17_l> pos_meas_gap_pre_cfg_to_release_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasGapSharingConfig ::= SEQUENCE
struct meas_gap_sharing_cfg_s {
  bool                                       ext                     = false;
  bool                                       gap_sharing_fr2_present = false;
  setup_release_c<meas_gap_sharing_scheme_e> gap_sharing_fr2;
  // ...
  // group 0
  copy_ptr<setup_release_c<meas_gap_sharing_scheme_e>> gap_sharing_fr1;
  copy_ptr<setup_release_c<meas_gap_sharing_scheme_e>> gap_sharing_ue;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdToAddModList ::= SEQUENCE (SIZE (1..64)) OF MeasIdToAddMod
using meas_id_to_add_mod_list_l = dyn_array<meas_id_to_add_mod_s>;

// MeasIdToRemoveList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..64)
using meas_id_to_rem_list_l = dyn_array<uint8_t>;

// MeasObjectToAddModList ::= SEQUENCE (SIZE (1..64)) OF MeasObjectToAddMod
using meas_obj_to_add_mod_list_l = dyn_array<meas_obj_to_add_mod_s>;

// MeasObjectToRemoveList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..64)
using meas_obj_to_rem_list_l = dyn_array<uint8_t>;

// QuantityConfig ::= SEQUENCE
struct quant_cfg_s {
  using quant_cfg_nr_list_l_ = dyn_array<quant_cfg_nr_s>;

  // member variables
  bool                 ext = false;
  quant_cfg_nr_list_l_ quant_cfg_nr_list;
  // ...
  // group 0
  copy_ptr<filt_cfg_s> quant_cfg_eutra;
  // group 1
  copy_ptr<quant_cfg_utra_fdd_r16_s> quant_cfg_utra_fdd_r16;
  copy_ptr<filt_cfg_cli_r16_s>       quant_cfg_cli_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigToAddModList ::= SEQUENCE (SIZE (1..64)) OF ReportConfigToAddMod
using report_cfg_to_add_mod_list_l = dyn_array<report_cfg_to_add_mod_s>;

// ReportConfigToRemoveList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..64)
using report_cfg_to_rem_list_l = dyn_array<uint8_t>;

// MeasConfig ::= SEQUENCE
struct meas_cfg_s {
  struct s_measure_cfg_c_ {
    struct types_opts {
      enum options { ssb_rsrp, csi_rsrp, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    s_measure_cfg_c_() = default;
    s_measure_cfg_c_(const s_measure_cfg_c_& other);
    s_measure_cfg_c_& operator=(const s_measure_cfg_c_& other);
    ~s_measure_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& ssb_rsrp()
    {
      assert_choice_type(types::ssb_rsrp, type_, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rsrp()
    {
      assert_choice_type(types::csi_rsrp, type_, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_rsrp() const
    {
      assert_choice_type(types::ssb_rsrp, type_, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rsrp() const
    {
      assert_choice_type(types::csi_rsrp, type_, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_rsrp();
    uint8_t& set_csi_rsrp();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                         ext                          = false;
  bool                         s_measure_cfg_present        = false;
  bool                         quant_cfg_present            = false;
  bool                         meas_gap_cfg_present         = false;
  bool                         meas_gap_sharing_cfg_present = false;
  meas_obj_to_rem_list_l       meas_obj_to_rem_list;
  meas_obj_to_add_mod_list_l   meas_obj_to_add_mod_list;
  report_cfg_to_rem_list_l     report_cfg_to_rem_list;
  report_cfg_to_add_mod_list_l report_cfg_to_add_mod_list;
  meas_id_to_rem_list_l        meas_id_to_rem_list;
  meas_id_to_add_mod_list_l    meas_id_to_add_mod_list;
  s_measure_cfg_c_             s_measure_cfg;
  quant_cfg_s                  quant_cfg;
  meas_gap_cfg_s               meas_gap_cfg;
  meas_gap_sharing_cfg_s       meas_gap_sharing_cfg;
  // ...
  // group 0
  bool interfreq_cfg_no_gap_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
